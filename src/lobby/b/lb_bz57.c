/* lb_bz57 - lobby UI/client 0x005C1570-0x005C15B0: CallBack_Event_RoomProperty (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char RoomInfo[];

void CallBack_Event_RoomProperty(void) {
    u16 sp1E;

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Get_RoomProperty(sp1E, (u8 *)&RoomInfo + ((sp1E - 1) * 0x15C) + 0x158);
}
