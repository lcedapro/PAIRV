// application/freertos/embedded_proto/src/worker_task.c
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "worker_task.h"
#include "noc.h"
#include "frame_builder.h"
#include "neuron_map.h"
#include "paicore_config.h"

// ── Init handler ─────────────────────────────────────────────────────────────

static void handle_init(uint32_t thread_id)
{
    tx_msg_t resp;
    memset(&resp, 0, sizeof(resp));
    resp.is_init_resp = 1;
    resp.thread_id    = thread_id;

    // Send init control frame under noc_mutex
    xSemaphoreTake(noc_mutex, portMAX_DELAY);
    frame64_t f = frame_build_control(0x01, 0, 0, 0, 0, 0, 0, thread_id);
    noc_write_down(f);
    xSemaphoreGive(noc_mutex);

    // Wait for hardware to signal completion
    BaseType_t ok = xSemaphoreTake(done_sem[thread_id],
                                   pdMS_TO_TICKS(INIT_TIMEOUT_MS));
    resp.success = (ok == pdTRUE) ? 1 : 0;

    tx_envelope_t env;
    env.env_type = TX_INFER_RESP;
    env.u.infer  = resp;
    xQueueSend(tx_queue, &env, portMAX_DELAY);
}

// ── Infer handler ─────────────────────────────────────────────────────────────

static void handle_infer(uint32_t thread_id, const worker_msg_t *req)
{
    tx_msg_t resp;
    memset(&resp, 0, sizeof(resp));
    resp.is_init_resp = 0;
    resp.thread_id    = thread_id;

    const uint32_t H   = g_map_ctx[thread_id].H;
    const uint32_t W   = g_map_ctx[thread_id].W;
    /* CHW per timestep; cap to what fits in the data buffer */
    const uint32_t CHW = g_map_ctx[thread_id].C * H * W;
    const uint32_t max_chw = (req->timesteps > 0)
        ? (PROTO_BUF_SIZE / req->timesteps)
        : CHW;
    const uint32_t iter_chw = (CHW < max_chw) ? CHW : max_chw;

    uint32_t t0 = (uint32_t)xTaskGetTickCount();

    // Serialize the full hardware round-trip across both workers
    xSemaphoreTake(accel_mutex, portMAX_DELAY);

    // Send work frames + sync frame under noc_mutex
    xSemaphoreTake(noc_mutex, portMAX_DELAY);

    // Set active_thread BEFORE sync so ISR routes output frames correctly
    active_thread = thread_id;

    // Clear any stale output from previous inference
    ringbuf_clear(&output_ringbuf[thread_id]);

    for (uint32_t t = 0; t < req->timesteps; t++) {
        for (uint32_t chw = 0; chw < iter_chw; chw++) {
            uint8_t val = req->data[t * iter_chw + chw];
            if (val == 0) continue;

            uint32_t c = chw / (H * W);
            uint32_t h = (chw / W) % H;
            uint32_t w = chw % W;

            const input_neuron_mapping_t *m =
                neuron_map_lookup(&g_map_ctx[thread_id], c, h, w);
            if (m == NULL) continue;

            frame64_t wf = frame_build_work(
                (uint16_t)t,
                m->core_xy, m->core_x, m->core_y,
                m->copy_xy, m->copy_x, m->copy_y,
                m->axon_addr, val);
            noc_write_down(wf);
        }
    }

    // Sync frame — load = timestep count
    frame64_t sync = frame_build_control(0x00, 0, 0, 0, 0, 0, 0,
                                         req->timesteps);
    noc_write_down(sync);

    xSemaphoreGive(noc_mutex);

    // Wait for inference completion
    BaseType_t ok = xSemaphoreTake(done_sem[thread_id],
                                   pdMS_TO_TICKS(INFER_TIMEOUT_MS));
    resp.success = (ok == pdTRUE) ? 1 : 0;

    // Collect output from ring buffer
    if (ok == pdTRUE) {
        uint64_t raw;
        uint16_t max_idx = 0;
        while (ringbuf_pop(&output_ringbuf[thread_id], &raw) == 0) {
            frame64_t f;
            f.high = (uint32_t)(raw >> 32);
            f.low  = (uint32_t)(raw & 0xFFFFFFFFU);

            uint16_t axon = frame_axon(&f);
            uint8_t  data = frame_data(&f);

            uint32_t c, h, w;
            if (output_map_lookup(&g_map_ctx[thread_id], axon, &c, &h, &w) == 0) {
                uint16_t idx = (uint16_t)(c * H * W + h * W + w);
                if (idx < PROTO_BUF_SIZE) {
                    resp.data[idx] = data;
                    if (idx + 1 > max_idx) max_idx = idx + 1;
                }
            }
        }
        resp.data_len = max_idx;
    }

    // Hardware cycle complete — release accel_mutex before building response
    xSemaphoreGive(accel_mutex);

    resp.cycles = (uint32_t)xTaskGetTickCount() - t0;

    tx_envelope_t env;
    env.env_type = TX_INFER_RESP;
    env.u.infer  = resp;
    xQueueSend(tx_queue, &env, portMAX_DELAY);
}

// ── Task entry point ──────────────────────────────────────────────────────────

void worker_task(void *param)
{
    uint32_t thread_id = (uint32_t)(uintptr_t)param;
    worker_msg_t req;

    for (;;) {
        if (xQueueReceive(input_queue[thread_id], &req, portMAX_DELAY) != pdTRUE)
            continue;

        switch (req.type) {
        case MSG_INIT_REQ:
            handle_init(thread_id);
            break;
        case MSG_INFER_REQ:
            handle_infer(thread_id, &req);
            break;
        default:
            break;
        }
    }
}
