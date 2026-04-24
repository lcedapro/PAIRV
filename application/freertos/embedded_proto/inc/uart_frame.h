// application/freertos/embedded_proto/inc/uart_frame.h
#ifndef UART_FRAME_H
#define UART_FRAME_H

#include <stdint.h>
#include <stddef.h>
#include "paicore_config.h"

// Frame format: [MAGIC 4B][LENGTH 2B LE][PAYLOAD N B]
// MAGIC = 'P','A','I','C' = 0x50 0x41 0x49 0x43
// Note: the state machine does not handle overlapping magic sequences (e.g., "PPAIC").
// The host must not produce such patterns.

typedef enum {
    UART_RX_WAIT_MAGIC0 = 0,
    UART_RX_WAIT_MAGIC1,
    UART_RX_WAIT_MAGIC2,
    UART_RX_WAIT_MAGIC3,
    UART_RX_WAIT_LEN_LO,
    UART_RX_WAIT_LEN_HI,
    UART_RX_RECV_PAYLOAD,
} uart_rx_state_t;

typedef struct {
    uart_rx_state_t state;
    uint8_t        *buf;        // payload buffer (caller-provided)
    uint16_t        buf_size;   // max payload size
    uint16_t        expected;   // payload length from header
    uint16_t        received;   // bytes received so far
} uart_rx_ctx_t;

#ifdef __cplusplus
extern "C" {
#endif

// Initialize receiver context
void uart_rx_init(uart_rx_ctx_t *ctx, uint8_t *buf, uint16_t buf_size);

// Reset parser state (e.g., on timeout). Does not change buf/buf_size.
void uart_rx_reset(uart_rx_ctx_t *ctx);

// Feed one byte into the state machine.
// Returns 1 when a complete frame is ready in ctx->buf[0..ctx->expected-1].
// Returns 0 if more bytes needed.
// Returns -1 on error (bad magic, payload too large).
int uart_rx_feed(uart_rx_ctx_t *ctx, uint8_t byte);

// Build a framed message into dst.
// Returns total bytes written (6 + payload_len), or -1 if dst_size too small.
int uart_frame_encode(uint8_t *dst, uint16_t dst_size,
                      const uint8_t *payload, uint16_t payload_len);

#ifdef __cplusplus
}
#endif

#endif // UART_FRAME_H
