/* lb_bz94 - lobby UI/client 0x005BE3D0-0x005BE44C: lbc_matching_failed_02 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;

void lbc_matching_failed_02(void) {
    s32 temp_a0;
    s32 temp_a2;
    void *temp_a1;
    void *temp_v1;

    temp_a2 = Get_sw2(0) & 0xFFFF;
    F(s8, pNet, 0xC) = 1;
    temp_a1 = (u8 *)cw;
    temp_a0 = F(s32, temp_a1, 0x2C4C);
    F(s32, temp_a1, 0x2C4C) = (temp_a0 + 1);
    if ((temp_a0 >= 0x12C) || ((F(s32, (u8 *)cw, 0x2C4C) >= 0x3C) && ((u16)temp_a2 & 0x60))) {
        temp_v1 = (u8 *)cw;
        F(u8, temp_v1, 0x2C33) = (u8) (F(u8, temp_v1, 0x2C33) + 1);
        fade_set(1, temp_a1, temp_a2);
    }
}
