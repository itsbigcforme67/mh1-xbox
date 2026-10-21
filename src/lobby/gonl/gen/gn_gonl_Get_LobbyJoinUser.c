/* G online (PC): gonl_Get_LobbyJoinUser, written from Monster Hunter G's code at lobby 0x5AC1F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

s32 gonl_Get_LobbyJoinUser(s32 arg0, u16 *arg1, u16 *arg2) {
    s32 temp_a3;

    temp_a3 = (arg0 & 0xFFFF) * 0x164;
    *arg1 = *(&(*(u8 *)((u8 *)&CnetSys_w + 0x6696)) + temp_a3);
    *arg2 = *(&(*(u8 *)((u8 *)&CnetSys_w + 0x6698)) + temp_a3);
    return 0;
}

