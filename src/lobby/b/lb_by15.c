/* lb_by15 - agent B promoted near-match 0x005B9EF0-0x005B9FD0: CallBack_Result_Plaza_PlazaExit2, CallBack_Result_Plaza_PlazaEntry2 (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Result_Plaza_PlazaExit2(CNET_RES res) {
    u8 temp_a0_2;
    int temp_a0;
    int temp_v1;

    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (temp_a0_2 = F(u8, temp_a0, 0x2C45), (temp_a0_2 == 9))) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_v1 = (int)cw;
            F(u8, temp_v1, 0x2C35) = (u8) (F(u8, temp_v1, 0x2C35) + 1);
            Init_InterruptFlag(temp_a0_2, 5, temp_a0 + 0x2C45);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 5U;
    }
}

void CallBack_Result_Plaza_PlazaEntry2(CNET_RES res) {
    int temp_a0;
    int temp_a0_2;
    temp_a0 = (int)cw;
    if ((F(u8, temp_a0, 0x2C31) != 5) && (F(u8, temp_a0, 0x2C45) == 4)) {
        F(u8, temp_a0, 0x2C45) = 0U;
        if (res.val == 0) {
            temp_a0_2 = (int)cw;
            F(u8, temp_a0_2, 0x2C35) = (u8) (F(u8, temp_a0_2, 0x2C35) + 1);
            return;
        }
        F(u8, (u8 *)cw, 0x2C35) = 5U;
    }
}
