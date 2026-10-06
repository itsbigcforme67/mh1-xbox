#include "lobby_a.h"
extern char CallBack_Result_SetRoomPropaty[];
s32 Lbc_SetPropaty(s32 arg0, int arg1) {
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
        F(s8, (u8 *)cw, 0x2C45) = 0x1B;
        cnLBS_Set_RoomProperty(arg1, &CallBack_Result_SetRoomPropaty);
block_11:
    default:
        return 2;
    case 1:
        Check_CallBackWait();
        goto block_11;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
}
