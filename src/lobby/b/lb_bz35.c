/* lb_bz35 - lobby UI/client 0x005BB6F0-0x005BB7A4: To_EnterLobby (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char RoomInfo[];
extern char ClassInfo[];
extern char CallBack_Result_Lobby_JoinUser[];

void To_EnterLobby(void) {
    F(s8, (u8 *)cw, 0x2C31) = 3;
    F(s8, (u8 *)cw, 0x2C32) = 0;
    F(s8, (u8 *)cw, 0x2C33) = 0;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    memset(&RoomInfo, 0, 0xAE0);
    F(s8, &ClassInfo, 8) = 0;
    F(s8, (u8 *)cw, 0x32C0) = 0;
    F(s8, (u8 *)cw, 0x32C1) = 0;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    F(s8, (u8 *)cw, 0x35D3) = 0;
    F(s8, (u8 *)cw, 0x35D4) = 0;
    F(s8, &lb_sys, 0x70) = 0;
    Plaza_chat_clear();
    Lb_clearChatList();
    cnLBS_Read_LobbyJoinUser(cnLbc_CheckInFloorOrder(1) & 0xFFFF, &CallBack_Result_Lobby_JoinUser);
}
