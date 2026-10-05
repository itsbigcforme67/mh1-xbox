#include "lobby_a.h"
extern char s64[];
extern char temp_a2[];
extern char sp30[];
extern char temp_a2[];
extern char sp30[];
extern char unksp31[];
extern char unksp31[];
extern char sp30[];
extern char temp_a2[];
extern char temp_v1[];
extern char temp_v1[];
extern char unksp31[];
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
extern char temp_a3[];
extern char temp_a3[];
extern char temp_a3[];
extern char LobbyInfo[];
void CallBack_Result_Plaza_ReadLobbyAllocation(int arg0) {
    int sp3C;
    long long sp30;
    int var_s1;
    s16 temp_v0;
    s32 var_s0;
    int temp_a2;
    int temp_a3;
    int temp_v1;

    temp_a2 = (int)cw;
    sp30 = arg0;
    if ((F(u8, temp_a2, 0x2C31) != 5) && (temp_a3 = temp_a2 + 0x2C45, (F(u8, temp_a2, 0x2C45) == 0xB))) {
        switch ((s8) sp30) {                        /* irregular */
        case 2:
            if (((s8)unksp31) == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp3C, unksp31, (s8) sp30, temp_a3);
                return;
            }
            if ((s8) sp30 == 0) {
                F(u8, temp_a2, 0x2C45) = 0U;
                temp_v1 = (int)cw;
                F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
                fade_set(1, unksp31, (s8) sp30, temp_a3);
                cnLBS_Get_LobbyCount((int)&ClassInfo + 6);
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
            break;
        }
    }
}
