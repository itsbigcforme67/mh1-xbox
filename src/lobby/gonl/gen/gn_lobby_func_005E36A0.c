/* G online (PC): lobby_func_005E36A0, written from Monster Hunter G's code at lobby 0x5E36A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

s32 lobby_func_005E36A0(s32 arg0, u16 *arg1) {
    *arg1 = (*(s32 *)((u8 *)&(*(u8 *)((u8 *)&CnetSys_w + 0x4272)) + ((arg0 & 0xFFFF) * 0x164)));
    return 0;
}

