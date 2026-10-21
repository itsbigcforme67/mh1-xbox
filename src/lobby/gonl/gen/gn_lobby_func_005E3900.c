/* G online (PC): lobby_func_005E3900, written from Monster Hunter G's code at lobby 0x5E3900 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * strcpy();
extern s32 CnetSys_g;

s32 lobby_func_005E3900(s32 arg0, void *arg1) {
    strcpy(arg1, (u8 *)&CnetSys_g + (((arg0 & 0xFFFF) - 1) * 0x164) + 0x44D2);
    return 0;
}

