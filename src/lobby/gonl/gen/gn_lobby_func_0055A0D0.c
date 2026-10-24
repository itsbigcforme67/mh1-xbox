/* G online (PC): lobby_func_0055A0D0, written from Monster Hunter G's code at lobby 0x55A0D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * lobby_func_00559F80();
s32 lobby_func_0055A0D0(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    u8 *temp_v0;

    temp_v0 = lobby_func_00559F80(arg0, arg1, arg2, arg3);
    if (temp_v0 == NULL) {
        return 0;
    }
    temp_a0 = arg0 & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0xBB9:
        return 4;
    case 0xBBA:
        return 6;
    default:
        return (*(u8 *)((u8 *)(temp_v0) + 2)) & 0xF;
    }
}

