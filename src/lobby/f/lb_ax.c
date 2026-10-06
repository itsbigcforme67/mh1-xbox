/* Lobby browser: line buffer / carriage return helpers, hand-written from m2c drafts. */
#include "lobby_f.h"
extern u8 *bsw;
void pos_cr();
void ck_line_no_add2();
void set_MAX_Y_SIZE();
void set_align_data();
void Disp_Text();
void Disp_Text_t();
void tagoutprintf_cr_t(u16 **p) {
    u8 *a2;
    u8 *s0;
    u16 *v;
    u16 *q;
    a2 = bsw;
    s0 = a2 + *(u16 *)(a2 + 0xD894) * 0x5C + 0x24E0;
    if (*(s8 *)(a2 + 0x186) == 1) {
        **p = *(u16 *)(a2 + 0xD8D2);
        v = *p;
        q = v + 1;
        if (v[1] == 0) {
            *q = bsw[0x180];
        }
    }
    *(s16 *)(bsw + 0x16) = 0;
    pos_cr(bsw + 0xD8CE, p);
    ck_line_no_add2(p);
    set_MAX_Y_SIZE(bsw + 0xD8CE);
    set_align_data(s0);
}
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
void tagprintf_t_cr(u16 **p) {
    u16 a1;
    u8 *a0;
    u8 *a2;
    u8 *t;
    a2 = bsw;
    a1 = *(u16 *)(a2 + 0xD894);
    if (*(s8 *)(a2 + 0x186) == 1) {
        **p = *(u16 *)(a2 + 0xD8D2);
    }
    a0 = bsw;
    *(s16 *)(a0 + 0xD8C8) = *(u16 *)(a0 + 0xD8D2) - *(u16 *)(a0 + 0xD8CE);
    t = bsw;
    (t + *(s32 *)(t + 0x1C))[0x20] = 0;
    Disp_Text_t(*p, a1, a2);
    *(s32 *)(bsw + 0x1C) = 0;
    pos_cr(bsw + 0xD8CE, p);
    ck_line_no_add2(p);
    set_MAX_Y_SIZE(bsw + 0xD8CE);
    set_align_data(a2 + a1 * 0x5C + 0x24E0);
}
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
