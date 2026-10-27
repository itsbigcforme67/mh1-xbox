/* G online (PC): lobby_func_00559C30, written from Monster Hunter G's code at lobby 0x559C30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559B40();
void * lobby_func_00559C70();
s32 lobby_func_00559C30(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {    /* HAND-FIXED: m2c made it void; G returns the
                                                                     * row (tail calls): every caller reads it */
    if ((arg0 & 0xFFFF) >= 0xC350) {
        return (s32)(long)lobby_func_00559C70(arg0, arg1, arg2, arg3);
    }
    return lobby_func_00559B40(arg0, arg1, arg2, arg3);
}

