#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern s8 COM_R_No_3;
extern s8 COM_R_No_4;
extern u8 COM_R_No_2;
extern u8 COM_R_No_5;
s32 connecting_00(void) {
    COM_R_No_4 = 0;
    COM_R_No_3 = 0;
    Vs_Cnt_0 = 0;
    COM_R_No_2 = (u8) (COM_R_No_2 + 1);
    cnLbc_Init_NgServerId();
    Ncm_spr_BG_set();
    if (COM_R_No_5 == 0) {
        Ncm_spr_set_diarog_b();
        Ncm_spr_CON_AN_set(0);
    } else {
        Ncm_spr_set_diarog_s();
        Vs_Cnt_1 = 0;
    }
    Vs_Cnt_0 = 8;
    *(s16 *)0x3A6E94 = 0;
    cnWrap_ScreenFadeIn(0, 0x14);
    return 0;
}
