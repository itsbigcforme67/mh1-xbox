#include "lobby_a.h"
extern char s64[];
extern char sp38[];
extern char sp38[];
extern char ClassInfo[];
extern char var_s0[];
extern char ClassInfo[];
extern char var_s1[];
extern char temp_v0[];
extern char var_s1[];
extern char temp_v0[];
extern char var_s0[];
extern char var_s0[];
extern char var_s0[];
extern char var_s1[];
extern char var_s0[];
extern char unksp39[];
extern char unksp39[];
extern char LobbyInfo[];
void CallBack_Result_Plaza_ReadLobbyAllocation2(int arg0) {
    long long sp38;
    int var_s1;
    s16 temp_v0;
    s32 var_s0;

    sp38 = arg0;
    if ((F(u8, (u8 *)cw, 0x2C31) != 5) && (((s8) sp38 != 2) || (((s8)unksp39) != 0xB)) && ((s8) sp38 == 0)) {
        cnLBS_Get_LobbyCount((int)&ClassInfo + 6, unksp39, (s8) sp38);
        var_s0 = 0;
        if (F(u16, &ClassInfo, 6) > 0) {
            var_s1 = (int)&LobbyInfo;
            do {
                temp_v0 = var_s0 + 1;
                (*(s16 *)var_s1) = temp_v0;
                cnLBS_Get_LobbyStatus(temp_v0 & 0xFFFF, var_s1 + 0x10);
                cnLBS_Get_mhLobbyJoinUser((var_s0 + 1) & 0xFFFF, var_s1 + 2, var_s1 + 0xE);
                cnLBS_Get_LobbyName((var_s0 + 1) & 0xFFFF, var_s1 + 0x14);
                var_s0 += 1;
                var_s1 += 0x15C;
            } while (var_s0 < F(u16, &ClassInfo, 6));
        }
    }
}
