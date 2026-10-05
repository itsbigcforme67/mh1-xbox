/* em33 - game.bin 0x006147D0-0x00618C9C. Per-monster AI for monster kind 33.
 * Same layout as em03 (see em03*.c): em33_init (stage/slot start position,
 * hit points, stay/runaway timers), em33_main (damage system), action steps
 * em_act00_00614D90-12, move states (em_mv00_00615590-13 walking/turning/jumping, em_atk00_00616590-08
 * shell attacks, em_dmg00_00617020-04 damage reactions, em_die00_00617500, demo), em33_move_sub
 * (mode dispatch), em33_act_set (distance/time for the chosen action),
 * em33_to_normal, em33_char_set and the sound/effect script (ef_move_sub_00617E60).
 * Written from em03; every function matches. Field meanings are guesses. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM33W {
    u8 eff;             /* 0x00 em33_effect_move step */
    u8 _pad01[5];
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    s32 spd[3];         /* 0x08 speed handed to speed_add_g ([1] = angle) */
    f32 x14;            /* 0x14 time left (counted down by the motion step) */
    u8 x18;             /* 0x18 non-zero: turn toward the target */
} EM33W;

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
s16 em_hp_vital_set2(EMW *, s16, s16);
extern s16 em33_stay_timer_tbl[];
extern s16 em33_runaway_timer_tbl[];
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
void em33_move_sub(EMW *em);
void em33_act_set(EMW *em, int kind, u16 no, u16 arg);
void em33_to_normal(EMW *em);
void em33_char_set(EMW *em, int no, int a, int b);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
u16 Em_Calc_angY(f32 *, f32 *);
int em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
void mot_miration_ret(EMW *, f32 *);
int em_frame_check2(EMW *, f32, int);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
void GetGroundHitArea(EMW *, f32 *, f32 *);
void Eft04_set_time(EMW *, int, int, f32);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void Em_Mahi_Start(EMW *);
void em_cdm_act_flag_ck(EMW *);
f32 CalcDistanceXZ(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
int Code_Make(int, int, int, int);
void shell02_set(EMW *, int);
void Eft13_set_em_scl(EMW *, int, f32, int);
void eft07_set(EMW *, int);
void eft01_set(EMW *, int);
void em_no_floor_ck2(EMW *);
void em_no_battle_area_ck(EMW *, int, int);
void em_char_set2(EMW *, int, int, int, int);
void flmatGetTrans(f32 *, void *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void Quest_enemy_escape(EMW *);
void em33_init(EMW *);
void em_mahi_eff_set(EMW *, int);

void em33_init(EMW *em) {
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
    em->x765 = 1;
    em->x11 = 0;
    em->x792 = em->x302 = em_hp_vital_set2(em, 0x578, 0x258);
    em->act_spd = 1.0f;
    em33_char_set(em, 1, 0, 0);
    em->stay_tm = em33_stay_timer_tbl[em->stg];
    em->runaway_tm = em33_runaway_timer_tbl[em->stg];
    em_dur_init(em);
    em_act_set(em, 0, 1);
}

#define EM33_HAGI0(em) ((em)->hagi[0].hp)

void em33_main(EMW *em) {
    u8 dmg[4];
    s8 pn;

    if (em_mode_timer_sub(em)) {
        for (pn = 0; pn < game_w.pl_num; pn++) {
            if (em->stg == ((PLW *)player_work)[pn].stg) {
                break;
            }
            if (pn == game_w.pl_num - 1) {
                em_cmd_reset(em);
                em->x839 = 1;
            }
        }
    }
    em_no_floor_ck2(em);
    em_no_battle_area_ck(em, 0, 1);
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 5:
    case 7:
    case 9:
    case 11:
    case 14:
        break;
    case 1:
    case 2:
        em33_act_set(em, 5, 0, 2);
        break;
    case 6:
        em_mahi_dmg_timer_set(em);
        em33_act_set(em, 4, 3, 2);
        break;
    case 8:
        em_sleep_dmg_timer_set(em);
        em33_act_set(em, 0, 8, 2);
        break;
    case 10:
        em->x88B = 1;
        Em_Sleep_End(em);
        em33_act_set(em, 4, 2, 2);
        break;
    case 12:
    case 13: {
        s32 d = (u16)em->dm_ang - em->ang[1];

        if ((u16)(d - 0x4000) < 0x8000 && EM33_HAGI0(em) > 0 && em->x388 == 0) {
            if ((u16)d < 0x8000) {
                em33_act_set(em, 4, 0, 2);
            } else {
                em33_act_set(em, 4, 1, 2);
            }
        } else {
            em33_act_set(em, 4, 2, 2);
        }
        break;
    }
    }
    if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em33_move_sub(em);
    if (em->x6FF != 0) {
        em33_move_sub(em);
        em->x6FF = 0;
    }
}
static void em_act00_00614D90(EMW *em, EM33W *w) {
}

static void em_act01_00614DA0(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act02_00614E10(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act03_00614E80(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act04_00614EF0(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 4, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act05_00614F60(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 5, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act06_00614FD0(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 6, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act07_00615040(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        if ((u16)(em->horm_ang - em->ang[1]) < 0x8001) {
            em33_char_set(em, 7, 0, 0);
        } else {
            em33_char_set(em, 8, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act08_00615110(EMW *em, EM33W *w) {
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
            em33_act_set(em, 0, 9, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em33_act_set(em, 0, 9, 4);
        }
        break;
    }
}

static void em_act09_00615210(EMW *em, EM33W *w) {
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
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act10_006152A0(EMW *em, EM33W *w) {
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
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act11_00615330(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 38, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_act12_006153A0(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 38, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 1, 8, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_move00_00615470(EMW *em, EM33W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_act00_00614D90(em, w); break;
    case 1: em_act01_00614DA0(em, w); break;
    case 2: em_act02_00614E10(em, w); break;
    case 3: em_act03_00614E80(em, w); break;
    case 4: em_act04_00614EF0(em, w); break;
    case 5: em_act05_00614F60(em, w); break;
    case 6: em_act06_00614FD0(em, w); break;
    case 7: em_act07_00615040(em, w); break;
    case 8: em_act08_00615110(em, w); break;
    case 9: em_act09_00615210(em, w); break;
    case 10: em_act10_006152A0(em, w); break;
    case 11: em_act11_00615330(em, w); break;
    case 12: em_act12_006153A0(em, w); break;
    }
}

static void em_mv00_00615590(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        em33_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (em09_dir_calc(&em->ang[1], &em->horm_ang, 0x100) < 0x100) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
    cpRotMatrix(em->ang, em->mat);
}

/* Turn toward the target by at most 0x40 per frame. */
#define EM33_TURN(em) \
    do { \
        int d; \
        d = (u16)((u16)Em_Calc_angY((em)->pos, (em)->tgt_pos) - (em)->ang[1]); \
        if (d <= 0x8000) { \
            if (d <= 0x3F) { \
                (em)->ang[1] += d; \
            } else { \
                (em)->ang[1] += 0x40; \
            } \
        } else if (d > 0xFFC0) { \
            (em)->ang[1] += d; \
        } else { \
            (em)->ang[1] -= 0x40; \
        } \
    } while (0)

static void em_mv01_00615650(EMW *em, EM33W *w) {
    f32 mv[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (w->x18) {
            EM33_TURN(em);
        }
        mot_miration_ret(em, mv);
        w->x14 = w->x14 - mv[2];
        if (w->x14 <= 0.0f) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv02_00615780(EMW *em, EM33W *w, int mode) {
    f32 mv[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 10, 0, 0);
        shell02_set(em, 16);
        break;
    case 1:
        if (w->x18) {
            EM33_TURN(em);
        }
        mot_miration_ret(em, mv);
        w->x14 = w->x14 - mv[2];
        if (w->x14 <= 0.0f && em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 14, 4, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv03_00615900(EMW *em, EM33W *w, int mode) {
    f32 mv[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 11, 0, 0);
        shell02_set(em, 16);
        break;
    case 1:
        if (w->x18) {
            EM33_TURN(em);
        }
        mot_miration_ret(em, mv);
        w->x14 = w->x14 - mv[2];
        if (em->x194 == 0) {
            if (w->x14 <= 0.0f) {
                em->x05++;
            } else {
                em_char_set2(em, 0x3F3, 0, 0, 0);
                em_char_set2(em, 0x4BB, 0, 0, 1);
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set2(em, 0x3F6, 0, 2, 0);
            em_char_set2(em, 0x4BE, 0, 2, 1);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv04_00615B00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 12, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv05_00615BB0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 13, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv06_00615C60(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 15, 0, 0);
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
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv07_00615D30(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 22, 0, 0);
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
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv08_00615E00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 18, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv11_00615E70(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 12, 0, 0);
        shell02_set(em, 13);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 13, 0, 0);
            shell02_set(em, 13);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 12, 0, 0);
            shell02_set(em, 13);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 13, 0, 0);
            shell02_set(em, 13);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv12_00615FC0(EMW *em) {
    s32 spd;
    f32 fr;
    u32 d;
    u32 dd;
    u32 h;
    u32 ang;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em->horm_ang = (u16)Em_Calc_angY(em->pos, em->tgt_pos);
        d = (u16)(em->horm_ang - em->ang[1]);
        if (d <= 0x3000 || d >= 0xD000) {
            em33_char_set(em, 18, 0, 0);
        } else if (d >= 0x8000) {
            em33_char_set(em, 22, 0, 0);
        } else {
            em33_char_set(em, 15, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em->char0 == 0x3FA) {
                fr = 26.0f;
                spd = 0x4000 / (u32)(fr / 2.0f);
            } else {
                fr = 30.0f;
                spd = 0x8000 / (u32)(fr / 2.0f);
            }
            ang = em->ang[1];
            h = em->horm_ang;
            dd = (u16)(h - (u16)ang);
            if ((u16)(dd + spd) < (u32)spd * 2) {
                em->ang[1] = h;
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
            em33_to_normal(em);
        }
        break;
    }
}

static void em_mv13_00616260(EMW *em, EM33W *w) {
    u8 f = 0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 20, 0, 0);
        em_rate_clear(em);
        w->spd[0] = 0;
        w->spd[2] = 0;
        if (em->pos[1] >= em->tgt_pos[1]) {
            em->adj_y = 75.0f;
            em->x3C0[1] = -10.0f;
            em->adj_z = 50.0f;
        } else {
            f32 dist = CalcDistanceXZ(em->pos, em->tgt_pos);

            em->adj_z = 100.0f;
            em->x3C0[1] = -10.0f;
            em->work08 = (s32)(dist / em->adj_z);
            em->adj_y = ((1000.0f + em->tgt_pos[1]) - em->pos[1]) / (f32)em->work08 -
                        (em->x3C0[1] * (f32)em->work08) / 2.0f;
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x388 = 2;
            f = 1;
            w->spd[1] = em->ang[1];
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
                em_char_set(em, 14, 4, 10);
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_move01_00616460(EMW *em, EM33W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_mv00_00615590(em); break;
    case 1: em_mv01_00615650(em, w); break;
    case 2: em_mv02_00615780(em, w, 0); break;
    case 3: em_mv03_00615900(em, w, 0); break;
    case 4: em_mv04_00615B00(em); break;
    case 5: em_mv05_00615BB0(em); break;
    case 6: em_mv06_00615C60(em); break;
    case 7: em_mv07_00615D30(em); break;
    case 8: em_mv08_00615E00(em); break;
    case 9: em_mv02_00615780(em, w, 1); break;
    case 10: em_mv03_00615900(em, w, 1); break;
    case 11: em_mv11_00615E70(em); break;
    case 12: em_mv12_00615FC0(em); break;
    case 13: em_mv13_00616260(em, w); break;
    }
}

static void em_atk00_00616590(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 38, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk01_00616600(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 17, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 14, 0, 26);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk02_006166B0(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 20, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 24, 0, 8);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk03_00616760(EMW *em, EM33W *w, int flag) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 24, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 0, 22.0f)) {
            if ((u8)flag == 0) {
                Shell08_set_ang(em, 23, 8, 0, 0, 0);
            } else {
                Shell08_set_ang(em, 23, 8, 6, 0, 0);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk04_006168A0(EMW *em, EM33W *w, int flag) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 0, 90.0f)) {
            if ((u8)flag == 0) {
                Shell08_set_ang(em, 23, 8, 1, 0, 0);
            } else {
                Shell08_set_ang(em, 23, 8, 7, 0, 0);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk05_006169E0(EMW *em, EM33W *w, int flag) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 0, 90.0f)) {
            if ((u8)flag == 0) {
                Shell08_set_ang(em, 23, 8, 2, 0, 0);
            } else {
                Shell08_set_ang(em, 23, 8, 8, 0, 0);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk06_00616B20(EMW *em, EM33W *w, int flag) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 0, 90.0f)) {
            if ((u8)flag == 0) {
                Shell08_set_ang(em, 23, 8, 3, 0, 0);
            } else {
                Shell08_set_ang(em, 23, 8, 9, 0, 0);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk07_00616C60(EMW *em, EM33W *w, int flag) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 0, 90.0f)) {
            if ((u8)flag == 0) {
                Shell08_set_ang(em, 23, 8, 4, 0, 0);
            } else {
                Shell08_set_ang(em, 23, 8, 10, 0, 0);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_atk08_00616DA0(EMW *em, EM33W *w, int flag) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 39, 0, 0);
        Eft04_set_time(em, 8, (int)(90.0f / (2.0f * em->act_spd)), 1.0f);
        break;
    case 1:
        if (em_frame_check(em, 0, 90.0f)) {
            if ((u8)flag == 0) {
                Shell08_set_ang(em, 23, 8, 5, 0, 0);
            } else {
                Shell08_set_ang(em, 23, 8, 11, 0, 0);
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_move03_00616EE0(EMW *em, EM33W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0: em_atk00_00616590(em, w); break;
    case 1: em_atk01_00616600(em, w); break;
    case 2: em_atk02_006166B0(em, w); break;
    case 3: em_atk03_00616760(em, w, 0); break;
    case 4: em_atk04_006168A0(em, w, 0); break;
    case 5: em_atk05_006169E0(em, w, 0); break;
    case 6: em_atk06_00616B20(em, w, 0); break;
    case 7: em_atk07_00616C60(em, w, 0); break;
    case 8: em_atk08_00616DA0(em, w, 0); break;
    case 9: em_atk03_00616760(em, w, 0); break;
    case 10: em_atk04_006168A0(em, w, 1); break;
    case 11: em_atk05_006169E0(em, w, 1); break;
    case 12: em_atk06_00616B20(em, w, 1); break;
    case 13: em_atk07_00616C60(em, w, 1); break;
    case 14: em_atk08_00616DA0(em, w, 1); break;
    }
}

static void em_dmg00_00617020(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 60, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_dmg01_006170A0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 61, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_dmg02_00617120(EMW *em, EM33W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 62, 0, 0);
        em_rate_clear(em);
        em->rate_x = 0.0f;
        em->adj_y = 17.0f;
        em->adj_z = -20.0f;
        em->x3C0[1] = -2.38f;
        em->x3C0[2] = 0.11f;
        em->x388 = 2;
        em_cmd_reset(em);
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em33_char_set(em, 63, 0, 0);
            em->work08 = 90;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em33_char_set(em, 64, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_dmg03_006172C0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 68, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        em_mahi_eff_set(em, 2);
        if (--em->work08 <= 0) {
            em->x05++;
            em33_act_set(em, 0, 10, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em33_act_set(em, 0, 10, 4);
        }
        break;
    }
}

static void em_dmg04_006173A0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 65, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em33_char_set(em, 64, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em33_to_normal(em);
        }
        break;
    }
}

static void em_move04_00617460(EMW *em, EM33W *w) {
    switch (em->x15) {
    case 0: em_dmg00_00617020(em); break;
    case 1: em_dmg01_006170A0(em); break;
    case 2: em_dmg02_00617120(em, w); break;
    case 3: em_dmg03_006172C0(em); break;
    case 4: em_dmg04_006173A0(em); break;
    }
}

static void em_die00_00617500(EMW *em, EM33W *w) {
    em->x40C = 10;
    em->x40E = 10;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em33_char_set(em, 62, 0, 0);
        em_rate_clear(em);
        em->rate_x = 0.0f;
        em->adj_y = 17.0f;
        em->adj_z = -23.0f;
        em->x3C0[1] = -3.15f;
        em->x3C0[2] = 0.08f;
        em->x388 = 2;
        Quest_enemy_die(em);
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add_g(em, w->spd);
        if (em->adj_z * em->x3C0[2] >= 0.0f) {
            em->x3C0[2] = 0.0f;
        }
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em33_char_set(em, 63, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 90;
            em->act_spd = 0.0f;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        Em_hagi_point_cnt_ck(em);
        break;
    }
    if (em->pos[1] < em->x5AC) {
        em->pos[1] = em->x5AC;
    }
}

static void em_move05_006176C0(EMW *em, EM33W *w) {
    switch (em->x15) {
    case 0: em_die00_00617500(em, w); break;
    }
}

static void em_demo00_00617700(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        break;
    case 1:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            Quest_enemy_escape(em);
            em->x04++;
            em->x01 = 0;
        }
        break;
    }
}

static void em_move06_00617790(EMW *em) {
    switch (em->x15) {
    case 0:
        em_demo00_00617700(em);
        break;
    }
}

void em33_move_sub(EMW *em) {
    EM33W *w = (EM33W *)em->ex;

    switch (em->mode) {
    case 0: em_move00_00615470(em, w); break;
    case 1: em_move01_00616460(em, w); break;
    case 2: em_move00_00615470(em, w); break;
    case 3: em_move03_00616EE0(em, w); break;
    case 4: em_move04_00617460(em, w); break;
    case 5: em_move05_006176C0(em, w); break;
    case 6: em_move06_00617790(em); break;
    case 7: em_move06_00617790(em); break;
    }
}

/* Sets the monster's next action: first the distance / time it moves for
 * (from the target), then the generic em_act_set2. */
void em33_act_set(EMW *em, int kind, u16 no, u16 arg) {
    EM33W *w = (EM33W *)em->ex;
    f32 v;

    if (em->x8C3 == 0) {
        em_cdm_act_flag_ck(em);
    }
    switch ((u16)kind) {
    case 0:
        break;
    case 1:
        switch ((u16)no) {
        case 0:
            if (em->x881 == 0) {
                w->x18 = 0;
                em->work08 = ((u16)em->x39A % 120 + 180) / 2;
                w->x14 = 1000.0f;
                break;
            }
            w->x18 = 1;
            w->x14 = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                if (em->x617 == -1) {
                    w->x18 = 0;
                    w->x14 = 1000.0f;
                    break;
                }
                w->x14 = w->x14 - 100.0f;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            if (em->x881 == 7) {
                w->x14 = w->x14 - 100.0f;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            em->work08 = (s32)w->x14 / 7;
            break;
        case 1:
        case 2:
        case 3:
            v = 100.0f;
            goto common;
        case 9:
        case 10:
            v = 300.0f;
        common:
            if (em->x881 == 0) {
                w->x18 = 0;
                em->work08 = ((u16)em->x39A % 120 + 180) / 2;
                w->x14 = 1000.0f;
                break;
            }
            w->x18 = 1;
            w->x14 = CalcDistanceXZ(em->pos, em->tgt_pos);
            if (em->x881 == 1 && em->x882 == 0) {
                if (em->x617 == -1) {
                    w->x18 = 0;
                    w->x14 = 1000.0f;
                    break;
                }
                w->x14 = w->x14 - v;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            if (em->x881 == 7) {
                w->x14 = w->x14 - v;
                if (w->x14 <= 0.0f) {
                    w->x14 = 0.0f;
                }
            }
            em->work08 = (s32)w->x14 / 7;
            break;
        }
        break;
    case 3:
        break;
    }
    em_act_set2(em, kind, no, arg);
}

void em33_to_normal(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em->x3F4 = 0;
    em->x839 = 1;
    em33_act_set(em, 0, 1, 0);
}

void em33_char_set(EMW *em, int no, int a, int b) {
    int n;

    em_char_set(em, no, a, b);
    if (em->x11 != 0) {
        switch (em->char0) {
        case 0x3E9: n = 0x4B1; break;
        case 0x3EA: n = 0x4B2; break;
        case 0x3EB: n = 0x4B3; break;
        case 0x3ED: n = 0x4B5; break;
        case 0x3F1: n = 0x4B9; break;
        case 0x3F2: n = 0x4BA; break;
        case 0x3F3: n = 0x4BB; break;
        default: n = 0; break;
        }
        if (n != 0) {
            em_char_set2(em, n, a, b, 1);
        }
    }
}

void em33_local_init(EMW *em) {
    eft07_set(em, 0);
    eft07_set(em, 1);
    eft01_set(em, 4);
}

void dummy_em_prog_00617D70(void) {
}

static void sound_call_sub_00617D80(EMW *em, int se, int idx) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + idx * 400);
    Em_se_req2(em, se, 0, pos, 7, 0);
}

static void sound_call_00617DF0(EMW *em, int frame, int se, int idx) {
    if (em_frame_check(em, 0, (f32)frame)) {
        sound_call_sub_00617D80(em, se, idx);
    }
}

static void move_default_00617E50(EMW *em) {
}

/* Sound and effect script per animation (sound_call_00617DF0(em, frame, se, joint)
 * plays a sound at the joint's position once the animation reaches the
 * frame; Eft13_set_em_scl and shell02_set spawn effects). */
static void ef_move_sub_00617E60(EMW *em, EM33W *w) {
    f32 v[3];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        break;
    case 0x3EA:
        sound_call_00617DF0(em, 92, 13, 23);
        sound_call_00617DF0(em, 122, 13, 23);
        sound_call_00617DF0(em, 146, 13, 23);
        sound_call_00617DF0(em, 182, 13, 23);
        sound_call_00617DF0(em, 210, 13, 23);
        sound_call_00617DF0(em, 240, 13, 23);
        sound_call_00617DF0(em, 270, 13, 23);
        break;
    case 0x3EB:
        sound_call_00617DF0(em, 14, 11, 23);
        break;
    case 0x3EC:
        sound_call_00617DF0(em, 6, 4, 23);
        sound_call_00617DF0(em, 26, 19, 0);
        sound_call_00617DF0(em, 32, 19, 0);
        sound_call_00617DF0(em, 50, 0, 0);
        break;
    case 0x3ED:
        sound_call_00617DF0(em, 4, Code_Make(16,4,17,4), 23);
        sound_call_00617DF0(em, 84, Code_Make(17,4,18,4), 23);
        sound_call_00617DF0(em, 200, Code_Make(16,4,18,4), 23);
        break;
    case 0x3EE:
        sound_call_00617DF0(em, 22, 14, 23);
        sound_call_00617DF0(em, 58, 14, 23);
        break;
    case 0x3F1:
        sound_call_00617DF0(em, 10, 0, 0);
        sound_call_00617DF0(em, 28, 0, 0);
        sound_call_00617DF0(em, 58, 0, 0);
        sound_call_00617DF0(em, 64, 0, 0);
        break;
    case 0x3F2:
        sound_call_00617DF0(em, 6, 19, 0);
        sound_call_00617DF0(em, 14, 19, 0);
        sound_call_00617DF0(em, 18, 19, 0);
        sound_call_00617DF0(em, 26, 19, 0);
        if (em_frame_check(em, 0, 4.0f)) {
            Eft13_set_em_scl(em, 21, 0.6f, 3);
        }
        if (em_frame_check(em, 0, 28.0f)) {
            Eft13_set_em_scl(em, 17, 0.6f, 3);
        }
        break;
    case 0x3F3:
        sound_call_00617DF0(em, 16, 19, 0);
        sound_call_00617DF0(em, 26, 19, 0);
        sound_call_00617DF0(em, 12, 19, 0);
        sound_call_00617DF0(em, 20, 19, 0);
        if (em_frame_check(em, 0, 12.0f)) {
            Eft13_set_em_scl(em, 21, 0.7f, 3);
        }
        if (em_frame_check(em, 0, 18.0f)) {
            Eft13_set_em_scl(em, 17, 0.7f, 3);
        }
        break;
    case 0x3F4:
    case 0x3F5:
        sound_call_00617DF0(em, 12, 1, 0);
        sound_call_00617DF0(em, 10, 19, 0);
        sound_call_00617DF0(em, 14, 19, 0);
        if (w->anim == 0x3F7) {
            if (em_frame_check(em, 0, 12.0f)) {
                Eft13_set_em_scl(em, 21, 0.9f, 3);
            }
        } else {
            if (em_frame_check(em, 0, 12.0f)) {
                Eft13_set_em_scl(em, 17, 0.9f, 3);
            }
        }
        break;
    case 0x3F6:
        sound_call_00617DF0(em, 12, 19, 0);
        sound_call_00617DF0(em, 16, 19, 0);
        sound_call_00617DF0(em, 46, 0, 0);
        if (em_frame_check(em, 0, 12.0f)) {
            Eft13_set_em_scl(em, 11, 0.6f, 3);
        }
        if (em_frame_check(em, 0, 16.0f)) {
            Eft13_set_em_scl(em, 11, 0.6f, 3);
        }
        if (em_frame_check(em, 0, 12.0f)) {
            Eft13_set_em_scl(em, 7, 0.6f, 3);
        }
        if (em_frame_check(em, 0, 16.0f)) {
            Eft13_set_em_scl(em, 7, 0.6f, 3);
        }
        break;
    case 0x3F7:
    case 0x3FE:
        sound_call_00617DF0(em, 12, 0, 0);
        sound_call_00617DF0(em, 16, 0, 0);
        sound_call_00617DF0(em, 22, 19, 0);
        sound_call_00617DF0(em, 30, 1, 0);
        sound_call_00617DF0(em, 56, 19, 0);
        sound_call_00617DF0(em, 58, 19, 0);
        if (w->anim == 0x3F7) {
            if (em_frame_check(em, 0, 34.0f)) {
                Eft13_set_em_scl(em, 21, 0.6f, 3);
            }
        } else {
            if (em_frame_check(em, 0, 34.0f)) {
                Eft13_set_em_scl(em, 17, 0.6f, 3);
            }
        }
        break;
    case 0x3F8:
        sound_call_00617DF0(em, 4, 7, 23);
        sound_call_00617DF0(em, 62, 8, 23);
        sound_call_00617DF0(em, 76, 9, 23);
        sound_call_00617DF0(em, 148, 10, 23);
        sound_call_00617DF0(em, 18, 0, 0);
        sound_call_00617DF0(em, 24, 0, 0);
        break;
    case 0x3F9:
        sound_call_00617DF0(em, 14, 2, 23);
        sound_call_00617DF0(em, 24, 1, 0);
        sound_call_00617DF0(em, 46, 0, 0);
        sound_call_00617DF0(em, 48, 0, 0);
        if (em_frame_check(em, 0, 20.0f)) {
            shell02_set(em, 11);
        }
        if (em_frame_check(em, 0, 24.0f)) {
            Eft13_set_em_scl(em, 22, 0.9f, 3);
        }
        break;
    case 0x3FA:
        sound_call_00617DF0(em, 24, 1, 0);
        sound_call_00617DF0(em, 26, 19, 0);
        sound_call_00617DF0(em, 44, 19, 0);
        sound_call_00617DF0(em, 46, 19, 0);
        if (em_frame_check(em, 0, 26.0f)) {
            Eft13_set_em_scl(em, 22, 0.9f, 3);
        }
        break;
    case 0x3FC:
        sound_call_00617DF0(em, 12, 19, 0);
        sound_call_00617DF0(em, 14, 19, 0);
        sound_call_00617DF0(em, 24, 19, 0);
        break;
    case 0x400:
        sound_call_00617DF0(em, 12, 3, 23);
        sound_call_00617DF0(em, 54, 0, 0);
        sound_call_00617DF0(em, 58, 0, 0);
        if (em_frame_check(em, 0, 14.0f)) {
            shell02_set(em, 12);
        }
        break;
    case 0x40E:
        sound_call_00617DF0(em, 12, 0, 0);
        sound_call_00617DF0(em, 46, 0, 0);
        sound_call_00617DF0(em, 64, 1, 0);
        sound_call_00617DF0(em, 4, 17, 0);
        break;
    case 0x40F:
        sound_call_00617DF0(em, 2, 11, 0);
        sound_call_00617DF0(em, 4, 23, 0);
        sound_call_00617DF0(em, 90, 2, 0);
        break;
    case 0x424:
    case 0x425:
        sound_call_00617DF0(em, 4, 5, 23);
        sound_call_00617DF0(em, 20, 0, 0);
        sound_call_00617DF0(em, 28, 0, 0);
        sound_call_00617DF0(em, 34, 0, 0);
        break;
    case 0x426:
        sound_call_00617DF0(em, 4, 12, 23);
        break;
    case 0x427:
        sound_call_00617DF0(em, 4, 6, 0);
        sound_call_00617DF0(em, 44, 20, 0);
        if (em_frame_check(em, 0, 44.0f)) {
            Eft13_set_em_scl(em, 2, 0.5f, 6);
        }
        break;
    case 0x428:
        sound_call_00617DF0(em, 14, 1, 0);
        sound_call_00617DF0(em, 50, 1, 0);
        sound_call_00617DF0(em, 24, 0, 0);
        sound_call_00617DF0(em, 44, 0, 0);
        sound_call_00617DF0(em, 58, 0, 0);
        sound_call_00617DF0(em, 76, 0, 0);
        break;
    case 0x429:
        sound_call_00617DF0(em, 14, 0, 0);
        sound_call_00617DF0(em, 38, 0, 0);
        sound_call_00617DF0(em, 66, 0, 0);
        sound_call_00617DF0(em, 124, 20, 0);
        sound_call_00617DF0(em, 98, 1, 0);
        sound_call_00617DF0(em, 4, 4, 0);
        break;
    case 0x42B:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 10.0f;
        em_sleep_eff_set(em, 23, v, 1.0f);
        break;
    case 0x42C:
        sound_call_00617DF0(em, 4, 15, 23);
        sound_call_00617DF0(em, 88, 19, 0);
        sound_call_00617DF0(em, 138, 15, 23);
        break;
    case 0x42D:
        sound_call_00617DF0(em, 4, Code_Make(16,4,17,4), 23);
        sound_call_00617DF0(em, 10, 0, 0);
        sound_call_00617DF0(em, 20, 0, 0);
        break;
    default:
        move_default_00617E50(em);
        break;
    }
}

void em33_effect_move(EMW *em) {
    EM33W *w = (EM33W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_00617E60(em, w);
        break;
    }
}
