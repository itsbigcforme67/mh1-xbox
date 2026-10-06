#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char temp_a1[];
extern char temp_a1[];
void CallBack_Result_LoginPersonalDataRegist2(int arg0) {
    long long sp18;
    int temp_a1;

    temp_a1 = (int)cw;
    sp18 = arg0;
    if (F(u8, temp_a1, 0x2C31) != 5) {
        if ((s8) sp18 == 0) {
            F(u8, temp_a1, 0x2C35) = (u8) (F(u8, temp_a1, 0x2C35) + 1);
            return;
        }
        F(u8, temp_a1, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
    }
}
