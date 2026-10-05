/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"











































void pl_mv093(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 1, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x19B, -6, 0);
            break;
        }
        pl_chr_set2(pl, 0x1A6, -6, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            if (arg1 == 0) {
                pl_chr_set2(pl, 0x195, -4, 0x24);
                break;
            }
            pl_chr_set2(pl, 0x1A3, 0, 0x44);
        }
        break;
    case 2:
        if ((Pl_master_ck(pl) == 1) && (frame_check(180.0f, pl, 0) != 0)) {
            pick_set_sub(pl, arg1);
        }
        if (arg1 == 0) {
            if (frame_check2(200.0f, pl, 0) != 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x19C, -4, 0);
                break;
            }
        } else if (frame_check2(294.0f, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x1A7, -2, 0);
            break;
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            if (pl->x07 == 0) {
                Pl_act_set2(pl, 0, 0x1D, 2);
                break;
            }
            egg_set(pl);
            Pl_act_set2(pl, 5, 9, 0);
        }
        break;
    }
}

void pl_mv076(PLW *pl) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl->work8CA = 0;
        pl->work8D0 = 0;
        pl_chr_set2(pl, 0x320, -4, 0);
        BBQcamera_set(pl);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x55, 2);
        }
        break;
    }
}

void pl_mv077(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x321, 0, 0);
            break;
        }
        pl_chr_set2(pl, 0x322, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
