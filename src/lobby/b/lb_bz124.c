/* lb_bz124 - lobby UI/client 0x005BE350-0x005BE3C8: lbc_matching_failed_01 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void lbc_matching_failed_01(void) {
    int temp_a0;
    int temp_v1;

    temp_a0 = (int)cw;
    F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    F(s32, (u8 *)cw, 0x2C4C) = 0;
    F(s8, pNet, 0xC) = 1;
    temp_v1 = (int)cw;
    if (F(u8, temp_v1, 0x2C32) == 0) {
        SetDialogData(0x33, 3);
    } else {
        SetDialogData_HTML(temp_v1 + 0x32D1);
    }
    F(s32, (u8 *)cw, 0x2C4C) = 0x14;
    fade_set(2);
}
