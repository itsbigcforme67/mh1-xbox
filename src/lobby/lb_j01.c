/* lb_j01 - lobby move handlers 0x005D0690-0x005D0750: lb_pl_mv044. Whole file in lb_j.c. */
#include "lobby.h"







void lb_pl_mv044(PLW *pl) {
    u8 s;
    if (PLU8(pl, 0x8EC) != 0) {
        Lb_act_set(pl, 0, 0x55);
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x1E, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_pl_to_normal(pl, 0, 0xA, 0);
        }
    }
}
