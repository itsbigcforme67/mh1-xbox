#include "lobby_a.h"

void CallBack_Event_AdminMessage(void) {
    int sp20;
    int temp_v0;

    memset((u8 *)cw + 0x2C5C, 0, 0x31C);
    temp_v0 = (int)cw;
    cnLBS_Get_RecvMessage(&sp20, temp_v0 + 0x2C5D, temp_v0 + 0x2C6E);
    F(s8, (u8 *)cw, 0x2C5C) = 1;
}
