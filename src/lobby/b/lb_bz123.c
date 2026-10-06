/* lb_bz123 - lobby UI/client 0x005BDF70-0x005BDFF4: CallBack_Result_Match_MatchInformation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_Match_MatchInformation(CNET_RES res) {
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;
    u8 *temp_a2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (temp_a0 = F(u8, temp_a1, 0x2C45), temp_a2 = temp_a1 + 0x2C45, (temp_a0 == 0x20))) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            all_reset();
            temp_a0_2 = (u8 *)cw;
            F(u8, temp_a0_2, 0x2C34) = (u8) (F(u8, temp_a0_2, 0x2C34) + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x2C0D) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a2);
    }
}
