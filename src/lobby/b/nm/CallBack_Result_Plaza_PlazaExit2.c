#include "lobby_a.h"
extern char s64[];
extern char sp18[];
extern char temp_a0[];
extern char temp_a0[];
extern char temp_a0[];
extern char sp18[];
extern char temp_v1[];
extern char temp_v1[];
extern char temp_a0_2[];
void CallBack_Result_Plaza_PlazaExit2(int arg0) {
    long long sp18;
    u8 temp_a0_2;
    int temp_a0;
    int temp_v1;

    sp18 = arg0;
    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (temp_a0_2 = F(u8, temp_a0, 0x2C45), (temp_a0_2 == 9))) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if ((s8) sp18 == 0) {
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C35) = (u8) (F(u8, temp_v1, 0x2C35) + 1);
            Init_InterruptFlag(temp_a0_2, 5, temp_a0 + 0x2C45);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 5U;
    }
}
