#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern s8 mcs_connect_flag;
extern u8 COM_R_No_1;
extern char D_4E36F4[];
extern char D_4E3714[];
void cmcs_00(void) {
    mcs_connect_flag = 0;
    Vs_Cnt_0 = 0;
    Vs_Cnt_1 = 0;
    COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    CpInetTcpAbort(*(s32 *)0x4E36F4);
    CpInetTcpDelete(&D_4E36F4);
    memset(&D_4E3714, 0, 8);
}
