// application/freertos/embedded_proto/src/ringbuf.c
#include "ringbuf.h"

// RISC-V memory barriers (no-op on other architectures)
#ifdef __riscv
#define FENCE_W_W __asm__ volatile ("fence w,w" ::: "memory")
#define FENCE_R_R __asm__ volatile ("fence r,r" ::: "memory")
#else
#define FENCE_W_W
#define FENCE_R_R
#endif

void ringbuf_init(ringbuf_t *rb, uint64_t *storage, uint32_t capacity)
{
    rb->buf = storage;
    rb->head = 0;
    rb->tail = 0;
    rb->capacity = capacity;
}

int ringbuf_push(ringbuf_t *rb, uint64_t val)
{
    uint32_t next = (rb->head + 1) & (rb->capacity - 1);
    if (next == rb->tail) return -1;
    rb->buf[rb->head] = val;
    FENCE_W_W;  // ensure data visible before head update
    rb->head = next;
    return 0;
}

int ringbuf_pop(ringbuf_t *rb, uint64_t *val)
{
    if (rb->head == rb->tail) return -1;
    *val = rb->buf[rb->tail];
    FENCE_R_R;  // ensure data read before tail update
    rb->tail = (rb->tail + 1) & (rb->capacity - 1);
    return 0;
}

uint32_t ringbuf_count(const ringbuf_t *rb)
{
    return (rb->head - rb->tail) & (rb->capacity - 1);
}

void ringbuf_clear(ringbuf_t *rb)
{
    rb->tail = rb->head;
}

