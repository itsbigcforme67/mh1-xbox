/* lb_bz127 - lobby UI/client 0x005C0870-0x005C08EC: CallBack_Result_Mail_SendMail (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_Mail_SendMail(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x23)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
    }
}
