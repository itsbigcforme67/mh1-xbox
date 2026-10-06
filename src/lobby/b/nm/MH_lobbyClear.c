#include "lobby_a.h"
extern char CallBack_Result_SendUserMiniData[];
void MH_lobbyClear(void) {
    u8 *m;

    memset((u8 *)cw + 0x10AC, 0, 0x17E0);
    memset((u8 *)cw + 0x2BFE, 0, 8);
    memset(&lbCommer, 0, 0xB80);
    F(s8, (u8 *)cw, 3) = 0;
    F(s8, (u8 *)cw, 0x2C06) = 0;
    F(s8, (u8 *)cw, 0x32C5) = 0;
    F(s8, (u8 *)cw, 0x2C07) = 0;
    Lb_clearChatList();
    m = (u8 *)&my_user_mini_data;
    if ((m[2] != 0) || (m[0x15] != 0)) {
        m[0x15] = 0;
        m[2] = 0;
        cnLBS_Send_UserMiniData(m, 0x40, &CallBack_Result_SendUserMiniData);
    }
}
