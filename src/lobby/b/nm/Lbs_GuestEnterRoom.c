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
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0x15;
        F(s8, &ClassInfo, 8) = (s8) (F(u8, &lb_sys, 0x73) + 1);
        cnLBS_RoomEntry(cnLbc_CheckInFloorOrder(2) & 0xFFFF, (int)&RoomRule + 2, &CallBack_Result_Lobby_RoomEntry);
        break;
    case 1:
        Check_CallBackWait(temp_a0);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        To_EnterRoom(temp_a0);
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}
