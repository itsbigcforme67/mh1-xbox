#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp38[];
extern char temp_a1[];
extern char temp_a1[];
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
extern char PlazaInfo[];
void CallBack_Result_Plaza_ReadAllocation(int arg0) {
    long long sp38;
    int var_s1;
    s16 temp_v0;
    s32 var_s0;
    int temp_a1;

    temp_a1 = (int)cw;
    sp38 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 6) && ((s8) sp38 != 2) && ((s8) sp38 == 0)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        F(s8, (u8 *)cw, 0x2C33) = 1;
        F(s8, (u8 *)cw, 0x2C34) = 0;
        cnLBS_Get_PlazaCount((int)&ClassInfo + 2, temp_a1, temp_a1 + 0x2C45);
        var_s0 = 0;
        if (F(u16, &ClassInfo, 2) > 0) {
            var_s1 = (int)&PlazaInfo;
            do {
                temp_v0 = var_s0 + 1;
                (*(s16 *)var_s1) = temp_v0;
                cnLBS_Get_PlazaStatus(temp_v0 & 0xFFFF, var_s1 + 0x10);
                cnLBS_Get_mhPlazaJoinUser((var_s0 + 1) & 0xFFFF, var_s1 + 2, var_s1 + 0xE);
                cnLBS_Get_PlazaName((var_s0 + 1) & 0xFFFF, var_s1 + 0x14);
                var_s0 += 1;
                var_s1 += 0x15C;
            } while (var_s0 < F(u16, &ClassInfo, 2));
        }
    }
}
