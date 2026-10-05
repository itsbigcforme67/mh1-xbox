/* Player code (SLPM_654.95 0x0013D5F0-0x0013DB40): pl_mv037, 038, 039, 040, 041, 042 (wall/ladder-type action handlers) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv037(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x27, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 5, 0);
        }
        break;
    }
}

void pl_mv038(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0x8000, 0, 0);
        pl->flag604 = 0;
        pl_chr_set2(pl, 0x13, 2, 0);
        vib_set_pl(pl, 1);
        Pl_set_quake_sub(pl, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}

void pl_mv039(PLW *pl) {
    pl->work40E = 2;
    Pl_basic_flagset(pl, 0x8002, 0, 0);
    kabe_hosei(pl, 1);
    ex_kabe_ck(pl);
}

void pl_mv040(PLW *pl) {
    int sp2C;
    u16 c;

    pl->work40E = 2;
    if (pl->x05 == 0) {
        pl->x05++;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        c = pl->char0;
        if (c != 0x22) {
            if (c == 0x23) {
                if (frame_check2(46.0f, pl, 0) != 0) {
                    pl_chr_set2(pl, 0x22, 0, 0x5A);
                } else {
                    pl_chr_set2(pl, 0x22, 0, 0x2E);
                }
            } else {
                pl_chr_set2(pl, 0x22, 4, 0);
            }
        }
    } else if (frame_check(46.0f, pl, 0) != 0 || frame_check(90.0f, pl, 0) != 0) {
        if (!(ex_kabe_ck(pl) & 0xFF) && (Pl_master_ck(pl) == 1)) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
    }
    if (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp2C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1B, 0);
    }
    kabe_hosei(pl, 1);
}

void pl_mv041(PLW *pl) {
    u16 c;

    pl->work40E = 2;
    if (pl->x05 == 0) {
        pl->x05++;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        c = pl->char0;
        if (c != 0x23) {
            if (c == 0x22) {
                if (frame_check2(92.0f, pl, 0) != 0) {
                    pl_chr_set2(pl, 0x23, 0, 0x2C);
                } else {
                    pl_chr_set2(pl, 0x23, 0, 0);
                }
            } else {
                pl_chr_set2(pl, 0x23, 4, 0);
            }
        }
    } else if (frame_check(0.0f, pl, 0) != 0 || frame_check(44.0f, pl, 0) != 0) {
        if (!(ex_kabe_ck(pl) & 0xFF) && (Pl_master_ck(pl) == 1)) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
    }
    if (pl->pos[1] <= pl->x5AC) {
        Pl_act_set(pl, 0, 0x2A, 0);
    }
    kabe_hosei(pl, 1);
}

void pl_mv042(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x24, 6, 0);
        kabe_hosei(pl, 1);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        } else {
            kabe_hosei(pl, 1);
        }
        break;
    }
}
