// application/freertos/embedded_proto/src/snn_isr.c
#include "worker_task.h"
#include "frame_builder.h"
#include "noc.h"
#include "FreeRTOS.h"
#include "semphr.h"

volatile uint32_t active_thread = 0;

// SNN interrupt handler — called when FIFO_UP has data
// Registered as CLIC interrupt for SNN_IRQn
void SNN_IRQHandler(void) __attribute__((interrupt));
void SNN_IRQHandler(void)
{
    noc_irq_clear();

    BaseType_t higher_woken = pdFALSE;
    while (noc_up_has_data()) {
        frame64_t f;
        if (noc_read_up(&f) != 0) break;

        uint8_t hdr = frame_header(&f);

        if (hdr == 0x3) {  // control frame
            uint8_t type = frame_ctrl_type(&f);
            if (type == 0x2) {  // complete frame (3型)
                uint32_t thread_id = frame_load(&f);
                if (thread_id >= MAX_THREADS) thread_id = 0; // clamp; thread 0 will time out naturally
                xSemaphoreGiveFromISR(done_sem[thread_id], &higher_woken);
            }
        } else if (hdr == 0x2) {  // work frame (output data)
            // CoreAddr fields are zeroed by NOC; use active_thread to route
            ringbuf_push(&output_ringbuf[active_thread],
                         ((uint64_t)f.high << 32) | f.low);
        }
    }
    portYIELD_FROM_ISR(higher_woken);
}

