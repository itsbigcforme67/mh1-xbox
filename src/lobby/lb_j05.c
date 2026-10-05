/* lb_j05 - lobby move handlers 0x005D1DA0-0x005D1F98: lb_pl_mv083, lb_pl_mv084, lb_pl_mv043. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();











void lb_pl_mv083(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0xCF, 2, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void lb_pl_mv084(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x26A, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1 && (F(u16, pl, 0x368) & 0x3C60)) {
            Lb_act_set(pl, 0, 0x2B);
        }
        break;
    }
}

void lb_pl_mv043(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x282, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}
