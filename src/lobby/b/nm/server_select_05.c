/* server_select_05 (0x5B6730): 9/170 differ: two branch delay slots (original leaves nop). Get_sw is u16. Not built. */
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
extern char ss_text_lobby_trans_ot[];
extern char D_3A3C7D[];
extern u16 Get_sw();
s32 server_select_05(void) {
    u16 sw;
    s32 ret;
    u8 no;

    sw = Get_sw(0);
    no = COM_R_No_3;
    ret = 0;
    switch (no) {
    case 0:
        COM_R_No_3 = (u8) (no + 1);
        cnWrap_BgmStop();
        cnWrap_ScreenFadeOut(0, 0x14);
        break;
    case 1:
        if (cnWrap_ScreenFadeCheck() == 0) {
            if (COM_R_No_2 != 0) {
                COM_R_No_3 = COM_R_No_3 + 1;
            } else {
                COM_R_No_3 = 7;
            }
            Lbc_set_prim(0, &Disp_back, &ss_text_lobby_trans_ot);
            Net_fade_kill();
            COM_R_No_Disconnect = 0;
        }
        break;
    case 2:
        COM_R_No_3 = (u8) (no + 1);
        break;
    case 3:
        COM_R_No_3 = (u8) (no + 1);
        FlushCache(0);
        memset(&FirstURL, 0, 0x100);
        MMBB_LOGIN = 2;
        BS_MODE_R_NO = 1;
        strcpy(&FirstURL, &D_3A3C7D);
        MainBsInitialize(0);
        break;
    case 4:
        if (MainBrowser() != 0) {
            MainBsDispose();
            COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        }
        break;
    case 5:
        cnLbc_LoadNetModel(1);
        COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        break;
    case 6:
        if (cnLbc_LoadModelWait(1) == 0) {
            return 0;
        }
        COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        break;
    case 7:
        if (disconnect() != 0) {
            COM_R_No_3 = (u8) (COM_R_No_3 + 1);
        }
        break;
    case 8:
        COM_R_No_3 = 9;
        Vs_Cnt_0 = 8;
        Net_fade_execute(0, 0xA, 0);
        if (COM_R_No_2 == 0) {
            SetDialogData(0xA, 0);
        } else {
            SetDialogData(8, 0);
        }
        F(s8, &network_work, 0xC) = 1;
        break;
    case 9:
        F(s8, &network_work, 0xC) = 1;
        Vs_Cnt_0 = (s16) (Vs_Cnt_0 - 1);
        if (Vs_Cnt_0 < 0) {
            COM_R_No_3 = (u8) (no + 1);
        }
        break;
    case 10:
        F(s8, &network_work, 0xC) = 1;
        if (sw & 0xFFFF & 0x20) {
            PPP_ErrorStatus = 0;
            ret = -4;
            MMBB_LOGIN = 0;
        }
        cnWrap_SetFontColor(5);
        flfntSetSize(0x14, 0x14);
        break;
    }
    lbc_text_lobby_trans(&network_work);
    return ret;
}
