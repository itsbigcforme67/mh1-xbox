/* Player code (SLPM_654.95 0x0013F970-0x00140100): pl_mv093 (pick up/gather), pl_mv076, pl_mv077, fish_com_ck (fishing commands), pl_mv078, pl_mv079 (fishing handlers) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv093(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 1, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x19B, -6, 0);
            break;
        }
        pl_chr_set2(pl, 0x1A6, -6, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            if (arg1 == 0) {
                pl_chr_set2(pl, 0x195, -4, 0x24);
                break;
            }
            pl_chr_set2(pl, 0x1A3, 0, 0x44);
        }
        break;
    case 2:
        if ((Pl_master_ck(pl) == 1) && (frame_check(180.0f, pl, 0) != 0)) {
            pick_set_sub(pl, arg1);
        }
        if (arg1 == 0) {
            if (frame_check2(200.0f, pl, 0) != 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x19C, -4, 0);
                break;
            }
        } else if (frame_check2(294.0f, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x1A7, -2, 0);
            break;
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            if (pl->x07 == 0) {
                Pl_act_set2(pl, 0, 0x1D, 2);
                break;
            }
            egg_set(pl);
            Pl_act_set2(pl, 5, 9, 0);
        }
        break;
    }
}

void pl_mv076(PLW *pl) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl->work8CA = 0;
        pl->work8D0 = 0;
        pl_chr_set2(pl, 0x320, -4, 0);
        BBQcamera_set(pl);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x55, 2);
        }
        break;
    }
}

void pl_mv077(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x321, 0, 0);
            break;
        }
        pl_chr_set2(pl, 0x322, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void fish_com_ck(PLW *pl) {
    if (Pl_master_ck(pl) != 0) {
        if (pl->flag14 != 0) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        if (Game_clear_ck(1) == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        switch (pl->flag15) {
        case 0x50:
            if (pl->sw.trg & 0x20) {
                if (pl->x881 > 0) {
                    Pl_act_set2(pl, 0, 0x53, 0);
                    break;
                }
                if (pl->x8EA == 1) {
                    Pl_act_set2(pl, 0, 0x64, 0);
                    break;
                }
                Pl_act_set2(pl, 0, 0x54, 0);
            }
            break;
        case 0x4E:
            if (pl->sw.trg & 0x20) {
                Pl_act_set(pl, 0, 0x4F, 0);
            }
            if (pl->sw.trg & 0x40) {
                pl_to_normal(pl, 0, 4, 0);
            }
            break;
        case 0x51:
        case 0x52:
            if (pl->sw.trg & 0x20) {
                Pl_act_set2(pl, 0, 0x53, 0);
            }
            break;
        case 0x4F:
        case 0x53:
            break;
        }
    }
}

void pl_mv078(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl->work08 = 0;
        pl->x07 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x324, 0, 0);
            break;
        }
        pl_chr_set2(pl, 0x326, 2, 0);
        break;
    case 1:
        if ((pl->x881 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
        }
        fish_com_ck(pl);
        break;
    }
}

void pl_mv079(PLW *pl) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl->x881 = 0;
        pl->fish_time = 0;
        pl->x8EA = 1;
        pl_chr_set2(pl, 0x325, -2, 0);
        break;
    case 1:
        if (frame_check(104.0f, pl, 0) != 0) {
            if (pl->work88A == 0x7D) {
                func_555A90(pl, 1);
            } else {
                func_555A90(pl, 0);
            }
        }
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x50, 0);
        }
        break;
    }
}
