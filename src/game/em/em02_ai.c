/* em02_ai, run 1: em02_hagi_set .. em_mv00_0057FF80 (game.bin 0x0057F1E0-0x005800B4). Matching functions of em02_ai_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))

/* em02's part of the per-monster work at EMW+0x444 (em02.c has the same start). */
typedef struct EM02W {
    u8 _pad00;
    u8 eff;             /* 0x01 effect script step (em02_effect_move) */
    s16 anim;           /* 0x02 animation the sound/effect script follows */
    u8 _pad04;
    u8 x05;             /* 0x05 */
    s16 x06;            /* 0x06 */
    u8 _pad08[8];
    f32 dist;           /* 0x10 distance to the target (1000 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16;
    u8 has_tgt;         /* 0x17 */
    s8 x18;             /* 0x18 1 while flying */
    u8 _pad19;
    s8 x1A;             /* 0x1A attack repeat counter */
    u8 _pad1B;
    s32 turn_left;      /* 0x1C */
    s32 bank_max;       /* 0x20 */
    f32 tp[3];          /* 0x24 saved position */
    s32 spd[3];         /* 0x30 passed to speed_add(_g) (angle in [1]) */
    s32 pitch_spd;      /* 0x3C ang[0] step */
    s32 turn;           /* 0x40 maximum turn per call */
    s32 bank_spd;       /* 0x44 ang[2] step */
    u8 _pad48[4];
    u8 adj_x;           /* 0x4C fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x4D */
    u8 adj_z;           /* 0x4E */
    u8 adj_type;        /* 0x4F table row */
    s16 adj_tm;         /* 0x50 time into the table */
    u8 _pad52[2];
    s8 hagi[3];         /* 0x54 hagi pick point numbers, -1 none */
} EM02W;

extern GAME_W game_w;

void Eft19_set(EMW *, int, int);
void eft09_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void shell01_set(EMW *, int);
s16 em_hp_vital_set2(EMW *, s16, s16);
int em_act_search(void *);
void em_char_set(EMW *, int, int, int);
void em_char_set2();
void em_char_set2();
void em_act_set(EMW *, int, u16);
int em_frame_check(EMW *, f32, int);
int em_frame_check2(EMW *, int, f32);
u16 Em_Calc_angY(f32 *, f32 *);
void Em_set_quake_sub(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
void em_hp_add(EMW *, s16);
void em_range_set(EMW *, s8);
void em_search_data_set(EMW *, u8);
void em_thirst_add(EMW *, s32);
void em_thirst_end(EMW *);
void em_hungry_add(EMW *, s32);
void em_niku_eat_set(EMW *);
void Em_Suimin_Start(EMW *);
void Em_Sleep_Start(EMW *);
void em_hinshi_end(EMW *);
int em_sleep_hp_add(EMW *, s16, s16, s16);
void cmd_target_kind_set(EMW *, f32 *);
void target_kind_set(EMW *, f32 *);
void Em_Sleep2_Start(EMW *);
void Em_Sleep2_End(EMW *);
void Em_Sleep_End(EMW *);
void Em_Mahi_End(EMW *);
void em_suimin_end(EMW *);
void em_hungry_end(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void mot_miration_ret(EMW *, f32 *);
void em_rate_clear(EMW *);
void em_rate_clear_g(EMW *);
u16 senkai_target(EMW *);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
void xang_calc_target(EMW *, int *, f32, f32);
void xang_calc_pl(EMW *, int *, f32, f32);
f32 CalcDistanceXZ(f32 *, f32 *);
void NextStage_Dir_Set(EMW *, f32 *);
void Em_Next_Stage_Pos();
int AreaFieldInCheck(u8, f32 *);
void WyvernAreaMove(PLW *);
typedef struct FLYNEED {
    u8 _pad00[0x14];
    s32 x14;
} FLYNEED;
extern FLYNEED *em_thirst_tbl[];
extern FLYNEED *em_hungry_tbl[];
typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 width;          /* 0x10 */
    f32 depth;          /* 0x14 */
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;
typedef f32 (*EM_POSP)[3];
STAGE_DATA *Stage_data_get(u8);
EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);
void em_area_move_init(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void eft11_set(EMW *, f32 *, int);
int em_pl_pos_set(EMW *, u8, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void Eft17_set(EMW *, int, int, int);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
int ran_suu();
void em_action_timer_calc(EMW *, int);
f32 flSqrt(f32);
void SetVector(f32 *, f32, f32, f32);
void senkai_player(EMW *);
s8 smell_search(EMW *, int, f32 *);
void ground_point_search(EMW *);
void xang_set_pl(EMW *, int, f32);
int Pl_stg_ck_tw(EMW *, PLW *);
int em_target_pl_samestage_ck(EMW *);
void World_calc2(u8, f32 *, f32 *);
void em_cmd_reset(EMW *);
void Quest_enemy_hagi_set(EMW *, int);
void Em_Sleep_Flag_Ck(EMW *);
void em_ana_loop_cnt_set(EMW *);
void em_mahi_eff_set(EMW *, int);
void em_tail_off_sub(EMW *);
void wyvern_kill_cnt_up(void *, int);
extern EMW em_work[];
void Quest_enemy_capture();
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void Eft13_set_em(EMW *, int, int);
int Event_flag_ck(int);
int em_mode_timer_sub(EMW *);
void em_no_floor_ck(EMW *);
void em_hinshi_ck(EMW *, f32);
void em_egg_ck(EMW *);
void em_thirst_ck(EMW *);
void em_hungry_ck(EMW *);
void em_sleep_ck(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void em_sleep2_dmg_timer_set(EMW *);
int em_hokaku_ck(EMW *, f32);
void em_ikari_add(EMW *, s16);
void em_cmd_ck(EMW *);
void em_dur_set(EMW *, int);
void Eft13_set_em_scl(EMW *, int, f32, int);
void Eft15_set3(EMW *, int, f32, int);
int em_frame_check3(EMW *, int, f32, f32);

extern s8 hagi_tbl_00388508[3][2];
extern f32 hagi_r_tbl_006544B0[3];
void em02_act_set(EMW *em, int kind, u16 no, u16 arg);
void em02_fly_adjy(EMW *, int);
u8 em02_fly_adjy2(EMW *);
void em02_fly_adjy2_init(EMW *, u8);
u16 em02_senkai_target(EMW *);
void em02_main_sub(EMW *em, EM02W *w);
void em02_to_normal(EMW *em);
void em02_senkai_player(EMW *);
void shell04_set(EMW *, int);
void Eft17_set(EMW *, int, int, int);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void mot_miration_ret(EMW *, f32 *);
void xang_calc_target(EMW *, int *, f32, f32);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
int em_frame_check(EMW *, f32, int);
void em02_to_fly(EMW *em, int mode);
void em_no_battle_area_ck(EMW *, int, int);
void F_DragonEscapeCamera(EMW *);
int Quest_clear_ck(int);
void get_joint_pos_em(EMW *, int, f32 *);
int Ext_pick_point_set(STIEM *, f32 *);
void Ext_pick_point_pos(int, f32 *);
int Ext_pick_point_cnt_ck(int);
void Ext_pick_point_clr(int);
void quake_call_00587230(EMW *em, int frame, int v);
void sound_call_sub_00587280(EMW *em, int se, int joint);
void sound_call_005872F0(EMW *em, int frame, int se, int joint);
void move_default_00583BA0(EMW *em);
void ef_move_sub_00583BF0(EMW *em, EM02W *w);
void em02_uvmove(EMW *em);
void em_uvset(EMW *em, u32 frame, u16 idx, s8 type);
void Em_set_quake_sub(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);

#define FLY_FLOOR(em)                    \
    if ((em)->pos[1] < (em)->x5AC) {     \
        (em)->pos[1] = (em)->x5AC;       \
    }

/* Turn toward the target by at most 0x40 per frame (same as em01/em03). */
#define EM02_TURN(em)                                                     \
    do {                                                                  \
        int d;                                                            \
        d = (u16)((u16)Em_Calc_angY((em)->pos, (em)->tgt_pos) - (em)->ang[1]); \
        if (d <= 0x8000) {                                                \
            if (d <= 0x3F) {                                              \
                (em)->ang[1] += d;                                        \
            } else {                                                      \
                (em)->ang[1] += 0x40;                                     \
            }                                                             \
        } else if (d > 0xFFC0) {                                          \
            (em)->ang[1] += d;                                            \
        } else {                                                          \
            (em)->ang[1] -= 0x40;                                         \
        }                                                                 \
    } while (0)

#define ATK_SIMPLE(NAME, CH)                     \
    static void NAME(EMW *em, EM02W *w) {        \
        switch (em->x05) {                       \
        case 0:                                  \
            em->x05++;                           \
            em->x388 = 0;                        \
            em->x3F4 = 0;                        \
            em_char_set(em, CH, 0, 0);           \
            break;                               \
        case 1:                                  \
            if (em->x194 == 0) {                 \
                em->x05++;                       \
                em02_to_normal(em);              \
            }                                    \
            break;                               \
        }                                        \
    }

#define DMG_SIMPLE(NAME, CH)                     \
    static void NAME(EMW *em, EM02W *w) {        \
        switch (em->x05) {                       \
        case 0:                                  \
            em->x05++;                           \
            em->x388 = 0;                        \
            em->x3F4 = 0;                        \
            em_cmd_reset(em);                    \
            em_char_set(em, CH, 0, 0);           \
            break;                               \
        case 1:                                  \
            if (em->x194 == 0) {                 \
                em->x05++;                       \
                em02_to_normal(em);              \
            }                                    \
            break;                               \
        }                                        \
    }

#define UV_RESET(i) \
    do { \
        uv[i][0] = 0.0f; \
        uv[i][1] = 0.0f; \
        tm[i] = 0xFFFF; \
        ty[i] = 0xFF; \
    } while (0)

void em02_hagi_set(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    STIEM s;
    f32 pos[3];
    int i;

    if (quest_w.x14E == 0) {
        s.id = 0xE6;
    } else {
        s.id = 0xE7;
    }
    for (i = 0; i < 3; i++) {
        s.rad = hagi_r_tbl_006544B0[i];
        s.stg = em->stg;
        s.x1A = 0;
        s.x19 = 1;
        get_joint_pos_em(em, hagi_tbl_00388508[i][0], pos);
        pos[1] = em->x5AC;
        s.x14 = 2;
        s.cnt = hagi_tbl_00388508[i][1];
        w->hagi[i] = Ext_pick_point_set(&s, pos);
    }
}

void em02_hagi_move(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    f32 pos[3];
    int i;

    for (i = 0; i < 3; i++) {
        if (w->hagi[i] != -1) {
            get_joint_pos_em(em, hagi_tbl_00388508[i][0], pos);
            pos[1] = em->x5AC;
            Ext_pick_point_pos(w->hagi[i], pos);
            if (Ext_pick_point_cnt_ck(w->hagi[i]) <= 0) {
                Ext_pick_point_clr(w->hagi[i]);
                w->hagi[i] = -1;
            }
        }
    }
}

void em02_hagi_clr(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    int i;

    for (i = 0; i < 3; i++) {
        if (w->hagi[i] != -1) {
            Ext_pick_point_clr(w->hagi[i]);
            w->hagi[i] = -1;
        }
    }
}

void em02_to_normal(EMW *em) {
    em->x839 = 1;
    em->act_spd = 1.0f;
    if (em->x39A & 1) {
        em02_act_set(em, 0, 1, 0);
    } else {
        em02_act_set(em, 0, 2, 0);
    }
}

void em02_to_fly(EMW *em, int mode) {
    em->x839 = 1;
    em->act_spd = 1.0f;
    switch ((u8)mode) {
    case 0:
        em02_act_set(em, 2, 2, 0);
        break;
    default:
        break;
    }
}

void em02_main(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    u8 dmg[4];
    int d;

    em->x56A = 0;
    EMF(em, s16, 0x572) = 0;
    em_no_floor_ck(em);
    em_no_battle_area_ck(em, 0, 1);
    if (em->x302 <= (s16)(0.65f * (f32)em->x792)) {
        if (em->x8B6 == 0) {
            em_ikari_add(em, em->x8B0);
        } else {
            em->x8B6 = 1;
            em->x8B4 = 0x708;
        }
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 0:
    case 14:
        break;
    case 1:
        if (em->x388 == 2) {
            em02_act_set(em, 5, 2, 2);
        } else if (em->x3F0 == 9) {
            em02_act_set(em, 5, 1, 2);
        } else {
            em02_act_set(em, 5, 0, 2);
        }
        break;
    case 3:
        Em_Sleep_Flag_Ck(em);
        em02_act_set(em, 4, 4, 2);
        break;
    case 5:
        if (em->mode != 4 || em->x15 != 3) {
            em02_act_set(em, 4, 3, 2);
        }
        break;
    case 6:
        if (!(em->mode == 4 && em->x15 == 5) && !(em->mode == 4 && em->x15 == 3)) {
            em_mahi_dmg_timer_set(em);
            em02_act_set(em, 4, 5, 2);
        }
        break;
    case 8:
        if (!(em->mode == 0 && em->x15 == 4) && !(em->mode == 4 && em->x15 == 3)) {
            em_sleep_dmg_timer_set(em);
            em02_act_set(em, 0, 4, 2);
        }
        break;
    case 10:
        if (em->mode == 0 && em->x15 == 4) {
            em02_act_set(em, 0, 5, 2);
            em->x839 = 0;
        }
        break;
    case 11:
        em02_act_set(em, 4, 4, 2);
        break;
    case 12:
        pl_flag_clr((PLW *)em, 0x20000);
        if (em->x388 == 2) {
            em02_act_set(em, 4, 3, 2);
        } else {
            d = em->x38E;
            switch (d) {
            case 0:
            case 1:
            case 4:
            case 5:
                em02_act_set(em, 4, 0, 2);
                break;
            case 6:
                em02_act_set(em, 4, 1, 2);
                break;
            case 2:
            case 3:
            case 7:
            default:
                if (d != 3) {
                    em02_act_set(em, 4, 2, 2);
                } else {
                    em02_act_set(em, 4, 6, 2);
                }
                break;
            }
        }
        break;
    case 13:
        if (em->x388 != 2) {
            em02_act_set(em, 4, 0, 2);
            em->x839 = 0;
        }
        break;
    }
    switch (quest_w.no) {
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0xB9:
    case 0xCF:
        if (em->stg == 0x19) {
            if (Event_flag_ck(0x1B) == 0) {
                if (game_w.info_stop == 1 && em->mode != 6) {
                    em02_act_set(em, 6, 1, 0);
                }
            } else if (Quest_clear_ck(0) == 1) {
                if (em->mode != 6 && em->mode != 5) {
                    em->x839 = 0;
                    if (game_w.stage == em->stg) {
                        F_DragonEscapeCamera(em);
                    }
                    if (em->x388 == 2) {
                        em02_act_set(em, 6, 2, 0);
                    } else {
                        em02_act_set(em, 6, 0, 0);
                    }
                }
            } else {
                goto cmd;
            }
            break;
        }
        goto cmd;
    default:
    cmd:
        if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        break;
    }
    em02_main_sub(em, w);
    if (em->x6FF != 0) {
        em02_main_sub(em, w);
        em->x6FF = 0;
    }
}

void em_act00_0057FA50(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        if (em->x2DE != 0x44D) {
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
        }
        if (em->x2E0 != 0x4B1) {
            em_char_set2(em, 0x579, 0xA, 0, 2);
        }
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x839 = 1;
        }
        break;
    }
}

void em_act01_0057FB30(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (em->x8C3 == 0 && em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_act02_0057FBC0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1E, 0, 0);
        break;
    case 1:
        if (em->x8C3 == 0 && em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_act03_0057FC50(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x20, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_act04_0057FCD0(EMW *em, EM02W *w) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x22, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em02_act_set(em, 0, 5, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em02_act_set(em, 0, 5, 3);
        }
        break;
    }
    if (em_frame_check2(em, 0, 296.0f)) {
        v[1] = 100.0f;
        v[2] = 200.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x33, v, 1.6f);
    }
}

void em_act05_0057FDF0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1C, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_act06_0057FEC0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1C, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

void em_mv00_0057FF80(EMW *em, EM02W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM02_TURN(em);
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}
