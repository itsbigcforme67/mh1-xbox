#include "lobby_f.h"

void CallBack_Event_PlazaRemove(void) {
    u8 temp_a0;
    void *temp_a1;

    temp_a1 = (u8 *)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 2) && (F(s8, temp_a1, 0x2C09) == 0)) {
        F(s8, temp_a1, 0x2C09) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
    }
}
