#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a0[];
extern char temp_a0[];
extern char temp_v1[];
extern char temp_v1[];
extern char temp_a1[];
void CallBack_SendMiniData(int arg0) {
    long long sp18;
    int temp_a0;
    int temp_a1;
    int temp_v1;

    temp_a1 = (int)cw;
    sp18 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 2)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if ((s8) sp18 == 0) {
            memcpy((u8 *)cw + 0x45A, &my_user_mini_data, 0x40);
            temp_a0 = (int)cw;
            F(u8, temp_a0, 0x2C34) = (u8) (F(u8, temp_a0, 0x2C34) + 1);
            return;
        }
        temp_v1 = (int)cw;
        F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
    }
}
