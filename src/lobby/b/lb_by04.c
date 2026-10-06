/* lb_by04 - agent B promoted near-match 0x005C1130-0x005C1178: CallBack_Event_AdminMessage (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Event_AdminMessage(CNET_RES res) {
    char sp20[32];
    int temp_v0;

    memset((u8 *)cw + 0x2C5C, 0, 0x31C);
    temp_v0 = (int)cw;
    cnLBS_Get_RecvMessage(&sp20, temp_v0 + 0x2C5D, temp_v0 + 0x2C6E);
    F(s8, (u8 *)cw, 0x2C5C) = 1;
}
