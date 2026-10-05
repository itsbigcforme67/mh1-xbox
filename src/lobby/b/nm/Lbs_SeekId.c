#include "lobby_a.h"
extern char CallBack_Result_SearchUserPlace[];
s32 Lbs_SeekId(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v0;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        cnLBS_SerchUserPlace((u8 *)cw + 0x2F80, &CallBack_Result_SearchUserPlace);
block_11:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0);
        goto block_11;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        temp_v0 = (int)cw;
        cnLBS_Get_SerchUserPlace(temp_v0 + 0x30B4, temp_v0 + 0x30B6, temp_v0 + 0x30B8, temp_v0 + 0x30BB);
        cnLBS_Get_SerchUserPlaceMessage((u8 *)cw + 0x30BC);
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1);
        return 1;
    }
}
