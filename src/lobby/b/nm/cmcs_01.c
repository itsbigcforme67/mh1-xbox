#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern u8 COM_R_No_1;
void cmcs_01(void) {
    u16 sp1E;
    s32 sp18;
    s16 temp_v1;
    s32 temp_v0;

    temp_v1 = Vs_Cnt_0 - 1;
    Vs_Cnt_0 = temp_v1;
    if (((s16)temp_v1) <= 0) {
        Vs_Cnt_1 = (s16) (Vs_Cnt_1 + 1);
        Vs_Cnt_0 = 0;
        if (Vs_Cnt_1 > 3) {
            COM_R_No_1 = 7U;
            Vs_Cnt_0 = 0;
            *(s8 *)0x4E4723 = 5;
            Vs_Cnt_1 = 0;
            return;
        }
        cnLBS_Get_GameServerAddress(&sp18, &sp1E);
        temp_v0 = connect_ps2(sp18, sp1E, 0);
        *(s32 *)0x4E36F4 = temp_v0;
        if (temp_v0 < 0) {
            Vs_Cnt_0 = 0x258;
            return;
        }
        Vs_Cnt_0 = 0x258;
        COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    }
}
