/* lb_n11 - lobby 0x005D7950-0x005D79E4: Lb_put_hint (hint text of a member into lb_sys). Whole file in lb_n.c.
   The 0x63 case calls sprintf with two arguments only (a2 still holds n * 4 from the pointer calculation). */
#include "lobby_f.h"

void Lb_put_hint(int a, int n) {
    int *p = hint_tbl[a] + n;
    if (n == 0x63) {
        sprintf((char *)&lb_sys + a * 0x1E + 0xA, lit_275_00665668);
        return;
    }
    sprintf((char *)&lb_sys + a * 0x1E + 0xA, lit_743_00665D60, *p);
}
