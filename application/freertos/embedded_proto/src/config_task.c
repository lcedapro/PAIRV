// application/freertos/embedded_proto/src/config_task.c
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "worker_task.h"
#include "config_meta.h"
#include "noc.h"
#include "paicore_config.h"

#define CONFIG_TEST_TIMEOUT_MS 10000

// Build the 64-bit config/test header frame.
// frame_type: 0=config, 1=test_request
// is_test_req: 1 for test request, 0 for config
// Copy fields are always zero (test requests never multicast).
static frame64_t build_cfg_header(uint8_t frame_type, const config_block_t *b,
                                   uint8_t is_test_req, uint32_t n_frames)
{
    frame64_t f = {0};
    uint32_t n    = n_frames & 0x3FFF;
    uint32_t sram = (uint32_t)(b->sram_addr & 0x1FF);
    f.high = ((uint32_t)frame_type          << 30)
           | ((uint32_t)(b->subtype & 0x3)  << 28)
           | ((uint32_t)(b->core_xy & 0x3F) << 22)
           | ((uint32_t)(b->core_x  & 0x3F) << 16)
           | ((uint32_t)(b->core_y  & 0x3F) << 10);
    f.low  = ((uint32_t)is_test_req << 23)
           | (sram << 14)
           | n;
    return f;
}

static void send_config(const config_block_t *b, config_resp_t *resp)
{
    uint32_t n_frames = b->data_words / 2;
    frame64_t hdr = build_cfg_header(0, b, 0, n_frames);
    noc_write_down(hdr);
    for (uint32_t i = 0; i < b->data_words; i += 2) {
        frame64_t f;
        f.high = b->data[i + 1];
        f.low  = b->data[i];
        noc_write_down(f);
    }
    resp->success = 1;
    resp->mismatch_frame = 0;
}

static void send_test_and_verify(const config_block_t *b, config_resp_t *resp)
{
    uint32_t n_frames = b->data_words / 2;

    frame64_t req_hdr = build_cfg_header(1, b, 1, n_frames);
    noc_write_down(req_hdr);

    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(CONFIG_TEST_TIMEOUT_MS);
    while (!noc_up_has_data()) {
        if (xTaskGetTickCount() >= deadline) {
            resp->success = 0;
            resp->mismatch_frame = 0;
            return;
        }
        vTaskDelay(1);
    }

    frame64_t resp_hdr;
    if (noc_read_up(&resp_hdr) != 0) {
        resp->success = 0;
        resp->mismatch_frame = 0;
        return;
    }

    uint8_t ftype   = (uint8_t)((resp_hdr.high >> 30) & 0x3);
    uint8_t is_resp = (uint8_t)((resp_hdr.low  >> 23) & 0x1);
    uint32_t resp_n = resp_hdr.low & 0x3FFF;
    if (ftype != 1 || is_resp != 0 || resp_n != n_frames) {
        resp->success = 0;
        resp->mismatch_frame = 0;
        return;
    }

    for (uint32_t i = 0; i < n_frames; i++) {
        // Reset deadline per frame — each frame gets a full timeout window
        deadline = xTaskGetTickCount() + pdMS_TO_TICKS(CONFIG_TEST_TIMEOUT_MS);
        while (!noc_up_has_data()) {
            if (xTaskGetTickCount() >= deadline) {
                resp->success = 0;
                resp->mismatch_frame = i;
                return;
            }
            vTaskDelay(1);
        }
        frame64_t df;
        if (noc_read_up(&df) != 0) {
            resp->success = 0;
            resp->mismatch_frame = i;
            return;
        }
        uint32_t exp_low  = b->data[i * 2];
        uint32_t exp_high = b->data[i * 2 + 1];
        if (df.low != exp_low || df.high != exp_high) {
            resp->success = 0;
            resp->mismatch_frame = i;
            return;
        }
    }
    resp->success = 1;
    resp->mismatch_frame = 0;
}

void config_task(void *param)
{
    (void)param;
    config_msg_t msg;

    for (;;) {
        if (xQueueReceive(config_queue, &msg, portMAX_DELAY) != pdTRUE)
            continue;

        if (msg.block_index >= CONFIG_BLOCK_COUNT) {
            tx_envelope_t env;
            env.env_type             = (msg.type == MSG_CONFIG_REQ) ? TX_CONFIG_RESP : TX_TEST_RESP;
            env.u.config.type        = msg.type;
            env.u.config.block_index = msg.block_index;
            env.u.config.success     = 0;
            env.u.config.mismatch_frame = 0;
            xQueueSend(tx_queue, &env, portMAX_DELAY);
            continue;
        }

        const config_block_t *b = &g_config_blocks[msg.block_index];

        xSemaphoreTake(accel_mutex, portMAX_DELAY);
        noc_irq_disable();

        config_resp_t resp;
        resp.type        = msg.type;
        resp.block_index = msg.block_index;

        if (msg.type == MSG_CONFIG_REQ) {
            send_config(b, &resp);
        } else {
            send_test_and_verify(b, &resp);
        }

        noc_irq_enable();
        xSemaphoreGive(accel_mutex);

        tx_envelope_t env;
        env.env_type = (msg.type == MSG_CONFIG_REQ) ? TX_CONFIG_RESP : TX_TEST_RESP;
        env.u.config = resp;
        xQueueSend(tx_queue, &env, portMAX_DELAY);
    }
}
