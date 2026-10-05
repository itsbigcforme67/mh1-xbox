/* lb_bz106 - lobby UI/client 0x005B87A0-0x005B881C: CallBack_Result_LoginPersonalDataRegist (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_LoginPersonalDataRegist(CNET_RES res) {
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xD)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            F(u8, (u8 *)cw, 0x2C34) = 8U;
            return;
        }
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
    }
}
