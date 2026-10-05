/* lb_bz58 - lobby UI/client 0x005C15B0-0x005C15F4: CallBack_Event_RoomJoinUser (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char RoomInfo[];

void CallBack_Event_RoomJoinUser(void) {
    u16 sp1E;
    int temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = (int)&RoomInfo + ((sp1E - 1) * 0x15C);
    cnLBS_Get_mhRoomJoinUser(sp1E, temp_v0 + 2, temp_v0 + 0xE);
}
