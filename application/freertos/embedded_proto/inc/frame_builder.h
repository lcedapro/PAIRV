// application/freertos/embedded_proto/inc/frame_builder.h
#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include <stdint.h>
#include "noc.h"

// Build a Type-1 work frame (普通数据帧)
// 64-bit layout:
//   [63:62] = 0b10 (work frame)
//   [61]    = 0b0  (type 1)
//   [60]    = TS[7]
//   [59:54] = core_xy (6-bit)
//   [53:48] = core_x  (6-bit)
//   [47:42] = core_y  (6-bit)
//   [41:36] = copy_xy (6-bit)
//   [35:30] = copy_x  (6-bit, spans high[3:0]/low[31:30])
//   [29:24] = copy_y  (6-bit)
//   [23:17] = TS[6:0]
//   [16:8]  = axon_addr (9-bit)
//   [7:0]   = data (8-bit)
// timestep: 8-bit value (0-255); values above 255 are silently truncated
frame64_t frame_build_work(uint16_t timestep, uint8_t core_xy, uint8_t core_x,
                            uint8_t core_y, uint8_t copy_xy, uint8_t copy_x,
                            uint8_t copy_y, uint16_t axon_addr, uint8_t data);

// Build a control frame
// type: 0x00=sync(1型), 0x01=init(2型), 0x02=complete(3型)
// load: sync->timestep count, init->0, complete->thread_id
frame64_t frame_build_control(uint8_t type, uint8_t core_xy, uint8_t core_x,
                               uint8_t core_y, uint8_t copy_xy, uint8_t copy_x,
                               uint8_t copy_y, uint32_t load);

// Parse frame header type
// Returns: 0x2=work frame, 0x3=control frame
static inline uint8_t frame_header(const frame64_t *f)
{
    return (uint8_t)((f->high >> 30) & 0x3);
}

// Parse control frame subtype (valid only if frame_header==0x3)
// Returns: 0x0=sync, 0x1=init, 0x2=complete
static inline uint8_t frame_ctrl_type(const frame64_t *f)
{
    return (uint8_t)((f->high >> 28) & 0x3);
}

// Parse LOAD field [23:0] from low word
static inline uint32_t frame_load(const frame64_t *f)
{
    return f->low & 0xFFFFFF;
}

// Parse axon address [16:8] from low word
static inline uint16_t frame_axon(const frame64_t *f)
{
    return (uint16_t)((f->low >> 8) & 0x1FF);
}

// Parse data byte [7:0] from low word
static inline uint8_t frame_data(const frame64_t *f)
{
    return (uint8_t)(f->low & 0xFF);
}

#endif // FRAME_BUILDER_H
