/* G online (PC): lb_set_pl_stage, written from Monster Hunter G's code at lobby 0x5596C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 game_w;
s32 Lb_Pl_basic_flagset(); /* extern */
s32 Lb_act_set();       /* extern */
s32 Lb_pl_chr_set(); /* extern */
s32 Lb_put_chat();               /* extern */
extern u8 lbCommer;
extern u8 player_work;

void lb_set_pl_stage(s32 arg0, u8 *arg1) {
    s32 temp_a1_2;
    s32 temp_s0;
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_s1;
    u8 *temp_s2;

    temp_s0 = arg0 & 0xFF;
    temp_s2 = (u8 *)&player_work + (temp_s0 * 0xA00);
    temp_a0 = (*(u8 *)((u8 *)(temp_s2) + 0x736));
    temp_s1 = (u8 *)&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00);
    if (*arg1 != temp_a0) {
        if (temp_a0 == 0) {
            (*(s8 *)((u8 *)(temp_s2) + 0x3E8)) = 1;
        }
        (*(s32 *)((u8 *)(temp_s2) + 0xA4)) = (s32) (*(u16 *)((u8 *)(temp_s2) + 0xE));
        (*(u8 *)((u8 *)(temp_s2) + 0x736)) = (u8) *arg1;
        if ((*(u8 *)((u8 *)(temp_s2) + 0x736)) != 0x4D) {
            (*(s8 *)((u8 *)(temp_s2) + 0x8EC)) = 0;
            Lb_Pl_basic_flagset(temp_s2, 0, 0, 0);
            Lb_pl_chr_set(temp_s2, 1, 0, 0);
            Lb_act_set(temp_s2, 0, 0);
        }
        (*(s8 *)((u8 *)(temp_s2) + 0x8EC)) = 0;
        (*(s8 *)((u8 *)(temp_s2) + 0x90F)) = 0;
        temp_a1 = (*(void **)((u8 *)(temp_s1) + 0x3B0));
        if ((temp_a1 != NULL) && ((*(u16 *)((u8 *)(temp_a1) + 0xC)) == (arg0 & 0xFF)) && ((*(u8 *)((u8 *)(temp_a1) + 0x1E)) == 0)) {
            (*(void **)((u8 *)(temp_s1) + 0x3B0)) = NULL;
        }
        if ((*arg1 == 0) && ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0)) {
            temp_a1_2 = temp_s0 * 0x5C;
            if ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&lbCommer + 0x8)) + temp_a1_2)) != 0) {
                Lb_put_chat(7, (u8 *)&lbCommer + temp_a1_2 + 8);
            }
        }
    }
}

