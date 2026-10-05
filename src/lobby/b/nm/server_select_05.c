#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s8 BS_MODE_R_NO;
extern s8 COM_R_No_Disconnect;
extern s8 MMBB_LOGIN;
extern s8 PPP_ErrorStatus;
extern u8 COM_R_No_2;
extern u8 COM_R_No_3;
extern char Disp_back[];
extern char FirstURL[];
extern char FirstURL[];
extern char ss_text_lobby_trans_ot[];
extern char D_3A3C7D[];
s32 server_select_05(void) {
    s32 temp_a1;
    s32 var_s0;
    u8 temp_a0;
    u8 var_v0;

    temp_a0 = COM_R_No_3;
    temp_a1 = Get_sw(0) & 0xFFFF;
    var_s0 = 0;
    switch (temp_a0) {
    case 0:
        COM_R_No_3 = (u8) (temp_a0 + 1);
        cnWrap_BgmStop(temp_a0, temp_a1);
        cnWrap_ScreenFadeOut(0, 0x14);
    default:
block_30:
        lbc_text_lobby_trans(&network_work);
        return var_s0;
    case 1:
        if (cnWrap_ScreenFadeCheck(temp_a0, temp_a1) == 0) {
            if (COM_R_No_2 != 0) {
                var_v0 = COM_R_No_3 + 1;
            } else {
                var_v0 = 7;
            }
            COM_R_No_3 = var_v0;
            Lbc_set_prim(0, &Disp_back, &ss_text_lobby_trans_ot);
            Net_fade_kill();
            COM_R_No_Disconnect = 0;
        }
        goto block_30;
    case 2:
        COM_R_No_3 = (u8) (temp_a0 + 1);
        goto block_30;
    case 3:
        COM_R_No_3 = (u8) (temp_a0 + 1);
        FlushCache(0, temp_a1);
        memset(&FirstURL, 0, 0x100);
        MMBB_LOGIN = 2;
        BS_MODE_R_NO = 1;
        strcpy(&FirstURL, &D_3A3C7D);
        MainBsInitialize(0);
        goto block_30;
    case 4:
        if (MainBrowser(temp_a0, temp_a1) != 0) {
            MainBsDispose();
            COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        }
        goto block_30;
    case 5:
        cnLbc_LoadNetModel(1, temp_a1);
        COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        goto block_30;
    case 6:
        if (cnLbc_LoadModelWait(1, temp_a1) == 0) {
            return 0;
        }
        COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        goto block_30;
    case 7:
        if (disconnect(temp_a0, temp_a1) != 0) {
            COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        }
        goto block_30;
    case 8:
        COM_R_No_3 = 9U;
        Vs_Cnt_0 = 8;
        Net_fade_execute(0, 0xA, 0);
        if (COM_R_No_2 == 0) {
            SetDialogData(0xA, 0);
        } else {
            SetDialogData(8, 0);
        }
        F(s8, &network_work, 0xC) = 1;
        goto block_30;
    case 9:
        F(s8, &network_work, 0xC) = 1;
        Vs_Cnt_0 = (s16) (Vs_Cnt_0 - 1);
        if (Vs_Cnt_0 < 0) {
            COM_R_No_3 = (u8) (temp_a0 + 1);
        }
        goto block_30;
    case 10:
        F(s8, &network_work, 0xC) = 1;
        if (temp_a1 & 0xFFFF & 0x20) {
            PPP_ErrorStatus = 0;
            var_s0 = -4;
            MMBB_LOGIN = 0;
        }
        cnWrap_SetFontColor(5, temp_a1);
        flfntSetSize(0x14, 0x14);
        goto block_30;
    }
}
