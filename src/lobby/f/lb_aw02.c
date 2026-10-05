/* lb_aw02 - tag output flush helpers 0x00604790-0x00604990: tagoutprintf3, tagoutprintf4, tagoutprintf6. Whole file in lb_aw.c. */
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

void tagoutprintf3(char *a) {
    u8 *sp2C;
    u8 *t;
    t = bsw;
    if (t[0xD892] != 0) {
        tagoutprintf3_t();
        return;
    }
    sp2C = t + *(u16 *)(t + 0x188) * 4 + 0x1540;
    tagprintf(a, &sp2C);
    lineBuf_print(sp2C);
    t = bsw;
    *(u16 *)(t + 0xD8BC) = *(u16 *)(t + 0xD8C0);
    t = bsw;
    *(u16 *)(t + 0xD8BE) = *(u16 *)(t + 0xD8C2);
    set_MAX_Y_SIZE(bsw + 0xD8BC);
    *(s32 *)(bsw + 4) = 0;
    *a = 0;
}

void tagoutprintf4(char *a) {
    u8 *sp2C;
    u8 *t;
    t = bsw;
    if (t[0xD892] != 0) {
        tagoutprintf4_t();
        return;
    }
    sp2C = t + *(u16 *)(t + 0x188) * 4 + 0x1540;
    tagprintf(a, &sp2C);
    lineBuf_print(sp2C);
    tagoutprintf_cr(&sp2C);
    set_MAX_Y_SIZE(bsw + 0xD8BC);
    *(s32 *)(bsw + 4) = 0;
    *a = 0;
}

void tagoutprintf6(char *a) {
    u8 *sp2C;
    u8 *t;
    t = bsw;
    if (t[0xD892] != 0) {
        tagoutprintf6_t();
        return;
    }
    sp2C = t + *(u16 *)(t + 0x188) * 4 + 0x1540;
    tagprintf(a, &sp2C);
    lineBuf_print(sp2C);
    if (*(u16 *)(bsw + 0xD8BC) != 0) {
        tagoutprintf_cr(&sp2C);
    }
    set_MAX_Y_SIZE(bsw + 0xD8BC);
    *(s32 *)(bsw + 4) = 0;
    *a = 0;
}
