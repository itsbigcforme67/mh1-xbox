/* lb_ax01 - browser line buffer helpers 0x006040C0-0x006041F4: tagprintf_cr, set_2byte_lineBuf. Whole file in lb_ax.c. */
#include "lobby_f.h"
extern u8 *bsw;
void pos_cr();
void ck_line_no_add2();
void set_MAX_Y_SIZE();
void set_align_data();
void Disp_Text();
void Disp_Text_t();

void tagprintf_cr(u16 **p) {
    u8 *a0;
    u8 *t;
    a0 = bsw;
    if (*(s8 *)(a0 + 0x186) == 1) {
        **p = *(u16 *)(a0 + 0xD8C0);
    }
    a0 = bsw;
    *(s16 *)(a0 + 0xD8C8) = *(u16 *)(a0 + 0xD8C0) - *(u16 *)(a0 + 0xD8BC);
    t = bsw;
    (t + *(s32 *)(t + 0x1C))[0x20] = 0;
    Disp_Text(*p);
    *(s32 *)(bsw + 0x1C) = 0;
    pos_cr(bsw + 0xD8BC, p);
    ck_line_no_add2(p);
    set_MAX_Y_SIZE(bsw + 0xD8BC);
}

void set_2byte_lineBuf(u8 **pp, u16 *cell) {
    u8 *a3;
    a3 = bsw;
    (a3 + *(s32 *)(a3 + 0x1C))[0x20] = **pp;
    a3 = bsw;
    *(s32 *)(a3 + 0x1C) = *(s32 *)(a3 + 0x1C) + 1;
    *pp += 1;
    a3 = bsw;
    (a3 + *(s32 *)(a3 + 0x1C))[0x20] = **pp;
    a3 = bsw;
    *(s32 *)(a3 + 0x1C) = *(s32 *)(a3 + 0x1C) + 1;
    cell[2] += bsw[0x180];
}
