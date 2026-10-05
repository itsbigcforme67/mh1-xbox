/* lb_j09 - lobby move handlers 0x005D2170-0x005D22A8: lb_pl_mv087. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();





















void lb_pl_mv087(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x25C, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x32A, 4, 0x2C);
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0xD8, 6, 0x24);
        }
        break;
    case 3:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 8, 0);
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x68 = 0;
        }
        break;
    }
}
