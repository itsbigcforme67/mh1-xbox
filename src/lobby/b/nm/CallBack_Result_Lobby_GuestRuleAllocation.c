#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a0[];
extern char temp_a0[];
extern char temp_a1[];
void CallBack_Result_Lobby_GuestRuleAllocation(int arg0) {
    long long sp18;
    int temp_a0;
    int temp_a1;

    temp_a1 = (int)cw;
    sp18 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x13) && ((s8) sp18 != 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        switch ((s8) sp18) {                        /* irregular */
        case 0:
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C35) = (u8) (F(u8, temp_a0, 0x2C35) + 1);
            return;
        case -1:
            cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            F(s32, &lb_sys, 0x6C) = 1;
            F(s32, &lb_sys, 0x68) = 0x17;
            F(u8, (u8 *)cw, 0x2C35) = 0U;
            F(s8, &lb_sys, 6) = 0;
            break;
        }
    }
}
