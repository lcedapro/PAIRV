// application/freertos/embedded_proto/test/test_uart_frame.c
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "uart_frame.h"

static uint8_t rx_buf[PROTO_BUF_SIZE];
static uint8_t tx_buf[PROTO_BUF_SIZE + 6];

static void feed_bytes(uart_rx_ctx_t *ctx, const uint8_t *data, int len, int *result)
{
    *result = 0;
    for (int i = 0; i < len; i++) {
        *result = uart_rx_feed(ctx, data[i]);
        if (*result != 0) break;
    }
}

static void test_encode_decode_roundtrip(void) {
    const uint8_t payload[] = {0x01, 0x02, 0x03, 0x04};
    int encoded = uart_frame_encode(tx_buf, sizeof(tx_buf), payload, sizeof(payload));
    assert(encoded == 10);  // 6 header + 4 payload
    assert(tx_buf[0] == 0x50 && tx_buf[1] == 0x41 && tx_buf[2] == 0x49 && tx_buf[3] == 0x43);
    assert(tx_buf[4] == 4 && tx_buf[5] == 0);  // length LE

    uart_rx_ctx_t ctx;
    uart_rx_init(&ctx, rx_buf, sizeof(rx_buf));
    int result;
    feed_bytes(&ctx, tx_buf, encoded, &result);
    assert(result == 1);
    assert(ctx.expected == 4);
    assert(memcmp(rx_buf, payload, 4) == 0);
    printf("test_encode_decode_roundtrip PASSED\n");
}

static void test_bad_magic(void) {
    uart_rx_ctx_t ctx;
    uart_rx_init(&ctx, rx_buf, sizeof(rx_buf));
    // Feed wrong magic
    uint8_t bad[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0xAB};
    int result;
    feed_bytes(&ctx, bad, sizeof(bad), &result);
    assert(result == 0);  // no complete frame
    assert(ctx.state == UART_RX_WAIT_MAGIC0);
    printf("test_bad_magic PASSED\n");
}

static void test_payload_too_large(void) {
    uart_rx_ctx_t ctx;
    uart_rx_init(&ctx, rx_buf, 4);  // tiny buffer
    // Encode a 5-byte payload
    const uint8_t payload[5] = {1, 2, 3, 4, 5};
    uint8_t frame[11];
    uart_frame_encode(frame, sizeof(frame), payload, 5);
    int result;
    feed_bytes(&ctx, frame, sizeof(frame), &result);
    assert(result == -1);  // payload too large for buffer
    printf("test_payload_too_large PASSED\n");
}

static void test_partial_magic_recovery(void) {
    uart_rx_ctx_t ctx;
    uart_rx_init(&ctx, rx_buf, sizeof(rx_buf));
    // Feed 'P' then wrong byte, then full valid frame
    const uint8_t payload[] = {0xDE, 0xAD};
    uint8_t frame[8];
    uart_frame_encode(frame, sizeof(frame), payload, 2);

    uart_rx_feed(&ctx, 0x50);  // 'P' — partial match
    uart_rx_feed(&ctx, 0xFF);  // wrong — resets to WAIT_MAGIC0
    assert(ctx.state == UART_RX_WAIT_MAGIC0);

    int result;
    feed_bytes(&ctx, frame, 8, &result);
    assert(result == 1);
    assert(ctx.expected == 2);
    assert(rx_buf[0] == 0xDE && rx_buf[1] == 0xAD);
    printf("test_partial_magic_recovery PASSED\n");
}

static void test_zero_length_payload(void) {
    uart_rx_ctx_t ctx;
    uart_rx_init(&ctx, rx_buf, sizeof(rx_buf));
    // Manually build a zero-length frame
    uint8_t frame[6] = {0x50, 0x41, 0x49, 0x43, 0x00, 0x00};
    int result;
    feed_bytes(&ctx, frame, 6, &result);
    assert(result == -1);  // zero length is invalid
    printf("test_zero_length_payload PASSED\n");
}

int main(void) {
    test_encode_decode_roundtrip();
    test_bad_magic();
    test_payload_too_large();
    test_partial_magic_recovery();
    test_zero_length_payload();
    printf("ALL uart_frame tests PASSED\n");
    return 0;
}
