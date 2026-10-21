/* G online (PC): get_flag_quest, written from Monster Hunter G's code at lobby 0x55E9C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Online_ck();                                    /* extern */
s32 Quest_clear_bit_ck();                        /* extern */
s32 lobby_func_005438F0();                       /* extern */
s32 ran_suu();                               /* extern */
extern u8 flag_quest_tbl;
extern u8 flag_quest_tbl_local;

u16 get_flag_quest(s32 arg0) {
    s32 temp_hi;
    s32 temp_s0;
    s32 temp_v1;
    s32 var_s1;
    s32 var_s2;
    s32 var_v1;
    u8 *var_v0;   /* (u16 **) */
    u8 *temp_s3;   /* (u16 *) */
    u8 *temp_s4;   /* (u16 *) */
    u8 *var_a0;   /* (u16 *) */

    if (arg0 == -1) {
        return 0U;
    }
    if (Online_ck() == 1) {
        var_v0 = (u8 *)&flag_quest_tbl + (arg0 * 4);
    } else {
        var_v0 = (u8 *)&flag_quest_tbl_local + (arg0 * 4);
    }
    temp_s3 = (*(u16 **)var_v0);
    var_v1 = 0;
    if ((*(u16 *)temp_s3) != 0) {
        var_a0 = temp_s3;
        do {
            var_a0 += 2;
            var_v1 = (var_v1 + 1) & 0xFFFF;
        } while ((*(u16 *)var_a0) != 0);
    }
    temp_s0 = var_v1 & 0xFFFF;
    temp_hi = (s32) (ran_suu(1) & 0xFFFF) % temp_s0;
    if (temp_s0 == 0) {
        M2C_BREAK(0);
    }
    var_s2 = 0;
    var_s1 = temp_hi & 0xFFFF;
    if (temp_s0 > 0) {
loop_13:
        temp_v1 = var_s1 & 0xFFFF;
        if ((temp_v1 < 0xBBC) || (temp_v1 >= 0xBC0)) {
            temp_s4 = temp_s3 + ((var_s1 & 0xFFFF) * 2);
            if ((Quest_clear_bit_ck((*(u16 *)temp_s4)) == 0) && (lobby_func_005438F0((*(u16 *)temp_s4)) != 1)) {
                return (*(u16 *)temp_s4);
            }
        }
        var_s1 = (var_s1 + 1) & 0xFFFF;
        if (var_s1 >= temp_s0) {
            var_s1 = 0;
        }
        var_s2 = (var_s2 + 1) & 0xFFFF;
        if (var_s2 >= temp_s0) {
            goto block_22;
        }
        goto loop_13;
    }
block_22:
    return 0U;
}

