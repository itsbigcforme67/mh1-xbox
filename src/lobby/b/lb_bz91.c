/* lb_bz91 - lobby UI/client 0x005BD0C0-0x005BD1BC: Lbc_SendMiniData (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_Result_SendUserMiniData[];

s32 Lbc_SendMiniData(void) {
    char sp10[0x20];

    Lb_set_mini_data(sp10);
    if (memcmp(sp10, &my_user_mini_data, 0x40) == 0) {
        return 0;
    }
    memcpy((s32)(game_w.master * 0x2FC) + cw + 0x1346, sp10, 0x40);
    memcpy((s32)cw + 0x45A, sp10, 0x40);
    memcpy(&my_user_mini_data, sp10, 0x40);
    memcpy((int)&lbCommer + (game_w.master * 0x5C) + 0x1C, sp10, 0x40);
    cnLBS_Send_UserMiniData(sp10, 0x40, &CallBack_Result_SendUserMiniData);
    F(s8, &lb_sys, 0x78) = 0;
    return 0;
}
