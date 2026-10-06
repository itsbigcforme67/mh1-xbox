/* em04 - game.bin 0x0058BA40-0x0058F4A0. Per-monster AI for monster kind 4
 * (action setters, action steps em_act*, move states em_move*, damage and
 * death). Names of the steps follow the split (em_act00, em_move00...).
 * Meanings of most fields are guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM04W {
    u8 eff;             /* 0x00 em04_effect_move step */
    u8 _pad01[5];
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    u8 _pad08[4];
    u16 x0C;            /* 0x0C counted down each frame */
    u8 _pad0E[6];
    f32 home[3];        /* 0x14 position it returns to (mov05) */
    u8 _pad20[4];
    u16 tgt_ang;        /* 0x24 facing to turn to (mov00) */
} EM04W;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
u16 em_act_search(void *);
void target_kind_set(EMW *, f32 *);
void em_action_timer_calc(EMW *, int);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
u16 Em_Calc_angY(f32 *, f32 *);
void em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
u32 ran_suu(int);
f32 flvecCalcDistance(f32 *, f32 *);
void pl_flag_set(EMW *, u32);
void pl_flag_clr(EMW *, u32);
void em_cmd_reset(EMW *);
void shell02_set(EMW *, int);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
int em_frame_check(EMW *, int, f32);
void em_rate_clear_g(EMW *);
int rate_add_g2(EMW *);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void em_rate_clear(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
int Event_flag_ck();
void em_dur_set(EMW *, int);
void em_cmd_ck(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
int act_ck(EMW *, int, int);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void Quest_enemy_escape(EMW *);
void em04_init(EMW *);
void em04_act_set();
#define em04_act_set_k em04_act_set
void em04_main_sub(EMW *em);
void move_default_0058E4F0(EMW *em);
void Eft13_set_em_scl(EMW *, int, f32, int);
int Code_Make(int, int, int, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
static void sound_call_0058F430(EMW *em, int frame, int se);

extern u8 em04_act_tbl[];
extern f32 em05_rev_set_tbl_st69[][6];
extern f32 em05_rev_set_tbl_st18[][6];

void em04_next_act_set(EMW *em);

void em04_act_set(em, kind, no)
EMW *em;
int kind;
u16 no;
{
    em->act_spd = 1.0f;
    switch ((u16)kind) {
    case 0:
        switch ((u16)no) {
        case 1:
            if (em->x888 == 1) {
                if (em->kind == 4) {
                    no = 12;
                } else {
                    no = 6;
                }
            }
            break;
        }
        break;
    case 1: {
        f32 *q;
        switch ((u16)no) {
        case 1:
        case 2:
        case 3:
            em->work08 = 1800;
            target_kind_set(em, em->tgt_pos);
            break;
        case 4:
            em->work08 = 600;
            switch (em->stg) {
            case 0x12:
                q = em05_rev_set_tbl_st18[em->type];
                break;
            case 0x45:
            default:
                q = em05_rev_set_tbl_st69[em->type];
                break;
            }
            em->pos[0] = *q++;
            em->pos[1] = *q++;
            em->pos[2] = *q++;
            em->tgt_pos[0] = *q++;
            em->tgt_pos[1] = *q++;
            em->tgt_pos[2] = *q++;
            break;
        }
        break;
    }
    case 3:
        target_kind_set(em, em->tgt_pos);
        switch ((u16)no) {
        case 0:
            em->work08 = 90;
            em->act_spd = 0.8f;
            break;
        case 1:
            em->work08 = 90;
            em->act_spd = 0.8f;
            break;
        case 2:
            em->work08 = 90;
            em->act_spd = 1.0f;
            break;
        case 3:
            em->work08 = 120;
            em->act_spd = 1.0f;
            break;
        case 4:
            em->work08 = 30;
            em->act_spd = 0.8f;
            break;
        case 5:
            em->work08 = 45;
            em->act_spd = 0.8f;
            break;
        }
        break;
    }
    em_act_set(em, kind, no);
}

void em04_next_act_set(EMW *em) {
    if (em->x734 == 3) {
        em->x839 = 1;
        em_act_set(em, 0, 1);
    } else {
        em_act_set(em, 0, em_act_search(em04_act_tbl));
    }
}

static void em_act00(EMW *em) {
}

static void em_act01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->work08 = 80;
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 10, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->work08 = (u16)ran_suu(1) % 60 + 30;
        em_char_set(em, 10, 6, 0);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 13, 6, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 7, 6, 0);
        em_action_timer_calc(em, 0);
        break;
    case 1:
        if (em->x39C >= 240) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act04(EMW *em, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->work08 = n;
        em_char_set(em, 14, 6, 0);
        em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
        cpRotMatrix(em->ang, em->mat);
        if (em->x194 == 0) {
            if (--em->work08 <= 0) {
                em04_next_act_set(em);
            }
        }
        break;
    }
}

static void em_act05(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 12, 6, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act06(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 17, 6, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act07(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 70, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 10.0f;
        em_sleep_eff_set(em, 11, v, 1.0f);
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 11);
        }
        break;
    }
}

static void em_act08(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x88B = 1;
        Em_Sleep_End(em);
        em_char_set(em, 73, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_act_set(em, 0, 9);
        }
        break;
    }
}

static void em_act09(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 11, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_act10(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 19, 6, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_move00(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_act00(em); break;
    case 1: em_act01(em); break;
    case 2: em_act02(em); break;
    case 3: em_act03(em); break;
    case 4: em_act04(em, 1); break;
    case 5: em_act05(em); break;
    case 6: em_act06(em); break;
    case 7: em_act04(em, 2); break;
    case 8: em_act04(em, 4); break;
    case 9: em_act09(em); break;
    case 12: em_act10(em); break;
    case 10: em_act07(em); break;
    case 11: em_act08(em); break;
    }
}

static void em_mov00(EMW *em, int flag) {
    EM04W *w = (EM04W *)em->ex;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        if (flag == 0) {
            w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        } else {
            w->tgt_ang = em->ang[1] + 0x4000;
        }
        d = (u16)(w->tgt_ang - em->ang[1]);
        if (d <= 0x1000 || d >= 0xF000) {
            pl_flag_set(em, 0x20000);
            em_char_set(em, 2, 0, 0);
        } else if (d >= 0x8000) {
            em_char_set(em, 5, 0, 0);
        } else {
            em_char_set(em, 4, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            u32 spd = (u32)(16384.0f / (em->x1A8 / 2.0f) * em->act_spd);
            u32 ang = em->ang[1];

            d = (u16)(w->tgt_ang - (u16)ang);
            if (em->x194 == 0) {
                if ((u16)(d + spd) < spd * 2) {
                    em->x05++;
                    pl_flag_clr(em, 0x20000);
                    em04_next_act_set(em);
                } else if (d <= 0x1000 || d >= 0xF000) {
                    pl_flag_set(em, 0x20000);
                    em_char_set(em, 2, 0, 0);
                } else {
                    pl_flag_clr(em, 0x20000);
                    if (d >= 0x8000) {
                        em_char_set(em, 5, 0, 0);
                    } else {
                        em_char_set(em, 4, 0, 0);
                    }
                }
            } else if ((u16)(d + spd) < spd * 2) {
                em->ang[1] = w->tgt_ang;
            } else if (d < 0x8000) {
                em->ang[1] = (u16)(ang + spd);
            } else {
                em->ang[1] = (u16)(ang - spd);
            }
        }
        break;
    }
}


static void em_mov01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 2, 6, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 200.0f || --em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 3, 6, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 8, 6, 0);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 200.0f || --em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov04(EMW *em) {
    u32 a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 2, 0, 0);
        a = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        em->horm_ang = a;
        em->ang[1] = a;
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 200.0f || --em->work08 <= 0) {
            em_cmd_reset(em);
            em04_next_act_set(em);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_mov05(EMW *em) {
    EM04W *w = (EM04W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->work08 = ((u16)ran_suu(1) & 0x7F) + 120;
        em_char_set(em, 2, 6, 0);
        break;
    case 1:
        if (em->work08 > 0) {
            em->work08--;
        }
        if (em->x8C3 == 0 && em->work08 <= 0) {
            em04_next_act_set(em);
        } else {
            em->horm_ang = (u16)Em_Calc_angY(em->pos, w->home);
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x80);
            cpRotMatrix(em->ang, em->mat);
        }
        break;
    }
}

static void em_move01(EMW *em) {
    switch (em->x15) {
    case 0: em_mov00(em, 0); break;
    case 1: em_mov01(em); break;
    case 2: em_mov02(em); break;
    case 3: em_mov03(em); break;
    case 4: em_mov04(em); break;
    case 5: em_mov05(em); break;
    case 6: em_mov00(em, 1); break;
    }
}

static void em_atk00_0058CC80(EMW *em, int kind) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 15, 10, 0);
        switch (kind) {
        case 0: shell02_set(em, 3); break;
        case 1: shell02_set(em, 8); break;
        case 2: shell02_set(em, 9); break;
        case 3: shell02_set(em, 10); break;
        case 4: shell02_set(em, 14); break;
        case 5: shell02_set(em, 15); break;
        }
        ang[0] = 0;
        ang[1] = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        ang[2] = 0;
        cpRotMatrix(ang, mat);
        in[0] = 0.0f;
        in[1] = 0.0f;
        in[2] = 100.0f;
        flvecApplyMat33(out, in, &mat);
        em->tgt_pos[0] += out[0];
        em->tgt_pos[2] += out[2];
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        if (flvecCalcDistance(em->pos, em->tgt_pos) < 100.0f || --em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 16, 10, 0);
        } else {
            em09_dir_calc(&em->ang[1], &em->horm_ang, 0x200);
            cpRotMatrix(em->ang, em->mat);
            if (em->work08 > 15) {
                s8 n = em->x617;

                if (n == -1) {
                    em->work08 = 15;
                } else if (em->stg != ((PLW *)player_work)[n].stg) {
                    em->work08 = 15;
                }
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_move03_0058CF10(EMW *em) {
    switch (em->x15) {
    case 0: em_atk00_0058CC80(em, 0); break;
    case 1: em_atk00_0058CC80(em, 1); break;
    case 2: em_atk00_0058CC80(em, 2); break;
    case 3: em_atk00_0058CC80(em, 3); break;
    case 4: em_atk00_0058CC80(em, 4); break;
    case 5: em_atk00_0058CC80(em, 5); break;
    }
}

static void em_dm00_0058CFB0(EMW *em) {
    s32 a;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        a = (u16)(em->dm_ang - em->ang[1]);
        if (a > 0x6000 && a < 0xA000) {
            em_char_set(em, 20, 0, 0);
        } else if (a < 0x8000) {
            em_char_set(em, 60, 0, 0);
        } else {
            em_char_set(em, 61, 0, 0);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_dm01_0058D0A0(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        ang[0] = (u16)(em->dm_ang - em->ang[1]);
        if (ang[0] < 0x8000) {
            em_char_set(em, 62, 0, 0);
        } else {
            em_char_set(em, 67, 0, 0);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 0, 10.0f)) {
            em->x05++;
            ang[0] = 0;
            ang[1] = em->dm_ang + 0x8000;
            ang[2] = 0;
            cpRotMatrix(ang, mat);
            in[0] = 0.0f;
            in[1] = 8.0f;
            in[2] = -21.0f;
            flvecApplyMat33(out, in, &mat);
            em_rate_clear_g(em);
            em->rate_x = out[0];
            em->adj_y = out[1];
            em->adj_z = out[2];
            em->x3C0[1] = -1.09f;
            em->x3C0[2] = 0.28f;
            em->x388 = 2;
        }
        break;
    case 2:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 63, 6, 0);
            em->work08 = 150;
        }
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 66, 6, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_dm02_0058D2E0(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        ang[0] = (u16)(em->dm_ang - em->ang[1]);
        if (ang[0] < 0x8000) {
            em_char_set(em, 62, 0, 10);
        } else {
            em_char_set(em, 67, 0, 10);
        }
        ang[0] = 0;
        ang[1] = em->dm_ang + 0x8000;
        ang[2] = 0;
        cpRotMatrix(ang, mat);
        in[0] = 0.0f;
        in[1] = 8.0f;
        in[2] = -21.0f;
        flvecApplyMat33(out, in, &mat);
        em_rate_clear_g(em);
        em->rate_x = out[0];
        em->adj_y = out[1];
        em->adj_z = out[2];
        em->x3C0[1] = -1.09f;
        em->x3C0[2] = 0.28f;
        em_cmd_reset(em);
        break;
    case 1:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 63, 6, 0);
            em->work08 = 150;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 66, 6, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_dm03_0058D4E0(EMW *em) {
    if (em->work08 > 0) {
        em->work08--;
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 71, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        em_mahi_eff_set(em, 2);
        if (em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 72, 0, 0);
            em->ang[1] = em->ang[1] + 0x8000;
            Em_Mahi_End(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x8BD = 0;
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_dm04_0058D600(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 71, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 72, 0, 0);
            em->ang[1] = em->ang[1] + 0x8000;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_move04_0058D6D0(EMW *em) {
    switch (em->x15) {
    case 0: em_dm00_0058CFB0(em); break;
    case 1: em_dm01_0058D0A0(em); break;
    case 2: em_dm02_0058D2E0(em); break;
    case 3: em_dm03_0058D4E0(em); break;
    case 4: em_dm04_0058D600(em); break;
    }
}

static void em_die00_0058D770(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        ang[0] = (u16)(em->dm_ang - em->ang[1]);
        if (ang[0] < 0x8000) {
            em_char_set(em, 62, 0, 0);
        } else {
            em_char_set(em, 67, 0, 0);
        }
        Quest_enemy_die(em);
        break;
    case 1:
        if (em_frame_check(em, 0, 10.0f)) {
            em->x05++;
            ang[0] = 0;
            ang[1] = em->dm_ang + 0x8000;
            ang[2] = 0;
            cpRotMatrix(ang, mat);
            in[0] = 0.0f;
            in[1] = 8.0f;
            in[2] = -21.0f;
            flvecApplyMat33(out, in, &mat);
            em_rate_clear_g(em);
            em->rate_x = out[0];
            em->adj_y = out[1];
            em->adj_z = out[2];
            em->x3C0[1] = -1.09f;
            em->x3C0[2] = 0.28f;
            em->x388 = 2;
        }
        break;
    case 2:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 63, 6, 0);
            em->work08 = 150;
            em_rate_clear(em);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 64, 0, 0);
            em->work08 = 60;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        if (em->x194 == 0) {
            em->x05++;
            if (em->kind == 4) {
                em->work08 = 2400;
            } else {
                em->work08 = 900;
            }
        }
        break;
    case 5:
        if (--em->work08 <= 0 || Em_hagi_point_cnt_ck(em) <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 6:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 2);
        }
        break;
    }
}

static void em_die01_0058DA90(EMW *em) {
    FLMAT mat;
    f32 in[3];
    f32 out[3];
    s32 ang[3];

    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        ang[0] = (u16)(em->dm_ang - em->ang[1]);
        if (ang[0] < 0x8000) {
            em_char_set(em, 62, 0, 10);
        } else {
            em_char_set(em, 67, 0, 10);
        }
        Quest_enemy_die(em);
        ang[0] = 0;
        ang[1] = em->dm_ang + 0x8000;
        ang[2] = 0;
        cpRotMatrix(ang, mat);
        in[0] = 0.0f;
        in[1] = 8.0f;
        in[2] = -21.0f;
        flvecApplyMat33(out, in, &mat);
        em_rate_clear_g(em);
        em->rate_x = out[0];
        em->adj_y = out[1];
        em->adj_z = out[2];
        em->x3C0[1] = -1.09f;
        em->x3C0[2] = 0.28f;
        break;
    case 1:
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (rate_add_g2(em)) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 63, 6, 0);
            em->work08 = 150;
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 64, 0, 0);
            em->work08 = 60;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        Em_hagi_point_cnt_ck(em);
        if (em->x194 == 0) {
            em->x05++;
            if (em->kind == 4) {
                em->work08 = 2400;
            } else {
                em->work08 = 900;
            }
        }
        break;
    case 4:
        if (--em->work08 <= 0 || Em_hagi_point_cnt_ck(em) <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 5:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 2);
        }
        break;
    }
}

static void em_die_rev_0058DD70(EMW *em) {
    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck(em) == 1) {
            em->x05++;
        } else {
            em->x04++;
        }
        break;
    case 1:
        em_status_init(em);
        em04_init(em);
        Quest_enemy_revival_set(em);
        if (em->kind == 5) {
            switch (em->stg) {
            case 0x16:
            case 0x2A:
            case 0x2C:
            case 0x2E:
            case 0x45:
                Quest_enemy_escape(em);
                em->x04++;
                em->x01 = 0;
                break;
            default:
                em04_act_set_k(em, 1, 4, 0);
                break;
            }
        } else {
            switch (em->stg) {
            case 1:
            case 0x16:
            case 0x17:
            case 0x20:
            case 0x21:
            case 0x23:
            case 0x2A:
            case 0x2C:
            case 0x2E:
            case 0x45:
                Quest_enemy_escape(em);
                em->x04++;
                em->x01 = 0;
                break;
            }
        }
        break;
    }
}

static void em_move05_0058DF20(EMW *em) {
    switch (em->x15) {
    case 0:
        em_die00_0058D770(em);
        break;
    case 1:
        em_die01_0058DA90(em);
        break;
    case 2:
        em_die_rev_0058DD70(em);
        break;
    }
}

static void em_demo00_0058DF90(EMW *em) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 1, 0, 0);
        em->x01 = 0;
        em->x839 = 0;
        break;
    case 1:
        if (Event_flag_ck(0xF) == 1) {
            em->x05++;
            em->x01 = 1;
            em04_next_act_set(em);
        }
        break;
    }
}

static void em_move06_0058E030(EMW *em) {
    switch (em->x15) {
    case 0:
        em_demo00_0058DF90(em);
        break;
    }
}

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number */
} QUEST_W;
extern QUEST_W quest_w;
extern GAME_W game_w;

void em04_main(EMW *em) {
    u8 dmg[4];
    EM04W *w = (EM04W *)em->ex;

    if (w->x0C != 0) {
        w->x0C--;
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 9:
    case 11:
    case 14:
        break;
    case 1:
    case 2:
        if (em->x388 == 2) {
            em_act_set(em, 5, 0);
        } else {
            em_act_set(em, 5, 1);
        }
        break;
    case 5:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2)) {
            break;
        }
        em_act_set(em, 4, 2);
        break;
    case 6:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2)) {
            break;
        }
        em_mahi_dmg_timer_set(em);
        em04_act_set_k(em, 4, 3, 0);
        break;
    case 7:
        em_act_set(em, 4, 2);
        break;
    case 8:
        if ((em->mode == 4 && em->x15 == 1) || (em->mode == 4 && em->x15 == 2)) {
            break;
        }
        em_sleep_dmg_timer_set(em);
        em04_act_set_k(em, 0, 10, 0);
        break;
    case 10:
        em->x88B = 1;
        Em_Sleep_End(em);
        if (em->x388 == 2) {
            em_act_set(em, 4, 2);
        } else {
            em04_act_set_k(em, 4, 1, 0);
        }
        break;
    case 12:
        if (em->x388 == 2) {
            em_act_set(em, 4, 2);
        } else if ((s16)act_ck(em, 4, 0)) {
            em_act_set(em, 4, 1);
        } else {
            em_act_set(em, 4, 0);
        }
        break;
    case 13:
        if (em->x388 == 2) {
            em_act_set(em, 4, 2);
        } else {
            em_act_set(em, 4, 1);
        }
        em_dur_set(em, 0);
        break;
    }
    if (quest_w.x08 == 0x87 && em->kind == 5 && em->stg == 0x2A && Event_flag_ck(0xF) == 0) {
        if (game_w.info_stop == 1 && em->mode != 6) {
            em04_act_set_k(em, 6, 0, 1);
        }
    } else {
        switch (em->x734) {
        case 3:
            if (em->x839 != 0) {
                em_cmd_ck(em);
                em->x839 = 0;
            }
            break;
        }
    }
    em04_main_sub(em);
    if (em->x6FF != 0) {
        em04_main_sub(em);
        em->x6FF = 0;
    }
}

void em04_main_sub(EMW *em) {
    switch (em->mode) {
    case 0: em_move00(em); break;
    case 1: em_move01(em); break;
    case 2: em_move00(em); break;
    case 3: em_move03_0058CF10(em); break;
    case 4: em_move04_0058D6D0(em); break;
    case 5: em_move05_0058DF20(em); break;
    case 6: em_move06_0058E030(em); break;
    case 7: em_move04_0058D6D0(em); break;
    }
}

void move_default_0058E4F0(EMW *em) {
}

/* Sound and effect script per animation: sound_call(em, frame, se) plays
 * the sound code se once when the animation reaches frame. */
static void ef_move_sub_0058E500(EMW *em, EM04W *w) {
    if (w->anim != em->char0) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_0058F430(em, 16, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 76, Code_Make(1, 2, 2, 2));
        break;
    case 0x3EA:
        sound_call_0058F430(em, 12, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 42, Code_Make(1, 2, 2, 2));
        sound_call_0058F430(em, 6, 19);
        sound_call_0058F430(em, 16, 18);
        sound_call_0058F430(em, 36, 19);
        break;
    case 0x3EB:
        sound_call_0058F430(em, 12, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 38, Code_Make(1, 2, 2, 2));
        sound_call_0058F430(em, 6, 18);
        sound_call_0058F430(em, 30, 19);
        sound_call_0058F430(em, 44, 19);
        break;
    case 0x3EC:
    case 0x3ED:
        sound_call_0058F430(em, 6, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 70, Code_Make(1, 2, 2, 2));
        sound_call_0058F430(em, 22, 19);
        sound_call_0058F430(em, 38, 19);
        sound_call_0058F430(em, 56, 18);
        sound_call_0058F430(em, 68, 19);
        sound_call_0058F430(em, 100, 19);
        break;
    case 0x3EE:
        sound_call_0058F430(em, 40, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 60, Code_Make(1, 4, 2, 4));
        sound_call_0058F430(em, 80, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 100, Code_Make(1, 4, 2, 4));
        sound_call_0058F430(em, 154, 15);
        sound_call_0058F430(em, 20, 18);
        sound_call_0058F430(em, 40, 18);
        sound_call_0058F430(em, 164, 18);
        break;
    case 0x3EF:
        sound_call_0058F430(em, 38, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 58, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 92, Code_Make(4, 4, 5, 4));
        sound_call_0058F430(em, 114, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 164, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 180, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 210, Code_Make(4, 4, 5, 4));
        sound_call_0058F430(em, 232, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 24, 18);
        sound_call_0058F430(em, 40, 18);
        sound_call_0058F430(em, 130, 18);
        sound_call_0058F430(em, 164, 18);
        sound_call_0058F430(em, 246, 18);
        break;
    case 0x3F0:
        sound_call_0058F430(em, 4, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 36, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 4, 19);
        sound_call_0058F430(em, 42, 19);
        sound_call_0058F430(em, 52, 18);
        break;
    case 0x3F1:
        sound_call_0058F430(em, 6, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 70, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 38, 19);
        sound_call_0058F430(em, 76, 19);
        break;
    case 0x3F2:
        sound_call_0058F430(em, 36, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 58, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 128, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 150, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 22, 18);
        sound_call_0058F430(em, 116, 19);
        break;
    case 0x3F3:
        sound_call_0058F430(em, 22, 15);
        sound_call_0058F430(em, 10, 18);
        sound_call_0058F430(em, 82, 18);
        break;
    case 0x3F4:
        sound_call_0058F430(em, 4, 14);
        sound_call_0058F430(em, 68, 15);
        sound_call_0058F430(em, 12, 18);
        sound_call_0058F430(em, 70, 19);
        sound_call_0058F430(em, 94, 19);
        break;
    case 0x3F5:
        sound_call_0058F430(em, 24, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 40, 18);
        break;
    case 0x3F6:
        sound_call_0058F430(em, 68, 13);
        sound_call_0058F430(em, 36, 20);
        sound_call_0058F430(em, 74, 16);
        if (em_frame_check(em, 0, 66.0f)) {
            Eft13_set_em_scl(em, 6, 0.7f, 3);
        }
        break;
    case 0x3F7:
        sound_call_0058F430(em, 10, 14);
        sound_call_0058F430(em, 24, 19);
        sound_call_0058F430(em, 36, 19);
        if (em_frame_check(em, 0, 16.0f)) {
            Eft13_set_em_scl(em, 4, 2.0f, 3);
        }
        if (em_frame_check(em, 0, 30.0f)) {
            Eft13_set_em_scl(em, 17, 3.5f, 3);
        }
        break;
    case 0x3F8:
        sound_call_0058F430(em, 20, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 8, 19);
        sound_call_0058F430(em, 14, 19);
        sound_call_0058F430(em, 12, 13);
        sound_call_0058F430(em, 98, 18);
        if (em_frame_check(em, 0, 10.0f) || em_frame_check(em, 0, 14.0f)) {
            Eft13_set_em_scl(em, 6, 1.2f, 3);
            Eft13_set_em_scl(em, 9, 1.2f, 3);
        }
        break;
    case 0x3F9:
        sound_call_0058F430(em, 32, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 70, 15);
        sound_call_0058F430(em, 200, Code_Make(4, 4, 5, 4));
        sound_call_0058F430(em, 28, 20);
        sound_call_0058F430(em, 46, 19);
        break;
    case 0x3FB:
        sound_call_0058F430(em, 14, 13);
        sound_call_0058F430(em, 60, Code_Make(1, 4, 2, 4));
        sound_call_0058F430(em, 80, Code_Make(0, 4, 1, 4));
        break;
    case 0x3FC:
        sound_call_0058F430(em, 8, Code_Make(7, 4, 8, 4));
        sound_call_0058F430(em, 26, 21);
        sound_call_0058F430(em, 60, 18);
        break;
    case 0x424:
    case 0x425:
        sound_call_0058F430(em, 12, Code_Make(7, 4, 8, 4));
        sound_call_0058F430(em, 96, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 38, 20);
        sound_call_0058F430(em, 42, 20);
        sound_call_0058F430(em, 84, 18);
        sound_call_0058F430(em, 108, 18);
        break;
    case 0x426:
    case 0x42B:
        sound_call_0058F430(em, 12, Code_Make(9, 4, 10, 4));
        break;
    case 0x427:
        sound_call_0058F430(em, 16, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 14, 17);
        if (em_frame_check(em, 0, 2.0f)) {
            Eft13_set_em_scl(em, 2, 0.8f, 7);
        }
        break;
    case 0x428:
        sound_call_0058F430(em, 8, 11);
        sound_call_0058F430(em, 8, 16);
        sound_call_0058F430(em, 30, 16);
        break;
    case 0x42A:
    case 0x430:
        sound_call_0058F430(em, 18, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 22, 16);
        sound_call_0058F430(em, 30, 18);
        sound_call_0058F430(em, 60, 19);
        sound_call_0058F430(em, 88, 18);
        sound_call_0058F430(em, 104, 18);
        sound_call_0058F430(em, 128, Code_Make(5, 4, 6, 4));
        break;
    case 0x42F:
        sound_call_0058F430(em, 6, Code_Make(9, 4, 10, 4));
        sound_call_0058F430(em, 36, 17);
        sound_call_0058F430(em, 66, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 96, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 124, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 180, Code_Make(0, 4, 1, 4));
        if (em_frame_check(em, 0, 34.0f)) {
            Eft13_set_em_scl(em, 2, 1.0f, 6);
        }
        break;
    case 0x42E:
        sound_call_0058F430(em, 4, 12);
        sound_call_0058F430(em, 20, 18);
        sound_call_0058F430(em, 52, 18);
        sound_call_0058F430(em, 62, 18);
        sound_call_0058F430(em, 120, 16);
        sound_call_0058F430(em, 280, Code_Make(1, 4, 2, 4));
        break;
    default:
        move_default_0058E4F0(em);
        break;
    }
}

void em04_effect_move_0058F3E0(EMW *em) {
    EM04W *w = (EM04W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_0058E500(em, w);
        break;
    }
}

static void sound_call_0058F430(EMW *em, int frame, int se) {
    if (em_frame_check(em, 0, (f32)frame)) {
        Em_se_req2(em, se, 0, em->pos, 6, 0);
    }
}

/* A file static in the original; data tables point at it. */
void dummy_em_prog_0058F490(void) {
}
