/* G online (PC): lb_set_pl_status, written from Monster Hunter G's code at lobby 0x559390 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 lb_sys;
s32 Lb_Pl_act_set2(); /* extern */
s32 Lb_Pl_adj_calc();            /* extern */
s32 Lb_act_set();            /* extern */
s32 flMemcpy();               /* extern */
s32 lobby_func_00595530();                    /* extern */
extern u8 player_work;

void lb_set_pl_status(s32 arg0) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp30 (*(s32 *)(g_frame + 0x30))
    s16 temp_v1;
    u8 temp_a3;
    u8 temp_v1_2;
    u8 *temp_s1;

    temp_s1 = (u8 *)&player_work + ((arg0 & 0xFF) * 0xA00);
    flMemcpy(&sp30, 0xA);
    if (((*(u8 *)((u8 *)(&sp30) + 8)) == 0) || ((*(u8 *)((u8 *)(temp_s1) + 0x736)) == 0x4D)) {
        (*(s8 *)((u8 *)(temp_s1) + 0x3E8)) = 1;
        temp_v1 = ((*(u8 *)((u8 *)(&sp30) + 4)) << 8) & 0xFFFF;
        if (((*(f32 *)((u8 *)(temp_s1) + 0xAC)) == 0.0f) && ((*(f32 *)((u8 *)(temp_s1) + 0xB0)) == 0.0f)) {
            (*(s32 *)((u8 *)(temp_s1) + 0xA4)) = (s32) (temp_v1 & 0xFFFF);
            (*(s16 *)((u8 *)(temp_s1) + 0xE)) = temp_v1;
        } else {
            (*(s16 *)((u8 *)(temp_s1) + 0xE)) = temp_v1;
        }
        (*(f32 *)((u8 *)(temp_s1) + 0x800)) = (f32) (*(s16 *)((u8 *)(&sp30) + 0));
        (*(f32 *)((u8 *)(temp_s1) + 0x808)) = (f32) (*(s16 *)((u8 *)(&sp30) + 2));
        (*(u8 *)((u8 *)(temp_s1) + 0x8EC)) = (u8) (*(u8 *)((u8 *)(&sp30) + 8));
        Lb_Pl_adj_calc(temp_s1, 0xA);
        if (lobby_func_00595530((*(u8 *)((u8 *)(&sp30) + 6)), (*(u8 *)((u8 *)(&sp30) + 7))) == 1) {
            temp_v1_2 = (*(u8 *)((u8 *)(&sp30) + 5));
            if (((s32) temp_v1_2 > 0) && ((s32) temp_v1_2 < 0xB)) {
                (*(u16 *)((u8 *)&lb_sys + 0x14)) |= 1 << temp_v1_2;
            }
        } else if (((*(u8 *)((u8 *)(&sp30) + 6)) == 0) && ((*(u8 *)((u8 *)(&sp30) + 7)) == 0x4D)) {
            (*(u16 *)((u8 *)&lb_sys + 0x14)) &= ~(1 << (*(u8 *)((u8 *)(&sp30) + 5)));
        }
        temp_a3 = (*(u8 *)((u8 *)(&sp30) + 7));
        if ((temp_a3 == 0x4C) && ((*(u8 *)((u8 *)(temp_s1) + 0x14)) == 0) && (temp_a3 != (*(u8 *)((u8 *)(temp_s1) + 0x15)))) {
            Lb_Pl_act_set2(temp_s1, 0, 0x4C, 0);
            return;
        }
        if ((*(u8 *)((u8 *)(temp_s1) + 0x15)) == temp_a3) {
            if ((*(u8 *)((u8 *)(temp_s1) + 0x14)) != (*(u8 *)((u8 *)(&sp30) + 6))) {
                goto block_22;
            }
        } else {
block_22:
            Lb_act_set(temp_s1, (*(u8 *)((u8 *)(&sp30) + 6)), temp_a3 & 0xFF, temp_a3);
        }
    }
}

