/* lb_by31 - agent B promoted near-match 0x005BD530-0x005BD5EC: Lbs_RoomExit (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_Result_Lobby_RoomExit[];

s32 Lbs_RoomExit(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;
    int temp_v1_3;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        /* fallthrough */
    case 1:
        temp_v1_3 = (int)cw;
        F(u8, temp_v1_3, 0x2C35) = (u8) (F(u8, temp_v1_3, 0x2C35) + 1);
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0x18;
        cnLBS_RoomExit(&CallBack_Result_Lobby_RoomExit);
        break;
    case 2:
        Check_CallBackWait(temp_a0);
        break;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 0;
}
