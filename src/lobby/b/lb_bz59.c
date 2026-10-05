/* lb_bz59 - lobby UI/client 0x005C1650-0x005C1690: CallBack_Event_RoomPasswordInfo (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char RoomInfo[];

void CallBack_Event_RoomPasswordInfo(void) {
    u16 sp1E;

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Get_RoomPasswordInfo(sp1E, (u8 *)&RoomInfo + ((sp1E - 1) * 0x15C) + 0x11);
}
