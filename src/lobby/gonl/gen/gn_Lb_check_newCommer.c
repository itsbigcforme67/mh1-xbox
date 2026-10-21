/* G online (PC): Lb_check_newCommer, written from Monster Hunter G's code at lobby 0x566990 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 game_w;
extern u8 lb_sys;
s32 Lb_player_load();                       /* extern */
s32 Lb_send_statusReq();                      /* extern */
s32 flCompact();                              /* extern */
s32 lb_member_changeCheck();                        /* extern */
s32 lb_member_inCheck();                        /* extern */
s32 lb_member_outCheck();                           /* extern */
extern u8 player_work;

void Lb_check_newCommer(void) {
    s32 var_s1;
    u8 *var_s0;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;

    if ((*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) != 0) {
        if (lb_member_outCheck() == 1) {
            temp_a0 = (*(u8 *)((u8 *)&game_w + 0x14));
            if (temp_a0 != 0x4C) {
                if (temp_a0 == 0x4D) {
                    goto block_5;
                }
            } else {
block_5:
                flCompact(temp_a0);
                (*(u8 *)((u8 *)&lb_sys + 0x8F)) = 3;
            }
            return;
        }
        if ((*(u8 *)((u8 *)&lb_sys + 0x8F)) == 0) {
            if (lb_member_changeCheck() == 1) {
                temp_a0_2 = (*(u8 *)((u8 *)&game_w + 0x14));
                if (temp_a0_2 != 0x4C) {
                    if (temp_a0_2 == 0x4D) {
                        goto block_12;
                    }
                } else {
block_12:
                    flCompact(temp_a0_2);
                    (*(u8 *)((u8 *)&lb_sys + 0x8F)) = 3;
                }
                return;
            }
            if ((*(u8 *)((u8 *)&lb_sys + 0x8F)) == 0) {
                lb_member_inCheck();
                temp_a0_3 = (*(u8 *)((u8 *)&game_w + 0x14));
                if (temp_a0_3 != 0x4D) {
                    if (temp_a0_3 == 0x4C) {
                        goto block_18;
                    }
                } else {
block_18:
                    var_s1 = 0;
                    var_s0 = &player_work;
                    do {
                        if ((*var_s0 != 0) && ((*(s8 *)((u8 *)(((*(void **)&cw) + (((s8) (var_s1))))) + 0xB64)) == 0)) {
                            Lb_player_load(var_s0);
                        }
                        var_s1 = ((s8) ((var_s1 + 1)));
                        var_s0 += 0xA00;
                    } while (var_s1 < 0xA);
                }
                if ((*(u8 *)((u8 *)&lb_sys + 0x94)) == 1) {
                    Lb_send_statusReq((*(u8 *)((u8 *)&lb_sys + 0x94)));
                    (*(u8 *)((u8 *)&lb_sys + 0x94)) = 0;
                }
            }
        }
    }
}

