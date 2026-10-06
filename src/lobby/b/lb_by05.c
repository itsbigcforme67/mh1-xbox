/* lb_by05 - agent B promoted near-match 0x005C1600-0x005C1650: CallBack_Event_RoomCapacity (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char RoomInfo[];

void CallBack_Event_RoomCapacity(CNET_RES res) {
    u16 sp1E;
    int temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = (int)&RoomInfo + ((sp1E - 1) * 0x15C);
    cnLBS_Get_RoomJoinInfo(sp1E, temp_v0 + 4, temp_v0 + 6, temp_v0 + 8, temp_v0 + 10, temp_v0 + 12);
}
