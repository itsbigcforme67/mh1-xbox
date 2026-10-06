/* lb_by01 - agent B promoted near-match 0x005C0A10-0x005C0A8C: CallBack_Event_System_ShutDown, CallBack_Event_LobbyFull (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Event_System_ShutDown(CNET_RES res) {
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        F(s8, temp_a1, 0x2C0E) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}

void CallBack_Event_LobbyFull(CNET_RES res) {
    void *temp_a1;
    

    temp_a1 = (u8 *)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        F(s8, temp_a1, 0x2C0E) = 2;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}
