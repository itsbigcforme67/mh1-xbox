#include "lobby_a.h"
extern char CallBack_GetDate[];
s32 Lbc_getDate(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        F(s8, (u8 *)cw, 0x2C45) = 0x17;
        CallBackWaitInit(temp_a0);
        cnLBS_Read_TimingValue(&CallBack_GetDate);
block_9:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0);
        goto block_9;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    }
}
