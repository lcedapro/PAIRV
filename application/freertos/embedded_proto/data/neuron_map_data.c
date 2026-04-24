// application/freertos/embedded_proto/data/neuron_map_data.c
// Placeholder mapping tables -- replace with tool-generated data before hardware use.
// Stored in .large_const_data section (maps to external FLASH).
#include "neuron_map.h"

// Placeholder: 2x2x2 network (C=2, H=2, W=2 -> 8 neurons)
// Each neuron maps to a unique axon address
static const input_neuron_mapping_t input_neuron_map_table[MAX_NEURONS]
    __attribute__((section(".large_const_data"))) = {
    // c=0,h=0,w=0  c=0,h=0,w=1  c=0,h=1,w=0  c=0,h=1,w=1
    {0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 2},  {0, 0, 0, 0, 0, 0, 3},
    // c=1,h=0,w=0  c=1,h=0,w=1  c=1,h=1,w=0  c=1,h=1,w=1
    {0, 0, 0, 0, 0, 0, 4},  {0, 0, 0, 0, 0, 0, 5},
    {0, 0, 0, 0, 0, 0, 6},  {0, 0, 0, 0, 0, 0, 7},
    // remaining entries are zero-initialized
};

// Output map: 2D array [thread][axon_addr] -> [c,h,w]
const output_neuron_mapping_t output_neuron_map_table[MAX_THREADS][MAX_AXON_ADDR]
    __attribute__((section(".large_const_data"))) = {
    // thread 0
    {
        // axon 0->(0,0,0), 1->(0,0,1), 2->(0,1,0), 3->(0,1,1)
        {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}, {0, 1, 1, 1},
        // axon 4->(1,0,0), 5->(1,0,1), 6->(1,1,0), 7->(1,1,1)
        {1, 0, 0, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 1},
        // remaining entries: valid=0
    },
    // thread 1
    {
        {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 1, 0, 1}, {0, 1, 1, 1},
        {1, 0, 0, 1}, {1, 0, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 1},
        // remaining entries: valid=0
    },
};

void neuron_map_init(void)
{
    g_map_ctx[0].input_map  = input_neuron_map_table;
    g_map_ctx[0].output_map = output_neuron_map_table[0];
    g_map_ctx[0].C = 2; g_map_ctx[0].H = 2; g_map_ctx[0].W = 2;
    g_map_ctx[0].max_axon = MAX_AXON_ADDR;

    g_map_ctx[1].input_map  = input_neuron_map_table;
    g_map_ctx[1].output_map = output_neuron_map_table[1];
    g_map_ctx[1].C = 2; g_map_ctx[1].H = 2; g_map_ctx[1].W = 2;
    g_map_ctx[1].max_axon = MAX_AXON_ADDR;
}

