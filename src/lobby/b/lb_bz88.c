/* lb_bz88 - lobby UI/client 0x005BC660-0x005BC6B0: CallBack_Result_Lobby_SetRoomRule (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
typedef struct { u8 pad0000[0x2C31]; u8 x2C31; u8 pad2C32[0x3]; u8 x2C35; u8 pad2C36[0xF]; u8 x2C45; } CWS_CallBack_Result_Lobby_SetRoomRule;

void CallBack_Result_Lobby_SetRoomRule(void) {
    if ((((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C31 != 5) && (((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C45 == 0x11)) {
        ((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C45 = 0U;
        ((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C35 = (u8) (((CWS_CallBack_Result_Lobby_SetRoomRule *)cw)->x2C35 + 1);
        F(s8, (u8 *)cw, 0x35D3) = 1;
    }
}
