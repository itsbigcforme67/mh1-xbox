/* lb_bz118 - lobby UI/client 0x005BD5F0-0x005BD66C: CallBack_Result_Lobby_RoomExit (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_RoomExit;

void CallBack_Result_Lobby_RoomExit(CNET_RES res) {
    if ((((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C45 == 0x18)) {
        ((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C45 = 0U;
        if (res.val == 0) {
            F(s8, (u8 *)cw, 0x35D3) = 0;
            ((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_RoomExit *)cw)->x2C35 + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x35D3) = 0;
        To_LogOut(4);
    }
}
