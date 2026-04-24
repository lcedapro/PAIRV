// application/freertos/embedded_proto/test/test_ringbuf.c
#include <stdio.h>
#include <assert.h>
#include "ringbuf.h"

#define CAP 8
static uint64_t storage[CAP];

static void test_push_pop(void) {
    ringbuf_t rb;
    ringbuf_init(&rb, storage, CAP);
    assert(ringbuf_count(&rb) == 0);

    uint64_t val;
    assert(ringbuf_pop(&rb, &val) == -1);

    for (int i = 0; i < CAP - 1; i++) {
        assert(ringbuf_push(&rb, (uint64_t)i) == 0);
    }
    assert(ringbuf_count(&rb) == CAP - 1);
    assert(ringbuf_push(&rb, 99) == -1); // full

    for (int i = 0; i < CAP - 1; i++) {
        assert(ringbuf_pop(&rb, &val) == 0);
        assert(val == (uint64_t)i);
    }
    assert(ringbuf_count(&rb) == 0);
    printf("test_push_pop PASSED\n");
}

static void test_wrap_around(void) {
    ringbuf_t rb;
    ringbuf_init(&rb, storage, CAP);

    for (int round = 0; round < 3; round++) {
        for (int i = 0; i < CAP - 1; i++) {
            assert(ringbuf_push(&rb, (uint64_t)(round * 100 + i)) == 0);
        }
        uint64_t val;
        for (int i = 0; i < CAP - 1; i++) {
            assert(ringbuf_pop(&rb, &val) == 0);
            assert(val == (uint64_t)(round * 100 + i));
        }
    }
    printf("test_wrap_around PASSED\n");
}

static void test_clear(void) {
    ringbuf_t rb;
    ringbuf_init(&rb, storage, CAP);
    ringbuf_push(&rb, 42);
    ringbuf_push(&rb, 43);
    ringbuf_clear(&rb);
    assert(ringbuf_count(&rb) == 0);
    uint64_t val;
    assert(ringbuf_pop(&rb, &val) == -1);
    printf("test_clear PASSED\n");
}

int main(void) {
    test_push_pop();
    test_wrap_around();
    test_clear();
    printf("ALL ringbuf tests PASSED\n");
    return 0;
}
