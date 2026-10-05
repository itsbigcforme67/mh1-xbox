/* lb_bz107 - lobby UI/client 0x005B9660-0x005B96CC: CallBack_Result_LoginPersonalDataRegist2 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_LoginPersonalDataRegist2(CNET_RES res) {
    u8 *temp_a1;

    temp_a1 = (u8 *)cw;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        if (res.val == 0) {
            F(u8, temp_a1, 0x2C35) = (u8) (F(u8, temp_a1, 0x2C35) + 1);
            return;
        }
        F(u8, temp_a1, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
    }
}
