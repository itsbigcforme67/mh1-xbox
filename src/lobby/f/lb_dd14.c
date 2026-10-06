/* lb_dd14 - browser: tagAct_602 0x005FFCB0-0x005FFD1C (font size tag handler; v > 1 puts the compare result in at). Hand-written from the asm (working copy in lb_dr2.c). */
#include "lobby_f.h"
extern u8 *bsw;

void font_data_clear();
void tagoutprintf3();
void font_data_set();
int tagAct_602(int arg0, char *s) {
    u8 *w;
    int v;
    font_data_clear();
    tagoutprintf3(s);
    w = bsw;
    v = w[*(s16 *)(w + 0x124) + 0x168];
    if (v > 1) {
        v = (v - 1) & 0xFF;
    }
    w[0x2D3] = (v & 0xFF) + 0x30;
    bsw[0x2D4] = 0;
    font_data_set();
    return 0;
}
