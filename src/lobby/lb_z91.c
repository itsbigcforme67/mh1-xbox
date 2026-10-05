/* lb_z91 - auto-drafted 0x00602B90-0x00602C00: ck_line_no_add, ck_line_no_add2 (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

void ck_line_no_add(void) {
    u16 temp_v1;
    void *temp_a1;

    temp_a1 = bsw;
    temp_v1 = F(u16, temp_a1, 0x188);
    F(u16, temp_a1, 0x188) = (u16) (temp_v1 + 1);
    if (temp_v1 >= 0x3E7) {
        F(u16, bsw, 0x188) = 0x3E7U;
    }
}

void ck_line_no_add2(s32 *arg0) {
    u16 temp_v1;
    void *temp_a2;

    temp_a2 = bsw;
    temp_v1 = F(u16, temp_a2, 0x188);
    F(u16, temp_a2, 0x188) = (u16) (temp_v1 + 1);
    if (temp_v1 >= 0x3E7) {
        F(u16, bsw, 0x188) = 0x3E7U;
        return;
    }
    *arg0 += 4;
}
