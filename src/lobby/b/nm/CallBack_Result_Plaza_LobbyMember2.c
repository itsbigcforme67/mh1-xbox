#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp38[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp38[];
extern char var_s1[];
extern char var_s0[];
extern char var_s1[];
extern char var_s1[];
extern char var_s0[];
extern char var_s1[];
extern char temp_a0[];
extern char temp_a0[];
extern char tl_member_buff[];
void CallBack_Result_Plaza_LobbyMember2(int arg0) {
    long long sp38;
    int var_s0;
    s32 var_s1;
    int temp_a0;
    int temp_a1;

    temp_a1 = (int)cw;
    sp38 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xA)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if ((s8) sp38 == 0) {
            var_s1 = 0;
            var_s0 = (int)&tl_member_buff;
            do {
                cnLBS_Get_LobbyMemberList(var_s1 & 0xFFFF, var_s0 + 0x280, var_s0 + 0x288, var_s0 + 0x29A);
                var_s1 += 1;
                var_s0 += 0x2FC;
            } while (var_s1 < 8);
        }
        temp_a0 = (int)pNet;
        F(u8, temp_a0, 0x12) = (u8) (F(u8, temp_a0, 0x12) + 1);
    }
}
