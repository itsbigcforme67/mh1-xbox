#include "lobby_a.h"
extern char s64[];
extern char sp18[];
extern char temp_a0[];
extern char temp_a0[];
extern char temp_a0[];
extern char sp18[];
extern char temp_a0[];
void CallBack_Result_Plaza_LobbyEntry(int arg0) {
    long long sp18;
    int temp_a0;

    sp18 = arg0;
    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (F(u8, temp_a0, 0x2C45) == 8)) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if ((s8) sp18 == 0) {
            F(s8, (u8 *)cw, 0x2C35) = 2;
            F(s8, (u8 *)cw, 0x35D5) = 1;
            return;
        }
        F(s8, (u8 *)cw, 0x2C35) = 5;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, 5, temp_a0 + 0x2C45);
    }
}
