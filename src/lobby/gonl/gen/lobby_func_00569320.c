/* G online (PC): lobby_func_00569320, written from Monster Hunter G's code at lobby 0x569320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s64 lobby_func_00569320(s64 arg0, s64 arg1) {
    s64 temp_a0;
    s64 temp_v1;
    s64 var_v0;

    temp_v1 = (s64) (arg1 << 0x30) >> 0x30;
    if (temp_v1 == 0) {
        return 1;
    }
    temp_a0 = (s64) (arg0 << 0x30) >> 0x30;
    if (temp_v1 == 0) {
        M2C_BREAK(0);
    }
    var_v0 = (s64) ((s64) (temp_a0 / temp_v1) << 0x30) >> 0x30;
    if (temp_v1 == 0) {
        M2C_BREAK(0);
    }
    if ((temp_a0 % temp_v1) != 0) {
        var_v0 = (s64) ((var_v0 + 1) << 0x30) >> 0x30;
    }
    return var_v0;
}

