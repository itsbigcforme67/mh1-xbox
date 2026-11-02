/* lb_j04 - lobby move handlers 0x005D1AD0-0x005D1C40: lb_pl_mv077, lb_pl_mv079. Whole file in lb_j.c. */
#include "lobby_f.h"






void pl_sleeping();











#if !defined(G_PART)
void lb_pl_mv077(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x262, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 1
void lb_pl_mv079(PLW *pl) {
    int t;
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        if (pl->char0 != 0x1AB) {
            Lb_pl_chr_set(pl, 0x1AB, 0, 0);
        }
        pl->work08 = 0x168;
        return;
    case 1:
        pl_sleeping(pl);
        t = pl->work08 - 1;
        pl->work08 = t;
        if (t <= 0) {
            Lb_Pl_act_set2(pl, 0, MH2V(0x35, 0x10), 0);
        }
        break;
    }
}
#endif
