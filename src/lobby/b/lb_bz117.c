/* lb_bz117 - lobby UI/client 0x005BD3B0-0x005BD434: CallBack_Result_Lobby_LobbyExit (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_LobbyExit;

void CallBack_Result_Lobby_LobbyExit(CNET_RES res) {
    if ((((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C45 == 0x16)) {
        ((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            ((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_LobbyExit *)cw)->x2C35 + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x2C32) = 0;
        F(s8, (u8 *)cw, 0x2C33) = 0;
        F(s8, (u8 *)cw, 0x2C34) = 0;
        cnLbc_EraseDialog(0x4C);
    }
}
