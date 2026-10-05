#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp8[];
extern char temp_a1[];
extern char sp8[];
extern char var_v1[];
extern char var_v1[];
extern char temp_a1[];
void CallBack_Result_LoginTopInformation(int arg0) {
    long long sp8;
    u8 var_v1;
    int temp_a1;

    temp_a1 = (int)cw;
    sp8 = arg0;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        if ((s8) sp8 == 0) {
            var_v1 = F(u8, temp_a1, 0x2C34) + 1;
        } else {
            var_v1 = 7;
        }
        F(u8, temp_a1, 0x2C34) = var_v1;
    }
}
