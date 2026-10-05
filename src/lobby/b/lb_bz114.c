/* lb_bz114 - lobby UI/client 0x005BC0C0-0x005BC144: CallBack_Result_Lobby_RoomCreate (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_Lobby_RoomCreate(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xF)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            F(s8, (u8 *)cw, 0x32C5) = 1;
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        F(u8, (u8 *)cw, 0x2C35) = 3U;
    }
}
