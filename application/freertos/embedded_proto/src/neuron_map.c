// application/freertos/embedded_proto/src/neuron_map.c
#include <stddef.h>
#include "neuron_map.h"

neuron_map_ctx_t g_map_ctx[MAX_THREADS];

const input_neuron_mapping_t *neuron_map_lookup(const neuron_map_ctx_t *ctx,
                                                 uint32_t c, uint32_t h, uint32_t w)
{
    if (c >= ctx->C || h >= ctx->H || w >= ctx->W) return NULL;
    uint32_t idx = c * ctx->H * ctx->W + h * ctx->W + w;
    return &ctx->input_map[idx];
}

int output_map_lookup(const neuron_map_ctx_t *ctx, uint16_t axon_addr,
                      uint32_t *out_c, uint32_t *out_h, uint32_t *out_w)
{
    if (axon_addr >= ctx->max_axon) return -1;
    const output_neuron_mapping_t *m = &ctx->output_map[axon_addr];
    if (!m->valid) return -1;
    *out_c = m->c;
    *out_h = m->h;
    *out_w = m->w;
    return 0;
}
