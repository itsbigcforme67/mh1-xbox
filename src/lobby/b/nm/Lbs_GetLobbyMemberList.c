#include "lobby_a.h"
extern char CallBack_Result_Plaza_LobbyMember2[];
s32 Lbs_GetLobbyMemberList(int arg0) {
    u8 temp_v1_2;
    int temp_v1;
    int temp_v1_3;

    temp_v1 = (int)pNet;
    temp_v1_2 = F(u8, temp_v1, 0x12);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(s8, (u8 *)cw, 0x2C45) = 0xA;
        temp_v1_3 = (int)pNet;
        F(u8, temp_v1_3, 0x12) = (u8) (F(u8, temp_v1_3, 0x12) + 1);
        cnLBS_Read_LobbyMemberList((( (arg0 << 0x30) >> 0x30) + 1) & 0xFFFF, &CallBack_Result_Plaza_LobbyMember2, 0xA);
block_8:
    default:
        return 0;
    case 1:
        Check_CallBackWait(temp_v1 + 0x12);
        goto block_8;
    case 2:
        F(u8, temp_v1, 0x12) = 0U;
        return 1;
    }
}
