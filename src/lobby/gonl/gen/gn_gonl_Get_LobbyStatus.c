/* G online (PC): gonl_Get_LobbyStatus, written from Monster Hunter G's code at lobby 0x5AC320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

s32 gonl_Get_LobbyStatus(s32 arg0, u8 *arg1) {
    *arg1 = *(&(*(u8 *)((u8 *)&CnetSys_w + 0x66AA)) + ((arg0 & 0xFFFF) * 0x164));
    return 0;
}

