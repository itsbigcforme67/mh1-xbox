/* G online (PC): CallBack_Event_PlazaJoinUser, written from Monster Hunter G's code at lobby 0x587160 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_mhRoomJoinUser(); /* extern */
s32 cnetGet_Room_LastRecvNumber();         /* extern */
s32 lobby_func_005E36D0();           /* extern */
extern u8 PlazaInfo;

void CallBack_Event_PlazaJoinUser(void) {
    union { u16 v; u8 b[0x2]; } u_sp1E;
#define sp1E (u_sp1E.v)
    u8 *temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = &PlazaInfo + ((sp1E - 1) * 0x15C);
    cnLBS_Get_mhRoomJoinUser(sp1E, temp_v0 + 6, temp_v0 + 0x12);
    lobby_func_005E36D0(sp1E, &PlazaInfo + ((sp1E - 1) * 0x15C) + 0x14);
}

