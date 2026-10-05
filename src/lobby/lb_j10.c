/* lb_j10 - lobby move handlers 0x005D25F0-0x005D2A28: lb_pl_mv089, lb_pl_mv090, lb_pl_mv078, lb_pl_mv092, lb_pl_mv094, lb_pl_mv041. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();





















void lb_pl_mv089(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_chidori_cnt_up(pl);
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x263, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv090(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x264, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv078(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x269, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv092(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x268, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->work08 = 0x3C;
            pl->x05 = pl->x05 + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1) {
            t = pl->work08;
            if (t <= 0) {
                if (F(u16, pl, 0x368) & 0x3C60) {
                    Lb_act_set(pl, 0, 0x4E);
                    return;
                }
            } else {
                pl->work08 = t - 1;
            }
        }
        break;
    }
}

void lb_pl_mv094(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x265, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1 && (F(u16, pl, 0x368) & 0x3C60)) {
            Lb_act_set(pl, 0, 0x29);
        }
        break;
    }
}

void lb_pl_mv041(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x266, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}
