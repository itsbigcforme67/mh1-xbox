/* G online (PC): lobby_func_005C1560, written from Monster Hunter G's code at lobby 0x5C1560 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 memcmp();                 /* extern */

s32 lobby_func_005C1560(s8 *arg0, s32 arg1, s32 arg2) {
    s32 temp_s0;
    s32 var_s2;
    s8 *var_s1;

    temp_s0 = arg1 & 0xFF;
    var_s2 = 0;
    if (temp_s0 > 0) {
        var_s1 = arg0;
loop_2:
        if ((*var_s1 != 0) && (memcmp(var_s1, arg2, 8) == 0)) {
            return var_s2;
        }
        var_s2 += 1;
        var_s1 += 0x18;
        if (var_s2 >= temp_s0) {
            goto block_6;
        }
        goto loop_2;
    }
block_6:
    return -1;
}

