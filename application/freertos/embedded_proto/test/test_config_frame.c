// test/test_config_frame.c
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "config_meta.h"
#include "noc.h"  // frame64_t

// Local copy of build_cfg_header for x86 testing (mirrors config_task.c).
// Kept here to avoid pulling in FreeRTOS headers.
static frame64_t build_cfg_header(uint8_t frame_type, const config_block_t *b,
                                   uint8_t is_test_req, uint32_t n_frames)
{
    frame64_t f = {0};
    uint32_t n    = n_frames & 0x3FFF;
    uint32_t sram = (uint32_t)(b->sram_addr & 0x1FF);
    f.high = ((uint32_t)frame_type          << 30)
           | ((uint32_t)(b->subtype & 0x3)  << 28)
           | ((uint32_t)(b->core_xy & 0x3F) << 22)
           | ((uint32_t)(b->core_x  & 0x3F) << 16)
           | ((uint32_t)(b->core_y  & 0x3F) << 10);
    f.low  = ((uint32_t)is_test_req << 23)
           | (sram << 14)
           | n;
    return f;
}

static void test_build_cfg_header_config(void) {
    config_block_t b = {
        .subtype    = 2,
        .core_xy    = 0x15,
        .core_x     = 0x0A,
        .core_y     = 0x03,
        .sram_addr  = 0x1FF,
        .data       = NULL,
        .data_words = 0,
    };
    frame64_t f = build_cfg_header(0, &b, 0, 3);

    // frame_type=0 → bits[31:30]=0
    assert(((f.high >> 30) & 0x3) == 0);
    // subtype=2 → bits[29:28]=2
    assert(((f.high >> 28) & 0x3) == 2);
    // core_xy=0x15 → bits[27:22]=0x15
    assert(((f.high >> 22) & 0x3F) == 0x15);
    // core_x=0x0A → bits[21:16]=0x0A
    assert(((f.high >> 16) & 0x3F) == 0x0A);
    // core_y=0x03 → bits[15:10]=0x03
    assert(((f.high >> 10) & 0x3F) == 0x03);
    // copy fields zero → bits[9:0]=0
    assert((f.high & 0x3FF) == 0);
    // is_test_req=0 → bit[23]=0
    assert(((f.low >> 23) & 0x1) == 0);
    // sram_addr=0x1FF → bits[22:14]=0x1FF
    assert(((f.low >> 14) & 0x1FF) == 0x1FF);
    // n_frames=3 → bits[13:0]=3
    assert((f.low & 0x3FFF) == 3);
    printf("test_build_cfg_header_config PASSED\n");
}

static void test_build_cfg_header_test_req(void) {
    config_block_t b = {
        .subtype    = 0,
        .core_xy    = 0x01,
        .core_x     = 0x01,
        .core_y     = 0x01,
        .sram_addr  = 0x00,
        .data       = NULL,
        .data_words = 0,
    };
    frame64_t f = build_cfg_header(1, &b, 1, 100);

    // frame_type=1 → bits[31:30]=1
    assert(((f.high >> 30) & 0x3) == 1);
    // is_test_req=1 → bit[23]=1
    assert(((f.low >> 23) & 0x1) == 1);
    // n_frames=100 → bits[13:0]=100
    assert((f.low & 0x3FFF) == 100);
    // copy fields zero
    assert((f.high & 0x3FF) == 0);
    assert(((f.low >> 24) & 0xFF) == 0);
    printf("test_build_cfg_header_test_req PASSED\n");
}

static void test_build_cfg_header_masks(void) {
    // Verify that out-of-range bits are masked off
    config_block_t b = {
        .subtype    = 0xFF,   // only low 2 bits used
        .core_xy    = 0xFF,   // only low 6 bits used
        .core_x     = 0xFF,   // only low 6 bits used
        .core_y     = 0xFF,   // only low 6 bits used
        .sram_addr  = 0xFFFF, // only low 9 bits used
        .data       = NULL,
        .data_words = 0,
    };
    frame64_t f = build_cfg_header(0, &b, 0, 0xFFFF); // n_frames: only low 14 bits

    assert(((f.high >> 28) & 0x3)   == 0x3);    // subtype masked to 2 bits
    assert(((f.high >> 22) & 0x3F)  == 0x3F);   // core_xy masked to 6 bits
    assert(((f.high >> 16) & 0x3F)  == 0x3F);   // core_x masked to 6 bits
    assert(((f.high >> 10) & 0x3F)  == 0x3F);   // core_y masked to 6 bits
    assert(((f.low  >> 14) & 0x1FF) == 0x1FF);  // sram_addr masked to 9 bits
    assert((f.low & 0x3FFF)         == 0x3FFF); // n_frames masked to 14 bits
    printf("test_build_cfg_header_masks PASSED\n");
}

static void test_config_meta_count(void) {
    assert(CONFIG_BLOCK_COUNT > 0);
    printf("test_config_meta_count PASSED\n");
}

static void test_config_meta_fields(void) {
    const config_block_t *b = &g_config_blocks[0];
    assert(b->subtype <= 3);
    assert(b->data != NULL);
    assert(b->data_words % 2 == 0);
    uint32_t n = b->data_words / 2;
    // subtype 0 (Core) is fixed at exactly 3 frames (192-bit parameter block)
    if (b->subtype == 0) assert(n == 3);
    // subtypes 1/2/3 have variable length; just check non-zero
    if (b->subtype != 0) assert(n > 0);
    printf("test_config_meta_fields PASSED\n");
}

int main(void) {
    test_config_meta_count();
    test_config_meta_fields();
    test_build_cfg_header_config();
    test_build_cfg_header_test_req();
    test_build_cfg_header_masks();
    printf("ALL config_frame tests PASSED\n");
    return 0;
}
