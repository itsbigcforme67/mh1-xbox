#include "lobby_a.h"
extern s32 netr_ret;
extern s8 COM_R_No_0;
extern s8 COM_R_No_1;
extern s8 COM_R_No_2;
extern s8 COM_R_No_3;
extern s8 COM_R_No_4;
extern s8 COM_R_No_5;
extern s8 COM_R_No_6;
extern s8 net_game_invalid_flag;
extern u16 System_timer;
extern char CallBack_Result_Match_Logout[];
extern char jtbl_2977[];
extern char CnetWork[];
extern char CnetWork[];
extern char CnetWork[];
void lbc_game_ready_04(void) {
    u8 temp_a1;
    int temp_a2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;

    temp_v1 = (int)cw;
    temp_a1 = F(u8, temp_v1, 0x2C34);
    temp_a2 = temp_v1 + 0x2C34;
    switch (temp_a1) {
    case 0:
        F(u8, temp_v1, 0x2C34) = (u8) (temp_a1 + 1);
        cnWrap_BgmFadeOut(0xF, temp_a1, temp_a2);
        /* fallthrough */
    case 1:
        temp_v1_2 = (int)cw;
        F(u8, temp_v1_2, 0x2C34) = (u8) (F(u8, temp_v1_2, 0x2C34) + 1);
        /* fallthrough */
    case 2:
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C34) = (u8) (F(u8, temp_v1_3, 0x2C34) + 1);
        /* fallthrough */
    case 3:
        temp_v1_4 = (int)cw;
        F(u8, temp_v1_4, 0x2C34) = (u8) (F(u8, temp_v1_4, 0x2C34) + 1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x21;
        cnLBS_LogoutLobbyServer(&CallBack_Result_Match_Logout);
        return;
    case 4:
        Check_CallBackWait(&jtbl_2977, temp_a1, temp_a2);
        return;
    case 5:
        F(s32, &CnetWork, 8) = 0;
        net_game_invalid_flag = 1;
        COM_R_No_1 = 0;
        F(s8, &CnetWork, 5) = 3;
        COM_R_No_0 = 4;
        COM_R_No_2 = 0;
        COM_R_No_3 = 0;
        COM_R_No_4 = 0;
        COM_R_No_5 = 0;
        COM_R_No_6 = 0;
        netr_ret = 0;
        F(s32, &CnetWork, 0x10) = System_timer;
        /* fallthrough */
    default:
        return;
    }
}
