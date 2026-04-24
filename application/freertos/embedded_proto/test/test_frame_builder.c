// application/freertos/embedded_proto/test/test_frame_builder.c
#include <stdio.h>
#include <assert.h>
#include "frame_builder.h"

static void test_work_frame_header(void) {
    frame64_t f = frame_build_work(0, 0, 0, 0, 0, 0, 0, 0, 0);
    assert(frame_header(&f) == 0x2);
    // type-1 bit [61] must be 0
    assert(((f.high >> 29) & 0x1) == 0);
    printf("test_work_frame_header PASSED\n");
}

static void test_work_frame_fields(void) {
    // timestep=0x1FF (9-bit: TS[7]=1, TS[6:0]=0x7F)
    // core_xy=0x3F, core_x=0x3F, core_y=0x3F
    // copy_xy=0x3F, copy_x=0x3F, copy_y=0x3F
    // axon_addr=0x1FF, data=0xFF
    frame64_t f = frame_build_work(0x1FF, 0x3F, 0x3F, 0x3F,
                                    0x3F, 0x3F, 0x3F, 0x1FF, 0xFF);
    // Check TS[7] at bit 60 of 64-bit = bit 28 of high
    assert(((f.high >> 28) & 0x1) == 1);
    // Check core_xy at [59:54] = bits [27:22] of high
    assert(((f.high >> 22) & 0x3F) == 0x3F);
    // Check core_x at [53:48] = bits [21:16] of high
    assert(((f.high >> 16) & 0x3F) == 0x3F);
    // Check core_y at [47:42] = bits [15:10] of high
    assert(((f.high >> 10) & 0x3F) == 0x3F);
    // Check copy_xy at [41:36] = bits [9:4] of high
    assert(((f.high >> 4) & 0x3F) == 0x3F);
    // Check copy_x: [35:32]=high[3:0], [31:30]=low[31:30]
    assert((f.high & 0xF) == 0xF);
    assert(((f.low >> 30) & 0x3) == 0x3);
    // Check copy_y at [29:24] = low[29:24]
    assert(((f.low >> 24) & 0x3F) == 0x3F);
    // Check TS[6:0] at [23:17] = low[23:17]
    assert(((f.low >> 17) & 0x7F) == 0x7F);
    // Check axon_addr at [16:8] = low[16:8]
    assert(((f.low >> 8) & 0x1FF) == 0x1FF);
    // Check data at [7:0]
    assert((f.low & 0xFF) == 0xFF);
    printf("test_work_frame_fields PASSED\n");
}

static void test_work_frame_zero(void) {
    frame64_t f = frame_build_work(0, 0, 0, 0, 0, 0, 0, 0, 0);
    // Only header bits should be set
    assert(f.high == (0x2u << 30));
    assert(f.low == 0);
    printf("test_work_frame_zero PASSED\n");
}

static void test_control_sync(void) {
    frame64_t f = frame_build_control(0x00, 0, 0, 0, 0, 0, 0, 100);
    assert(frame_header(&f) == 0x3);
    assert(frame_ctrl_type(&f) == 0x0);
    assert(frame_load(&f) == 100);
    printf("test_control_sync PASSED\n");
}

static void test_control_init(void) {
    frame64_t f = frame_build_control(0x01, 0, 0, 0, 0, 0, 0, 0);
    assert(frame_header(&f) == 0x3);
    assert(frame_ctrl_type(&f) == 0x1);
    printf("test_control_init PASSED\n");
}

static void test_control_complete(void) {
    frame64_t f = frame_build_control(0x02, 0, 0, 0, 0, 0, 0, 1);
    assert(frame_header(&f) == 0x3);
    assert(frame_ctrl_type(&f) == 0x2);
    assert(frame_load(&f) == 1);  // thread_id=1
    printf("test_control_complete PASSED\n");
}

static void test_parse_helpers(void) {
    frame64_t f = frame_build_work(5, 1, 2, 3, 0, 0, 0, 42, 0xAB);
    assert(frame_axon(&f) == 42);
    assert(frame_data(&f) == 0xAB);
    printf("test_parse_helpers PASSED\n");
}

int main(void) {
    test_work_frame_header();
    test_work_frame_fields();
    test_work_frame_zero();
    test_control_sync();
    test_control_init();
    test_control_complete();
    test_parse_helpers();
    printf("ALL frame_builder tests PASSED\n");
    return 0;
}
