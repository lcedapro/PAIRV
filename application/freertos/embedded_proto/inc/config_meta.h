// inc/config_meta.h
#ifndef CONFIG_META_H
#define CONFIG_META_H

#include <stdint.h>

typedef struct {
    uint8_t         subtype;    // 0=Core(1型) 1=LUT(2型) 2=Neuron(3型) 3=Input(4型)
    uint8_t         core_xy;    // [5:0] Core_XY
    uint8_t         core_x;    // [5:0] Core_X
    uint8_t         core_y;    // [5:0] Core_Y
    uint16_t        sram_addr; // raw value for header [22:14]
    const uint32_t *data;      // pointer to 32-bit word pairs in flash
    uint32_t        data_words; // number of 32-bit words (must be even; frames = data_words/2)
} config_block_t;

extern const config_block_t g_config_blocks[1];
#define CONFIG_BLOCK_COUNT ((uint32_t)(sizeof(g_config_blocks)/sizeof(g_config_blocks[0])))

#endif // CONFIG_META_H
