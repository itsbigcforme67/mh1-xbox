/* lb_bz105 - lobby UI/client 0x005B84D0-0x005B856C: CallBack_SendMiniData (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_SendMiniData(CNET_RES res) {
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_v1;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            memcpy((u8 *)cw + 0x45A, &my_user_mini_data, 0x40);
            temp_a0 = (u8 *)cw;
            F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
            return;
        }
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
    }
}
