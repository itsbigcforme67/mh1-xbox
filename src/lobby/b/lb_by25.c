/* lb_by25 - agent B promoted near-match 0x005B95A0-0x005B965C: Lbc_SendBrowserResult (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char BrPersonalData[];
extern char CallBack_Result_LoginPersonalDataRegist2[];

s32 Lbc_SendBrowserResult(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;
    int temp_v1_3;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        CallBackWaitInit(temp_a0);
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C35) = (u8) (F(u8, temp_v1_3, 0x2C35) + 1);
        cnLBS_RegistPersonalData(&BrPersonalData, &CallBack_Result_LoginPersonalDataRegist2);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        return 1;
    }
    return 2;
}
