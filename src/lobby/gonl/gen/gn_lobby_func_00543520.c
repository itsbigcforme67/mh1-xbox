/* G online (PC): lobby_func_00543520, written from Monster Hunter G's code at lobby 0x543520 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck();
s32 ran_suu();
extern u16 D_005E7810;
extern u16 g_lb_5E7812;
extern u16 g_lb_5E7814;

void lobby_func_00543520(u16 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 temp_a1;
    s32 temp_s5;
    s32 temp_s6;
    s32 temp_v1;
    s32 temp_v1_3;
    s32 var_a4;
    s32 var_a6;
    s32 var_s0;
    u8 *temp_a1_2;   /* (u16 *) */
    u8 *temp_a4;   /* (u16 *) */
    u8 *var_a3;   /* (u16 *) */
    u8 *var_a5;   /* (u16 *) */
    u16 temp_a0;
    u16 temp_v1_2;
    u16 var_a0;
    u16 var_a7;

    temp_s6 = arg1 & 0xFFFF;
    temp_v1 = ((s32) (ran_suu(1) & 0xFFFF) % 100) & 0xFFFF;
    if ((temp_s6 != 0xBBF) && (temp_s6 != 0xBBE) && (temp_s6 != 0xBBD) && (temp_s6 != 0xBBC) && (temp_s6 != 0xBBB)) {
        var_a0 = 0;
        var_a7 = 0;
        if (D_005E7810 != 0xFFFF) {
            var_a6 = 0;
            var_a5 = &D_005E7810;
loop_10:
            temp_s5 = var_a6 * 2;
            temp_a4 = &(&D_005E7810)[var_a6];
            if (temp_s6 == (*(u16 *)((u8 *)(temp_a4) + 0))) {
                var_a7 = (*(u16 *)((u8 *)&g_lb_5E7814 + temp_s5));
                var_a0 = (*(u16 *)((u8 *)&g_lb_5E7812 + temp_s5));
                goto block_23;
            }
            if (temp_s6 == (*(u16 *)((u8 *)(temp_a4) + 2))) {
                if (((temp_v1 & 0xFFFF) < 0x33) || (Quest_clear_bit_ck(arg1) != 1)) {
                    var_a7 = (*(u16 *)((u8 *)&g_lb_5E7814 + temp_s5));
                    var_a0 = (*(u16 *)((u8 *)(&(&D_005E7810)[var_a6]) + 0));
                    goto block_23;
                }
            } else if (temp_s6 == (*(u16 *)((u8 *)(temp_a4) + 4))) {
                if (((temp_v1 & 0xFFFF) < 0x1F) || (Quest_clear_bit_ck(arg1) != 1)) {
                    var_a7 = (*(u16 *)((u8 *)&g_lb_5E7812 + temp_s5));
                    var_a0 = (*(u16 *)((u8 *)(&(&D_005E7810)[var_a6]) + 0));
                    goto block_23;
                }
            } else {
                var_a5 += 6;
                var_a6 += 3;
                if ((*(u16 *)var_a5) == 0xFFFF) {
                    goto block_23;
                }
                goto loop_10;
            }
        } else {
block_23:
            temp_a1 = arg4 & 0xFF;
            var_s0 = arg2 & 0xFF;
            var_a4 = 0;
            if (temp_a1 > 0) {
                var_a3 = arg0;
loop_25:
                temp_v1_2 = (*(u16 *)var_a3);
                if (temp_v1_2 != 0) {
                    if (temp_s6 == temp_v1_2) {
                        var_s0 = var_a4;
                    } else if ((var_a0 & 0xFFFF) == temp_v1_2) {
                        var_s0 = var_a4;
                    } else if ((var_a7 & 0xFFFF) == temp_v1_2) {
                        var_s0 = var_a4;
                    } else {
                        goto block_33;
                    }
                } else {
block_33:
                    var_a4 += 1;
                    var_a3 += 2;
                    if (var_a4 < temp_a1) {
                        goto loop_25;
                    }
                }
            }
            if ((var_s0 == 0) && (Quest_clear_bit_ck((s32) *arg0) == 0)) {
                if (*arg0 == 0) {
                    goto block_38;
                }
            } else {
block_38:
                temp_v1_3 = arg2 & 0xFF;
                if (arg3 & 0xFF) {
                    if (var_s0 == temp_v1_3) {
                        goto block_41;
                    }
                    temp_a1_2 = arg0 + (temp_v1_3 * 2);
                    temp_a0 = (*(u16 *)temp_a1_2);
                    (*(u16 *)temp_a1_2) = (u16) arg1;
                    *(arg0 + (var_s0 * 2)) = temp_a0;
                    return;
                }
block_41:
                *(arg0 + (var_s0 * 2)) = (s16) arg1;
            }
        }
    }
}

