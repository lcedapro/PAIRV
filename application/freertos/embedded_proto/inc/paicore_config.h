// application/freertos/embedded_proto/inc/paicore_config.h
#ifndef PAICORE_CONFIG_H
#define PAICORE_CONFIG_H

#include <stdint.h>

// ── Hardware addresses ──
#define FIFO_DOWN_ADDR          0x10008000
#define FIFO_UP_ADDR            0x10010000
#define REG_SOC_IRQ0            0x10000000
#define CLIC_CLILINTIE_BASE     0x18020000
#define SNN_CLILINTIE_OFF       0x10bd
#define SPI_BASE_ADDR           0x10014000

// ── Thread configuration ──
#define MAX_THREADS             2
#define INFER_TIMEOUT_MS        5000
#define INIT_TIMEOUT_MS         2000

// ── Buffer sizes ──
#define UART_RX_BUF_SIZE        2048
#define PROTO_BUF_SIZE          2048
#define OUTPUT_RINGBUF_SIZE     512   // number of 64-bit frames per worker

// ── UART framing ──
#define UART_FRAME_MAGIC        0x50414943  // 'P','A','I','C'
#define UART_FRAME_MAGIC_B0     0x50
#define UART_FRAME_MAGIC_B1     0x41
#define UART_FRAME_MAGIC_B2     0x49
#define UART_FRAME_MAGIC_B3     0x43

// ── SNN interrupt ──
// evalsoc.h defines SNN_IRQn as SOC_INT47_IRQn; do not redefine here.

// ── Neuron mapping (placeholder dimensions, set per-network) ──
#define MAX_C                   64
#define MAX_H                   8
#define MAX_W                   8
#define MAX_NEURONS             (MAX_C * MAX_H * MAX_W)
#define MAX_AXON_ADDR           512   // 9-bit axon address space (default LCN 8:9)

#endif // PAICORE_CONFIG_H
