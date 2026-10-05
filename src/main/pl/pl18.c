/* Player code (SLPM_654.95 0x0013E410-0x0013EA38): pl_mv054 (sougun aim), 072, 055, 057, 058, 059 (action handlers) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv054(PLW *pl) {
    f32 sp20[3];
    u16 sp2C;
    u8 sp2F;
    u8 s;
    s32 w;
    u8 b;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19E, 4, 0);
        action_timer_calc(pl, 0);
        pl->work08 = 0x3C;
        pl->x06 = 0x1E;
        pl->x8EE = 0;
        if ((St_unique_ck(pl, sp20, &sp2C, &sp2F) & 0xFFFF) == 0x11) {
            pl->work2D4 = sp2C;
            break;
        }
        pl->work2D4 = *(u16 *)&pl->ang_y;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        b = pl->x06;
        if (b > 0) {
            pl->x06 = b - 1;
        }
        if (Pl_master_ck(pl) == 1) {
            if (Game_clear_ck(1) == 1) {
                pl_to_normal(pl, 0, 6, 0);
                break;
            }
            if (pl->x06 == 0) {
                sougun_adj_sub(pl, 0x19E);
                if ((pl->sw.trg & 0x20) && ((s16)Pl_item_num_ck(pl, 0xA2) != 0)) {
                    Pl_act_set2(pl, 0, 0x48, 0xC);
                }
            }
            if ((pl->work08 == 0) && (pl->sw.pad08[2] & 0x40) && (pl->work88C == 0)) {
                pl_to_normal(pl, 0, 6, 0);
            }
        }
        break;
    }
}

void pl_mv072(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19E, 0, 0x42);
        action_timer_calc(pl, 0);
        pl->work08 = 0x3C;
        if ((Pl_master_ck(pl) != 1) || ((s16)Pl_item_num_ck(pl, 0xA2) != 0)) {
            Pl_item_stack(pl, 0xA2, -1);
            func_637F60(pl, 0);
        }
        break;
    case 1:
        pl->work08 = pl->work08 - 1;
        if (pl->work08 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv055(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0xD3, 4, 0);
        action_timer_calc(pl, 0);
        vib_set_pl(pl, 3);
        Pl_set_quake_sub(pl, 5);
        func_637F60(pl, 1);
        break;
    case 1:
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xD4, 2, 0);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv057(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 1, 0, 0);
        pl_chr_set2(pl, 9, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv058(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x12, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv059(PLW *pl) {
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0xA, 8, 0);
        Pl_basic_flagset(pl, 1, 0, 0);
        pl->flag12 = 0;
        pl_flag_set(pl, 0x1000);
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
                Pl_act_set(pl, 0, 0x2D, 4);
                break;
            }
            sit_com_ck(pl);
        }
        break;
    }
}
