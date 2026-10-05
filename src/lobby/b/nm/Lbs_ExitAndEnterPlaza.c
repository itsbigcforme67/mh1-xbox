#include "lobby_a.h"
extern char CallBack_Result_Plaza_PlazaExit2[];
extern char CallBack_Result_Plaza_PlazaEntry2[];
s32 Lbs_ExitAndEnterPlaza(s32 arg0, int arg1, int arg2) {
    u8 temp_a0;
    int temp_a1;
    int temp_v0;

    temp_v0 = (int)cw;
    temp_a0 = F(u8, temp_v0, 0x2C35);
    temp_a1 = temp_v0 + 0x2C35;
    switch (temp_a0) {
    case 0:
        F(u8, temp_v0, 0x2C35) = (u8) (temp_a0 + 1);
        CallBackWaitInit(temp_a0, temp_a1, arg0);
        F(s8, (u8 *)cw, 0x2C45) = 9;
        cnLBS_PlazaExit(&CallBack_Result_Plaza_PlazaExit2);
    default:
block_12:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0, temp_a1, arg0);
        goto block_12;
    case 2:
        F(u8, temp_v0, 0x2C35) = (u8) (temp_a0 + 1);
        CallBackWaitInit(temp_a0, temp_a1, arg0);
        F(s8, (u8 *)cw, 0x2C45) = 4;
        cnLBS_PlazaEntry(arg2 & 0xFFFF, &CallBack_Result_Plaza_PlazaEntry2);
        goto block_12;
    case 3:
        Check_CallBackWait(temp_a0, temp_a1, arg0);
        goto block_12;
    case 4:
        F(u8, temp_v0, 0x2C35) = 0U;
        return 0;
    case 5:
        F(u8, temp_v0, 0x2C35) = 0U;
        return 1;
    }
}
