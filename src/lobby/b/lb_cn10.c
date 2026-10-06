/* lb_cn10 - agent C 0x005B4F80-0x005B5044: connecting_10. */
#include "lobby_a.h"
extern u8 InetSys[];
#include "netcw.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern s8 COM_R_No_3;
extern s8 COM_R_No_4;
extern s8 COMconnect;
extern s8 reset_NG_flag;
extern u8 COM_R_No_2;
extern u8 COM_R_No_5;
s32 connecting_10(void) {
    if (COM_R_No_5 == 1) {
        Vs_Cnt_1 = (s16) (Vs_Cnt_1 + 1);
        if (Vs_Cnt_1 > 0x78) {
            Vs_Cnt_1 = 0;
        }
        if (Vs_Cnt_1 > 0x3c) {
            Ncm_mssage_disp_req(0x79);
        }
    }
    Vs_Cnt_0 = (s16) (Vs_Cnt_0 - 1);
    if (Vs_Cnt_0 > 0) {
        return 0;
    }
    reset_NG_flag = 1;
    COMconnect = 1;
    *(s16 *)((u8 *)InetSys + 6) = 0;
    COM_R_No_4 = 0;
    COM_R_No_3 = 0;
    net_common_w.timer = 0;
    COM_R_No_2 = (u8) (COM_R_No_2 + 1);
    if (COM_R_No_5 == 0) {
        Ncm_spr_PRG_BAR_set();
    }
    return 0;
}
