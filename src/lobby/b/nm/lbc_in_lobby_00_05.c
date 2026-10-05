#include "lobby_a.h"
extern char RoomInfo[];
extern char ClassInfo[];
extern char ClassInfo[];
void lbc_in_lobby_00_05(void) {
    s32 temp_a1;
    s8 temp_a2;
    u8 temp_v1;
    int temp_a0;
    int temp_a3;

    temp_a0 = (int)cw;
    F(s32, temp_a0, 0x2C4C) = (F(s32, temp_a0, 0x2C4C) - 1);
    temp_a3 = (int)cw;
    if (F(s32, temp_a3, 0x2C4C) < 0) {
        temp_a2 = F(s8, temp_a3, 0x32C3);
        temp_a1 = (temp_a2 + F(s8, temp_a3, 0x32C4)) * 0x15C;
        temp_v1 = (*(int *)((u8 *)&RoomInfo + 0x10 + temp_a1));
        switch (temp_v1) {
        case 3:
            F(s8, &ClassInfo, 8) = (s8) (*(int *)((u8 *)&RoomInfo + temp_a1));
            F(s8, temp_a3, 0x2C33) = 2;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            F(s8, (u8 *)cw, 0x2C36) = 0;
            return;
        case 1:
            F(s8, &ClassInfo, 8) = (s8) (*(int *)((u8 *)&RoomInfo + temp_a1));
            F(s8, temp_a3, 0x2C33) = 1;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            F(s8, (u8 *)cw, 0x2C35) = 0;
            F(s8, (u8 *)cw, 0x2C36) = 0;
            F(s8, (u8 *)cw, 0x32C2) = 0;
            return;
        default:
        case 0:
        case 2:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            cnLBS_Get_ServerMessage(temp_a3 + 0x32D1, temp_a1, temp_a2, temp_a3);
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            F(s32, &lb_sys, 0x6C) = 1;
            F(s32, &lb_sys, 0x68) = 0x17;
            break;
        }
    }
}
