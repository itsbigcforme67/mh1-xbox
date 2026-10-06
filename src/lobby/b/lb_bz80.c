/* lb_bz80 - lobby UI/client 0x005B5150-0x005B529C: connecting_30, connecting_40 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern u8 COM_R_No_2;
extern u8 COM_R_No_5;

s32 connecting_30(void) {
    COM_R_No_2 = (u8) (COM_R_No_2 + 1);
    if (COM_R_No_5 == 1) {
        Vs_Cnt_1 = (s16) (Vs_Cnt_1 + 1);
        if (Vs_Cnt_1 > 0x78) {
            Vs_Cnt_1 = 0;
        }
        if (Vs_Cnt_1 > 0x3c) {
            Ncm_mssage_disp_req(0x79);
        }
    }
    if (COM_R_No_5 == 0) {
        Ncm_spr_kill(0x1000);
    }
    Vs_Cnt_0 = 8;
    cnWrap_ScreenFadeOut(0, 0x14);
    return 0;
}

s32 connecting_40(void) {
    s16 temp_v0;

    if (COM_R_No_5 == 1) {
        Vs_Cnt_1 = (s16) (Vs_Cnt_1 + 1);
        if (Vs_Cnt_1 > 0x78) {
            Vs_Cnt_1 = 0;
        }
        if (Vs_Cnt_1 > 0x3c) {
            Ncm_mssage_disp_req(0x79);
        }
    }
    temp_v0 = Vs_Cnt_0 - 1;
    Vs_Cnt_0 = temp_v0;
    if (((s16)temp_v0) > 0) {
        return 0;
    }
    if (cnWrap_ScreenFadeCheck() != 0) {
        return 0;
    }
    Ncm_spr_kill_all();
    return 1;
}
