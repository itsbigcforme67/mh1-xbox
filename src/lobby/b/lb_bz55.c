/* lb_bz55 - lobby UI/client 0x005C14F0-0x005C1530: CallBack_Event_RoomName (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char RoomInfo[];

void CallBack_Event_RoomName(void) {
    u16 sp1E;

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Get_RoomName(sp1E, (u8 *)&RoomInfo + ((sp1E - 1) * 0x15C) + 0x14);
}
