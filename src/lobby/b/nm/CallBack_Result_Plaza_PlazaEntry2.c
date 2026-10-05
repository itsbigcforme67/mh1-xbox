#include "lobby_a.h"
extern char s64[];
extern char sp8[];
extern char temp_a0[];
extern char temp_a0[];
extern char temp_a0[];
extern char sp8[];
extern char temp_a0_2[];
extern char temp_a0_2[];
void CallBack_Result_Plaza_PlazaEntry2(int arg0) {
    long long sp8;
    int temp_a0;
    int temp_a0_2;

    sp8 = arg0;
    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (F(u8, temp_a0, 0x2C45) == 4)) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if ((s8) sp8 == 0) {
            temp_a0_2 = (int)cw;
            F(u8, temp_a0_2, 0x2C35) = (u8) (F(u8, temp_a0_2, 0x2C35) + 1);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 5U;
    }
}
