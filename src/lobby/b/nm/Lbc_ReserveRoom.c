#include "lobby_a.h"
extern char RoomInfo[];
extern char ClassInfo[];
extern char CallBack_Result_Lobby_RoomCreate[];
s32 Lbc_ReserveRoom(void) {
    int var_a0;
    s8 var_a2;
    u8 temp_a0;
    int temp_a1;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = F(u8, temp_v1, 0x2C35);
    temp_a1 = temp_v1 + 0x2C35;
    switch (temp_a0) {                              /* irregular */
    case 0:
        var_a2 = 0;
        var_a0 = (int)&RoomInfo;
loop_6:
        if (F(u8, var_a0, 0x10) == 1) {
            F(s8, &ClassInfo, 8) = (s8) (*(int *)((u8 *)&RoomInfo + (var_a2 * 0x15C)));
            F(s8, &lb_sys, 0x73) = var_a2;
        } else {
            var_a2 += 1;
            var_a0 += 0x15C;
            if (var_a2 < 8) {
                goto loop_6;
            }
        }
        F(u8, temp_v1, 0x2C35) = (u8) (F(u8, temp_v1, 0x2C35) + 1);
        CallBackWaitInit(var_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0xF;
        cnLBS_RoomCreate(cnLbc_CheckInFloorOrder(2) & 0xFFFF, &CallBack_Result_Lobby_RoomCreate);
block_16:
    default:
        return 2;
    case 1:
        Check_CallBackWait();
        goto block_16;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
}
