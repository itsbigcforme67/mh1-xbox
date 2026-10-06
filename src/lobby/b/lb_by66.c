/* lb_by66 - agent B promoted near-match 0x005BAD50-0x005BAE2C: lbc_in_plaza_03 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char CallBack_Result_Plaza_PlazaExit[];

void lbc_in_plaza_03(void) {
    u8 var_a0;
    int temp_a1;
    int temp_a2;

    temp_a2 = (int)cw;
    var_a0 = F(u8, temp_a2, 0x2C34);
    temp_a1 = temp_a2 + 0x2C34;
    switch (var_a0) {                               /* irregular */
    case 0:
        F(u8, temp_a2, 0x2C34) = (u8) (var_a0 + 1);
        Lbc_init_network_work(var_a0, temp_a1, temp_a2);
        F(s32, (u8 *)cw, 0x2C4C) = 0x44;
        SetDialogData_HTML((u8 *)cw + 0x32D1);
        /* fallthrough */
    case 1:
        F(s8, pNet, 0xC) = 1;
        F(s32, (u8 *)cw, 0x2C4C) = (F(s32, (u8 *)cw, 0x2C4C) - 1);
        if (F(s32, (u8 *)cw, 0x2C4C) <= 0) {
        case 2:
            F(u8, (u8 *)cw, 0x2C34) = (u8) (F(u8, (u8 *)cw, 0x2C34) + 1);
            CallBackWaitInit();
            F(s8, (u8 *)cw, 0x2C45) = 9;
            cnLBS_PlazaExit(&CallBack_Result_Plaza_PlazaExit);
            return;
        }
        break;
    case 3:
        Check_CallBackWait();
    }
}
