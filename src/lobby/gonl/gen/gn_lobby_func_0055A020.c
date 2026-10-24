/* G online (PC): lobby_func_0055A020, written from Monster Hunter G's code at lobby 0x55A020 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lb_key_quest_ck();
void * lobby_func_00559F80();
s32 lobby_func_0055A020(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_s0;
    s32 var_s1;
    u8 *temp_v0;   /* (u16 *) */

    temp_v0 = lobby_func_00559F80(arg0, arg1, arg2, arg3);
    if (temp_v0 == NULL) {
        return 0;
    }
    temp_s0 = arg0 & 0xFFFF;
    var_s1 = 0;
    if (temp_s0 >= 0xC350) {

    } else {
        var_s1 = (*(u16 *)temp_v0) & 7;
    }
    if (lb_key_quest_ck(arg0) == 1) {
        var_s1 = (var_s1 | 8) & 0xFF;
    } else if (temp_s0 >= 0xC350) {
        var_s1 = (var_s1 | 0x10) & 0xFF;
    }
    return var_s1;
}

