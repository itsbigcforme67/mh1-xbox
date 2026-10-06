/* lb_bz122 - lobby UI/client 0x005BDBF0-0x005BDC6C: CallBack_Result_SetRoomPropaty (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_SetRoomPropaty(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x1B)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        F(u8, (u8 *)cw, 0x2C35) = 3U;
    }
}
