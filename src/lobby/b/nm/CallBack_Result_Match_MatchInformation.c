#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a0[];
extern char temp_a0_2[];
extern char temp_a0_2[];
extern char temp_a1[];
extern char temp_a2[];
extern char temp_a2[];
void CallBack_Result_Match_MatchInformation(int arg0) {
    long long sp18;
    u8 temp_a0;
    int temp_a0_2;
    int temp_a1;
    int temp_a2;

    temp_a1 = (int)cw;
    sp18 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (temp_a0 = F(u8, temp_a1, 0x2C45), temp_a2 = temp_a1 + 0x2C45, (temp_a0 == 0x20))) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if ((s8) sp18 == 0) {
            all_reset(temp_a0, temp_a1, temp_a2);
            temp_a0_2 = (int)cw;
            F(u8, temp_a0_2, 0x2C34) = (u8) (F(u8, temp_a0_2, 0x2C34) + 1);
            return;
        }
        F(s8, (u8 *)cw, 0x2C0D) = 1;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a2);
    }
}
