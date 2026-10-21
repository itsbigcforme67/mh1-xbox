/* G online (PC): lobby_func_005935C0, written from Monster Hunter G's code at lobby 0x5935C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 strlen_sp();
s64 lobby_func_005935C0(void *arg0, s64 arg1, s64 arg2) {
    s32 temp_v0;
    s32 var_v1;

    temp_v0 = (((s16) (arg2))) * strlen_sp(arg0);
    var_v1 = temp_v0 >> 2;
    if (temp_v0 < 0) {
        var_v1 = (s32) (temp_v0 + 3) >> 2;
    }
    return ((s16) (((((s16) (arg1))) - var_v1)));
}

