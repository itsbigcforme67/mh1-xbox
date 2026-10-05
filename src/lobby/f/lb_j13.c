/* lb_j13 - lobby move handlers 0x005D2A30-0x005D2C20: lb_pl_mv095, lb_pl_mv096. Whole file in lb_j.c. */
#include "lobby_f.h"






void pl_sleeping();



























void lb_pl_mv095(PLW *pl, s8 m) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x262, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            if (m == 0) {
                Lb_pl_chr_set(pl, 0x27D, 6, 0);
            } else {
                Lb_pl_chr_set(pl, 0x197, 6, 0x14);
                Eft06_set(4.0f, pl, 0, 8, 0xA);
            }
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) <= 0) {
            u16 id;
            Lb_pl_to_normal(pl, 0, 6, 0);
            id = pl->id;
            if (id == game_w.master) {
                Lb_eat_to_end(id);
            }
        }
        break;
    }
}

void lb_pl_mv096(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x267, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1 && (F(u16, pl, 0x368) & 0x3C60)) {
            Lb_act_set(pl, 0, 0x2A);
        }
        break;
    }
}
