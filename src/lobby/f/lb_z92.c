/* lb_z92 - auto-drafted 0x00602E70-0x00602F20: tagprintf_cr2 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsw;

s32 tagprintf_cr2(s16 **arg0, s32 arg1) {
    void *temp_a0;
    void *temp_v0;

    temp_a0 = bsw;
    if (F(s8, temp_a0, 0x186) == 1) {
        **arg0 = F(u16, temp_a0, 0xD8C4) + (arg1 & 0xFFFF);
    }
    F(s32, bsw, 0x1C) = 0;
    pos_cr(bsw + 0xD8BC, arg0);
    ck_line_no_add2(arg0);
    set_MAX_Y_SIZE(bsw + 0xD8BC);
    temp_v0 = bsw;
    return (F(u16, temp_v0, 0xD8BC) + (F(u16, temp_v0, 0x12) + F(u16, temp_v0, 0xD8C4))) & 0xFFFF;
}
