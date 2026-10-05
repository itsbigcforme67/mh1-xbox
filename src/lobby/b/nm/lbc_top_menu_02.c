#include "lobby_a.h"
extern char CallBack_Result_Plaza_PlazaEntry[];
void lbc_top_menu_02(void) {
    s32 temp_a1;
    u8 temp_a0_2;
    int temp_a0;

    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2C34;
    temp_a0_2 = F(u8, temp_a0, 0x2C34);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C34) = (u8) (temp_a0_2 + 1);
        CallBackWaitInit(temp_a0_2, temp_a1);
        F(s8, (u8 *)cw, 0x2C45) = 4;
        cnLBS_PlazaEntry(cnLbc_CheckInFloorOrder(0) & 0xFFFF, &CallBack_Result_Plaza_PlazaEntry);
        return;
    case 1:
        Check_CallBackWait(temp_a0_2, temp_a1);
        return;
    }
}
