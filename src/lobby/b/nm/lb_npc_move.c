#include "lobby_a.h"
typedef struct { u8 pad0000[0xA0]; u16 x00A0; u8 pad00A2[0x2]; u16 x00A4; u8 pad00A6[0x2]; u16 x00A8; u8 pad00AA[0x2]; f32 x00AC; f32 x00B0; f32 x00B4; u8 pad00B8[0xE8]; f32 x01A0; u8 pad01A4[0x4C]; f32 x01F0; u8 pad01F4[0x4C]; f32 x0240; u8 pad0244[0x4C]; f32 x0290; u8 pad0294[0x17A]; s16 x040E; u8 pad0410[0x42]; u8 x0452; u8 pad0453[0x14D]; f32 x05A0; f32 x05A4; f32 x05A8; f32 x05AC; u8 pad05B0[0x380]; f32 x0930; } ARG_lb_npc_move_arg0;

void lb_npc_move(ARG_lb_npc_move_arg0 *arg0) {
    u8 temp_a0;

    arg0->x040E = 0xA;
    Lb_pl_timer_calc();
    Lb_hit_stop_calc((u8 *)arg0);
    arg0->x05A0 = (f32) arg0->x00AC;
    arg0->x05A4 = (f32) arg0->x00B0;
    arg0->x05A8 = (f32) arg0->x00B4;
    Lb_Em_pos_adj((u8 *)arg0);
    Lb_npc_move_sub((u8 *)arg0);
    arg0->x00A0 = arg0->x00A0;
    arg0->x00A4 = arg0->x00A4;
    arg0->x00A8 = arg0->x00A8;
    cpRotMatrixYXZ2((u8 *)arg0 + 0xA0, (u8 *)arg0 + 0x20);
    arg0->x01A0 = (f32) (2.0f * arg0->x0930);
    arg0->x01F0 = (f32) (2.0f * arg0->x0930);
    arg0->x0240 = (f32) (2.0f * arg0->x0930);
    arg0->x0290 = (f32) (2.0f * arg0->x0930);
    lb_npc_chr_sub((u8 *)arg0);
    temp_a0 = arg0->x0452;
    if ((temp_a0 != 0x2F) && (temp_a0 != 0x35) && (temp_a0 != 0x34) && (temp_a0 != 0x2C) && (temp_a0 != 0x14) && (temp_a0 != 0x15) && (temp_a0 != 0x48) && (temp_a0 != 0x4D) && (temp_a0 != 0x4A)) {
        GetGroundHitStatusAreaEm((u8 *)arg0, (u8 *)arg0 + 0xAC, (u8 *)arg0 + 0x70C, (u8 *)arg0 + 0x5AC);
        arg0->x00B0 = (f32) arg0->x05AC;
    }
}
