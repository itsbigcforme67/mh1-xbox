/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"






























































void pl_mv103(PLW *pl) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x3C, 4, 0);
        pl->work90A = 0;
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && ((pl->sw.trg & 0x40) || (pl->work90A != 0))) {
            Pl_act_set2(pl, 0, 0x69, 0);
        }
        break;
    }
}

void pl_mv104(PLW *pl) {
    s32 w;
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->work90A = 0;
        pl->work08 = 0;
        break;
    case 1:
        w = pl->work08 + 1;
        pl->work08 = w;
        if ((w >= 0x259) || (pl->work90A != 0)) {
            if (pl->work90A == 1) {
                Pl_act_set2(pl, 0, 0x6A, 0);
                break;
            }
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}

void pl_mv105(PLW *pl) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x3D, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv106(PLW *pl) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x280, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
