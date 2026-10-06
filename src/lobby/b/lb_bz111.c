/* lb_bz111 - lobby UI/client 0x005BA8B0-0x005BA934: CallBack_Result_Plaza_LobbyEntry (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_Plaza_LobbyEntry(CNET_RES res) {
    u8 *temp_a0;
    temp_a0 = (u8 *)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (F(u8, temp_a0, 0x2C45) == 8)) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if (res.val == 0) {
            F(s8, (u8 *)cw, 0x2C35) = 2;
            F(s8, (u8 *)cw, 0x35D5) = 1;
            return;
        }
        F(s8, (u8 *)cw, 0x2C35) = 5;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, 5, temp_a0 + 0x2C45);
    }
}
