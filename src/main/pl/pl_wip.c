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

void pl_dm002(PLW *pl) {
    f32 sp90[3];
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];
    f32 f;
    u8 s;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xCD, 0, 0x10);
        get_joint_pos(pl, 2, sp90);
        pl->pos[1] = sp90[1];
        Pl_basic_flagset(pl, 2, 0, 1);
        pl->flag604 = 0;
        rate_clear_g(pl);
        sp70[0] = 0;
        sp70[1] = pl->ang[1];
        sp70[2] = 0;
        cpRotMatrix(sp70, sp30);
        sp90[0] = 0.0f;
        sp90[1] = 0.0f;
        sp90[2] = -11.17f;
        flvecApplyMat33(sp80, sp90, sp30);
        pl->vel[0] = 2.0f * sp80[0];
        pl->vel[2] = 2.0f * sp80[2];
        pl->vel[1] = 6.0f;
        rate_g_calc(pl, 0xC);
        pl->vel[1] = 0.0f;
        pl->acc[1] = -0.33333334f;
        pl->work08 = 0x10;
        break;
    case 1:
        if ((pl->work194 == 0) && (pl->char0 == 0xCD)) {
            pl_chr_set2(pl, 0xD6, 0, 0);
        }
        rate_add_g(pl);
        if (pl->work08 > 0) {
            pl->work08 = pl->work08 - 1;
            f = 63.0f + pl->x5AC;
            if (pl->pos[1] <= f) {
                pl->pos[1] = f;
            }
        } else if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= (63.0f + pl->x5AC))) {
            pl->x05++;
            pl->pos[1] = pl->x5AC;
            Pl_se_req2_com(pl, 0x42, 0, pl->pos, 1, 0);
            armor_sd_req(pl, 3);
            pl_chr_set2(pl, 0xCE, 0, 0);
            pl->st = 3;
            if (game_w.stage == 0) {
                func_546860(pl, pl->pos, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            if (pl->vital > 0) {
                pl->x05 = s + 1;
                pl_chr_set2(pl, 0xCF, 2, 0);
                break;
            }
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 3, 1, 0);
                break;
            }
            pl->x05 = 4;
            pl_flag_set(pl, 0x20000);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            pl->work40C = 6;
        }
        break;
    case 4:
        break;
    }
}


void pl_dm005(PLW *pl) {
    f32 sp90[3];
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];
    f32 f;
    u8 s;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xD0, 0, 0);
        get_joint_pos(pl, 2, sp90);
        pl->pos[1] = sp90[1];
        pl->ang_y = pl->ang[1];
        Pl_basic_flagset(pl, 2, 0, 1);
        pl->flag604 = 0;
        rate_clear_g(pl);
        sp70[0] = 0;
        sp70[1] = (u16)pl->ang[1];
        sp70[2] = 0;
        cpRotMatrix(sp70, sp30);
        sp90[0] = 0.0f;
        sp90[1] = 0.0f;
        sp90[2] = 11.5f;
        flvecApplyMat33(sp80, sp90, sp30);
        pl->vel[0] = 2.0f * sp80[0];
        pl->vel[2] = 2.0f * sp80[2];
        pl->vel[1] = 0.0f;
        pl->acc[1] = -0.25f;
        pl->work08 = 0x10;
        break;
    case 1:
        if ((pl->work194 == 0) && (pl->char0 == 0xD0)) {
            pl_chr_set2(pl, 0xDC, 0, 0);
        }
        rate_add_g(pl);
        if (pl->work08 > 0) {
            pl->work08 = pl->work08 - 1;
            f = 63.0f + pl->x5AC;
            if (pl->pos[1] <= f) {
                pl->pos[1] = f;
            }
        } else if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= (63.0f + pl->x5AC))) {
            pl->x05++;
            pl->pos[1] = pl->x5AC;
            Pl_se_req2_com(pl, 0x42, 0, pl->pos, 1, 0);
            armor_sd_req(pl, 3);
            pl_chr_set2(pl, 0xD1, 0, 0);
            pl->st = 3;
            if (game_w.stage == 0) {
                func_546860(pl, pl->pos, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            if (pl->vital > 0) {
                pl->x05 = s + 1;
                pl_chr_set2(pl, 0xD8, 0, 0);
                break;
            }
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 3, 2, 0);
                break;
            }
            pl->x05 = 4;
            pl_flag_set(pl, 0x20000);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            pl->work40C = 6;
        }
        break;
    case 4:
        break;
    }
}

void pl_dm006(PLW *pl) {
    u8 s;
    u16 r;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0xCC, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        if (((ran_suu(1) & 0xFFFF) % 3) == 0) {
            pl_voice_req(pl, ((r = ran_suu(1)) & 1) + 0x20);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xCF, 6, 0x3A);
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
