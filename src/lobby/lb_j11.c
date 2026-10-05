/* lb_j11 - lobby move handlers 0x005D2C20-0x005D2CB8: lb_pl_mv042. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();





















void lb_pl_mv042(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x283, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}
