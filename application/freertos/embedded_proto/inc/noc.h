// application/freertos/embedded_proto/inc/noc.h
#ifndef NOC_H
#define NOC_H

#include <stdint.h>
#include "paicore_config.h"

typedef struct {
    uint32_t high;
    uint32_t low;
} frame64_t;

#ifndef TEST_X86
static inline int noc_up_has_data(void)
{
    volatile uint32_t *status = (volatile uint32_t *)(FIFO_UP_ADDR + 8);
    return (*status != 0);
}

static inline void noc_write_down(frame64_t f)
{
    volatile uint32_t *fifo = (volatile uint32_t *)FIFO_DOWN_ADDR;
    *fifo = f.high;
    *fifo = f.low;
}

static inline int noc_read_up(frame64_t *f)
{
    if (!noc_up_has_data()) return -1;
    volatile uint32_t *fifo = (volatile uint32_t *)FIFO_UP_ADDR;
    f->high = *fifo;
    f->low  = *fifo;
    return 0;
}

static inline void noc_irq_enable(void)
{
    volatile uint32_t *ie = (volatile uint32_t *)(CLIC_CLILINTIE_BASE + SNN_CLILINTIE_OFF * 4);
    *ie = 1;
}

static inline void noc_irq_disable(void)
{
    volatile uint32_t *ie = (volatile uint32_t *)(CLIC_CLILINTIE_BASE + SNN_CLILINTIE_OFF * 4);
    *ie = 0;
}

static inline void noc_irq_clear(void)
{
    volatile uint32_t *ip = (volatile uint32_t *)(CLIC_CLILINTIE_BASE + SNN_CLILINTIE_OFF * 4 + 0x400);
    *ip = 0;
}

static inline void noc_init(void)
{
    noc_irq_enable();
}
#else
// x86 stubs for unit testing
#include <string.h>
#include <assert.h>
extern frame64_t _test_fifo_down[256];
extern int _test_fifo_down_count;
extern frame64_t _test_fifo_up[256];
extern int _test_fifo_up_head;
extern int _test_fifo_up_tail;

static inline void noc_write_down(frame64_t f)
{
    assert(_test_fifo_down_count < 256);
    _test_fifo_down[_test_fifo_down_count++] = f;
}

static inline int noc_read_up(frame64_t *f)
{
    if (_test_fifo_up_head == _test_fifo_up_tail) return -1;
    *f = _test_fifo_up[_test_fifo_up_tail++];
    return 0;
}

static inline int noc_up_has_data(void)
{
    return (_test_fifo_up_head != _test_fifo_up_tail);
}

static inline void noc_irq_enable(void)  {}
static inline void noc_irq_disable(void) {}
static inline void noc_irq_clear(void)   {}
static inline void noc_init(void)        {}
#endif

#endif // NOC_H
