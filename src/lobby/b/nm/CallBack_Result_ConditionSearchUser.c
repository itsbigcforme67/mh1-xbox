#include "lobby_a.h"
extern char temp_a1[];
extern char sp8[];
extern char temp_a1[];
extern char sp8[];
extern char var_v1[];
extern char var_v1[];
extern char temp_a1[];
void CallBack_Result_ConditionSearchUser(int arg0) {
    long long sp8;
    u8 var_v1;
    int temp_a1;

    temp_a1 = (int)cw;
    sp8 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0xA)) {
        if ((s8) sp8 == 0) {
            var_v1 = F(u8, temp_a1, 0x2C35) + 1;
        } else {
            var_v1 = 3;
        }
        F(u8, temp_a1, 0x2C35) = var_v1;
    }
}
