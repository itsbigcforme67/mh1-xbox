/* Player code (SLPM_654.95 0x0013EB10-0x0013ED48): pl_mv061, 062, 063 (action handlers) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv061(PLW *pl) {
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x25, 6, 0);
        Pl_basic_flagset(pl, 1, 0, 0);
        pl_flag_set(pl, 0x02000000);
        pl->work8F0 = 1;
        pl->work08 = 0xF;
        break;
    case 1:
        w = pl->work08;
        if (w != 0) {
            pl->work08 = w - 1;
        }
        if (Pl_master_ck(pl) == 1) {
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x2D, 0);
                break;
            }
            sit_com_ck(pl);
        }
        break;
    }
}

void pl_mv062(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x29, 2, 0);
        Pl_basic_flagset(pl, 0x8000, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}

void pl_mv063(PLW *pl) {
    u8 s;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->ang[1] = *(u16 *)&pl->ang_y;
        pl_chr_set2(pl, 0x2A, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x24, 0);
        }
        break;
    }
}
