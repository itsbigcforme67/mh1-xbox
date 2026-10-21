/* G online (PC): lobby_func_005AC580, written from Monster Hunter G's code at lobby 0x5AC580 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

s32 lobby_func_005AC580(s32 arg0, s32 *arg1) {
    *arg1 = *(&(*(u8 *)((u8 *)&CnetSys_w + 0x6690)) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

