// application/freertos/embedded_proto/src/dispatch_task.c
#include "FreeRTOS.h"
#include "queue.h"
#include "worker_task.h"
#include "paicore_config.h"

void dispatch_task(void *param)
{
    (void)param;
    worker_msg_t msg;

    for (;;) {
        if (xQueueReceive(dispatch_queue, &msg, portMAX_DELAY) != pdTRUE)
            continue;

        if (msg.thread_id < MAX_THREADS)
            xQueueSend(input_queue[msg.thread_id], &msg, portMAX_DELAY);
        // silently drop if thread_id out of range
    }
}
