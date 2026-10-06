/* lb_bz115 - lobby UI/client 0x005BC3E0-0x005BC49C: CallBack_Result_RuleAllocation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_RuleAllocation(CNET_RES res) {
    u8 *temp_a1;
    u8 *temp_a1_2;

    temp_a1 = (u8 *)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x10) && (res.val != 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a1_2 = (u8 *)cw;
            F(u8, temp_a1_2, 0x2C35) = (u8) (F(u8, temp_a1_2, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x32C2) = 1;
            return;
        }
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        *(s8 *)0x3F36AB = 0;
        F(s32, &lb_sys, 0x6C) = 1;
        F(s32, &lb_sys, 0x68) = 0x17;
        F(u8, (u8 *)cw, 0x2C35) = 0U;
        F(s8, &lb_sys, 6) = 0;
    }
}
