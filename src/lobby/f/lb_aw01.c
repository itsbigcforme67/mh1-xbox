/* lb_aw01 - tag output flush helpers 0x00602E40-0x00602E70: pos_cr. Whole file in lb_aw.c. */
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

void pos_cr(u16 *p, u16 **q) {
    p[2] = 0;
    p[3] = p[3] + (*q)[1];
    p[0] = p[2];
    p[1] = p[3];
    *(s8 *)((u8 *)p + 0x10) = 0;
}
