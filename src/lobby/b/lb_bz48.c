/* lb_bz48 - lobby UI/client 0x005BF7D0-0x005BF808: CallBack_Logout_ShutDown (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 COM_R_No_Logout;

void CallBack_Logout_ShutDown(void) {
    u8 temp_a0_2;
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    if (F(u8, temp_a0, 0x2C45) == 0x26) {
        F(u8, temp_a0, 0x2C45) = 0U;
        temp_a0_2 = COM_R_No_Logout;
        if (temp_a0_2 == 2) {
            COM_R_No_Logout = (u8) (temp_a0_2 + 1);
        }
    }
}
