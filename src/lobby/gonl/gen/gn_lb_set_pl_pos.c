/* G online (PC): lb_set_pl_pos, written from Monster Hunter G's code at lobby 0x559580 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFAE0;
extern u8 g_main_2EFAE0;
s32 Lb_Pl_adj_calc();                /* extern */
s32 flMemcpy();               /* extern */
s32 lobby_func_00595530();                    /* extern */
extern u8 player_work;

void lb_set_pl_pos(s32 arg0, s32 arg2) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp38 (*(s32 *)(g_frame + 0x38))
    s32 temp_a0;
    u8 *temp_s1;

    temp_s1 = (u8 *)&player_work + ((arg0 & 0xFF) * 0xA00);
    flMemcpy(&sp38, 5);
    temp_a0 = arg2 & 0xFF;
    if ((*(u8 *)((u8 *)(temp_s1) + 0x3E8)) != 0) {
        switch (temp_a0) {                          /* irregular */
        case 0:
            if (lobby_func_00595530((*(u8 *)((u8 *)(temp_s1) + 0x14)), (*(u8 *)((u8 *)(temp_s1) + 0x15))) == 0) {
                (*(f32 *)((u8 *)(temp_s1) + 0x800)) = (f32) (*(s16 *)((u8 *)(&sp38) + 0));
                (*(f32 *)((u8 *)(temp_s1) + 0x808)) = (f32) (*(s16 *)((u8 *)(&sp38) + 2));
                Lb_Pl_adj_calc(temp_s1, (*(s16 *)&g_main_2EFAE0));
                (*(u8 *)((u8 *)(temp_s1) + 0x736)) = (u8) (*(u8 *)((u8 *)(&sp38) + 4));
                if ((*(u8 *)((u8 *)(temp_s1) + 0x736)) != 0x4D) {
                    (*(s8 *)((u8 *)(temp_s1) + 0x8EC)) = 0;
                    (*(s8 *)((u8 *)(temp_s1) + 0x90F)) = 0;
                    return;
                }
            }
            break;
        case 2:
            (*(f32 *)((u8 *)(temp_s1) + 0x800)) = (f32) (*(s16 *)((u8 *)(&sp38) + 0));
            (*(f32 *)((u8 *)(temp_s1) + 0x808)) = (f32) (*(s16 *)((u8 *)(&sp38) + 2));
            (*(f32 *)((u8 *)(temp_s1) + 0xAC)) = (f32) (*(f32 *)((u8 *)(temp_s1) + 0x800));
            (*(f32 *)((u8 *)(temp_s1) + 0xB0)) = (f32) (*(f32 *)((u8 *)(temp_s1) + 0x804));
            (*(f32 *)((u8 *)(temp_s1) + 0xB4)) = (f32) (*(f32 *)((u8 *)(temp_s1) + 0x808));
            (*(f32 *)((u8 *)(temp_s1) + 0x5A0)) = (f32) (*(f32 *)((u8 *)(temp_s1) + 0xAC));
            (*(f32 *)((u8 *)(temp_s1) + 0x5A4)) = (f32) (*(f32 *)((u8 *)(temp_s1) + 0xB0));
            (*(f32 *)((u8 *)(temp_s1) + 0x5A8)) = (f32) (*(f32 *)((u8 *)(temp_s1) + 0xB4));
            break;
        }
    }
}

