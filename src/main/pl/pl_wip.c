/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

















































void pl_mv083(PLW *pl) {
    EMW *e;
    s16 i;
    u8 s;

    e = em_work;
    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        pl->work2DA = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        i = 0;
        do {
            if ((e->stg == pl->stg) && (e->x8B9 != 0) && (e->x87C == pl->id)) {
                pl->x06 = 1;
            }
            i++;
            e++;
        } while (i < 0x14);
        pl_chr_set2(pl, 0x327, 2, 0);
        break;
    case 1:
        if (pl->x06 == 0) {
            if (frame_check(118.0f, pl, 0) != 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x328, 2, 0);
                break;
            }
            i = 0;
            do {
                if ((e->stg == pl->stg) && (e->x8B9 != 0) && (e->x87C == pl->id)) {
                    pl->x06 = 1;
                }
                i++;
                e++;
            } while (i < 0x14);
            break;
        }
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x07++;
            if (pl->x07 >= 2) {
                pl->x05 = 3;
                pl_chr_set2(pl, 0x32A, 4, 0);
                if (Game_clear_ck(1) == 0) {
                    Pl_item_stack(pl, pl->work88A, -1);
                }
            }
        }
        break;
    case 2:
        if ((frame_check(120.0f, pl, 0) != 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(1, 0xB, (s16)pl->fish_time);
            adx_se_set(pl, 5);
            Pl_item_stack(pl, pl->work88A, -1);
            if ((s16)ItemStockRequest(pl, pl->fish_time, pl->x8E6, 0) == 3) {
                set01_set(1, 3, (s16)pl->fish_time);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 2, 0x24);
        }
        break;
    case 4:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv084(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x329, 2, 0);
        break;
    case 1:
        if ((frame_check(120.0f, pl, 0) != 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            if (arg1 == 0) {
                adx_se_set(pl, 8);
                Pl_item_stack(pl, pl->work88A, -1);
                set01_set(0, 3, 0);
            } else {
                set01_set(0, 0xD, 0);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
