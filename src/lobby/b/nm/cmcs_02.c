#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern u8 COM_R_No_1;
extern char D_4E36F4[];
extern char D_4E3714[];
extern char D_4E36F4[];
void cmcs_02(void) {
    s16 temp_v0;

    temp_v0 = Vs_Cnt_0 - 1;
    Vs_Cnt_0 = temp_v0;
    if (((s16)temp_v0) <= 0) {
        Vs_Cnt_0 = 0xA;
        COM_R_No_1 = (u8) (COM_R_No_1 - 1);
        CpInetTcpAbort(*(s32 *)0x4E36F4);
        CpInetTcpDelete(&D_4E36F4);
        return;
    }
    if (CpInetTcpGetStatus(*(u8 *)0x4E36F4, &D_4E3714) < 0) {
        COM_R_No_1 = (u8) (COM_R_No_1 - 1);
        Vs_Cnt_0 = (s16) (Vs_Cnt_0 + 0xA);
        CpInetTcpAbort(*(u8 *)0x4E36F4);
        CpInetTcpDelete(&D_4E36F4);
        return;
    }
    if (*(s16 *)0x4E3714 == 4) {
        Vs_Cnt_0 = 0;
        Vs_Cnt_1 = 0;
        COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    }
}
