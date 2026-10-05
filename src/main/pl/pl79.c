/* Player code (SLPM_654.95 0x0014D320-0x0014DC8C): Basic_item_set (item effects) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void Pl_vital_calc_item(PLW *, int);
void Pl_max_vital_calc(PLW *, int);
void func_639DF0(PLW *, int);
void set01_set(int, int, int);

void Basic_item_set(PLW *pl) {
    PLW *t = &player_work[game_w.master];
    pl->work917 = 1;
    switch (pl->work88A) {
    case 0xD:
    case 0xE:
        if (Pl_master_ck(pl) == 1) {
            if (pl->work88A == 0xD) {
                pl->work6A4 = 3;
            } else {
                pl->work6A4 = 5;
            }
        }
        Eft06_set(4.0f, pl, 0, 2, 0xA);
        goto chr197;
    case 0xF:
    case 0x10:
        if (Pl_master_ck(pl) == 1) {
            if (pl->work88A == 0xF) {
                pl->work6A8 = 3;
            } else {
                pl->work6A8 = 5;
            }
        }
        Eft06_set(4.0f, pl, 0, 3, 0xA);
        goto chr197;
    case 0x53:
        if (Pl_master_ck(pl) == 1) {
            pl->work6A5 = 0xA;
            pl->work6A6 = 0x1518;
        }
        Eft06_set(4.0f, pl, 0, 2, 0xA);
        if (Pl_master_ck(pl) == 0 && pl->stg == t->stg && Pl_Skill_ck(pl, 0x1E) == 1) {
            t->work6A5 = 0xA;
            t->work6A6 = 0x1518;
            Eft06_set(4.0f, t, 0, 2, 0xA);
        }
        goto chr197;
    case 0x54:
        if (Pl_master_ck(pl) == 1) {
            pl->work6A9 = 0x14;
            pl->work6AA = 0x1518;
        }
        Eft06_set(4.0f, pl, 0, 3, 0xA);
        if (Pl_master_ck(pl) == 0 && pl->stg == t->stg && Pl_Skill_ck(pl, 0x1F) == 1) {
            t->work6A9 = 0x14;
            t->work6AA = 0x1518;
            Eft06_set(4.0f, t, 0, 3, 0xA);
        }
        goto chr197;
    case 0xA1:
        pl->work91A = 0x4650;
        Eft06_set(4.0f, pl, 0, 6, 0xA);
        if (Pl_master_ck(pl) == 1) {
            set01_set(0, 0x11, 0);
        }
        goto chr197;
    case 0xA0:
        pl->work918 = 0x4650;
        Eft06_set(4.0f, pl, 0, 5, 0xA);
        if (Pl_master_ck(pl) == 1) {
            set01_set(0, 0x10, 0);
        }
        goto chr197;
    case 0xA4:
        pl->work930 = 0x12C;
        Eft06_set(4.0f, pl, 0, 9, 0xA);
        goto chr197;
    case 0x99:
        pl->work91C = 0x2328;
        Eft06_set(4.0f, pl, 0, 7, 0xA);
        if (Pl_master_ck(pl) == 1) {
            set01_set(0, 0x17, 0);
        }
        goto chr197;
    case 9:
        Pl_max_stamina_calc(pl, 0x4B);
        pl->work8CC = 0x1518;
        if (Pl_master_ck(pl) == 1) {
            set01_set(0, 0x18, 0);
        }
        goto eft0;
    case 0xA:
        Pl_max_stamina_calc(pl, 0x96);
        pl->work8CC = 0x2A30;
        if (Pl_master_ck(pl) == 1) {
            set01_set(0, 0x18, 0);
        }
        goto eft0;
    case 8:
        Pl_max_stamina_calc(pl, 0x1C2);
    case 7:
        Pl_max_vital_calc(pl, 0x96);
        Pl_vital_calc_item(pl, 0x96);
        goto eft0;
    case 0x41:
        Pl_vital_calc_item(pl, 0x14);
        goto eft0;
    case 0x9A:
        Pl_vital_calc_item(pl, 0x32);
        if (Pl_master_ck(pl) == 0 && pl->stg == t->stg) {
            Pl_vital_calc_item(t, 0x32);
            Eft06_set(4.0f, t, 0, 0, 0xA);
        }
        goto eft0;
    case 1:
        Pl_vital_calc_item(pl, 0x1E);
        if (Pl_master_ck(pl) == 0 && pl->stg == t->stg && Pl_Skill_ck(pl, 0x1C) == 1) {
            Pl_vital_calc_item(t, 0x1E);
            Eft06_set(4.0f, t, 0, 0, 0xA);
        }
        goto eft0;
    case 0x9C:
        Pl_vital_calc_item(pl, 0x1E);
        goto eft0;
    case 0x5F:
        Pl_vital_calc_item(pl, 0x14);
        goto eft0;
    case 0x4B:
        func_639DF0(pl, 0xA);
        if (pl->cnt39A & 1) {
            Pl_max_vital_calc(pl, 0xA);
        } else {
            Pl_max_vital_calc(pl, -0xA);
        }
        pl_chr_set2(pl, 0x198, 4, 0);
        break;
    case 0x5B:
        Pl_vital_calc(pl, -0xA);
        if (pl->vital <= 0) {
            pl->vital = 1;
        }
        if (pl->cnt39A & 1) {
            pl->x7BA = 0;
            pl_chr_set2(pl, 0x197, 4, 0);
            Eft06_set(4.0f, pl, 0, 1, 0xA);
        } else {
            pl_chr_set2(pl, 0x198, 4, 0);
        }
        break;
    case 0x42:
        if (pl->cnt39A & 1) {
            pl->x7BA = 0;
            pl_chr_set2(pl, 0x197, 4, 0);
            Eft06_set(4.0f, pl, 0, 1, 0xA);
        } else {
            pl_chr_set2(pl, 0x198, 4, 0);
        }
        break;
    case 2:
        Pl_vital_calc_item(pl, 0x32);
eft0:
        Eft06_set(4.0f, pl, 0, 0, 0xA);
chr197:
        pl_chr_set2(pl, 0x197, 4, 0);
        break;
    case 3:
        Pl_max_vital_calc(pl, 0xA);
        goto eft0b;
    case 4:
        Pl_max_vital_calc(pl, 0x14);
eft0b:
        Eft06_set(4.0f, pl, 0, 0, 0xA);
        pl_chr_set2(pl, 0x197, 4, 0);
        break;
    case 0x9D:
        Pl_max_stamina_calc(pl, 0x4B);
        Eft06_set(4.0f, pl, 0, 4, 0xA);
        goto chr197;
    case 0x13:
        Pl_max_stamina_calc(pl, 0x4B);
        goto eft4;
    case 0x14:
        Pl_max_stamina_calc(pl, 0x96);
eft4:
        Eft06_set(4.0f, pl, 0, 4, 0xA);
        pl_chr_set2(pl, 0x192, 4, 0);
        break;
    case 0x15:
        if (pl->cnt39A & 1) {
            Pl_max_stamina_calc(pl, 0x4B);
            goto eft4;
        }
        Pl_max_stamina_calc(pl, -0x4B);
        pl_chr_set2(pl, 0x198, 4, 0);
        break;
    case 6:
        Pl_vital_calc_item(pl, 0xA);
    case 5:
        pl->x7BA = 0;
        pl_chr_set2(pl, 0x197, 4, 0);
        Eft06_set(4.0f, pl, 0, 1, 0xA);
        if (Pl_master_ck(pl) == 0 && pl->stg == t->stg && Pl_Skill_ck(pl, 0x1D) == 1 && pl->work88A == 5) {
            t->x7BA = 0;
            Eft06_set(4.0f, t, 0, 1, 0xA);
        }
        break;
    }
    Pl_item_cnt_up(pl);
    if (Game_clear_ck(1) == 0) {
        Pl_item_stack(pl, pl->work88A, -1);
    }
}
