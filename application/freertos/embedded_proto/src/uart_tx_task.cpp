// application/freertos/embedded_proto/src/uart_tx_task.cpp
// UART TX: tx_queue → protobuf serialize → UART frame → TXFIFO

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "nuclei_sdk_soc.h"
#include "paicore_config.h"
#include "uart_frame.h"
#include "worker_task.h"
#include "paicore.h"
#include <WriteBufferFixedSize.h>

// Frame output buffer: 6-byte header + up to PROTO_BUF_SIZE payload
static uint8_t tx_frame_buf[6 + PROTO_BUF_SIZE];

extern "C" void uart_tx_task(void *param)
{
    (void)param;

    tx_envelope_t env;

    // Build DeviceMessage
    using DevMsg = paicore::DeviceMessage<PROTO_BUF_SIZE>;
    static DevMsg dmsg;
    static EmbeddedProto::WriteBufferFixedSize<PROTO_BUF_SIZE> wbuf;

    for (;;) {
        if (xQueueReceive(tx_queue, &env, portMAX_DELAY) != pdTRUE)
            continue;

        dmsg.clear();
        wbuf.clear();

        if (env.env_type == TX_INFER_RESP) {
            const tx_msg_t &tmsg = env.u.infer;
            if (tmsg.is_init_resp) {
                dmsg.mutable_init_resp().set_thread_id(tmsg.thread_id);
                dmsg.mutable_init_resp().set_success(tmsg.success != 0);
            } else {
                dmsg.mutable_infer_resp().set_thread_id(tmsg.thread_id);
                dmsg.mutable_infer_resp().set_success(tmsg.success != 0);
                dmsg.mutable_infer_resp().set_cycles(tmsg.cycles);
                if (tmsg.data_len > 0)
                    dmsg.mutable_infer_resp().mutable_data().set(tmsg.data, tmsg.data_len);
            }
        } else if (env.env_type == TX_CONFIG_RESP) {
            dmsg.mutable_config_resp().set_block_index(env.u.config.block_index);
            dmsg.mutable_config_resp().set_success(env.u.config.success != 0);
        } else if (env.env_type == TX_TEST_RESP) {
            dmsg.mutable_test_resp().set_block_index(env.u.config.block_index);
            dmsg.mutable_test_resp().set_success(env.u.config.success != 0);
            dmsg.mutable_test_resp().set_mismatch_frame(env.u.config.mismatch_frame);
        } else {
            continue;
        }

        // Serialize to write buffer
        if (dmsg.serialize(wbuf) != EmbeddedProto::Error::NO_ERRORS)
            continue;

        uint32_t plen  = wbuf.get_size();
        const uint8_t *pdata = wbuf.get_data();

        // Wrap in UART frame
        int frame_len = uart_frame_encode(tx_frame_buf, sizeof(tx_frame_buf),
                                          pdata, (uint16_t)plen);
        if (frame_len <= 0)
            continue;

        // Write bytes to TXFIFO, spin-waiting on full
        for (int i = 0; i < frame_len; i++) {
            while (UART0->TXFIFO & UART_TXFIFO_FULL)
                ;
            UART0->TXFIFO = tx_frame_buf[i];
        }
    }
}
