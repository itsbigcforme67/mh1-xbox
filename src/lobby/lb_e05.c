/* lb_e05 - lobby members/cockpit/icons 0x005CCE90-0x005CCED4: Lb_put_msg. Whole file in lb_e.c. */
#include "lobby.h"










void Lb_put_msg(s16 *p) {
    flfntLocate(p[0], p[1]);
    font_print(lit_429_00664C38, p + 2);
    strlen_sp(p + 2);
}
