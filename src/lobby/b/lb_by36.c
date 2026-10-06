/* lb_by36 - agent B promoted near-match 0x005B6F20-0x005B701C: internet_disconnect (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern s16 Vs_Cnt_0;
extern s8 COM_RET;
extern s8 COM_R_No_3;
extern s8 COM_R_No_Disconnect;
extern s8 PPP_ErrorStatus;
extern u8 COM_R_No_1;
extern u8 MMBB_LOGIN;
extern char D_4E36F4[];

s32 internet_disconnect(void) {
    s16 temp_v0;
    s32 var_s0;
    u8 temp_v1;

    temp_v1 = COM_R_No_1;
    var_s0 = 0;
    switch (temp_v1) {                              /* irregular */
    case 0:
        COM_R_No_1 = (u8) (temp_v1 + 1);
        break;
    case 1:
        Vs_Cnt_0 = 1;
        COM_R_No_3 = 0;
        COM_R_No_1 = (u8) (temp_v1 + 1);
        COM_R_No_Disconnect = 0;
        break;
    case 2:
        temp_v0 = Vs_Cnt_0 - 1;
        Vs_Cnt_0 = temp_v0;
        if (((s16)temp_v0) < 0) {
            COM_R_No_1 = (u8) (temp_v1 + 1);
        }
        break;
    case 3:
        if (MMBB_LOGIN != 0) {
            CpInetTcpAbort(*(s32 *)0x4E36F4);
            CpInetTcpDelete(&D_4E36F4);
            var_s0 = 1;
            COM_RET = (s8) PPP_ErrorStatus;
            PPP_ErrorStatus = 0;
        } else if (disconnect() != 0) {
            var_s0 = 1;
            COM_RET = (s8) PPP_ErrorStatus;
            PPP_ErrorStatus = 0;
        }
        break;
    }
    return var_s0;
}
