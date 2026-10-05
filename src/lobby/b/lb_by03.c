/* lb_by03 - agent B promoted near-match 0x005C0AC0-0x005C0B04: CallBack_Event_MatchCancel (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Event_MatchCancel(CNET_RES res) {
    u8 temp_a0;
    int temp_a1;

    temp_a1 = (int)cw;
    temp_a0 = F(u8, temp_a1, 0x2C31);
    if ((temp_a0 != 5) && (temp_a0 == 4)) {
        cnLBS_Get_ServerMessage(temp_a1 + 0x32D1, temp_a1);
        F(s8, (u8 *)cw, 0x2C0D) = 2;
    }
}
