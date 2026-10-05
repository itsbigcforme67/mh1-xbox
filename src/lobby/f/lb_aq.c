/* Lobby browser: more tag handlers (tagAct_043/044/145/604 ...), hand-written. */
#include "lobby_f.h"
extern u8 *bsw;
void tagoutprintf3();
void tagoutprintf6();
void set_align_data();
void get_tag_in_parameter();
int get_numeric_parameter2();
s32 tagAct_604(int arg0, s32 arg1) {
    (bsw + bsw[0xE96C])[0xE96D] = 0;
    bsw[0xE96C]++;
    tagoutprintf3(arg1);
    return 0;
}
s32 tagAct_043(int arg0, s32 arg1) {
    u8 *t;
    u8 *c;
    u8 *d;
    u8 a1;
    int v;
    tagoutprintf6(arg1);
    t = bsw;
    if (t[0xD892] != 0) {
        c = t + *(u16 *)(t + 0xD894) * 0x5C;
        a1 = c[0x2531];
        d = c + 0x24E0;
        v = a1 & 0xF;
        if (v != 0xF) {
            d[0x51] = (a1 & 0xF0) | ((v + 1) & 0xFF);
        }
        set_align_data(d, a1);
    } else {
        a1 = t[0x14];
        v = a1 & 0xF;
        if (v != 0xF) {
            t[0x14] = (a1 & 0xF0) | ((v + 1) & 0xFF);
        }
    }
    return 0;
}
s32 tagAct_044(int arg0, s32 arg1) {
    u8 *t;
    u8 *c;
    u8 *d;
    u8 a1;
    int v;
    tagoutprintf6(arg1);
    t = bsw;
    if (t[0xD892] != 0) {
        c = t + *(u16 *)(t + 0xD894) * 0x5C;
        a1 = c[0x2531];
        d = c + 0x24E0;
        v = a1 & 0xF;
        if (v != 0) {
            d[0x51] = (a1 & 0xF0) | ((v - 1) & 0xFF);
        }
    } else {
        a1 = t[0x14];
        v = a1 & 0xF;
        if (v != 0) {
            t[0x14] = (a1 & 0xF0) | ((v - 1) & 0xFF);
        }
    }
    return 0;
}
s32 tagAct_145(s32 arg0) {
    u8 sp10[0x108];
    u16 *p;
    u16 v;
    get_tag_in_parameter(arg0, sp10, 0x100);
    *(u16 *)(bsw + 0x4E8) = get_numeric_parameter2(sp10);
    p = (u16 *)(bsw + 0x4E8);
    v = *(u16 *)(bsw + 0x4E8);
    if (v != 0) {
        if ((v & 0xFFFF) > 0x100) {
            *p = 0x100;
        }
    } else {
        *p = 0x100;
    }
    return 0;
}
void tagoutprintf2();
s32 tagAct_500(int arg0, char *buf) {
    int c;
    u8 *t;
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    t = bsw;
    c = *(s32 *)(t + 4);
    *(s32 *)(t + 4) = c + 1;
    buf[c] = 60;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_501(int arg0, char *buf) {
    int c;
    u8 *t;
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    t = bsw;
    c = *(s32 *)(t + 4);
    *(s32 *)(t + 4) = c + 1;
    buf[c] = 62;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_502(int arg0, char *buf) {
    int c;
    u8 *t;
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    t = bsw;
    c = *(s32 *)(t + 4);
    *(s32 *)(t + 4) = c + 1;
    buf[c] = 34;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_503(int arg0, char *buf) {
    int c;
    u8 *t;
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    t = bsw;
    c = *(s32 *)(t + 4);
    *(s32 *)(t + 4) = c + 1;
    buf[c] = 38;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
s32 tagAct_504(int arg0, char *buf) {
    int c;
    u8 *t;
    if (*(s32 *)(bsw + 4) > 0x7D) {
        tagoutprintf2(bsw + 0xD8E4);
    }
    t = bsw;
    c = *(s32 *)(t + 4);
    *(s32 *)(t + 4) = c + 1;
    buf[c] = 7;
    buf[*(s32 *)(bsw + 4)] = 0;
    return 0;
}
