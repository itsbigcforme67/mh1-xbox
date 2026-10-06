#include "lobby_a.h"
extern char RoomInfo[];
void CallBack_Event_RoomCapacity(void) {
    u16 sp1E;
    int temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = (int)&RoomInfo + ((sp1E - 1) * 0x15C);
    cnLBS_Get_RoomJoinInfo(sp1E, temp_v0 + 4, temp_v0 + 6, temp_v0 + 8);
}
