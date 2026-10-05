/* em19b - game.bin 0x005E8060-0x005EB3A8. Monster kind 19 (and 24, which
 * moves faster): a small flier. This file holds its action steps (rest,
 * sleep, walk, fly, attack with Shell11 shots, flinch, paralysis, death,
 * revival, demo), em19_main / main_sub and the sound and effect hooks.
 * em19_init, em19_move_sub, em19_dir_adj, em19_fly_adjy2* live in other
 * files (g_em19_*). Height while flying is kept between the ground
 * (EMW+0x5AC) and 1000 above it. Names of the action steps follow the split
 * (em_act00, em_fly00...). */
#include "em.h"
#include "game.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM19W {
    u8 eff;             /* 0x00 em19_effect_move step */
    u8 _pad01[5];
    s16 char0;          /* 0x06 animation seen by ef_move_sub */
    u8 _pad08[0x34 - 0x08];
    f32 dist;           /* 0x34 distance left to the walk target */
    u8 _pad38[3];
    u8 tgt;             /* 0x3B non-zero: has a target to walk/turn to */
    s16 x3C;            /* 0x3C */
} EM19W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 no;             /* 0x08 quest number */
} QUEST_W;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

/* Keep the flier between the ground and 1000 above it. */
#define EM19_CLAMP_Y(em)                                    \
    if ((em)->pos[1] < (em)->x5AC) {                        \
        (em)->pos[1] = (em)->x5AC;                          \
    } else if ((em)->pos[1] > 1000.0f + (em)->x5AC) {       \
        (em)->pos[1] = 1000.0f + (em)->x5AC;                \
    }
#define EM19_CLAMP_Y2(em)                                   \
    if ((em)->pos[1] <= (em)->x5AC) {                       \
        (em)->pos[1] = (em)->x5AC;                          \
    } else if ((em)->pos[1] > 1000.0f + (em)->x5AC) {       \
        (em)->pos[1] = 1000.0f + (em)->x5AC;                \
    }

extern u8 em19_act_tbl[];
extern GAME_W game_w;
extern QUEST_W quest_w;

void em19_act_set(EMW *, int, u16, u16);
u16 em_act_search(void *);
void em_act_set(EMW *, int, int);
void em_char_set(EMW *, int, int, int);
void em_cmd_ck(EMW *);
void em_cmd_reset(EMW *);
void em19_init(EMW *);
void em19_move_sub(EMW *);
void em19_dir_adj(EMW *);
void em19_fly_adjy2(EMW *);
void em19_fly_adjy2_init(EMW *, int);
void em19_rate_add_calc(EMW *);
int em_frame_check(EMW *, int, f32);
int em_frame_check2(EMW *, int, f32);
int frame_check(EMW *, int, f32);
void em_rate_clear(EMW *);
void em_rate_add_g(EMW *);
void rate_add(EMW *);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void pl_flag_set(EMW *, int);
void pl_flag_clr(EMW *, int);
void Shell11_set(EMW *, int);
void flvecRotY(f32 *, f32);
void flvecCopy(f32 *, f32 *);
void flmatGetTrans(f32 *, void *);
u32 ran_suu(int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_set(EMW *, int);
void Em_hagi_point_clr(EMW *);
int Event_flag_ck();
u8 Em_Dmg_Sys(EMW *, void *);
void em_mode_timer_sub(EMW *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
int Code_Make(int, int, int, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void eft01_set(EMW *, s16);

void em19_main_sub(EMW *em);
static void sound_call(EMW *em, int frame, int code, int joint);

void em19_next_act_set(EMW *em) {
    if (em->x734 == 3) {
        em->x839 = 1;
        if (em->x388 == 2) {
            em_act_set(em, 2, 3);
        } else {
            em_act_set(em, 0, 1);
        }
    } else {
        em19_act_set(em, 0, em_act_search(em19_act_tbl), 0);
    }
}

static void em_act00(EMW *em) {
}

static void em_act01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em->work08 = (em->x39A & 3) * 0x1E + 0x1E;
        if (em->char0 != 0x3ED) {
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_act02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        if (em->char0 != 0x3FB) {
            em_char_set(em, 0x13, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_act03(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x88B = 0;
        if (em->char0 == 0x3F6) {
            em_char_set(em, 0x10, 0, 0);
            em->x05 = 2;
        } else {
            em_char_set(em, 0x15, 0, 0);
            em->x05++;
        }
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 20.0f;
        em_sleep_eff_set(em, 8, v, 0.6f);
        if (--em->work08 <= 0) {
            em->x05++;
            em_act_set(em, 0, 4);
        }
        break;
    }
}

static void em_act04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x88B = 1;
        Em_Sleep_End(em);
        em_char_set(em, 0x16, 0, 0);
        em->x05++;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_move00(EMW *em) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->x15) {
    case 0:
        em_act00(em);
        break;
    case 1:
        em_act01(em);
        break;
    case 2:
        em_act02(em);
        break;
    case 3:
        em_act03(em);
        break;
    case 4:
        em_act04(em);
        break;
    }
}

static void em_mov00(EMW *em) {
    EM19W *w = (EM19W *)em->ex;

    if (em->kind == 0x18) {
        em->act_spd = 1.2f;
    }
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 8, 0, 0);
        break;
    case 1:
        if (w->tgt != 0) {
            em19_move_sub(em);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_mov01(EMW *em) {
    EM19W *w = (EM19W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 9, 0, 0);
        break;
    case 1:
        if (w->tgt != 0) {
            em19_move_sub(em);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_mov02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        pl_flag_set(em, 0x20000);
        em_char_set(em, 6, 0, 0);
        break;
    case 1:
        em19_dir_adj(em);
        if (em_frame_check(em, 0, 16.0f) != 0) {
            em->x05++;
            em_char_set(em, 7, 0xA, 0);
        }
        break;
    case 2:
        em19_dir_adj(em);
        if (em->x194 == 0) {
            pl_flag_clr(em, 0x20000);
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_move01(EMW *em) {
    switch (em->x15) {
    case 0:
        em_mov00(em);
        break;
    case 1:
        em_mov01(em);
        break;
    case 2:
        em_mov02(em);
        break;
    }
}

static void em_fly00(EMW *em, s16 x) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em->work08 = 100;
        em_char_set(em, 2, 0, 0);
        em19_fly_adjy2_init(em, 0);
        break;
    case 1:
        em19_fly_adjy2(em);
        EM19_CLAMP_Y(em);
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_fly01(EMW *em, s16 fast) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_char_set(em, 3, 0, 0);
        if (fast == 0) {
            em->work08 = 100;
            em19_fly_adjy2_init(em, 1);
        } else {
            em->act_spd = 8.0f;
            em->work08 = 10;
            em19_fly_adjy2_init(em, 6);
        }
        break;
    case 1:
        if (fast != 0) {
            em->act_spd = 8.0f;
        }
        em19_fly_adjy2(em);
        EM19_CLAMP_Y(em);
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_fly02(EMW *em, s16 x) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 6, 0, 0);
        em19_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 0, 6.0f) != 0) {
            em->x05++;
            em->x388 = 2;
            em19_fly_adjy2(em);
            Shell11_set(em, 7);
        }
        break;
    case 2:
        em19_fly_adjy2(em);
        if (em_frame_check(em, 0, 32.0f) != 0) {
            em->x05++;
            em_char_set(em, 7, 0, 0);
            em_rate_clear(em);
            em->x388 = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_fly03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_rate_clear(em);
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0, 0);
        }
        em->work08 = (em->x39A & 3) * 0x1E + 0x1E;
        break;
    case 1:
        EM19_CLAMP_Y(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_fly04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 6, 0, 0);
        em19_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 0, 6.0f) != 0) {
            em->x05++;
            em->x388 = 2;
            em19_fly_adjy2(em);
        }
        break;
    case 2:
        em19_fly_adjy2(em);
        if (em_frame_check(em, 0, 16.0f) != 0) {
            em->x05++;
            em_char_set(em, 1, 0xA, 0);
            em->work08 = 10;
        }
        break;
    case 3:
        em19_fly_adjy2(em);
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
    EM19_CLAMP_Y(em);
}

static void em_fly05(EMW *em) {
    EM19W *w = (EM19W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        if (em->char0 != 0x3E9) {
            em->work08 = ((u16)ran_suu(1) & 3) * 0x1E + 0x1E;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 1:
        if (w->tgt != 0) {
            em19_dir_adj(em);
            w->dist -= 10.0f;
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        em_rate_add_g(em);
        EM19_CLAMP_Y(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_fly06(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em->x3B4 = 0.0f;
        em->adj_y = -5.0f;
        em->adj_z = 0.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = 0.0f;
        em->x3C0[2] = 0.0f;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        em_rate_add_g(em);
        if (em->pos[1] < em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x11, 0, 0);
            em->x388 = 0;
        } else if (em->pos[1] > 1000.0f + em->x5AC) {
            em->pos[1] = 1000.0f + em->x5AC;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_fly08(EMW *em) {
    EM19W *w = (EM19W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_rate_clear(em);
        if (em->x39A & 1) {
            em->x3B4 = 8.0f;
        } else {
            em->x3B4 = -8.0f;
        }
        em->adj_y = (em->x39A & 7) - 4;
        em->adj_z = ((em->x39A >> 4) & 7) - 4;
        if (em->adj_z >= 0.0f) {
            em_char_set(em, 2, 0, 0);
        } else {
            em_char_set(em, 3, 0, 0);
        }
        flvecRotY(&em->x3B4, DEG2RAD(ANG2DEG(em->ang[1])));
        flvecCopy(em->x3C0, &em->x3B4);
        em->work08 = 3;
        w->x3C = em->work08;
        break;
    case 1:
        em_rate_add_g(em);
        if (w->tgt != 0) {
            em19_dir_adj(em);
        }
        if (--em->work08 <= 0) {
            em->work08 = ((em->x39A >> 1) & 3) + 4;
            em->x3C0[0] = -em->x3C0[0];
            em->x3C0[1] = -em->x3C0[1];
            em->x3C0[2] = -em->x3C0[2];
            em->x05++;
        }
        break;
    case 2:
        rate_add(em);
        if (w->tgt != 0) {
            em19_dir_adj(em);
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em->work08 = 3;
        }
        break;
    case 3:
        em_rate_add_g(em);
        if (w->tgt != 0) {
            em19_dir_adj(em);
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
    EM19_CLAMP_Y(em);
}

static void em_fly09(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em->work08 = (em->x39A & 3) * 0xF + 0x1E;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        em19_dir_adj(em);
        em19_rate_add_calc(em);
        EM19_CLAMP_Y(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_move02(EMW *em) {
    switch (em->x15) {
    case 0:
        em_fly00(em, 0);
        break;
    case 1:
        em_fly01(em, 0);
        break;
    case 2:
        em_fly02(em, 0);
        break;
    case 3:
        em_fly03(em);
        break;
    case 4:
        em_fly04(em);
        break;
    case 5:
        em_fly05(em);
        break;
    case 6:
        em_fly06(em);
        break;
    case 7:
        em_fly01(em, 1);
        break;
    case 8:
        em_fly08(em);
        break;
    case 9:
        em_fly09(em);
        break;
    }
}

static void em_atk00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_char_set(em, 0xA, 0, 0);
        em19_fly_adjy2_init(em, 2);
        break;
    case 1:
        em19_fly_adjy2(em);
        EM19_CLAMP_Y2(em);
        if (em->x194 == 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_atk01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_char_set(em, 0xB, 0, 0);
        em->x3B4 = 0.0f;
        em->adj_y = 0.0f;
        em->adj_z = 2.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = 0.0f;
        em->x3C0[2] = 0.0f;
        flvecRotY(&em->x3B4, DEG2RAD(ANG2DEG(em->ang[1])));
        break;
    case 1:
        em_rate_add_g(em);
        EM19_CLAMP_Y2(em);
        if (em->x194 == 0) {
            em_rate_clear(em);
            em19_next_act_set(em);
        }
        if (!(em->type & 1)) {
            if (frame_check(em, 0, 34.0f) != 0) {
                Shell11_set(em, 6);
            }
        } else {
            if (frame_check(em, 0, 34.0f) != 0) {
                Shell11_set(em, 0xB);
            }
        }
        break;
    }
}

static void em_atk02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 6, 0, 0);
        em19_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 0, 6.0f) != 0) {
            em->x05++;
            em->x388 = 2;
            em19_fly_adjy2(em);
            Shell11_set(em, 7);
        }
        break;
    case 2:
        em19_fly_adjy2(em);
        if (em_frame_check(em, 0, 32.0f) != 0) {
            em->x05++;
            em_char_set(em, 7, 0, 0);
            em_rate_clear(em);
            em->x388 = 0;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_atk03(EMW *em) {
    EM19W *w;

    em->act_spd = 1.3f;
    w = (EM19W *)em->ex;
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 9, 0, 0);
        Shell11_set(em, 0xA);
        break;
    case 1:
        if (w->tgt != 0) {
            em19_move_sub(em);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        if (--em->work08 <= 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_atk04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_char_set(em, 0xB, 0, 0);
        em->x3B4 = 0.0f;
        em->adj_y = 0.0f;
        em->adj_z = 2.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = 0.0f;
        em->x3C0[2] = 0.0f;
        flvecRotY(&em->x3B4, DEG2RAD(ANG2DEG(em->ang[1])));
        break;
    case 1:
        em_rate_add_g(em);
        EM19_CLAMP_Y2(em);
        if (em->x194 == 0) {
            em_rate_clear(em);
            em19_next_act_set(em);
        }
        if (!(em->type & 1)) {
            if (frame_check(em, 0, 34.0f) != 0) {
                Shell11_set(em, 0xE);
            }
        } else {
            if (frame_check(em, 0, 34.0f) != 0) {
                Shell11_set(em, 0xF);
            }
        }
        break;
    }
}

static void em_move03(EMW *em) {
    switch (em->x15) {
    case 0:
        em_atk00(em);
        break;
    case 1:
        em_atk01(em);
        break;
    case 2:
        em_atk02(em);
        break;
    case 3:
        em_atk03(em);
        break;
    case 4:
        em_atk04(em);
        break;
    }
}

static void em_dm00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_char_set(em, 0xC, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_dm01(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        em_char_set(em, 0xD, 0, 0);
        em_cmd_reset(em);
        em19_fly_adjy2_init(em, 4);
        break;
    case 1:
        em19_fly_adjy2(em);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0xE, 0, 0);
            em_rate_clear(em);
        } else if (em->pos[1] > 1000.0f + em->x5AC) {
            em->pos[1] = 1000.0f + em->x5AC;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_dm02(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        if (em->char0 == 0x3F6) {
            em_char_set(em, 0x10, 0, 0);
            em->x05 = 2;
        } else {
            em_char_set(em, 0x15, 0, 0);
            em->x05++;
        }
        em_cmd_reset(em);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        em_mahi_eff_set(em, 2);
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x16, 0, 0);
            Em_Mahi_End(em);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_dm03(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        em_char_set(em, 0x12, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_dm04(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 2;
        em->x05++;
        if (em->char0 != 0x3F5) {
            em_char_set(em, 0xD, 0, 0);
            em19_fly_adjy2_init(em, 4);
        }
        em_cmd_reset(em);
        break;
    case 1:
        em19_fly_adjy2(em);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0xE, 0, 0);
            em_rate_clear(em);
        } else if (em->pos[1] > 1000.0f + em->x5AC) {
            em->pos[1] = 1000.0f + em->x5AC;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
            em->work08 = 200;
        }
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x16, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_dm05(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x388 = 0;
        em->x05++;
        if (em->char0 != 0x3FD) {
            em_char_set(em, 0x15, 0, 0);
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
            em->work08 = 200;
        }
        break;
    case 2:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x16, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_move04(EMW *em) {
    switch (em->x15) {
    case 0:
        em_dm00(em);
        break;
    case 1:
        em_dm01(em);
        break;
    case 2:
        em_dm02(em);
        break;
    case 3:
        em_dm03(em);
        break;
    case 4:
        em_dm04(em);
        break;
    case 5:
        em_dm05(em);
        break;
    }
}

static void em_die00(EMW *em) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        Quest_enemy_die(em);
        em->x05++;
        if (em->x388 == 2) {
            if (em->char0 != 0x3F7) {
                em_char_set(em, 0xF, 0, 0);
            }
        } else if (em->char0 != 0x3FC) {
            em_char_set(em, 0x14, 0, 0);
        }
        break;
    case 1:
        em->x95C = 2;
        em->x7D6 = 2;
        if (em_frame_check2(em, 0, em->char0 == 0x3F7 ? 100 : 50) != 0) {
            em->x798 -= 0.016666668f;
            if (em->x798 <= 0.0f) {
                if (em->x194 == 0) {
                    em->x01 = 0;
                    em_act_set(em, 5, 2);
                }
                em->x798 = 0.0f;
            }
        }
        break;
    }
}

static void em_die01(EMW *em) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        Quest_enemy_die(em);
        if (em->x388 == 2) {
            em->x05++;
            if (em->char0 != 0x3F5) {
                em_char_set(em, 0xD, 0, 0);
                em19_fly_adjy2_init(em, 4);
            }
        } else {
            em->x05 = 2;
            if (em->char0 != 0x3FD) {
                em_char_set(em, 0x15, 0, 0);
            }
        }
        break;
    case 1:
        em19_fly_adjy2(em);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0xE, 0, 0);
            em_rate_clear(em);
        } else if (em->pos[1] > 1000.0f + em->x5AC) {
            em->pos[1] = 1000.0f + em->x5AC;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            em->work08 = 900;
        }
        break;
    case 3:
        if (Em_hagi_point_cnt_ck(em) <= 0 || --em->work08 <= 0) {
            em->x05++;
            Em_hagi_point_clr(em);
        }
        break;
    case 4:
        em->x798 -= 0.016666668f;
        if (em->x798 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 2);
            em->x798 = 0.0f;
        }
        break;
    }
}

static void em_die02(EMW *em) {
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->work08 = 150;
        em->x05++;
        em->x56A = 0;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
        }
        break;
    case 2:
        if (Quest_enemy_revival_ck(em) == 1) {
            em_status_init(em);
            em19_init(em);
            Quest_enemy_revival_set(em);
            em_cmd_reset(em);
            em->x839 = 0;
            em->mode = 5;
            em->x15 = 2;
            em->pos[1] += 300.0f;
            em->x798 = 0.0f;
            em_act_set(em, 7, 0);
        } else {
            em->x04++;
        }
        break;
    }
}

static void em_move05(EMW *em) {
    em->x40C = 10;
    em->x40E = 10;
    switch (em->x15) {
    case 0:
        em_die00(em);
        break;
    case 1:
        em_die01(em);
        break;
    case 2:
        em_die02(em);
        break;
    }
}

static void em_demo00(EMW *em) {
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
        if (Event_flag_ck(0xD, 1) == 1) {
            em->x05++;
            em->x01 = 1;
            em19_next_act_set(em);
        }
        break;
    }
}

static void em_move06(EMW *em) {
    switch (em->x15) {
    case 0:
        em_demo00(em);
        break;
    }
}

static void em_revival00(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        em->x388 = 2;
        em_rate_clear(em);
        em->adj_y = -10.0f;
        em->work08 = 0x1E;
        break;
    case 1:
        em->x798 += 0.033333335f;
        if (em->x798 > 1.0f) {
            em->x798 = 1.0f;
        }
        em_rate_add_g(em);
        EM19_CLAMP_Y(em);
        if (--em->work08 <= 0) {
            em->work08 = 0;
            em->x05++;
            em->x9E1 = 0;
            em19_next_act_set(em);
            em->x798 = 1.0f;
        }
        break;
    }
}

static void em_move07(EMW *em) {
    em->x40C = 10;
    em->x40E = 10;
    em->x9E1 = 5;
    switch (em->x15) {
    case 0:
        em_revival00(em);
        break;
    }
}

void em19_main(EMW *em) {
    u8 dmg[4];

    em->act_spd = 1.0f;
    em_mode_timer_sub(em);
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 3:
    case 4:
    case 7:
    case 9:
    case 11:
    case 13:
        break;
    case 1:
        if (em->mode != 5) {
            if (em->x39A % 20 == 0) {
                em_act_set(em, 5, 1);
            } else {
                em_act_set(em, 5, 0);
            }
        }
        em->x839 = 0;
        break;
    case 2:
        if (em->mode != 5) {
            em_act_set(em, 5, 1);
        }
        em->x839 = 0;
        break;
    case 5:
        if (em->mode != 4 || em->x15 != 1) {
            em_act_set(em, 4, 1);
        }
        break;
    case 6:
        if (em->mode != 4 || em->x15 != 2) {
            em_mahi_dmg_timer_set(em);
            em_act_set(em, 4, 2);
        }
        break;
    case 8:
        if (em->mode != 0 || em->x15 != 3) {
            em_sleep_dmg_timer_set(em);
            em_act_set(em, 0, 3);
            em->x839 = 0;
        }
        break;
    case 10:
        switch (em->x15) {
        case 3:
            em_act_set(em, 0, 4);
            em->x839 = 0;
            break;
        }
        break;
    case 12:
    case 14:
        if (em->x388 == 2) {
            em_act_set(em, 4, 0);
        } else {
            em_act_set(em, 4, 3);
        }
        em->x839 = 0;
        break;
    }
    if (quest_w.no == 0x8A && em->stg == 0x25 && Event_flag_ck(0xD) == 0) {
        if (game_w.info_stop == 1 && em->mode != 6) {
            em19_act_set(em, 6, 0, 1);
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
    em19_main_sub(em);
    if (em->x6FF != 0) {
        em19_main_sub(em);
        em->x6FF = 0;
    }
}

void em19_main_sub(EMW *em) {
    switch (em->mode) {
    case 0:
        em_move00(em);
        break;
    case 1:
        em_move01(em);
        break;
    case 2:
        em_move02(em);
        break;
    case 3:
        em_move03(em);
        break;
    case 4:
        em_move04(em);
        break;
    case 5:
        em_move05(em);
        break;
    case 6:
        em_move06(em);
        break;
    case 7:
        em_move07(em);
        break;
    }
}

static void ef_move_sub(EMW *em, EM19W *w) {
    if (em->char0 != w->char0) {
        w->char0 = em->char0;
    }
    switch (w->char0) {
    case 0x3E9:
    case 0x3EA:
    case 0x3EB:
        sound_call(em, 4, 0, 0);
        sound_call(em, 0x2A, 0, 0);
        sound_call(em, 0x50, 0, 0);
        sound_call(em, 0x76, 0, 0);
        break;
    case 0x3ED:
        sound_call(em, 4, Code_Make(0xD, 1, 0xD, 1), 0);
        break;
    case 0x3EE:
        sound_call(em, 4, 4, 0);
        sound_call(em, 4, 5, 0);
        sound_call(em, 0x22, 0, 0);
        break;
    case 0x3EF:
        sound_call(em, 4, 5, 0);
        break;
    case 0x3F0:
        sound_call(em, 4, 1, 0);
        sound_call(em, 0x3C, 1, 0);
        sound_call(em, 0x78, 1, 0);
        sound_call(em, 0xB4, 1, 0);
        sound_call(em, 0xF0, 1, 0);
        break;
    case 0x3F1:
        sound_call(em, 4, 3, 0);
        sound_call(em, 0x10, 3, 0);
        break;
    case 0x3F2:
        sound_call(em, 4, 0, 0);
        sound_call(em, 0x24, 0, 0);
        sound_call(em, 4, 5, 0);
        if (frame_check(em, 0, 12.0f) != 0) {
            Shell11_set(em, 5);
        }
        break;
    case 0x3F3:
        sound_call(em, 4, 6, 0);
        sound_call(em, 0x1C, 7, 0);
        sound_call(em, 4, 0, 0);
        sound_call(em, 0x26, 0, 0);
        sound_call(em, 0x48, 0, 0);
        sound_call(em, 4, 5, 0);
        break;
    case 0x3F4:
        sound_call(em, 4, 9, 0);
        sound_call(em, 0x44, 0, 0);
        break;
    case 0x3F5:
        sound_call(em, 4, 0xA, 0);
        break;
    case 0x3F6:
        sound_call(em, 4, 0xB, 0);
        sound_call(em, 4, 5, 0);
        break;
    case 0x3F7:
        sound_call(em, 4, 8, 0);
        break;
    case 0x3F8:
        sound_call(em, 4, 0xC, 0);
        break;
    case 0x3F9:
        sound_call(em, 0x16, 5, 0);
        sound_call(em, 0x20, 2, 0);
        break;
    case 0x3FA:
        sound_call(em, 4, 9, 0);
        sound_call(em, 0x14, 2, 0);
        sound_call(em, 0x10, 5, 0);
        break;
    case 0x3FB:
        sound_call(em, 4, Code_Make(0xD, 1, 0xD, 1), 0);
        break;
    case 0x3FC:
        sound_call(em, 4, 8, 0);
        break;
    case 0x3FD:
        sound_call(em, 4, 0xA, 0);
        sound_call(em, 0xC, 5, 0);
        break;
    case 0x3FE:
        sound_call(em, 4, 5, 0);
        sound_call(em, 0x32, 2, 0);
        sound_call(em, 0x4C, 3, 0);
        break;
    }
}

void em19_effect_move(EMW *em) {
    EM19W *w = (EM19W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub(em, w);
        break;
    }
}

static void sound_call(EMW *em, int frame, int code, int joint) {
    f32 p[3];

    if (em_frame_check(em, 0, frame) != 0) {
        flmatGetTrans(p, em->mdl->bone + joint * 400);
        Em_se_req2(em, code, 0, p, 5, 0);
    }
}

void em19_local_init(EMW *em) {
    eft01_set(em, 0);
}

/* A file static in the original; named by its address because data outside
 * this file (the monster program tables) points at it. */
void dummy_em_prog_005EB3A0(void) {
}
