/* lb_aw03 - tag output flush helpers 0x00609460-0x00609748: tagoutprintf3_t, tagoutprintf4_t, tagoutprintf6_t. Whole file in lb_aw.c. */
#include "lobby_f.h"
extern u8 *bsw;
void tagoutprintf3_t();
void tagoutprintf4_t();
void tagoutprintf6_t();


void tagprintf();
void tagprintf_t();
void lineBuf_print();
void lineBuf_print_t();
void tagoutprintf_cr();
void tagoutprintf_cr_t();
void set_MAX_Y_SIZE();
void set_align_data();

void tagoutprintf3_t(char *a) {
    u8 *sp2C;
    u8 *t;
    u8 *a1;
    t = bsw;
    sp2C = t + *(u16 *)(t + 0x188) * 4 + 0x1540;
    tagprintf_t(a, &sp2C);
    lineBuf_print_t(sp2C);
    t = bsw;
    *(u16 *)(t + 0xD8CE) = *(u16 *)(t + 0xD8D2);
    t = bsw;
    *(u16 *)(t + 0xD8D0) = *(u16 *)(t + 0xD8D4);
    a1 = bsw;
    if (*(s8 *)(a1 + 0x186) == 1) {
        BSC(u16, a1, *(u16 *)(a1 + 0xD894), 0x2518) = *(u16 *)(a1 + 0xD8D4);
    }
    set_MAX_Y_SIZE(bsw + 0xD8CE, a1);
    *(s32 *)(bsw + 4) = 0;
    *a = 0;
}

void tagoutprintf4_t(char *a) {
    u8 *sp3C;
    u8 *a2;
    u8 *s0;
    u8 *a1;
    a2 = bsw;
    sp3C = a2 + *(u16 *)(a2 + 0x188) * 4 + 0x1540;
    s0 = a2 + *(u16 *)(a2 + 0xD894) * 0x5C + 0x24E0;
    tagprintf_t(a, &sp3C, a2);
    lineBuf_print_t(sp3C);
    tagoutprintf_cr_t(&sp3C);
    a1 = bsw;
    if (*(s8 *)(a1 + 0x186) == 1) {
        BSC(u16, a1, *(u16 *)(a1 + 0xD894), 0x2518) = *(u16 *)(a1 + 0xD8D4);
    }
    set_MAX_Y_SIZE(bsw + 0xD8CE, a1);
    set_align_data(s0);
    *(s32 *)(bsw + 4) = 0;
    *a = 0;
}

void tagoutprintf6_t(char *a) {
    u8 *sp3C;
    u8 *a2;
    u8 *s0;
    u8 *a1;
    a2 = bsw;
    sp3C = a2 + *(u16 *)(a2 + 0x188) * 4 + 0x1540;
    s0 = a2 + *(u16 *)(a2 + 0xD894) * 0x5C + 0x24E0;
    tagprintf_t(a, &sp3C, a2);
    lineBuf_print_t(sp3C);
    if (*(u16 *)(bsw + 0xD8CE) != 0) {
        tagoutprintf_cr_t(&sp3C);
    }
    a1 = bsw;
    if (*(s8 *)(a1 + 0x186) == 1) {
        BSC(u16, a1, *(u16 *)(a1 + 0xD894), 0x2518) = *(u16 *)(a1 + 0xD8D4);
    }
    set_MAX_Y_SIZE(bsw + 0xD8CE, a1);
    set_align_data(s0);
    *(s32 *)(bsw + 4) = 0;
    *a = 0;
}
