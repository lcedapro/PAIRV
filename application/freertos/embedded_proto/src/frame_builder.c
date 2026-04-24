// application/freertos/embedded_proto/src/frame_builder.c
#include "frame_builder.h"

frame64_t frame_build_work(uint16_t timestep, uint8_t core_xy, uint8_t core_x,
                            uint8_t core_y, uint8_t copy_xy, uint8_t copy_x,
                            uint8_t copy_y, uint16_t axon_addr, uint8_t data)
{
    frame64_t f;
    f.high  = (0x2u << 30);                               // [63:62]=0b10, [61]=0b0
    f.high |= ((uint32_t)((timestep >> 7) & 0x1) << 28);  // [60]=TS[7]
    f.high |= ((uint32_t)(core_xy  & 0x3F) << 22);        // [59:54]
    f.high |= ((uint32_t)(core_x   & 0x3F) << 16);        // [53:48]
    f.high |= ((uint32_t)(core_y   & 0x3F) << 10);        // [47:42]
    f.high |= ((uint32_t)(copy_xy  & 0x3F) << 4);         // [41:36]
    f.high |= ((uint32_t)(copy_x   >> 2)   & 0xF);        // [35:32] = copy_x[5:2]
    f.low   = ((uint32_t)(copy_x   & 0x3)  << 30);        // [31:30] = copy_x[1:0]
    f.low  |= ((uint32_t)(copy_y   & 0x3F) << 24);        // [29:24]
    f.low  |= ((uint32_t)(timestep & 0x7F) << 17);        // [23:17]=TS[6:0]
    f.low  |= ((uint32_t)(axon_addr & 0x1FF) << 8);       // [16:8]
    f.low  |= ((uint32_t)(data     & 0xFF));               // [7:0]
    return f;
}

frame64_t frame_build_control(uint8_t type, uint8_t core_xy, uint8_t core_x,
                               uint8_t core_y, uint8_t copy_xy, uint8_t copy_x,
                               uint8_t copy_y, uint32_t load)
{
    frame64_t f;
    f.high  = (0x3u << 30);                               // [63:62]=0b11
    f.high |= ((uint32_t)(type     & 0x3)  << 28);        // [61:60]=type
    f.high |= ((uint32_t)(core_xy  & 0x3F) << 22);        // [59:54]
    f.high |= ((uint32_t)(core_x   & 0x3F) << 16);        // [53:48]
    f.high |= ((uint32_t)(core_y   & 0x3F) << 10);        // [47:42]
    f.high |= ((uint32_t)(copy_xy  & 0x3F) << 4);         // [41:36]
    f.high |= ((uint32_t)(copy_x   >> 2)   & 0xF);        // [35:32]
    f.low   = ((uint32_t)(copy_x   & 0x3)  << 30);        // [31:30]
    f.low  |= ((uint32_t)(copy_y   & 0x3F) << 24);        // [29:24]
    f.low  |= (load & 0xFFFFFF);                           // [23:0]
    return f;
}
