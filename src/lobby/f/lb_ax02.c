/* lb_ax02 - browser line buffer helpers 0x00604240-0x006042A8: set_1byte_lineBuf. Whole file in lb_ax.c. */
#include "lobby_f.h"
extern u8 *bsw;
void pos_cr();
void ck_line_no_add2();
void set_MAX_Y_SIZE();
void set_align_data();
void Disp_Text();
void Disp_Text_t();

void set_1byte_lineBuf(u8 **pp, u16 *cell) {
    u8 a0;
    u8 *a2;
    a0 = **pp;
    if (a0 != 9) {
        a2 = bsw;
        (a2 + *(s32 *)(a2 + 0x1C))[0x20] = a0;
    } else {
        a2 = bsw;
        (a2 + *(s32 *)(a2 + 0x1C))[0x20] = 0x20;
    }
    a0 = 0;
    *(s32 *)(bsw + 0x1C) = *(s32 *)(bsw + 0x1C) + 1;
    cell[2] += bsw[0x181];
}
