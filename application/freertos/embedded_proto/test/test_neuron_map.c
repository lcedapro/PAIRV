// application/freertos/embedded_proto/test/test_neuron_map.c
#include <stdio.h>
#include <assert.h>
#include "neuron_map.h"

// Minimal test context (no FLASH section needed for x86)
static const input_neuron_mapping_t test_input_map[8] = {
    {0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 2},  {0, 0, 0, 0, 0, 0, 3},
    {0, 0, 0, 0, 0, 0, 4},  {0, 0, 0, 0, 0, 0, 5},
    {0, 0, 0, 0, 0, 0, 6},  {0, 0, 0, 0, 0, 0, 7},
};

static const output_neuron_mapping_t test_output_map[16] = {
    {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}, {0, 1, 1, 1},
    {1, 0, 0, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 1},
    // axon 8-15: invalid (zero-initialized, valid=0)
};

static neuron_map_ctx_t test_ctx = {
    .input_map  = test_input_map,
    .output_map = test_output_map,
    .C = 2, .H = 2, .W = 2,
    .max_axon = 16,
};

static void test_lookup(void) {
    const input_neuron_mapping_t *m;

    // c=0,h=0,w=0 -> axon 0
    m = neuron_map_lookup(&test_ctx, 0, 0, 0);
    assert(m != NULL);
    assert(m->axon_addr == 0);

    // c=1,h=1,w=1 -> axon 7
    m = neuron_map_lookup(&test_ctx, 1, 1, 1);
    assert(m != NULL);
    assert(m->axon_addr == 7);

    // out of bounds
    assert(neuron_map_lookup(&test_ctx, 2, 0, 0) == NULL);
    assert(neuron_map_lookup(&test_ctx, 0, 2, 0) == NULL);

    printf("test_lookup PASSED\n");
}

static void test_output_lookup(void) {
    uint32_t c, h, w;

    // axon 0 -> (0,0,0)
    assert(output_map_lookup(&test_ctx, 0, &c, &h, &w) == 0);
    assert(c == 0 && h == 0 && w == 0);

    // axon 7 -> (1,1,1)
    assert(output_map_lookup(&test_ctx, 7, &c, &h, &w) == 0);
    assert(c == 1 && h == 1 && w == 1);

    // axon 8 -> invalid (valid=0)
    assert(output_map_lookup(&test_ctx, 8, &c, &h, &w) == -1);

    // axon >= max_axon -> out of bounds
    assert(output_map_lookup(&test_ctx, 16, &c, &h, &w) == -1);

    printf("test_output_lookup PASSED\n");
}

int main(void) {
    test_lookup();
    test_output_lookup();
    printf("ALL neuron_map tests PASSED\n");
    return 0;
}
