/* em03 - game.bin 0x005873D0-0x0058ABxx. Per-monster AI for monster kind 3
 * (setup, main loop with the damage system, action steps, move states,
 * damage and death, sound/effect script). Names of the steps follow the
 * split (em_act00, em_move00...). Field meanings are mostly guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM03W {
    u8 _pad00[8];
    s32 spd[3];         /* 0x08 speed handed to speed_add_g ([1] = angle) */
    f32 x14;            /* 0x14 time left (counted down by the motion step) */
    u8 x18;             /* 0x18 non-zero: turn toward the target */
} EM03W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;
extern EMW em_work[];
extern u8 em_boss_tbl[];

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);
u32 ran_suu(int);
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
void em_escape_mind_set(EMW *, u8, u8);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
u8 Em_Smoke_Ck(EMW *);
void em_cmd_ck(EMW *);
void em03_move_sub(EMW *em);
void em03_act_set();
void em03_to_normal(EMW *em);
void em03_char_set(EMW *em, int no, int a, int b);
void Em_Sleep_Start(EMW *);
void Em_Mahi_End(EMW *);
u16 Em_Calc_angY(f32 *, f32 *);
int em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
void mot_miration_ret(EMW *, f32 *);
int em_frame_check2(EMW *, f32, int);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
void GetGroundHitArea(EMW *, f32 *, f32 *);

void em03_init(EMW *em) {
    if (quest_w.x08 == 0) {
        switch (game_w.stage) {
        case 0xF:
            em->pos[0] = 9500.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 9200.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 0x10:
            em->pos[0] = 6000.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 7000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 0x13:
            em->pos[0] = 11000.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        case 0x25:
            em->pos[0] = 9500.0f + 150.0f * (f32)(s32)((u32)em->x13 >> 3);
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f + 150.0f * (f32)(em->x13 & 7);
            break;
        default:
            em->pos[0] = 5000.0f - 80.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 5000.0f - 80.0f * (f32)(u32)em->x13;
            break;
        }
    }
    em->x88B = 1;
    em->x388 = 0;
    em->act_spd = 1.0f;
    if (em->kind == 3) {
        em->x8C3 = 0;
        if (quest_w.x08 == 0) {
            em->x11 = (u16)ran_suu(1) & 1;
            em->x11 = 0;
        } else {
            em->x11 = em->type;
        }
        em->x792 = em->x302 = em_hp_vital_set(em, 0x30);
        if (em->x11 != 0) {
            em->x792 = em->x302 = em_hp_vital_set(em, 0x30);
            em->scale[0] *= 0.85f;
            em->scale[1] *= 0.85f;
            em->scale[2] *= 0.85f;
        }
    }
    em03_char_set(em, 1, 0, 0);
    em_dur_init(em);
    em_act_set(em, 0, 1);
}

#define EM03_HAGI0(em) (*(s16 *)&(em)->hagi[0][0])

void em03_main(EMW *em) {
    u8 dmg[4];
    u8 b = 0;
    u8 c = 0;
    u8 a = 0;
    s16 i;
    EMW *p = em_work;
    s8 pn;

    if (em_mode_timer_sub(em)) {
        em_cmd_reset(em);
        for (pn = 0; pn < game_w.pl_num; pn++) {
            if (em->stg == ((PLW *)player_work)[pn].stg) {
                break;
            }
            if (pn == game_w.pl_num - 1) {
                em->x839 = 1;
            }
        }
    }
    if (em->x889 != 1 && em->mode != 6) {
        for (i = 0; i < 20; i++, p++) {
            if (p->be_flag && p->x01 && (u8)Pl_stg_ck_tw(em, (PLW *)p)) {
                if (em_boss_tbl[p->kind]) {
                    if (p->x888 == 1) {
                        b = 1;
                        break;
                    }
                    if (p->x888 == 0) {
                        a = 1;
                    }
                }
                if (p->type == 0 && p->mode == 6) {
                    c = 1;
                }
            }
        }
        if ((b & 0xFF) || ((a & 0xFF) && (c & 0xFF))) {
            em_escape_mind_set(em, 1, 0x90);
            em_cmd_reset(em);
            em->x839 = 1;
        }
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 5:
    case 7:
    case 9:
    case 11:
        break;
    case 1:
    case 2:
        em_act_set(em, 5, 0);
        break;
    case 6:
        em_mahi_dmg_timer_set(em);
        em03_act_set(em, 4, 3, 2);
        break;
    case 8:
        em_sleep_dmg_timer_set(em);
        em03_act_set(em, 0, 8, 2);
        break;
    case 10:
        em->x88B = 1;
        Em_Sleep_End(em);
        em03_act_set(em, 4, 2, 2);
        break;
    case 12:
    case 13: {
        s32 d = (u16)em->dm_ang - em->ang[1];

        if ((u16)(d - 0x4000) < 0x8000 && EM03_HAGI0(em) > 0 && em->x388 == 0) {
            if ((u16)d < 0x8000) {
                em_act_set(em, 4, 0);
            } else {
                em_act_set(em, 4, 1);
            }
        } else {
            em_act_set(em, 4, 2);
        }
        break;
    }
    case 14:
        if (em->x388 == 0) {
            s32 d = (u16)(em->dm_ang - em->ang[1]);

            em->x839 = 0;
            if (d < 0x8000) {
                em_act_set(em, 4, 0);
            } else {
                em_act_set(em, 4, 1);
            }
        }
        break;
    }
    if (em->x889 != 1 && Em_Smoke_Ck(em) == 1) {
        em_escape_mind_set(em, 2, 0x90);
        em_cmd_reset(em);
        em->x839 = 1;
    }
    if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em03_move_sub(em);
    if (em->x6FF != 0) {
        em03_move_sub(em);
        em->x6FF = 0;
    }
}

static void em_act00(EMW *em, EM03W *w) {
}

static void em_act01(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act02(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act03(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act04(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 4, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act05(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 5, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act06(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 6, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act07(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        if ((u16)(em->horm_ang - em->ang[1]) < 0x8001) {
            em03_char_set(em, 7, 0, 0);
        } else {
            em03_char_set(em, 8, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act08(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x88B = 0;
        Em_Sleep_Start(em);
        em_char_set(em, 65, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 67, 0, 0);
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 9);
        }
        break;
    }
}

static void em_act09(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x88B = 1;
        Em_Sleep_End(em);
        em_char_set(em, 64, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act10(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 69, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x8BD = 0;
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_act11(EMW *em, EM03W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 38, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_move00_00588180(EMW *em, EM03W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_act00(em, w); break;
    case 1: em_act01(em, w); break;
    case 2: em_act02(em, w); break;
    case 3: em_act03(em, w); break;
    case 4: em_act04(em, w); break;
    case 5: em_act05(em, w); break;
    case 6: em_act06(em, w); break;
    case 7: em_act07(em, w); break;
    case 8: em_act08(em, w); break;
    case 9: em_act09(em, w); break;
    case 10: em_act10(em, w); break;
    case 11: em_act11(em, w); break;
    }
}

static void em_mv00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        em03_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (em09_dir_calc(&em->ang[1], &em->horm_ang, 0x100) < 0x100) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
    cpRotMatrix(em->ang, em->mat);
}

/* Turn toward the target by at most 0x40 per frame. */
#define EM03_TURN(em) \
    do { \
        s32 cur, d; \
        d = Em_Calc_angY((em)->pos, (em)->tgt_pos); \
        cur = (em)->ang[1]; \
        d = (u16)(d - cur); \
        if (d < 0x8001) { \
            if (d < 0x40) { \
                (em)->ang[1] = cur + d; \
            } else { \
                (em)->ang[1] = cur + 0x40; \
            } \
        } else if (d >= 0xFFC1) { \
            (em)->ang[1] = cur + d; \
        } else { \
            (em)->ang[1] = cur - 0x40; \
        } \
    } while (0)

static void em_mv01_00588350(EMW *em, EM03W *w) {
    f32 mv[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (w->x18) {
            EM03_TURN(em);
        }
        mot_miration_ret(em, mv);
        w->x14 = w->x14 - mv[2];
        if (w->x14 <= 0.0f) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv02_00588480(EMW *em, EM03W *w, int mode) {
    f32 mv[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 10, 0, 0);
        break;
    case 1:
        if (w->x18) {
            EM03_TURN(em);
        }
        mot_miration_ret(em, mv);
        w->x14 = w->x14 - mv[2];
        if (w->x14 <= 0.0f && em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 4, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv03_005885F0(EMW *em, EM03W *w, int mode) {
    f32 mv[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 11, 0, 0);
        break;
    case 1:
        if (w->x18) {
            EM03_TURN(em);
        }
        mot_miration_ret(em, mv);
        w->x14 = w->x14 - mv[2];
        if (w->x14 <= 0.0f && em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 4, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 12, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv05(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 13, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv06(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 15, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            em->ang[1] = em->ang[1] + 0x888;
            if (em_frame_check2(em, 30.0f, 0)) {
                em->x05++;
                em->ang[1] = em->ang[1] + 8;
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv07(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 22, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            em->ang[1] = em->ang[1] - 0x888;
            if (em_frame_check2(em, 30.0f, 0)) {
                em->x05++;
                em->ang[1] = em->ang[1] - 8;
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv08(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 18, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv11(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em03_char_set(em, 12, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 13, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 12, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em03_char_set(em, 13, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv12(EMW *em) {
    s32 spd;
    f32 fr;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        d = (u16)(em->horm_ang - em->ang[1]);
        if (d <= 0x3000 || d >= 0xD000) {
            em03_char_set(em, 18, 0, 0);
        } else if (d >= 0x8000) {
            em03_char_set(em, 22, 0, 0);
        } else {
            em03_char_set(em, 15, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            u32 dd, ang;

            if (em->char0 == 0x3FA) {
                fr = 26.0f;
                spd = 0x4000 / (u32)(fr / 2.0f);
            } else {
                fr = 30.0f;
                spd = 0x8000 / (u32)(fr / 2.0f);
            }
            ang = em->ang[1];
            dd = (u16)(em->horm_ang - (u16)ang);
            if ((u16)(dd + spd) < (u32)spd * 2) {
                em->ang[1] = em->horm_ang;
            } else if (dd < 0x8000) {
                em->ang[1] = (u16)(ang + spd);
            } else {
                em->ang[1] = (u16)(ang - spd);
            }
            if (em_frame_check2(em, fr, 0)) {
                em->x05++;
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_mv13(EMW *em, EM03W *w) {
    u8 f = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 20, 0, 0);
        em_rate_clear(em);
        em->adj_y = 75.0f;
        em->x3C0[1] = -10.0f;
        em->adj_z = 50.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        if (em_frame_check2(em, 26.0f, 0)) {
            em->x388 = 2;
            w->spd[1] = em->ang[1];
            f = 1;
            speed_add_g(em, w->spd);
        }
        if (em_frame_check2(em, 36.0f, 0)) {
            em->act_spd = 0.0f;
        }
        GetGroundHitArea(em, em->pos, &em->x5AC);
        if (f) {
            if (em->pos[1] <= em->x5AC) {
                em->x05++;
                em->x388 = 0;
                em->pos[1] = em->x5AC;
                em->act_spd = 1.0f;
            }
        }
        break;
    case 2:
        if (em_frame_check2(em, 44.0f, 0)) {
            em->x05++;
            em_char_set(em, 14, 4, 10);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em03_to_normal(em);
        }
        break;
    }
}

static void em_move01_00589040(EMW *em, EM03W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_mv00(em); break;
    case 1: em_mv01_00588350(em, w); break;
    case 2: em_mv02_00588480(em, w, 0); break;
    case 3: em_mv03_005885F0(em, w, 0); break;
    case 4: em_mv04(em); break;
    case 5: em_mv05(em); break;
    case 6: em_mv06(em); break;
    case 7: em_mv07(em); break;
    case 8: em_mv08(em); break;
    case 9: em_mv02_00588480(em, w, 1); break;
    case 10: em_mv03_005885F0(em, w, 1); break;
    case 11: em_mv11(em); break;
    case 12: em_mv12(em); break;
    case 13: em_mv13(em, w); break;
    }
}
