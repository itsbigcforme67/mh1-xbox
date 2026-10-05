/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_dm000(PLW *pl) {
    u8 s;
    u16 r;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0xCA, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        if (((ran_suu(1) & 0xFFFF) % 3) == 0) {
            pl_voice_req(pl, ((r = ran_suu(1)) & 1) + 0x20);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 6, 0x22);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            pl->work40C = 6;
        }
        break;
    }
}

void pl_dm001(PLW *pl, s32 arg1) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x586, 2, 0);
        } else {
            pl_chr_set2(pl, 0x587, 0, 0);
        }
        action_timer_calc(pl, 0);
        Pl_se_req2_com(pl, 0x67, 0, pl->pos, 1, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        vib_set_pl(pl, 1);
        pl_flag_set(pl, 2);
        pl_flag_set(pl, 0x8000);
        break;
    case 1:
        if (arg1 == 0) {
            if (frame_check2(30.0f, pl, 0) == 0) {
                sp30[0] = 0.0f;
                sp30[1] = 0.0f;
                switch (pl->kind) {
                case 3:
                    sp30[2] = -15.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 32.0f)) / 360.0f)));
                    break;
                case 0:
                case 4:
                    sp30[2] = -20.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 32.0f)) / 360.0f)));
                    break;
                }
                flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
                pl->pos[0] = pl->pos[0] + sp20[0];
                pl->pos[2] = pl->pos[2] + sp20[2];
            }
        } else {
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            switch (pl->kind) {
            case 3:
                if (frame_check2(104.0f, pl, 0) == 0) {
                    sp30[2] = -16.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 104.0f)) / 360.0f)));
                }
                break;
            case 0:
                if (frame_check2(104.0f, pl, 0) == 0) {
                    sp30[2] = -22.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 104.0f)) / 360.0f)));
                }
                break;
            case 4:
                if (frame_check2(94.0f, pl, 0) == 0) {
                    sp30[2] = -18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 94.0f)) / 360.0f)));
                }
                break;
            }
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp20[0];
            pl->pos[2] = pl->pos[2] + sp20[2];
        }
        if (pl->work194 == 0) {
            switch (pl->kind) {
            case 3:
                Pl_act_set2(pl, 2, 0xC, 0);
                break;
            case 4:
                pl_to_normal(pl, 0, 6, 0);
                break;
            default:
            case 0:
                pl_to_normal(pl, 0, 0xA, 0);
                break;
            }
        }
        break;
    }
}
