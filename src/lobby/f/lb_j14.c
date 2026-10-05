/* lb_j14 - lobby move handlers 0x005D2CC0-0x005D2FE4: lb_pl_mv097, lb_pl_mv098, lb_pl_mv099. Whole file in lb_j.c. */
#include "lobby_f.h"






void pl_sleeping();



























void lb_pl_mv097(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x27B, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x261, 6, 0);
        }
        break;
    case 2:
        if (frame_check2(10.0f, pl, 0) != 0) {
            switch (eatResult) {
            case 0:
                Lb_act_set(pl, 0, 0x62);
                return;
            case 1:
                Lb_act_set(pl, 0, 0x5F);
                return;
            case 2:
                Lb_act_set(pl, 0, 0x50);
                break;
            }
        }
        break;
    }
}

void lb_pl_mv098(PLW *pl) {
    u16 r;
    int t;
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x27C, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0xD2, 0, 0x92);
            *(u16 *)&pl->ang_y = *(u16 *)&pl->ang_y + 0x4000;
            pl->ang[1] = pl->ang[1] + 0x4000;
            r = ran_suu(1);
            pl->work08 = (r & 0x1F) + 0x3C;
        }
        break;
    case 2:
        t = pl->work08 - 1;
        pl->work08 = t;
        if (t <= 0) {
            Lb_act_set(pl, 0, 0x53);
            Lb_eat_to_end();
        }
        break;
    }
}

void lb_pl_mv099(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x25D, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x25E, 0, 0);
        }
        break;
    case 2:
        break;
    case 3:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
