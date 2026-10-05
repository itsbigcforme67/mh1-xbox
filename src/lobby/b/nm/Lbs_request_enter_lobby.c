#include "lobby_a.h"
extern char CallBack_Result_Plaza_LobbyEntry[];
extern char CallBack_Result_Plaza_LobbyMember[];
s32 Lbs_request_enter_lobby(void) {
    u8 temp_a0;
    int temp_a1;
    int temp_v0;

    temp_v0 = (int)cw;
    temp_a0 = F(u8, temp_v0, 0x2C35);
    temp_a1 = temp_v0 + 0x2C35;
    switch (temp_a0) {
    case 0:
        F(u8, temp_v0, 0x2C35) = (u8) (temp_a0 + 1);
        MH_lobbyClear(temp_a0, temp_a1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 8;
        cnLBS_LobbyEntry(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Plaza_LobbyEntry);
    default:
block_10:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0, temp_a1);
        goto block_10;
    case 2:
        F(u8, temp_v0, 0x2C35) = (u8) (temp_a0 + 1);
        CallBackWaitInit(temp_a0, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        cnLBS_Read_LobbyMemberList(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Plaza_LobbyMember);
        goto block_10;
    case 3:
        Check_CallBackWait(temp_a0, temp_a1);
        goto block_10;
    case 4:
        F(u8, temp_v0, 0x2C35) = 0U;
        return 0;
    case 5:
        F(u8, temp_v0, 0x2C35) = 0U;
        return 1;
    }
}
