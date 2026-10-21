/* G online (PC): lobby_func_00587260, written from Monster Hunter G's code at lobby 0x587260 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_PlazaName();           /* extern */
s32 cnetGet_Room_LastRecvNumber();         /* extern */
extern u8 LobbyInfo;

void lobby_func_00587260(void) {
    union { u16 v; u8 b[0x2]; } u_sp1E;
#define sp1E (u_sp1E.v)

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Get_PlazaName(sp1E, &LobbyInfo + ((sp1E - 1) * 0x15C) + 0x1A);
}

