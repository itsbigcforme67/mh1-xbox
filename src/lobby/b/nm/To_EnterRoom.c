#include "lobby_a.h"
extern char ClassInfo[];
extern char CallBack_Result_InRoom00_Member[];
extern char CallBack_Result_InRoom00_JoinUser[];
extern char CallBack_Result_InRoom00_EntryJoinUser[];
void To_EnterRoom(void) {
    u8 temp_s0;

    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    F(s16, (u8 *)cw, 0x32C8) = 0;
    F(s16, (u8 *)cw, 0x32C6) = 0;
    F(s8, (u8 *)cw, 0x32C1) = 1;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    F(s16, (u8 *)cw, 0x32CA) = 0;
    temp_s0 = F(u8, &ClassInfo, 8);
    cnLBS_Read_RoomMemberList(temp_s0, &CallBack_Result_InRoom00_Member);
    cnLBS_Read_RoomJoinUser(temp_s0 & 0xFFFF, &CallBack_Result_InRoom00_JoinUser);
    cnLBS_Read_MatchEntryJoinUser(temp_s0 & 0xFFFF, &CallBack_Result_InRoom00_EntryJoinUser);
}
