/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"











void pl_dm015(PLW *pl) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xDA, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        break;
    case 1:
        if (frame_check2(40.0f, pl, 0) == 0) {
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            sp30[2] = -18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 42.0f)) / 360.0f)));
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp20[0];
            pl->pos[2] = pl->pos[2] + sp20[2];
            if ((pl->work39C % 7) == 0) {
                eft13_set(pl, 5, 9);
                eft13_set(pl, 8, 9);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xC, 0);
        }
        break;
    }
}

void pl_dm016(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        if (arg1 == 0) {
            pl->x07 = 4;
        } else {
            pl->x07 = 1;
        }
        pl_chr_set2(pl, 0xD3, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->work08 = 0;
        break;
    case 1:
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x06++;
            if (pl->x06 >= pl->x07) {
                pl->x05++;
                pl_chr_set2(pl, 0xD4, 2, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xC, 0);
        }
        break;
    }
}

void pl_dm017(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        if (pl->flag12 != 0) {
            pl_chr_set2(pl, 0x4BB, 0, 0);
        } else {
            pl_chr_set2(pl, 0xC9, 0, 0);
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void piyo_reset(PLW *pl) {
    pl->x7AC = 0;
    pl->x7AA = 0;
    pl->work886 = 0x1E;
}
