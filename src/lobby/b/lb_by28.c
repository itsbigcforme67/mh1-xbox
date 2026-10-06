/* lb_by28 - agent B promoted near-match 0x005BBA30-0x005BBB64: Lbc_ReadRoomInfo (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char RoomInfo[];
extern char CallBack_Result_Lobby_ReadRoomAllocation[];

s32 Lbc_ReadRoomInfo(void) {
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        memset(&RoomInfo, 0, 0xAE0);
        F(s16, &RoomInfo, 0) = 1;
        F(s16, &RoomInfo, 0x15C) = 2;
        F(s16, &RoomInfo, 0x2B8) = 3;
        F(s16, &RoomInfo, 0x414) = 4;
        F(s16, &RoomInfo, 0x570) = 5;
        F(s16, &RoomInfo, 0x6CC) = 6;
        F(s16, &RoomInfo, 0x828) = 7;
        F(s16, &RoomInfo, 0x984) = 8;
        CallBackWaitInit();
        F(s8, (u8 *)cw, 0x2C45) = 0xE;
        cnLBS_Read_RoomAllocation(0, 0xBB, &CallBack_Result_Lobby_ReadRoomAllocation);
        break;
    case 1:
        Check_CallBackWait(temp_v1 + 0x2C35);
        break;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
    return 2;
}
