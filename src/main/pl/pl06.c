/* Player code (SLPM_654.95 0x001361B0-0x00136378): timer_calc_sub_em, pl_timer_calc. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void timer_calc_sub_em(PLW *pl) {
    s16 temp_v1;
    u8 temp_a1;

    if ((pl->work8C3 == 0) && (temp_a1 = pl->work56A, (temp_a1 != 0xFF)) && (temp_a1 != 0)) {
        temp_v1 = pl->work572;
        if (temp_v1 != 0) {
            pl->work572 = (s16) (temp_v1 - 1);
            return;
        }
        pl->work56A = 0U;
    }
}

void pl_timer_calc(PLW *pl) {
    s16 temp_v0_3;
    s16 temp_v0_5;
    u16 temp_v0;
    u16 temp_v0_4;
    u8 temp_v0_2;
    u8 temp_v0_6;

    if (pl->x40A == 0) {
        if (pl_flag_ck(pl, 0x400001) != 0) {
            pl->work398 = (u16) (pl->work398 + 2);
        } else {
            pl->work398 = 0U;
        }
        pl->work39C = (s32) (pl->work39C + 2);
        if (pl->work39C >= 0x186A0) {
            pl->work39C = 0x2710;
        }
        temp_v0 = pl->work40C;
        if (temp_v0 != 0) {
            pl->work40C = (u16) (temp_v0 - 1);
        }
        temp_v0_2 = pl->work440;
        if (temp_v0_2 != 0) {
            pl->work440 = (u8) (temp_v0_2 - 1);
        }
        temp_v0_3 = pl->x43E;
        if (temp_v0_3 != 0) {
            pl->x43E = (s16) (temp_v0_3 - 1);
        }
        temp_v0_4 = pl->work40E;
        if (temp_v0_4 != 0) {
            pl->work40E = (u16) (temp_v0_4 - 1);
        }
        temp_v0_5 = pl->work4E0;
        if (temp_v0_5 != 0) {
            pl->work4E0 = (s16) (temp_v0_5 - 1);
        }
        temp_v0_6 = pl->work7D6;
        if (temp_v0_6 != 0) {
            pl->work7D6 = (u8) (temp_v0_6 - 1);
        }
        if (pl->x10 == 0) {
            timer_calc_sub_pl(pl);
        } else {
            timer_calc_sub_em(pl);
        }
    }
    pl_flag_clr(pl, 0x800);
    if (pl_flag_ck(pl, 1) != 0) {
        pl_flag_set(pl, 0x2000);
        return;
    }
    pl_flag_clr(pl, 0x2000);
}
