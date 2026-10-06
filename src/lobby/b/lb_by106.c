/* lb_by106 - agent B promoted near-match 0x005BCAC0-0x005BCBA4: Lbs_GuestEnterRoom (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char ClassInfo[];
extern char RoomRule[];
extern char CallBack_Result_Lobby_RoomEntry[];

s32 Lbs_GuestEnterRoom(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0x15;
        F(s8, &ClassInfo, 8) = (s8) (F(u8, &lb_sys, 0x73) + 1);
        cnLBS_RoomEntry(cnLbc_CheckInFloorOrder(2) & 0xFFFF, &RoomRule[2], &CallBack_Result_Lobby_RoomEntry);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        To_EnterRoom();
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}
