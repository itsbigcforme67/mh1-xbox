/* lb_gh01 - near-match fixes 0x005CE5C0-0x005CE660: Lb_Pl_basic_flagset. Whole file in lb_h.c. */
#include "lobby_f.h"








void Lb_Pl_basic_flagset(PLW *pl, int a) {
    int t = a & 0xFF;
    int f = a & 0xFFFF;
    switch (t) {
    default:
        PLU8(pl, 0x388) = 0;
        break;
    case 1:
        PLU8(pl, 0x388) = 1;
        break;
    case 2:
        PLU8(pl, 0x388) = 2;
    }
    if (f & 0x8000) {
        lb_pl_flag_clr(pl, 8);
    } else {
        Lb_pl_flag_set(pl, 8);
    }
    lb_pl_flag_clr(pl, 1);
    lb_pl_flag_clr(pl, 2);
}
