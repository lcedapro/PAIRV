// application/freertos/embedded_proto/src/noc.c
#include "noc.h"

#ifdef TEST_X86
frame64_t _test_fifo_down[256];
int _test_fifo_down_count = 0;
frame64_t _test_fifo_up[256];
int _test_fifo_up_head = 0;
int _test_fifo_up_tail = 0;
#endif
