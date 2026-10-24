/* G online (PC): lobby_func_00542820, written from Monster Hunter G's code at lobby 0x542820 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck();
extern u16 *quest_lv_tbl;

s32 lobby_func_00542820(void) {
    s32 var_s2;
    u8 *var_s0;   /* (u16 **) */
    u8 *var_s1;   /* (u16 *) */
    u16 temp_v1;
    u16 var_a0;

    var_s2 = 0;
    var_s0 = &quest_lv_tbl;
loop_1:
    var_s1 = (*(u16 **)var_s0);
    var_a0 = (*(u16 *)var_s1);
    if (var_a0 != 0) {
loop_2:
        if (((s32) var_a0 < 0x2710) && (Quest_clear_bit_ck((s32) var_a0) == 0)) {
            temp_v1 = (*(u16 *)var_s1);
            if ((temp_v1 != 0xBBF) && (temp_v1 != 0xBBE) && (temp_v1 != 0xBBD) && (temp_v1 != 0xBBC)) {
                return 0;
            }
        }
        var_s1 += 2;
        var_a0 = (*(u16 *)var_s1);
        if (var_a0 == 0) {
            goto block_10;
        }
        goto loop_2;
    }
block_10:
    var_s2 = (var_s2 + 1) & 0xFF;
    var_s0 += 4;
    if (var_s2 >= 8) {
        return 1;
    }
    goto loop_1;
}

