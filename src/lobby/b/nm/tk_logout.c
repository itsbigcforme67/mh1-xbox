#include "lobby_a.h"
extern u8 COM_R_No_Logout;
extern s8 COM_R_No_Disconnect;
extern char CallBack_Logout_ShutDown[];
typedef struct { u8 pad0000[0x2C45]; s8 x2C45; u8 pad2C46[6]; s32 x2C4C; } CWS_tk_logout;
#define TKCW ((CWS_tk_logout *)cw)

/* logout state machine of the lobby client (near-match, tk_ = "task") */
s32 tk_logout(arg0)
int arg0;
{
    s32 var_s0;
    u8 st;

    var_s0 = 0;
    switch (COM_R_No_Logout) {
    case 0:
        COM_R_No_Logout = COM_R_No_Logout + 1;
        break;
    case 1:
        TKCW->x2C4C = 0x708;
        if (*(u8 *)0x3F35CC != 0) {
            switch (arg0 & 0xFF) {
            case 0:
            case 2:
            case 7:
                st = COM_R_No_Logout + 1;
                COM_R_No_Logout = st;
                TKCW->x2C45 = 0x26;
                cnLBS_LogoutLobbyServer(&CallBack_Logout_ShutDown);
                break;
            case 1:
            case 3:
            case 4:
            case 5:
                st = COM_R_No_Logout + 1;
                COM_R_No_Logout = st;
                TKCW->x2C45 = 0x26;
                cnLBS_ShutDownLobbyServer(&CallBack_Logout_ShutDown);
                break;
            case 6:
                COM_R_No_Logout = 3;
                break;
            }
        } else {
            COM_R_No_Logout = 3;
        }
        break;
    case 2:
        TKCW->x2C4C = TKCW->x2C4C - 1;
        if (CpInetGetStatus() == 0) {
            if (TKCW->x2C4C < 0) {
                COM_R_No_Logout = COM_R_No_Logout + 1;
            }
        } else {
            COM_R_No_Logout = COM_R_No_Logout + 1;
        }
        tk_logout_message_sub(0, arg0);
        break;
    case 3:
        COM_R_No_Logout = COM_R_No_Logout + 1;
        if (*(u8 *)0x3F35CC != 0) {
            TKCW->x2C4C = 0x3C;
        } else {
            TKCW->x2C4C = 1;
        }
        tk_logout_message_sub(0, arg0);
        break;
    case 4:
        TKCW->x2C4C = TKCW->x2C4C - 1;
        if (TKCW->x2C4C < 0) {
            COM_R_No_Disconnect = 0;
            COM_R_No_Logout = COM_R_No_Logout + 1;
        }
        tk_logout_message_sub(0, arg0);
        break;
    case 5:
        switch (arg0 & 0xFF) {
        case 0:
        case 2:
        case 7:
            var_s0 = 1;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            if (disconnect() != 0) {
                var_s0 = 1;
            } else {
                tk_logout_message_sub(1, arg0);
            }
            break;
        }
        break;
    }
    return var_s0;
}
