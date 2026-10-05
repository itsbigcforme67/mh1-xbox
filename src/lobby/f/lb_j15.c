/* lb_j15 - x 0x005D0120-0x005D0454: lb_pl_mv001. Whole file in lb_j.c. */
#include "lobby_f.h"






void pl_sleeping();




























void lb_pl_mv001(PLW *pl, int mode) {
    int t;
    u8 a;
    u8 s;
    u8 c;
    u16 ch;
    s = pl->x05;
    if (s == 0) {
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0xA;
        pl->work08 = 0xA;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_flag_set(pl, 0x1000);
        switch (mode) {
        case 0:
            if (PLU8(pl, 0x8EC) != 0) {
                if (pl->char0 != 0x27) {
                    Lb_pl_chr_set(pl, 0x27, 6, 0);
                }
            } else if (pl->char0 != 3) {
                Lb_pl_chr_set(pl, 3, 6, 0);
            }
            Lb_pl_flag_set(pl, 0x02000000);
            break;
        case 1:
            if (PLU8(pl, 0x8EC) != 0) {
                if (pl->char0 != 0x258) {
                    Lb_pl_chr_set(pl, 0x258, 4, 0);
                }
            } else if (pl->work011 == 1) {
                Lb_pl_chr_set(pl, 0x40, 4, 0);
            } else {
                Lb_pl_chr_set(pl, 2, 4, 0);
            }
            break;
        case 6:
            pl->work760 = 0x1E;
            if (PLU8(pl, 0x8EC) != 0) {
                if (pl->char0 != 0x27) {
                    Lb_pl_chr_set(pl, 0x27, 4, 0);
                }
            } else if (pl->char0 != 3) {
                Lb_pl_chr_set(pl, 3, 0, 0x3A);
            }
            Lb_pl_flag_set(pl, 0x02000000);
            break;
        case 9:
            Lb_pl_chr_set(pl, 0x258, 4, 0);
            break;
        }
        pl->work39C = 0;
    }
    a = Lb_stick_pow_get(pl);
    c = pl->x07;
    if (c != 0) {
        pl->x07 = c - 1;
    }
    t = pl->work08;
    if (t > 0) {
        pl->work08 = t - 1;
    }
    if (a == 0 && pl->work08 == 0) {
        ch = pl->char0;
        if (ch == 3 || ch == 4) {
            if (pl->x07 == 0) {
                Lb_Pl_act_set(pl, 0, 0x2C, 0);
            } else {
                Lb_pl_to_normal(pl, 0, 4, 0);
            }
        } else {
            Lb_pl_to_normal(pl, 0, 4, 0);
        }
        return;
    }
    if (pl->char0 == 3 && a < 5) {
        Lb_pl_to_normal(pl, 0, 4, 0);
        return;
    }
    if (pl->id == game_w.master && a != 0) {
        pl->ang_y = Lb_stick_dir_set(pl, 0);
    }
    lb_basic_com_ck(pl);
}
