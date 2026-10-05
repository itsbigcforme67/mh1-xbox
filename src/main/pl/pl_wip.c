/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv014(PLW *pl, s32 arg1) {
    int a2;
    f32 f;
    u8 s;
    u8 t;
    u8 idx;
    int off;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x191, 2, 0);
        } else {
            pl_chr_set2(pl, 0x1A1, 2, 0);
        }
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (arg1 != 0) {
            f = 20.0f;
        } else {
            f = 24.0f;
        }
        if (frame_check(f, pl, 0) != 0) {
            a2 = 0;
            if (pl->work016 == 0) {
                t = pl->work017;
                if (t == 1) {
                    a2 = 2;
                }
                if (t == 2) {
                    a2 = 1;
                }
            }
            switch (pl->work88A) {
            case 0x1A:
                func_628FB0(pl, 0, a2);
                break;
            case 0xA5:
                func_628FB0(pl, 7, a2);
                break;
            case 0x1B:
                func_628FB0(pl, 1, a2);
                break;
            case 0x1C:
                func_628FB0(pl, 5, a2);
                break;
            case 0x80:
                func_628FB0(pl, 6, a2);
                break;
            case 0x68:
                func_628FB0(pl, 8, a2);
                break;
            case 0x21:
                func_628FB0(pl, 9, a2);
                break;
            case 0x9F:
                func_628FB0(pl, 0xA, a2);
                break;
            }
            if (Game_clear_ck(1) == 0) {
                Pl_item_stack(pl, pl->work88A, -1);
            }
        }
        if (pl->work194 == 0) {
            switch (arg1) {
            case 0:
                pl_to_normal(pl, 0, 0xA, 0);
                break;
            case 1:
                pl_to_normal(pl, 0, 2, 0);
                break;
            case 2:
                if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
                    idx = game_w.x2F;
                    pl->x738 = 1;
                    pl->work73C = stage_start_pos[idx][0];
                    pl->work740 = stage_start_pos[idx][1];
                    pl->work744 = stage_start_pos[idx][2];
                    pl->work570 = stage_start_ang[idx];
                    pl->x73A = idx;
                    Pl_ofs_set(pl, &pl->work73C, pl->work570);
                    net_send_pl(pl, 5, 0);
                } else {
                    pl_to_normal(pl, 0, 2, 0);
                }
                break;
            }
        }
        break;
    case 2:
        break;
    }
}

void pl_mv017(PLW *pl, s32 arg1) {
    f32 spCC;
    s32 spC0[3];
    f32 spB0[3];
    f32 spA0[3];
    f32 sp90[3];
    f32 sp50[16];
    u8 s;
    s32 w;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->ang_y = pl->ang[1];
        pl_chr_set2(pl, 0x11, 0, 0);
        pl->work08 = 0x1E;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 1) {
            spC0[0] = 0;
            spC0[1] = pl->ang[1];
            spC0[2] = 0;
            cpRotMatrix(spC0, sp50);
            spB0[0] = 0.0f;
            spB0[1] = 0.0f;
            spB0[2] = 55.0f;
            flvecApplyMat33(sp90, spB0, sp50);
            spA0[0] = pl->pos[0] + sp90[0];
            spA0[1] = pl->pos[1] + sp90[1];
            spA0[2] = pl->pos[2] + sp90[2];
            if (GetGroundHitAreaUpper(pl, spA0, &spCC) == 1) {
                pl->pos[1] = spCC;
            }
            spB0[2] = 55.0f;
            flvecApplyMat33(sp90, spB0, (f32 *)((u8 *)pl + 0x60));
            pl->pos[0] = pl->pos[0] + sp90[0];
            pl->pos[1] = pl->pos[1] + sp90[1];
            pl->pos[2] = pl->pos[2] + sp90[2];
        }
        if ((act_ck(pl, 0, 0x1B) == 1) && (pl->work016 == 0) && (pl->work017 == 9)) {
            pl->work937++;
        }
        break;
    case 1:
        w = pl->work08;
        if (w != 0) {
            pl->work08 = w - 1;
            if (pl->work08 <= 0) {
                pl->work08 = 0;
            }
        }
        if (pl->work194 == 0 || (pl->sw.trg & 0x20) || (pl->sw.an_now & 0x2000)) {
            Pl_act_set2(pl, 0, 0x3A, 0xC);
        } else if ((Pl_master_ck(pl) == 1) && (pl->sw.an_trg & 0x1000) && (pl->work08 == 0)) {
            Pl_act_set2(pl, 0, 0x38, 4);
        }
        break;
    }
}
