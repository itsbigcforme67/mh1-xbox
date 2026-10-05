#include "lobby_a.h"
extern char s64[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1[];
extern char temp_a1[];
extern char sp18[];
extern char temp_a1_2[];
extern char temp_a1_2[];
extern char temp_a1[];
void CallBack_Result_Lobby_RoomEntry(int arg0) {
    long long sp18;
    int temp_a1;
    int temp_a1_2;

    temp_a1 = (int)cw;
    sp18 = arg0;
    if ((F(u8, temp_a1, 0x2C31) != 5) && (F(u8, temp_a1, 0x2C45) == 0x15)) {
        F(u8, temp_a1, 0x2C45) = 0U;
        if ((s8) sp18 == 0) {
            temp_a1_2 = (int)cw;
            F(u8, temp_a1_2, 0x2C35) = (u8) (F(u8, temp_a1_2, 0x2C35) + 1);
            F(s8, (u8 *)cw, 0x35D3) = 1;
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 3U;
        cnLBS_Get_ServerMessage((u8 *)cw + 0x32D1, temp_a1, temp_a1 + 0x2C45);
        SetDialogData_HTML((u8 *)cw + 0x32D1);
    }
}
