/* lb_c05 - lobby small helpers 0x005CCEE0-0x005CCF5C: Lb_put_msg_type2, Lb_put_msg2. Whole file in lb_c.c. */
#include "lobby_f.h"













void Lb_put_msg_type2(s16 *p) {
    flfntLocate(p[0], p[1]);
    font_print(lit_429_00664C38, *(s32 *)(p + 2));
}

void Lb_put_msg2(int a0, int a1, char *msg) {
    flfntLocate();
    font_print(lit_429_00664C38, msg);
    strlen_sp(msg);
}
