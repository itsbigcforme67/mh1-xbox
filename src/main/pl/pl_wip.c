/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv000(PLW *pl) {
    u16 c;

    c = pl->char0;
    switch (c) {
    case 0x1:
    case 0x3F:
        if (pl->work882 < 0x4C) {
            pl_chr_set2(pl, 0x193, 6, 0);
        } else if (pl->work39C >= 0xF0 && pl->work011 == 1 && c != 0x3F) {
            pl_chr_set2(pl, 0x3F, 2, 0);
        }
        break;
    case 0x15:
        if (pl->work194 == 0) {
            pl_chr_set2(pl, 5, 4, 0x90);
        }
        break;
    case 0x5:
        if (!(*(u16 *)&game_w.x1E & 0x1F)) {
            Eft02_set6(0.1f, pl, 8, 0x14);
        }
        break;
    case 0x193:
        if (pl->work882 > 0x4B) {
            pl_chr_set2(pl, 1, 4, 0);
        }
        break;
    case 0x3E9:
        if (pl->flag12 != 0) {
            if (pl->kind == 1 || pl->kind == 5) {
                blend_set(pl, 0x3ED, 0x3EE);
            }
        }
        break;
    }
    gun_adj_sub(pl);
    basic_com_ck(pl);
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
