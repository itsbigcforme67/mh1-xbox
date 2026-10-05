/* lb_bz140 - lobby UI/client 0x005BA480-0x005BA544: lbc_in_plaza_00 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_Result_Plaza_ReadLobbyAllocation[];

void lbc_in_plaza_00(void) {
    s32 temp_a2;
    u8 temp_a0_2;
    int temp_a0;

    F(s8, pNet, 0xC) = 1;
    temp_a0 = (int)cw;
    temp_a2 = temp_a0 + 0x2C34;
    temp_a0_2 = F(u8, temp_a0, 0x2C34);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C34) = (u8) (temp_a0_2 + 1);
        CallBackWaitInit(temp_a0_2, 1, temp_a2);
        F(s8, (u8 *)cw, 0x2C45) = 0xB;
        cnLBS_Read_LobbyAllocation(0, 7, &CallBack_Result_Plaza_ReadLobbyAllocation);
        return;
    case 1:
        Check_CallBackWait(temp_a0_2, 1, temp_a2);
        return;
    case 2:
        if ((Fade_busy_ck(temp_a0_2, 1, temp_a2) & 0xFF) != 1) {
            F(s8, (u8 *)cw, 0x2C33) = 1;
            F(u8, (u8 *)cw, 0x2C34) = 0U;
        }
    }
}
