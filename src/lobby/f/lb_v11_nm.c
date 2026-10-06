/* lb_v11 - lobby avatar turn 0x005CFE20-0x005D0028: lb_pl_turn_sub (turns a member toward its target heading and leans the work750 angle). */
#include "lobby_f.h"
int Lb_act_ck();
void lb_pl_turn_sub(PLW *pl) {
    int t1;
    u32 t0;
    int a3;
    int a2;
    int v1;
    int a1;
    u16 a0;
    if ((s16)Lb_act_ck(pl, 0, 1) != 0 || (s16)Lb_act_ck(pl, 0, 0x24) != 0) {
        t1 = 0x71C;
    } else {
        if ((s16)Lb_act_ck(pl, 0, 0x1F) != 0) {
            t1 = 0x5B0;
        } else if ((s16)Lb_act_ck(pl, 0, 0x3F) != 0) {
            t1 = 0;
        } else {
            t1 = 0xFA4;
        }
    }
    a2 = pl->ang[1];
    a3 = *(u16 *)&pl->ang_y;
    a0 = pl->char0;
    t0 = (a3 - (a2 & 0xFFFF)) & 0xFFFF;
    switch (a0) {
    case 0x27:
        t1 = 0x71C;
        a1 = 0x80;
        break;
    case 3:
        a1 = 0x80;
        break;
    case 4:
        a1 = 0xA0;
        break;
    default:
        a1 = 0;
        break;
    }
    if ((u32)((t0 + t1) & 0xFFFF) < (u32)(t1 * 2)) {
        pl->ang[1] = a3;
        pl->work2F8 = 0;
        a0 = pl->work750;
        v1 = a0 + 0x300;
        if (v1 < 0x601) {
            pl->work750 = 0;
        } else {
            if (a0 < 0x8000) {
                v1 = a0 - 0x300;
            }
            pl->work750 = v1;
        }
    } else {
        if (t0 < 0x8000U) {
            pl->ang[1] = (a2 + t1) & 0xFFFF;
            v1 = pl->work750 - a1;
        } else {
            pl->ang[1] = (a2 - t1) & 0xFFFF;
            v1 = pl->work750 + a1;
        }
        pl->work750 = v1;
    }
    a0 = pl->work750;
    if (a0 < 0xF601 && a0 >= 0xA00) {
        if (a0 < 0x8000) {
            pl->work750 = 0xA00;
        } else {
            pl->work750 = 0xF600;
        }
    }
}
