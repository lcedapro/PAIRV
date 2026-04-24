// application/freertos/embedded_proto/inc/neuron_map.h
#ifndef NEURON_MAP_H
#define NEURON_MAP_H

#include <stdint.h>
#include "paicore_config.h"

// Input mapping: [C,H,W] -> [CoreAddr, AxonAddr]
// Indexed by [c*H*W + h*W + w]
typedef struct {
    uint8_t  core_xy;
    uint8_t  core_x;
    uint8_t  core_y;
    uint8_t  copy_xy;
    uint8_t  copy_x;
    uint8_t  copy_y;
    uint16_t axon_addr;
} input_neuron_mapping_t;

// Output mapping: [AxonAddr] -> [C,H,W]
// Indexed by axon_addr, one table per thread
typedef struct {
    uint32_t c, h, w;
    uint8_t  valid;
} output_neuron_mapping_t;

// Context holding both tables and dimensions
typedef struct {
    const input_neuron_mapping_t  *input_map;   // [MAX_NEURONS]
    const output_neuron_mapping_t *output_map;  // [MAX_AXON_ADDR]
    uint32_t C, H, W;
    uint32_t max_axon;
} neuron_map_ctx_t;

// Look up input mapping for neuron at [c,h,w]
// Returns pointer to mapping entry, or NULL if out of bounds
const input_neuron_mapping_t *neuron_map_lookup(const neuron_map_ctx_t *ctx,
                                                 uint32_t c, uint32_t h, uint32_t w);

// Look up output mapping for axon_addr
// Fills out_c, out_h, out_w; returns 0 on success, -1 if invalid
int output_map_lookup(const neuron_map_ctx_t *ctx, uint16_t axon_addr,
                      uint32_t *out_c, uint32_t *out_h, uint32_t *out_w);

// Global map contexts, one per thread
extern neuron_map_ctx_t g_map_ctx[MAX_THREADS];

// 2D output neuron map table: [thread][axon_addr]
extern const output_neuron_mapping_t output_neuron_map_table[MAX_THREADS][MAX_AXON_ADDR];

// Initialize g_map_ctx from the data tables in neuron_map_data.c
void neuron_map_init(void);

#endif // NEURON_MAP_H
