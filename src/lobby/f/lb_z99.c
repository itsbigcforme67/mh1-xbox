/* lb_z99 - auto-drafted 0x00608C10-0x00608CD4: tagprintf_t_cr2 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsw;

s32 tagprintf_t_cr2(u8 *arg0, s16 **arg1, s32 arg2) {
    void *temp_a0;
    void *temp_v0;

    temp_a0 = bsw;
    if (F(s8, temp_a0, 0x186) == 1) {
        **arg1 = F(u16, temp_a0, 0xD8D6) + (arg2 & 0xFFFF);
    }
    F(s32, bsw, 0x1C) = 0;
    pos_cr(bsw + 0xD8CE, arg1);
    ck_line_no_add2(arg1);
    set_MAX_Y_SIZE(bsw + 0xD8CE);
    set_align_data(arg0);
    temp_v0 = bsw;
    return (F(u16, temp_v0, 0xD8CE) + (F(u16, arg0, 0x3E) + F(u16, temp_v0, 0xD8D6))) & 0xFFFF;
}
