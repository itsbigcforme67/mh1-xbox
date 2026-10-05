/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"






void pl_mv028(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work39C = 0;
        pl->work40C = 6;
        pl->work08 = 0;
        Pl_view_reset(pl);
        pl->x43E = 0;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        t = pl->work56B;
        if (t & 0xF) {
            pl->work56B = t & 0xF0;
            func_549200(pl, 4);
        }
        if (arg1 == 0) {
            pl->x05 = 2;
            if ((pl->kind == 2) && (pl->flag12 != 0)) {
                pl_chr_set2(pl, 0x3F3, 2, 0);
                break;
            }
            pl_chr_set2(pl, 0x1C, 2, 0);
            break;
        }
        pl->x05++;
        pl_chr_set2(pl, 0x16, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x1C, 0, 6);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((Pl_master_ck(pl) == 1) && (arg1 == 0) && (pl->flag12 != 0)) {
            switch (pl->kind) {
            case 4:
                if ((pl->sw.an_trg & 0x20) && (frame_check3(10.0f, 44.0f, pl, 0) != 0) && (pl->x06 == 0)) {
                    pl->x06 = 1;
                    if (pl->sw.an_now & 0x800) {
                        pl->x06 = 2;
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->x06 = 3;
                    }
                }
                if (frame_check2(44.0f, pl, 0) != 0) {
                    t = pl->x06;
                    if (t != 0) {
                        if ((t & 0xFF) == 2) {
                            pl->ang[1] = pl->ang[1] + 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        if (pl->x06 == 3) {
                            pl->ang[1] = pl->ang[1] - 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        Pl_act_set2(pl, 1, 0x49, 0xC);
                    }
                }
                break;
            case 2:
                if ((frame_check2(40.0f, pl, 0) != 0) && (pl->sw.now & 0x80)) {
                    Pl_act_set(pl, 1, 0x18, 0);
                }
                break;
            }
        }
        break;
    }
}

void pl_mv030(PLW *pl) {
    f32 sp7C;
    s32 sp70[3];
    f32 sp60[3];
    f32 sp50[3];
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x12, 0, 0x58);
        pl->ang_y = pl->ang[1];
        sp70[0] = 0;
        sp70[1] = 0;
        sp70[2] = 0x425C0000;
        flvecApplyMat33(sp50, (f32 *)sp70, (f32 *)pl->rot);
        sp60[0] = pl->pos[0] + sp50[0];
        sp60[1] = pl->pos[1] + sp50[1];
        sp60[2] = pl->pos[2] + sp50[2];
        if (GetGroundHitAreaUpper(pl, sp60, &sp7C) == 1) {
            pl->pos[1] = sp7C;
        }
        sp70[2] = 0x425C0000;
        flvecApplyMat33(sp50, (f32 *)sp70, (f32 *)pl->rot);
        pl->pos[0] = pl->pos[0] + sp50[0];
        pl->pos[1] = pl->pos[1] + sp50[1];
        pl->pos[2] = pl->pos[2] + sp50[2];
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
