/* lb_gaq01 - near-match fixes 0x005FDFE0-0x005FE140: tagAct_043, tagAct_044. Whole file in lb_aq.c. */
#include "lobby_f.h"
extern u8 *bsw;
void tagoutprintf3();
void tagoutprintf6();
void set_align_data();
void get_tag_in_parameter();
int get_numeric_parameter2();
void tagoutprintf2();

s32 tagAct_043(int arg0, s32 arg1) {
    u8 *t;
    u8 *c;
    u8 *d;
    u8 a1;
    u8 v;
    tagoutprintf6(arg1);
    t = bsw;
    if (t[0xD892] != 0) {
        c = t + *(u16 *)(t + 0xD894) * 0x5C;
        a1 = c[0x2531];
        d = c + 0x24E0;
        v = a1 & 0xF;
        if (v != 0xF) {
            v += 1;
            d[0x51] = (a1 & 0xF0) | v;
        }
        set_align_data(d, a1);
    } else {
        a1 = t[0x14];
        v = a1 & 0xF;
        if (v != 0xF) {
            v += 1;
            t[0x14] = (a1 & 0xF0) | v;
        }
    }
    return 0;
}

s32 tagAct_044(int arg0, s32 arg1) {
    u8 *t;
    u8 *c;
    u8 *d;
    u8 a1;
    u8 v;
    tagoutprintf6(arg1);
    t = bsw;
    if (t[0xD892] != 0) {
        c = t + *(u16 *)(t + 0xD894) * 0x5C;
        a1 = c[0x2531];
        d = c + 0x24E0;
        v = a1 & 0xF;
        if (v != 0) {
            v -= 1;
            d[0x51] = (a1 & 0xF0) | v;
        }
    } else {
        a1 = t[0x14];
        v = a1 & 0xF;
        if (v != 0) {
            v -= 1;
            t[0x14] = (a1 & 0xF0) | v;
        }
    }
    return 0;
}
