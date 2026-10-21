/* G online (PC): lobby_func_00569320, written from Monster Hunter G's code at lobby 0x569320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s64 lobby_func_00569320(s32 arg0, s32 arg1) {
    s32 temp_a0;
    s32 temp_v1;
    s32 var_v0;

    temp_v1 = ((s16) (arg1));
    if (temp_v1 == 0) {
        return 1;
    }
    temp_a0 = ((s16) (arg0));
    if (temp_v1 == 0) {
        M2C_BREAK(0);
    }
    var_v0 = ((s16) ((s32) (temp_a0 / temp_v1)));
    if (temp_v1 == 0) {
        M2C_BREAK(0);
    }
    if ((temp_a0 % temp_v1) != 0) {
        var_v0 = ((s16) ((var_v0 + 1)));
    }
    return var_v0;
}

