#include "lobby_a.h"
extern s8 COM_R_No_Disconnect;
extern s8 COMconnect;
extern s8 reset_NG_flag;
extern char Vs_Cnt_0[];
s32 disconnect(void) {
    reset_NG_flag = 1;
    if (InetDisconnectAll(&COM_R_No_Disconnect, &Vs_Cnt_0) == 1) {
        COMconnect = 0;
        COM_R_No_Disconnect = 0;
        return 1;
    }
    return 0;
}
