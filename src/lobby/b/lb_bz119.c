/* lb_bz119 - lobby UI/client 0x005BD820-0x005BD874: CallBack_Result_InRoom00_EntryJoinUser (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern char s64[];
extern char ClassInfo[];

void CallBack_Result_InRoom00_EntryJoinUser(CNET_RES res) {
    u8 *temp_v0;
    if (res.val == 0) {
        temp_v0 = (u8 *)cw;
        cnLBS_Get_MatchEntryJoinUser(F(u8, &ClassInfo, 8), temp_v0 + 0x32C6, temp_v0 + 0x32C8);
        return;
    }
    F(s16, (u8 *)cw, 0x32C6) = 0;
    F(s16, (u8 *)cw, 0x32C8) = 0;
}
