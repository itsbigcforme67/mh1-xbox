/* lb_j12 - lobby move handlers 0x005D14D0-0x005D15B0: lb_pl_mv053. Whole file in lb_j.c. */
#include "lobby_f.h"






void pl_sleeping();



























void lb_pl_mv053(PLW *pl) {
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
        pl->ang[1] = (pl->ang[1] + 0x7FFF + 1) & 0xFFFF;
        pl->ang_y = pl->ang[1];
        Lb_pl_chr_set(pl, 0x1AD, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            lb_sys.x68 = 0;
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
