/* G online (PC): lobby_func_005C1510, written from Monster Hunter G's code at lobby 0x5C1510 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005C1510(s8 *arg0, s32 arg1) {
    s32 temp_a1;
    s32 var_v0;
    s8 *var_a0;

    var_a0 = arg0;
    temp_a1 = arg1 & 0xFF;
    var_v0 = 0;
    if (temp_a1 > 0) {
loop_1:
        if (*var_a0 == 0) {
            return var_v0;
        }
        var_v0 += 1;
        var_a0 += 0x18;
        if (var_v0 >= temp_a1) {
            goto block_5;
        }
        goto loop_1;
    }
block_5:
    return -1;
}

