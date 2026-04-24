// application/freertos/embedded_proto/src/main.c
#include <stdio.h>
#include "nuclei_sdk_soc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "paicore_config.h"
#include "worker_task.h"
#include "neuron_map.h"
#include "noc.h"

// ── FreeRTOS objects ──────────────────────────────────────────────────────────
QueueHandle_t     dispatch_queue;
QueueHandle_t     input_queue[MAX_THREADS];
SemaphoreHandle_t done_sem[MAX_THREADS];
QueueHandle_t     tx_queue;
QueueHandle_t     config_queue;
SemaphoreHandle_t noc_mutex;
SemaphoreHandle_t accel_mutex;

// ── Output ring buffers ───────────────────────────────────────────────────────
ringbuf_t  output_ringbuf[MAX_THREADS];
static uint64_t _ringbuf_storage[MAX_THREADS][OUTPUT_RINGBUF_SIZE];

// ── FreeRTOS hooks ────────────────────────────────────────────────────────────
void vApplicationMallocFailedHook(void) { while (1); }
void vApplicationStackOverflowHook(TaskHandle_t t, char *n) { (void)t; (void)n; while (1); }
void vApplicationIdleHook(void) {}

// Forward declaration for ISR registration
void SNN_IRQHandler(void);

int main(void)
{
    printf("[PAICORE] PaiCore RISC-V Firmware\n");
    printf("[PAICORE] CPU: %lu Hz\n", (unsigned long)SystemCoreClock);

    // Initialize neuron mapping tables
    neuron_map_init();

    // Initialize ring buffers
    for (int i = 0; i < MAX_THREADS; i++) {
        ringbuf_init(&output_ringbuf[i], _ringbuf_storage[i], OUTPUT_RINGBUF_SIZE);
    }

    // Create FreeRTOS synchronization objects
    dispatch_queue = xQueueCreate(4, sizeof(worker_msg_t));
    tx_queue       = xQueueCreate(4, sizeof(tx_envelope_t));
    config_queue   = xQueueCreate(2, sizeof(config_msg_t));
    noc_mutex      = xSemaphoreCreateMutex();
    accel_mutex    = xSemaphoreCreateMutex();

    for (int i = 0; i < MAX_THREADS; i++) {
        input_queue[i] = xQueueCreate(2, sizeof(worker_msg_t));
        done_sem[i]    = xSemaphoreCreateBinary();
    }

    // Register SNN interrupt with CLIC
    noc_init();
    ECLIC_Register_IRQ(SNN_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                       ECLIC_LEVEL_TRIGGER, 1, 0, SNN_IRQHandler);

    // Create tasks
    xTaskCreate(uart_rx_task,  "uart_rx",  768, NULL, 3, NULL);
    xTaskCreate(uart_tx_task,  "uart_tx",  768, NULL, 3, NULL);
    xTaskCreate(dispatch_task, "dispatch", 768, NULL, 2, NULL);
    xTaskCreate(config_task,   "config",   768, NULL, 3, NULL);
    for (int i = 0; i < MAX_THREADS; i++) {
        xTaskCreate(worker_task, "worker", 1536, (void *)(uintptr_t)i, 1, NULL);
    }

    __enable_irq();
    vTaskStartScheduler();
    while (1);
    return 0;
}
