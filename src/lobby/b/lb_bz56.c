/* lb_bz56 - lobby UI/client 0x005C1530-0x005C1570: CallBack_Event_RoomStatus (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char RoomInfo[];

void CallBack_Event_RoomStatus(void) {
    u16 sp1E;

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Get_RoomStatus(sp1E, (u8 *)&RoomInfo + ((sp1E - 1) * 0x15C) + 0x10);
}
