/* lb_z93 - auto-drafted 0x00602F20-0x0060301C: tag_button_x_over_check (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * bsw;

void tag_button_x_over_check(u16 *arg0, s16 *arg1, s32 arg2, s32 *arg3) {
    s32 temp_a2;
    u16 temp_a1;
    void *temp_t0;
    void *temp_v1;

    temp_t0 = bsw;
    if (F(u8, temp_t0, 0xD892) != 0) {
        tag_button_x_over_check_t();
        return;
    }
    temp_a1 = *arg0;
    temp_a2 = (temp_a1 + (arg2 & 0xFFFF)) & 0xFFFF;
    if (F(s8, temp_t0, 0x186) == 0) {
        *arg0 += (F(u16, bsw, 0xD8C4) + (get_center_data(*arg3, temp_a1, temp_a2) & 0xFFFF)) & 0xFFFF;
        return;
    }
    if ((F(u16, temp_t0, 0xD8BC) != 0) && (F(u16, temp_t0, 0x10) < (temp_a2 & 0xFFFF))) {
        *arg0 = tagprintf_cr2(arg3, temp_a1, temp_a2);
        temp_v1 = bsw;
        *arg1 = F(u16, temp_v1, 0xD8C6) + F(u16, temp_v1, 0xD8BE);
    }
}
