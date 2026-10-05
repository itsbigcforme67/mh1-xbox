/* lb_j08 - lobby move handlers 0x005D1FA0-0x005D20A8: lb_pl_mv085. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();





















void lb_pl_mv085(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        if (pl->char0 != 1) {
            Lb_pl_chr_set(pl, 1, 9, 0);
        }
        break;
    case 1:
        if (F(s32, pl, 0x194) < 2) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x15, 4, 0);
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 5, 4, 0x90);
        }
        lb_basic_com_ck(pl);
        return;
    case 3:
        if (pl->work39C >= 0xF0) {
            Lb_act_set(pl, 0, 0x40);
            return;
        }
        lb_basic_com_ck(pl);
        break;
    }
}
