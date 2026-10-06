#include "lobby_a.h"
extern char CallBack_Result_SendUserMiniData[];
void MH_lobbyClear(void) {
    memset((u8 *)cw + 0x10AC, 0, 0x17E0);
    memset((u8 *)cw + 0x2BFE, 0, 8);
    memset(&lbCommer, 0, 0xB80);
    F(s8, (u8 *)cw, 3) = 0;
    F(s8, (u8 *)cw, 0x2C06) = 0;
    F(s8, (u8 *)cw, 0x32C5) = 0;
    F(s8, (u8 *)cw, 0x2C07) = 0;
    Lb_clearChatList();
    if ((F(u8, &my_user_mini_data, 2) != 0) || (F(u8, &my_user_mini_data, 0x15) != 0)) {
        F(u8, &my_user_mini_data, 0x15) = 0U;
        F(u8, &my_user_mini_data, 2) = 0U;
        cnLBS_Send_UserMiniData(&my_user_mini_data, 0x40, &CallBack_Result_SendUserMiniData);
    }
}
