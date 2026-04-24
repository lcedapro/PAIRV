// application/freertos/embedded_proto/inc/ringbuf.h
#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    volatile uint64_t *buf;  // shared between ISR (producer) and task (consumer)
    volatile uint32_t head;  // written by producer (ISR)
    volatile uint32_t tail;  // read by consumer (task)
    uint32_t capacity;       // must be power of 2
} ringbuf_t;

void     ringbuf_init(ringbuf_t *rb, uint64_t *storage, uint32_t capacity);
int      ringbuf_push(ringbuf_t *rb, uint64_t val);   // returns 0 on success, -1 if full
int      ringbuf_pop(ringbuf_t *rb, uint64_t *val);    // returns 0 on success, -1 if empty
uint32_t ringbuf_count(const ringbuf_t *rb);
void     ringbuf_clear(ringbuf_t *rb);

#endif
