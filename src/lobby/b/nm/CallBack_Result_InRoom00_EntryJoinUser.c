#include "lobby_a.h"
extern char s64[];
extern char sp18[];
extern char sp18[];
extern char temp_v0[];
extern char ClassInfo[];
void CallBack_Result_InRoom00_EntryJoinUser(int arg0) {
    long long sp18;
    int temp_v0;

    sp18 = arg0;
    if ((s8) sp18 == 0) {
        temp_v0 = (int)cw;
        cnLBS_Get_MatchEntryJoinUser(F(u8, &ClassInfo, 8), temp_v0 + 0x32C6, temp_v0 + 0x32C8);
        return;
    }
    F(s16, (u8 *)cw, 0x32C6) = 0;
    F(s16, (u8 *)cw, 0x32C8) = 0;
}
