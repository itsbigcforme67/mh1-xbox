/* lb_by02 - agent B promoted near-match 0x005C0C90-0x005C0DAC: CallBack_Event_PlazaRemove, CallBack_Event_LobbyRemove, CallBack_Event_RoomRemove (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
int cnLBS_Get_ServerMessage(u8 *, ...);

void CallBack_Event_PlazaRemove(CNET_RES res) {
    u8 temp_a0;
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 2) && (F(s8, temp_a1, 0x2C09) == 0)) {
        F(s8, temp_a1, 0x2C09) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}

void CallBack_Event_LobbyRemove(CNET_RES res) {
    u8 temp_a0;
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 3) && (F(s8, temp_a1, 0x2C0A) == 0)) {
        F(s8, temp_a1, 0x2C0A) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}

void CallBack_Event_RoomRemove(CNET_RES res) {
    u8 temp_a0;
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 3) && (F(s8, temp_a1, 0x2C0C) == 0) && (F(s8, temp_a1, 0x2C0B) == 0)) {
        F(s8, temp_a1, 0x2C0B) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}
