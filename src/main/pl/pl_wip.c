/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"




























void pl_at036(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x06 = 0;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        switch (arg1) {
        case 0:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57D, 4, 0);
            func_6362B0(pl, 0x10);
            break;
        case 1:
            pl->x05++;
            pl_chr_set2(pl, 0x3F2, 2, 0);
            pl->flag12 = 1;
            func_6362B0(pl, 0x14);
            break;
        case 2:
        case 3:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57D, 6, 0xA);
            if (arg1 == 3) {
                func_6362B0(pl, 0x24);
                break;
            }
            func_6362B0(pl, 0x10);
            break;
        case 4:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57D, 4, 0xA);
            func_6362B0(pl, 0x10);
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57D, 0, 0x22);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((lance_kan_ck(pl) != 1) && (Pl_master_ck(pl) == 1) && (frame_check3(36.0f, 62.0f, pl, 0) != 0)) {
            ex_atk_ck(pl, 0);
            if ((pl->sw.an_trg & 0x3C) && (arg1 != 3)) {
                if (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC) {
                    if (arg1 == 2) {
                        Pl_act_set(pl, 1, 0x36, 4);
                        break;
                    }
                    Pl_act_set(pl, 1, 0x35, 4);
                    break;
                }
                if (arg1 == 2) {
                    Pl_act_set(pl, 1, 0x3C, 4);
                    break;
                }
                Pl_act_set(pl, 1, 0x3B, 4);
            }
        }
        break;
    }
}

void pl_at037(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x40A = 0;
        pl->work409 = 0;
        pl_chr_set2(pl, 0x57C, 0, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0;
        break;
    case 1:
        Pl_stamina_calc(pl, -2);
        if (pl->work194 == 0) {
            if ((arg1 == 0) && (pl->x06 == 0)) {
                Pl_act_set2(pl, 1, 0x26, 0);
                break;
            }
            Pl_act_set2(pl, 1, 0x23, 0);
            break;
        }
        if ((pl->sw.an_trg & 0x10) || (pl->sw.trg & 1)) {
            pl->x06 = 1;
        }
        break;
    }
}

void pl_at040(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x588, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x12);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
            break;
        }
        if ((lance_kan_ck(pl) != 1) && (frame_check2(18.0f, pl, 0) != 0) && (frame_check2(28.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
