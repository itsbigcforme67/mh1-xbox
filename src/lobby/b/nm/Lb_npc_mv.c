#include "lobby_a.h"
typedef struct { u8 pad0000[0x1]; u8 x0001; u8 pad0002[0x2]; u8 x0004; u8 pad0005[0xA7]; f32 x00AC; f32 x00B0; f32 x00B4; u8 pad00B8[0x4AC]; int x0564; u8 pad0568[0x1CE]; u8 x0736; } ARG_Lb_npc_mv_arg0;

s32 Lb_npc_mv(ARG_Lb_npc_mv_arg0 *arg0) {
    u8 temp_v1;

    temp_v1 = arg0->x0004;
    switch (temp_v1) {                              /* irregular */
    case 0:
        lb_npc_init();
        lb_npc_effect_move((u8 *)arg0);
        Lb_World_calc((u8 *)arg0);
        return 0;
    case 1:
        lb_npc_move();
    default:
block_9:
        lb_npc_effect_move((u8 *)arg0);
        if ((arg0->x0001 != 0) && (arg0->x0736 == game_w.stage)) {
            F(f32, arg0->x0564, 8) = (f32) arg0->x00AC;
            F(f32, arg0->x0564, 0xC) = (f32) arg0->x00B0;
            F(f32, arg0->x0564, 0x10) = (f32) arg0->x00B4;
            add_prim(&ot1, arg0->x0564, 0x20, 0);
        }
        Lb_World_calc((u8 *)arg0);
        return 0;
    case 2:
        lb_npc_die();
        goto block_9;
    case 3:
        lb_npc_erase();
        return 1;
    }
}
