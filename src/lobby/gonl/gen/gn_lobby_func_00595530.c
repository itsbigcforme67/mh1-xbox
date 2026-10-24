/* G online (PC): lobby_func_00595530, written from Monster Hunter G's code at lobby 0x595530 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00595530(s32 arg0, s32 arg1) {
    s32 temp_v1;

    if (arg0 & 0xFF) {
        return 0;
    }
    temp_v1 = arg1 & 0xFF;
    if ((temp_v1 != 0x56) && (temp_v1 != 0x61) && (temp_v1 != 0x2B) && (temp_v1 != 0x29) && (temp_v1 != 0x2A) && (temp_v1 != 0x4E) && (temp_v1 != 0x60) && (temp_v1 != 0x5E) && (temp_v1 != 0x5D) && (temp_v1 != 0x5C) && (temp_v1 != 0x5A) && (temp_v1 != 0x59) && (temp_v1 != 0x54) && (temp_v1 != 0x4C)) {
        return 0;
    }
    return 1;
}

