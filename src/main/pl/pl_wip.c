/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

















void pl_mv043(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        wall_vec_set(pl, 0);
        pl_chr_set2(pl, 0x22, 4, 0);
        break;
    case 1:
        if (frame_check2(96.0f, pl, 0) != 0) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
        break;
    }
    kabe_hosei(pl, 1);
}

void pl_mv044(PLW *pl) {
    u8 s;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x1E, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}

void pl_mv046(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x195, -0xE, 0);
        break;
    case 1:
        if (frame_check2(130.0f, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x19A, 4, 0);
        }
        break;
    case 2:
        if (frame_check2(60.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x2F, 0xC);
            } else {
                pl->x05++;
            }
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}

void pl_mv047(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19A, 0, 0x3C);
        if (Pl_master_ck(pl) == 1) {
            Ana_item_set(pl);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x195, -4, 0xCE);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}
