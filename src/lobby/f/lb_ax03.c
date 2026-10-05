/* lb_ax03 - browser line buffer helpers 0x00609390-0x00609460: tagoutprintf_cr_t. Whole file in lb_ax.c. */
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
