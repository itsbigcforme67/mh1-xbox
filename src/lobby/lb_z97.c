/* lb_z97 - auto-drafted 0x00605730-0x0060580C: set_TR_data_1st, set_TR_position (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

void set_TR_data_1st(u8 *arg0, u8 *arg1) {
    u8 temp_a2;

    if (F(s8, bsw, 0x186) == -0xA) {
        F(u8, arg0, 0x4C) = (u8) F(u8, arg1, 0x4C);
        temp_a2 = F(u8, arg1, 0x4D);
        F(u8, arg1, 0x4D) = (u8) (temp_a2 + 1);
        F(u8, arg0, 0x4D) = temp_a2;
        F(u8, arg0, 0x45) = (u8) F(u8, arg1, 0x45);
        F(u16, arg0, 0x32) = (u16) F(u16, arg1, 0x32);
        F(u16, arg0, 0x30) = (u16) F(u16, arg1, 0x30);
        F(s32, arg0, 0x54) = F(s32, bsw, 0xF18);
        F(u8, arg0, 0x4A) = (u8) F(u8, bsw, 0xF16);
        F(u8, arg0, 0x4B) = (u8) F(u8, bsw, 0xF17);
        F(u8, arg0, 0x46) = (u8) F(u8, arg1, 0x46);
        F(s8, arg0, 0x44) = 0;
        F(s16, arg0, 0x20) = 0;
        F(s16, arg0, 0x1C) = (s16) (F(u16, arg1, 0x1C) - ((F(u8, arg0, 0x45) + F(u16, arg0, 0x32)) * 2));
    }
}

void set_TR_position(u8 *arg0, u8 *arg1) {
    F(s16, arg0, 0x28) = (s16) (F(u16, arg0, 0x32) + (F(u16, arg1, 0x28) + F(u16, arg1, 0x2C)));
    F(s16, arg0, 0x2A) = (s16) (F(u16, arg0, 0x32) + (F(u16, arg1, 0x2A) + F(u16, arg1, 0x2E)));
    F(s16, arg0, 0x2C) = 0;
    F(s16, arg0, 0x2E) = 0;
}
