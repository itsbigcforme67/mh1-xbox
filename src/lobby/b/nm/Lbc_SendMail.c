#include "lobby_a.h"
extern char CallBack_Result_Mail_SendMail[];
s32 Lbc_SendMail(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v0;
    int temp_v1;
    int temp_v1_3;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0x23;
        temp_v0 = (int)cw;
        cnLBS_SendMessage(temp_v0 + 0x301A, temp_v0 + 0x3033, &CallBack_Result_Mail_SendMail);
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C35) = (u8) (F(u8, temp_v1_3, 0x2C35) + 1);
block_12:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0);
        goto block_12;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
}
