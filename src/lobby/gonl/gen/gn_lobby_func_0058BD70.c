/* G online (PC): lobby_func_0058BD70, written from Monster Hunter G's code at lobby 0x58BD70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_0058BD70(s32 arg0) {
    s32 var_a0;

    var_a0 = arg0;
    if (var_a0 & 0xFFFF & 0x40) {
        var_a0 = 0x40;
    }
    return var_a0;
}

