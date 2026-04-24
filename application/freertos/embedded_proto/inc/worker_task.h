// application/freertos/embedded_proto/inc/worker_task.h
#ifndef WORKER_TASK_H
#define WORKER_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "paicore_config.h"
#include "neuron_map.h"
#include "ringbuf.h"

// Message types for input_queue
typedef enum {
    MSG_INIT_REQ  = 0,
    MSG_INFER_REQ = 1,
} worker_msg_type_t;

// Message passed from dispatch_task to worker_task
typedef struct {
    worker_msg_type_t type;
    uint32_t          thread_id;
    uint32_t          timesteps;    // for INFER_REQ
    uint8_t           data[PROTO_BUF_SIZE]; // input tensor bytes
    uint16_t          data_len;
} worker_msg_t;

// Message passed from worker_task to uart_tx_task
typedef struct {
    uint8_t  is_init_resp;  // 1=InitResponse, 0=InferResponse
    uint32_t thread_id;
    uint8_t  success;
    uint8_t  data[PROTO_BUF_SIZE]; // output tensor bytes (InferResponse only)
    uint16_t data_len;
    uint32_t cycles;               // inference cycles (InferResponse only)
} tx_msg_t;

// Message types for config_queue
typedef enum {
    MSG_CONFIG_REQ = 0,
    MSG_TEST_REQ   = 1,
} config_msg_type_t;

// Message passed from uart_rx_task to config_task
typedef struct {
    config_msg_type_t type;
    uint32_t          block_index;
} config_msg_t;

// Response passed from config_task to uart_tx_task
typedef struct {
    config_msg_type_t type;
    uint32_t          block_index;
    uint8_t           success;
    uint32_t          mismatch_frame;
} config_resp_t;

// Tagged union for tx_queue — carries either infer/init response or config/test response
typedef enum {
    TX_INFER_RESP  = 0,
    TX_CONFIG_RESP = 1,
    TX_TEST_RESP   = 2,
} tx_envelope_type_t;

typedef struct {
    tx_envelope_type_t env_type;
    union {
        tx_msg_t      infer;
        config_resp_t config;
    } u;
} tx_envelope_t;

// Global FreeRTOS synchronization objects
extern QueueHandle_t     dispatch_queue;
extern QueueHandle_t     input_queue[MAX_THREADS];
extern SemaphoreHandle_t done_sem[MAX_THREADS];
extern QueueHandle_t     tx_queue;
extern QueueHandle_t     config_queue;
extern SemaphoreHandle_t noc_mutex;
extern SemaphoreHandle_t accel_mutex;

// Output ring buffers (one per thread, written by ISR, read by worker)
extern ringbuf_t output_ringbuf[MAX_THREADS];

// Active thread index — set by worker_task before sending sync frame,
// read by SNN ISR to route incoming work frames.
// Protected by accel_mutex — serializes the full hardware round-trip.
extern volatile uint32_t active_thread;

// Task entry points
#ifdef __cplusplus
extern "C" {
#endif
void worker_task(void *param);
void uart_rx_task(void *param);
void uart_tx_task(void *param);
void dispatch_task(void *param);
void config_task(void *param);
#ifdef __cplusplus
}
#endif

#endif // WORKER_TASK_H
