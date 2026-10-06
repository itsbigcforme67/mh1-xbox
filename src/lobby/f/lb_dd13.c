/* lb_dd13 - browser: set_align_data 0x00604F20-0x00604FE8 (text alignment inside a table cell). Hand-written from the asm (working copy in lb_dr2.c). */
#include "lobby_f.h"
extern u8 *bsw;

void set_align_data(u8 *arg0) {
    s32 temp_a1;
    s32 temp_t0;
    u16 temp_a3;
    u8 temp_a2_2;
    u8 *temp_a2;
    u16 *tp;

    if (arg0[0x50] & 1) {
        *(s16 *)(arg0 + 0x3A) = 0;
        return;
    }
    temp_a2 = bsw;
    tp = (u16 *)(temp_a2 + (*(u16 *)(temp_a2 + 0x188) * 4) + 0x1540);
    if (*(s8 *)(temp_a2 + 0x186) == 0) {
        *(s16 *)(arg0 + 0x3A) = 0;
        temp_a3 = *tp;
        temp_t0 = *(u16 *)(bsw + 0xD8DC) - *(u16 *)(arg0 + 0x3E);
        if (temp_a3 < temp_t0) {
            temp_a2_2 = arg0[0x4A];
            if (temp_a2_2 == 2 || arg0[0x51] != 0 || arg0[0x1B] == 3) {
                temp_a1 = temp_t0 - temp_a3;
                *(s16 *)(arg0 + 0x3A) = temp_a1 / 2;
            } else if (temp_a2_2 == 3) {
                *(s16 *)(arg0 + 0x3A) = temp_t0 - temp_a3;
            }
        }
    }
}
