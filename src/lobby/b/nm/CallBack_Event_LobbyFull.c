#include "lobby_f.h"

void CallBack_Event_LobbyFull(void) {
    void *temp_a1;
    

    temp_a1 = (u8 *)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        F(s8, temp_a1, 0x2C0E) = 2;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}
