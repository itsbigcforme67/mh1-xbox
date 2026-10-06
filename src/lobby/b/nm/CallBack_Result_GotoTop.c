#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
void CallBack_Result_GotoTop(int arg0) {
    long long sp18;
    int temp_a1;

    temp_a1 = (int)cw;
    sp18 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x22)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if ((s8) sp18 == 0) {
            F(u8, (u8 *)cw, 0x2C31) = 1U;
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 0;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            return;
        }
        To_LogOut(4, temp_a1, temp_a1 + 0x2C45);
    }
}
