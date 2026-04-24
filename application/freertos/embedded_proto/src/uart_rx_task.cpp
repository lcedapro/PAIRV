// application/freertos/embedded_proto/src/uart_rx_task.cpp
// UART RX: ISR drains RXFIFO → byte queue → frame parser → protobuf → dispatch_queue

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "nuclei_sdk_soc.h"
#include "paicore_config.h"
#include "uart_frame.h"
#include "worker_task.h"
#include "paicore.h"
#include <ReadBufferFixedSize.h>

// Byte queue between ISR and task
static QueueHandle_t uart_byte_queue;

// ISR: drain RXFIFO into uart_byte_queue
extern "C" void UART0_IRQHandler(void) __attribute__((interrupt));
extern "C" void UART0_IRQHandler(void)
{
    BaseType_t higher_woken = pdFALSE;
    uint32_t rxval;
    while (!((rxval = UART0->RXFIFO) & UART_RXFIFO_EMPTY)) {
        uint8_t byte = (uint8_t)(rxval & 0xFF);
        xQueueSendFromISR(uart_byte_queue, &byte, &higher_woken);
    }
    portYIELD_FROM_ISR(higher_woken);
}

extern "C" void uart_rx_task(void *param)
{
    (void)param;

    // Create byte queue (ISR → task)
    uart_byte_queue = xQueueCreate(UART_RX_BUF_SIZE, sizeof(uint8_t));
    configASSERT(uart_byte_queue != NULL);

    // Register UART0 interrupt with ECLIC
    ECLIC_Register_IRQ(UART0_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                       ECLIC_LEVEL_TRIGGER, 2, 0,
                       (void *)UART0_IRQHandler);
    // Enable RXWM interrupt in UART0
    UART0->IE |= UART_IP_RXWM;

    // Frame parser state
    static uint8_t rx_buf[UART_RX_BUF_SIZE];
    uart_rx_ctx_t ctx;
    uart_rx_init(&ctx, rx_buf, sizeof(rx_buf));

    for (;;) {
        uint8_t byte;
        if (xQueueReceive(uart_byte_queue, &byte, portMAX_DELAY) != pdTRUE)
            continue;

        int ret = uart_rx_feed(&ctx, byte);
        if (ret != 1)
            continue;

        // Complete frame received — deserialize HostMessage
        using HostMsg = paicore::HostMessage<PROTO_BUF_SIZE>;
        static HostMsg hmsg;

        static EmbeddedProto::ReadBufferFixedSize<PROTO_BUF_SIZE> rbuf;
        uint16_t plen = ctx.expected;
        hmsg.clear();
        rbuf.clear();
        memcpy(rbuf.get_data(), ctx.buf, plen);
        rbuf.set_bytes_written(plen);

        if (hmsg.deserialize(rbuf) != EmbeddedProto::Error::NO_ERRORS)
            continue;

        worker_msg_t wmsg;
        memset(&wmsg, 0, sizeof(wmsg));

        if (hmsg.has_init_req()) {
            wmsg.type      = MSG_INIT_REQ;
            wmsg.thread_id = hmsg.get_init_req().get_thread_id();
            wmsg.timesteps = 0;
            wmsg.data_len  = 0;
        } else if (hmsg.has_infer_req()) {
            wmsg.type      = MSG_INFER_REQ;
            wmsg.thread_id = hmsg.get_infer_req().get_thread_id();
            wmsg.timesteps = hmsg.get_infer_req().get_timesteps();
            uint32_t dlen  = hmsg.get_infer_req().get_data().get_length();
            if (dlen > PROTO_BUF_SIZE)
                dlen = PROTO_BUF_SIZE;
            memcpy(wmsg.data, hmsg.get_infer_req().get_data().get_const(), dlen);
            wmsg.data_len = (uint16_t)dlen;
        } else if (hmsg.has_config_req()) {
            config_msg_t cmsg;
            cmsg.type        = MSG_CONFIG_REQ;
            cmsg.block_index = hmsg.get_config_req().get_block_index();
            xQueueSend(config_queue, &cmsg, portMAX_DELAY);
            continue;
        } else if (hmsg.has_test_req()) {
            config_msg_t cmsg;
            cmsg.type        = MSG_TEST_REQ;
            cmsg.block_index = hmsg.get_test_req().get_block_index();
            xQueueSend(config_queue, &cmsg, portMAX_DELAY);
            continue;
        } else {
            continue;  // unknown message type, drop
        }

        xQueueSend(dispatch_queue, &wmsg, portMAX_DELAY);
    }
}
