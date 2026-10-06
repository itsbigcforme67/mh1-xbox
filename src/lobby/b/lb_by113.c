/* lb_by113 - agent B promoted near-match 0x005BD670-0x005BD6FC: To_EnterRoom (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
int cnLBS_Read_RoomJoinUser(u16 id, void *cb);
int cnLBS_Read_MatchEntryJoinUser(u16 id, void *cb);
extern char ClassInfo[];
extern char CallBack_Result_InRoom00_Member[];
extern char CallBack_Result_InRoom00_JoinUser[];
extern char CallBack_Result_InRoom00_EntryJoinUser[];

void To_EnterRoom(void) {
    int temp_s0;

    F(s8, (u8 *)cw, 0x2C35) = 0;
    Init_InterruptFlag();
    F(s16, (u8 *)cw, 0x32C8) = 0;
    F(s16, (u8 *)cw, 0x32C6) = 0;
    F(s8, (u8 *)cw, 0x32C1) = 1;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    F(s16, (u8 *)cw, 0x32CA) = 0;
    temp_s0 = F(u8, &ClassInfo, 8);
    cnLBS_Read_RoomMemberList(temp_s0, &CallBack_Result_InRoom00_Member);
    cnLBS_Read_RoomJoinUser(temp_s0, &CallBack_Result_InRoom00_JoinUser);
    cnLBS_Read_MatchEntryJoinUser(temp_s0, &CallBack_Result_InRoom00_EntryJoinUser);
}
