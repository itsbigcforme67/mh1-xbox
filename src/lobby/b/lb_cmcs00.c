/* lb_cmcs00 - agent C 0x005B6A60-0x005B6AB8: cmcs_00. */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern s8 mcs_connect_flag;
extern u8 COM_R_No_1;
extern s32 ConnWork[];
void cmcs_00(void) {
    Vs_Cnt_0 = 0;
    Vs_Cnt_1 = 0;
    mcs_connect_flag = 0;
    COM_R_No_1 = (u8) (COM_R_No_1 + 1);
    CpInetTcpAbort(ConnWork[1]);
    CpInetTcpDelete(&ConnWork[1]);
    memset((u8 *)ConnWork + 0x24, 0, 8);
}
