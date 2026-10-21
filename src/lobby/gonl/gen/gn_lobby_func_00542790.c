/* G online (PC): lobby_func_00542790, written from Monster Hunter G's code at lobby 0x542790 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Quest_clear_bit_ck();
extern u8 flag_quest_tbl;
extern u8 flag_quest_tbl_local;

s32 lobby_func_00542790(s32 arg0, s32 arg1) {
    u8 *var_s0;   /* (u16 *) */
    u16 temp_a0;

    if (!(arg0 & 0xFF)) {
        var_s0 = (*(s32 *)((u8 *)&flag_quest_tbl + ((arg1 & 0xFF) * 4)));
    } else {
        var_s0 = (*(s32 *)((u8 *)&flag_quest_tbl_local + ((arg1 & 0xFF) * 4)));
    }
loop_7:
    temp_a0 = (*(u16 *)var_s0);
    if (temp_a0 == 0) {
        return 1;
    }
    if (Quest_clear_bit_ck((s32) temp_a0) == 0) {
        return 0;
    }
    var_s0 += 2;
    goto loop_7;
}

