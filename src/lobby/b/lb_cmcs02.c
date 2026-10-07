/* lb_cmcs02 - agent C 0x005B6B80-0x005B6C64: cmcs_02 (ConnWork.sock/st instead of literal 4E36F4, COM_R_No_1 decremented before Vs_Cnt_0 store). */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s16 Vs_Cnt_1;
extern u8 COM_R_No_1;
typedef struct { s32 x00; s32 sock; u8 pad08[0x1C]; s16 st; u8 pad26[6]; } CONNW;
extern CONNW ConnWork;
void cmcs_02(void) {
    s16 temp_v0;

    temp_v0 = Vs_Cnt_0 - 1;
    Vs_Cnt_0 = temp_v0;
    if (((s16)temp_v0) <= 0) {
        COM_R_No_1 = COM_R_No_1 - 1;
        Vs_Cnt_0 = 0xA;
        CpInetTcpAbort(ConnWork.sock);
        CpInetTcpDelete(&ConnWork.sock);
        return;
    }
    if (CpInetTcpGetStatus(ConnWork.sock, &ConnWork.st) < 0) {
        COM_R_No_1 = COM_R_No_1 - 1;
        Vs_Cnt_0 = Vs_Cnt_0 + 0xA;
        CpInetTcpAbort(ConnWork.sock);
        CpInetTcpDelete(&ConnWork.sock);
        return;
    }
    if (ConnWork.st == 4) {
        Vs_Cnt_0 = 0;
        Vs_Cnt_1 = 0;
        COM_R_No_1 = COM_R_No_1 + 1;
    }
}
