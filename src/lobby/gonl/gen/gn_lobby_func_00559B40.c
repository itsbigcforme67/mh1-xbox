/* HAND-FIXED (the bucket table read). G online (PC): lobby_func_00559B40, written from Monster Hunter G's code at lobby 0x559B40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 D_005FBA80;

s32 lobby_func_00559B40(s32 arg0) {
    s32 temp_a1;
    s32 var_v0;

    temp_a1 = arg0 & 0xFFFF;
    if (temp_a1 < 0x2710) {
        var_v0 = ((arg0 & 0xFFFF) / 1000) + ((u32) temp_a1 >> 0x1F);
    } else {
        var_v0 = 4;
        if (temp_a1 < 0x4E20) {

        } else {
            var_v0 = 5;
        }
    }
    return *(s32 *)(*(u8 **)((u8 *)&D_005FBA80 + (var_v0 * 4)) + ((temp_a1 % 1000) * 4));   /* HAND-FIXED: a row pointer from the bucket's table */
}

