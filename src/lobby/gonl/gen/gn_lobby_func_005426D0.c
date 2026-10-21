/* G online (PC): lobby_func_005426D0, written from Monster Hunter G's code at lobby 0x5426D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Online_ck();
s32 Quest_clear_bit_ck();
extern u8 quest_lv_tbl;

s32 lobby_func_005426D0(s32 arg0) {
    u8 *var_s0;   /* (u16 *) */
    u16 temp_a0;
    u16 temp_a0_2;

    var_s0 = (*(s32 *)((u8 *)&quest_lv_tbl + ((arg0 & 0xFF) * 4)));
    if ((*(u16 *)var_s0) != 0) {
loop_1:
        if (Online_ck() == 1) {
            temp_a0 = (*(u16 *)var_s0);
            if (((s32) temp_a0 < 0x2710) && (Quest_clear_bit_ck((s32) temp_a0) == 0)) {
                return 0;
            }
            goto block_9;
        }
        temp_a0_2 = (*(u16 *)var_s0);
        if (((s32) temp_a0_2 >= 0x2710) && (Quest_clear_bit_ck((s32) temp_a0_2) == 0)) {
            return 0;
        }
block_9:
        var_s0 += 2;
        if ((*(u16 *)var_s0) == 0) {
            goto block_10;
        }
        goto loop_1;
    }
block_10:
    return 1;
}

