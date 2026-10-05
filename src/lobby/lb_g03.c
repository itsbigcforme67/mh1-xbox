/* lb_g03 - lobby player basics 0x005CE2C0-0x005CE3B8: lb_to_normal, lb_action_timer_calc. Whole file in lb_g.c. */
#include "lobby.h"










void lb_to_normal(PLW *pl, int blend, int tm) {
    pl->act_tm0 = tm;
    pl->act_tm1 = tm;
    pl->char0 = 1;
    pl->char1 = 0x65;
    pl->blend0 = blend / 2;
    pl->blend1 = blend / 2;
    pl->flag14 = 0;
    pl->flag15 = 0;
    if (ck_pl_send(pl) != 0) {
        Lb_send_pl_status(pl);
    }
}

void lb_action_timer_calc(PLW *pl, int flag) {
    s16 b = pl->blend0;
    s16 t = pl->act_tm0;
    int bb;
    pl->work39C = 0;
    if (b < 0) {
        b = -b;
    }
    bb = b;
    pl->work39C += t - bb;
    if ((s16)flag != 0) {
        if (bb != 0) {
            b--;
        }
        pl->work3D0 += (b - t) & 0xFF;
    }
}
