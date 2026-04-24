// application/freertos/embedded_proto/src/uart_frame.c
#include "uart_frame.h"
#include <string.h>

void uart_rx_init(uart_rx_ctx_t *ctx, uint8_t *buf, uint16_t buf_size)
{
    ctx->state    = UART_RX_WAIT_MAGIC0;
    ctx->buf      = buf;
    ctx->buf_size = buf_size;
    ctx->expected = 0;
    ctx->received = 0;
}

void uart_rx_reset(uart_rx_ctx_t *ctx)
{
    ctx->state    = UART_RX_WAIT_MAGIC0;
    ctx->expected = 0;
    ctx->received = 0;
}

int uart_rx_feed(uart_rx_ctx_t *ctx, uint8_t byte)
{
    switch (ctx->state) {
    case UART_RX_WAIT_MAGIC0:
        ctx->state = (byte == UART_FRAME_MAGIC_B0) ? UART_RX_WAIT_MAGIC1 : UART_RX_WAIT_MAGIC0;
        return 0;
    case UART_RX_WAIT_MAGIC1:
        ctx->state = (byte == UART_FRAME_MAGIC_B1) ? UART_RX_WAIT_MAGIC2 : UART_RX_WAIT_MAGIC0;
        return 0;
    case UART_RX_WAIT_MAGIC2:
        ctx->state = (byte == UART_FRAME_MAGIC_B2) ? UART_RX_WAIT_MAGIC3 : UART_RX_WAIT_MAGIC0;
        return 0;
    case UART_RX_WAIT_MAGIC3:
        ctx->state = (byte == UART_FRAME_MAGIC_B3) ? UART_RX_WAIT_LEN_LO : UART_RX_WAIT_MAGIC0;
        return 0;
    case UART_RX_WAIT_LEN_LO:
        ctx->expected = byte;
        ctx->state    = UART_RX_WAIT_LEN_HI;
        return 0;
    case UART_RX_WAIT_LEN_HI:
        ctx->expected |= ((uint16_t)byte << 8);
        if (ctx->expected == 0 || ctx->expected > ctx->buf_size) {
            ctx->state = UART_RX_WAIT_MAGIC0;
            return -1;
        }
        ctx->received = 0;
        ctx->state    = UART_RX_RECV_PAYLOAD;
        return 0;
    case UART_RX_RECV_PAYLOAD:
        ctx->buf[ctx->received++] = byte;
        if (ctx->received == ctx->expected) {
            ctx->state = UART_RX_WAIT_MAGIC0;
            return 1;
        }
        return 0;
    default:
        ctx->state = UART_RX_WAIT_MAGIC0;
        return -1;
    }
}

int uart_frame_encode(uint8_t *dst, uint16_t dst_size,
                      const uint8_t *payload, uint16_t payload_len)
{
    uint16_t total = 6 + payload_len;
    if (total > dst_size) return -1;
    dst[0] = UART_FRAME_MAGIC_B0;
    dst[1] = UART_FRAME_MAGIC_B1;
    dst[2] = UART_FRAME_MAGIC_B2;
    dst[3] = UART_FRAME_MAGIC_B3;
    dst[4] = (uint8_t)(payload_len & 0xFF);
    dst[5] = (uint8_t)(payload_len >> 8);
    memcpy(dst + 6, payload, payload_len);
    return total;
}
