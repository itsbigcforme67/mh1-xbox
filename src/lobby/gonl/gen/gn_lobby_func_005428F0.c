/* G online (PC): lobby_func_005428F0, written from Monster Hunter G's code at lobby 0x5428F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck();                        /* extern */
extern u16 *quest_lv_tbl;

s32 lobby_func_005428F0(void) {
    s32 var_s2;
    u8 *var_s0;   /* (u16 **) */
    u8 *var_s1;   /* (u16 *) */
    u16 var_a0;

    var_s2 = 0;
    var_s0 = &quest_lv_tbl;
loop_1:
    var_s1 = (*(u16 **)var_s0);
    var_a0 = (*(u16 *)var_s1);
    if (var_a0 != 0) {
loop_2:
        if (((s32) var_a0 >= 0x2710) && ((s32) var_a0 < 0x4E20) && (Quest_clear_bit_ck((s32) var_a0) == 0) && ((*(u16 *)var_s1) != 0x2740)) {
            return 0;
        }
        var_s1 += 2;
        var_a0 = (*(u16 *)var_s1);
        if (var_a0 == 0) {
            goto block_9;
        }
        goto loop_2;
    }
block_9:
    var_s2 = (var_s2 + 1) & 0xFF;
    var_s0 += 4;
    if (var_s2 >= 6) {
        return 1;
    }
    goto loop_1;
}

