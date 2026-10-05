/* Player code (SLPM_654.95 0x0013ED50-0x0013F1B0): kabe_hosei (push along the wall), kabe_com_ck (wall-hug command), pl_mv064, pl_mv065 */
#include "pl.h"
#include "game.h"
#include "plf.h"

void kabe_hosei(PLW *pl, int arg1) {
    f32 sp90[3];
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];

    sp70[0] = 0;
    sp70[2] = 0;
    sp70[1] = pl->ang[1];
    cpRotMatrix(sp70, sp30);
    sp90[0] = 0.0f;
    sp90[1] = 0.0f;
    sp90[2] = -3.0f;
    if ((s16)arg1 != 0) {
        sp90[2] *= -1.0f;
    }
    flvecApplyMat33(sp80, sp90, sp30);
    pl->pos[0] = pl->pos[0] + sp80[0];
    pl->pos[2] = pl->pos[2] + sp80[2];
}

void kabe_com_ck(PLW *pl) {
    s32 p;
    s32 d;

    if (Pl_master_ck(pl) != 0) {
        p = stick_pow_get(pl, 0) & 0xFF;
        switch (p) {
        case 1:
        case 2:
        case 3:
            d = ((stick_dir_set(pl, 0) & 0xFFFF) - *(u16 *)&pl->ang[1]) & 0xFFFF;
            if ((d >= 0x2000) && (d < 0x6001)) {
                if (act_ck(pl, 0, 0x42) == 0) {
                    Pl_act_set(pl, 0, 0x42, 0);
                }
            } else if ((d >= 0xA000) && (d < 0xE001)) {
                if (act_ck(pl, 0, 0x41) == 0) {
                    Pl_act_set(pl, 0, 0x41, 0);
                }
            } else if ((act_ck(pl, 0, 0x40) == 0) && (act_ck(pl, 0, 0x43) == 0)) {
                Pl_act_set(pl, 0, 0x43, 0);
            }
            break;
        default:
            if ((act_ck(pl, 0, 0x40) == 0) && (act_ck(pl, 0, 0x43) == 0)) {
                Pl_act_set(pl, 0, 0x43, 0);
            }
            break;
        }
        if (!(pl->sw.trg & 0x40)) {
            if (!(wall_vec_set(pl, 1) & 0xFF)) {
                goto go;
            }
        } else {
go:
            pl->work8F0 = 1;
            Pl_act_set2(pl, 0, 0x2C, 0);
        }
    }
}

void pl_mv064(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x2B, 2, 0);
        } else {
            pl_chr_set2(pl, 0x2B, 2, 0x1E);
        }
        Pl_basic_flagset(pl, 0x8000, 0, 0);
        kabe_hosei(pl, 0);
        break;
    case 1:
        kabe_hosei(pl, 0);
        if (frame_check2(30.0f, pl, 0) != 0) {
            kabe_com_ck(pl);
        }
        break;
    }
}

void pl_mv065(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x2C, 2, 0);
        } else {
            pl_chr_set2(pl, 0x2D, 2, 0);
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        kabe_hosei(pl, 0);
        break;
    case 1:
        kabe_hosei(pl, 0);
        kabe_com_ck(pl);
        break;
    }
}
