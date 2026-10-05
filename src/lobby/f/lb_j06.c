/* lb_j06 - lobby move handlers 0x005D20B0-0x005D2168: lb_pl_mv086. Whole file in lb_j.c. */
#include "lobby_f.h"






void pl_sleeping();











void lb_pl_mv086(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x25A, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}
