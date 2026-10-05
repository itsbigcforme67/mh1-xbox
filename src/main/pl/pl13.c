/* Player code (SLPM_654.95 0x0013BCB0-0x0013BF20): pl_mv008 (sit down / stand up), pl_mv013. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv008(PLW *pl, int arg1) {
    u8 s;
    u8 t;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        switch (arg1) {
        case 0:
            pl->x05 = s + 1;
            pl->x06 = 0;
            pl->work39C = 0;
            pl_chr_set2(pl, 7, 2, 0);
            Pl_basic_flagset(pl, 0, 0, 0);
            pl->flag12 = 0;
            t = pl->work56B;
            if (t & 0xF) {
                pl->work56B = t & 0xF0;
                func_549200(pl, 4);
            }
            break;
        case 1:
            pl->x05 = 2;
            pl->x06 = 0;
            if (pl->char0 == 0x19C) {
                pl_chr_set2(pl, 8, 8, 0);
            } else {
                pl_chr_set2(pl, 8, 2, 0);
            }
            action_timer_calc(pl, 0);
            Pl_basic_flagset(pl, 0x8001, 0, 0);
            pl->flag12 = 0;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 8, 6, 0);
            Pl_basic_flagset(pl, 0x8001, 0, 0);
            action_timer_calc(pl, 0);
        }
        break;
    case 2:
        sit_com_ck(pl);
        break;
    }
}

void pl_mv013(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work603 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->x06 = 0x10;
            normal_char_set(pl, pl->x06, 0);
            break;
        }
        if (frame_check2(4.0f, pl, 0) != 0) {
            pl->work601 = 1;
        }
        break;
    case 2:
        s = pl->x06;
        pl->x06 = s - 1;
        if (s == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
