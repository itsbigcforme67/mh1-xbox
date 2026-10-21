/* G online (PC): sub_main_func_004C8F50, written from Monster Hunter G's code at lobby 0x4C8F50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 sub_main_func_004C8F50(s32 arg0) {
    s32 var_v0;

    if (arg0 == 2) {
        return 0x150;
    }
    var_v0 = 0;
    if (arg0 == 1) {
        var_v0 = 0x2C;
    }
    return var_v0;
}

