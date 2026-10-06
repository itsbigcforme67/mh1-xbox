#include "lobby_f.h"

int cnLBS_Get_ServerMessage(u8 *, ...);
void CallBack_Event_LobbyRemove(void) {
    u8 temp_a0;
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 3) && (F(s8, temp_a1, 0x2C0A) == 0)) {
        F(s8, temp_a1, 0x2C0A) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}
