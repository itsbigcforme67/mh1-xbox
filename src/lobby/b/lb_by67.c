/* lb_by67 - agent B promoted near-match 0x005BC9B0-0x005BCA74: CallBack_Result_Lobby_GuestRuleAllocation (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];

void CallBack_Result_Lobby_GuestRuleAllocation(CNET_RES res) {
    int temp_a0;
    int temp_a1;

    temp_a1 = (int)cw;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x13) && (res.val != 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
        } else if (res.val == -1) {
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            F(s32, &lb_sys, 0x6C) = 1;
            F(s32, &lb_sys, 0x68) = 0x17;
            F(u8, (u8 *)cw, 0x2C35) = 0U;
            F(s8, &lb_sys, 6) = 0;
        }
    }
}
