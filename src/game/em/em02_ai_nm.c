/* em02 AI - game.bin 0x0057F1E0-0x00587390: monster kind 2: hagi (tail cut) points, em02_main (damage system), action
 * steps em_act00-06, walk (mv), flight (fly), attacks (atk), damage reactions (dmg), death (die), move-state dispatchers,
 * event demos, uvmove and the per-animation sound/effect script (ef_move_sub). The setters and turn helpers are in
 * em02.c; the whole file is in this _nm file, the matching runs are em02_ai*.c. Field meanings are guesses. */
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
    s8 x57;             /* 0x57 G: kind 0x24's counter (em02_g_tick), -1 idle */
} EM02W;

extern GAME_W game_w;


void Eft19_set(EMW *, int, int);
void eft09_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void shell01_set(EMW *, int);
s16 em_hp_vital_set2(EMW *, s16, s16);
unsigned short em_act_search(void *);
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
static void quake_call_00587230(EMW *em, int frame, int v);
static void sound_call_sub_00587280(EMW *em, int se, int joint);
static void sound_call_005872F0(EMW *em, int frame, int se, int joint);
static void move_default_00583BA0(EMW *em);
static void ef_move_sub_00583BF0(EMW *em, EM02W *w);
void em02_uvmove(EMW *em);
static void em_uvset(EMW *em, u32 frame, u16 idx, s8 type);
void Em_set_quake_sub(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);

void em02_hagi_set(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    STIEM s;
    f32 pos[3];
    int i;

#ifdef VERSION_MHG
    /* G (0x5A9FC0): the carve set by the quest's rank byte (header +0x42), kind 2 and its subspecies 0x24 differ
     * in rank 2 */
    switch (((s8 *)quest_w.x64)[0x42]) {
    default:
        s.id = 0xCB;
        break;
    case 1:
        s.id = 0xF3;
        break;
    case 2:
        s.id = em->kind == 2 ? 0x13B : 0x15C;
        break;
    case 3:
        s.id = 0x119;
        break;
    }
#else
    if (quest_w.x14E == 0) {
        s.id = 0xE6;
    } else {
        s.id = 0xE7;
    }
#endif
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

#ifdef VERSION_MHG
/* G 0x5A9F00 (G only), first thing in em02_main: kind 0x24's counter at w+0x57. While enraged (x8B6) it counts
 * 41 ticks and moves x8B7 from 0 to 1 (then holds at 0); calm, it runs up to 51 and parks at -1 (em02_init: -1). */
static void em02_g_tick(EMW *em, EM02W *w) {
    if (em->kind != 0x24) {
        return;
    }
    if (em->x8B6 != 0) {
        switch (em->x8B7) {
        case 0:
            if (++w->x57 >= 41) {
                em->x8B7++;
                w->x57 = 0;
            }
            break;
        case 1:
            w->x57 = 0;
            break;
        }
    } else {
        em->x8B7 = 0;
        if (w->x57 != -1 && ++w->x57 >= 51) {
            w->x57 = -1;
        }
    }
}
#endif

void em02_main(EMW *em) {
    EM02W *w = (EM02W *)em->ex;
    u8 dmg[4];
    int d;

#ifdef VERSION_MHG
    em02_g_tick(em, w);
#endif
    em->x56A = 0;
    EMF(em, s16, 0x572) = 0;
    em_no_floor_ck(em);
    em_no_battle_area_ck(em, 0, 1);
#ifdef VERSION_MHG
    if (em->mode == 5) {        /* G: no anger while dying */
        em->x8B6 = 0;
    } else
#endif
    if (em->x302 <= (s16)(0.65f * (f32)em->x792)) {
        if (em->x8B6 == 0) {
            em_ikari_add(em, em->x8B0);
        } else {
            em->x8B6 = 1;
            em->x8B4 = 0x708;
        }
    }
#ifdef VERSION_MHG
    /* G (game 0x5AA460): kind 0x24 uses its second hit-zone table between 20% and 50% of its hit points */
    if (em->kind == 0x24) {
        if (em->x302 <= (s16)(0.2f * (f32)em->x792)) {
            em->meat_no = 0;
        } else if (em->x302 <= (s16)(0.5f * (f32)em->x792)) {
            em->meat_no = 1;
        } else {
            em->meat_no = 0;
        }
    }
#endif
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
#ifdef VERSION_MHG
        if (em->kind == 0x24) {     /* G: the crimson dragon's carves by broken parts (Quest_enemy_hagi_set bits) */
            if (EMF(em, u8, 0x332) >= 3) {
                Quest_enemy_hagi_set(em, 0x200);
            }
            if (EMF(em, u8, 0x332) > 0) {
                Quest_enemy_hagi_set(em, 0x800);
            }
            if (EMF(em, u8, 0x332) >= 2) {
                Quest_enemy_hagi_set(em, 0x400);
            }
            if (EMF(em, u8, 0x342) >= 10) {
                Quest_enemy_hagi_set(em, 0x1000);
            }
            if (EMF(em, u8, 0x30A) > 0) {
                Quest_enemy_hagi_set(em, 0x2000);
            }
        }
#endif
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
#ifdef VERSION_MHG
    /* G (em02_main 0x5AA8A8): the black dragon (kind 2) on stage 0x19 in any quest (G dropped MH1's quest list), the
     * other dragons of this program (kinds 36 ...) on stage 0x58 with event flag 0x24: G's demo 3 (em_demo03_g) */
    if (em->kind != 2) {
        if (em->stg == 0x58 && Event_flag_ck(0x24) == 0) {
            if (game_w.info_stop == 1 && em->mode != 6) {
                em02_act_set(em, 6, 3, 0);
            }
        } else if (em->x839 != 0) {
            em_cmd_ck(em);
            em->x839 = 0;
        }
        goto sub;
    }
    switch (em->kind) {
    case 2:
#else
    switch (quest_w.no) {
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0xB9:
    case 0xCF:
#endif
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
#ifdef VERSION_MHG
sub:
#endif
    em02_main_sub(em, w);
    if (em->x6FF != 0) {
        em02_main_sub(em, w);
        em->x6FF = 0;
    }
}

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

static void em_act00_0057FA50(EMW *em, EM02W *w) {
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

static void em_act01_0057FB30(EMW *em, EM02W *w) {
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

static void em_act02_0057FBC0(EMW *em, EM02W *w) {
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

static void em_act03_0057FC50(EMW *em, EM02W *w) {
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

static void em_act04_0057FCD0(EMW *em, EM02W *w) {
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

static void em_act05_0057FDF0(EMW *em, EM02W *w) {
#ifdef VERSION_MHG
    /* G (0x5AADF0): one waking motion (8) and the sleep-2 end, then back to normal */
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 8, 0, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    return;
#endif
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

static void em_act06_0057FEC0(EMW *em, EM02W *w) {
#ifdef VERSION_MHG
    /* G (0x5AAE80): as act 5, ending the paralysis */
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 8, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    return;
#endif
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

static void em_mv00_0057FF80(EMW *em, EM02W *w) {
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

static void em_mv01_005800C0(EMW *em, EM02W *w) {
    u32 spd;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        d = (w->dang - em->ang[1]) & 0xFFFF;
        if (d <= 0xE38U || d >= 0xF1C8U) {
            pl_flag_set((PLW *)em, 0x20000);
            em_char_set(em, 2, 0, 0);
        } else if (d >= 0x8000U) {
            em_char_set(em, 3, 0, 0);
        } else {
            em_char_set(em, 4, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            spd = (u32)(372.0f * em->act_spd);
            d = (w->dang - (u16)em->ang[1]) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32)((d + spd) & 0xFFFF) < spd * 2) {
                    em->x05++;
                    pl_flag_clr((PLW *)em, 0x20000);
                    em02_to_normal(em);
                } else if (d <= 0xE38U || d >= 0xF1C8U) {
                    pl_flag_set((PLW *)em, 0x20000);
                    em_char_set(em, 2, 0, 0);
                } else {
                    pl_flag_clr((PLW *)em, 0x20000);
                    if (d >= 0x8000U) {
                        em_char_set(em, 3, 0, 0);
                    } else {
                        em_char_set(em, 4, 0, 0);
                    }
                }
                break;
            }
            if (em_frame_check2(em, 0, 89.0f) == 0) {
                if ((u32)((d + spd) & 0xFFFF) < spd * 2) {
                    em->ang[1] = (u16)w->dang;
                } else if (d < 0x8000U) {
                    em->ang[1] = (em->ang[1] + spd) & 0xFFFF;
                } else {
                    em->ang[1] = (em->ang[1] - spd) & 0xFFFF;
                }
            }
        }
        break;
    }
}

static void em_fly00_005803B0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 9, 0, 0);
        em02_fly_adjy2_init(em, 0);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 2:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
}

static void em_fly01_005804B0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em->ang[0] = 0;
        em->ang[2] = 0;
        em_char_set(em, 0xB, 0, 0);
        em_rate_clear(em);
        em->adj_y = -10.0f;
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (!(700.0f + em->x5AC <= em->pos[1])) {
            em->x05++;
            em->adj_y = -20.0f;
        }
        break;
    case 2:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0xC, 0, 0);
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->x388 = 0;
            Em_set_quake_sub(em, 1);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly02_00580680(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        if (em->char0 != 0x3F2) {
            em_char_set2(em, 0x3F2, 0, 0, 0);
        }
        if (em->x2DE != 0x4BA) {
            em_char_set2(em, 0x4BA, 0, 0, 1);
        }
        if (em->x2E0 != 0x582) {
            em_char_set2(em, 0x582, 0, 0, 2);
        }
        w->x18 = 0;
        SetVector(w->tp, em->pos[0], em->pos[1], em->pos[2]);
        break;
    case 1:
        em02_fly_adjy(em, 1);
        if (!(em->pos[1] < w->tp[1])) {
            em->pos[1] = w->tp[1];
        }
        if (--em->work08 <= 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        if (!(em->pos[1] < w->tp[1])) {
            em->pos[1] = w->tp[1];
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly03_00580800(EMW *em, EM02W *w) {
    int t;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        em->work08 = 0x12C;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        t = em02_senkai_target(em) & 0xFF;
        if (--em->work08 <= 0 || t != 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly04_00580900(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly05_00580A50(EMW *em, EM02W *w) {
    f32 v[4]; /* never filled: the original passes this uninitialized local (Capcom bug) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 10.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (--em->work08 <= 0 || !(em->pos[1] <= 1500.0f + em->tgt_pos[1])) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly06_00580B80(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        w->spd[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly07_00580CD0(EMW *em, EM02W *w) {
    f32 v[4]; /* never filled (see fly05) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = -10.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (--em->work08 <= 0 || em->pos[1] <= 500.0f + em->x5AC) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly08_00580E00(EMW *em, EM02W *w) {
    f32 v[4]; /* never filled (see fly05) */

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        em_rate_clear(em);
        em->adj_y = 10.0f;
        break;
    case 1:
        w->spd[1] = Em_Calc_angY(em->pos, v) & 0xFFFF;
        speed_add(em, w->spd);
        if (!(em->pos[1] < 6000.0f)) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly09_00580F10(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

static void em_fly10_00581040(EMW *em, EM02W *w) {
    f32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xA, 0, 0);
        w->x18 = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em02_fly_adjy(em, 1);
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        d = CalcDistanceXZ(em->pos, em->tgt_pos);
        if (em->x74C & 0xF000000F) {
            em->pos[1] += 20.0f;
        }
        if (--em->work08 <= 0 || d <= 100.0f * em->adj_z) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    case 2:
        em02_fly_adjy(em, 1);
        break;
    }
    FLY_FLOOR(em);
}

#ifdef VERSION_MHG
/* G 0x5AC150 (G only), fly act 11: take off (fly_adjy2 type 6), climb at 10 per tick and leave; above 6000 it flies
 * on (em02_to_fly). G aims the climb at a stack vector it never fills (Em_Calc_angY(pos, <uninitialised>)) but the
 * speed it adds there has no forward part (em_rate_clear), so the heading does not move it: the PC keeps its own
 * heading. [verified: referee g_fatalis_acts, forced act 2/11: same steps, x/z unchanged, y 1127 -> 7046 on both] */
static void em_fly11_g(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 9, 0, 0);
        em02_fly_adjy2_init(em, 6);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 2:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em_rate_clear(em);
            em->adj_y = 10.0f;
        }
        break;
    case 3:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (!(em->pos[1] < 6000.0f)) {
            em->x05++;
            em_rate_clear(em);
            em02_to_fly(em, 0);
        }
        break;
    }
}
#endif

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

ATK_SIMPLE(em_atk00_005811C0, 0x12)
ATK_SIMPLE(em_atk01_00581240, 0x13)

static void em_atk02_005812C0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xD, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 102.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_atk03_005813A0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xE, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 98.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

ATK_SIMPLE(em_atk04_00581480, 0xF)

#ifdef VERSION_MHG
/* G's fire rain: shell08 type 12 shots (fall from above the hunter at one of 24 offsets, shell08_type12_ofs_g) at
 * frames 80 / 140 / 200 / 260 / 320 of motion 0xF; the offset index comes from a list per variant (x06 counts the
 * shots). The lists: em02_rain_ofs_g = main 0x2ECFC8 (variants 1-4, 8 bytes each; G's table of pointers to them is
 * game 0x695ECC, entry 0 = none), em02_rain2_ofs_g = game 0x695EC0 (act 18, ten shots over two loops). */
extern s8 em02_rain_ofs_g[4][8];
extern s8 em02_rain2_ofs_g[12];

static int em02_rain_frame_g(EMW *em) {
    return em_frame_check(em, 80.0f, 0) || em_frame_check(em, 140.0f, 0) || em_frame_check(em, 200.0f, 0) ||
           em_frame_check(em, 260.0f, 0) || em_frame_check(em, 320.0f, 0);
}

/* G 0x5AC580 (G only; MH1's atk04 is its variant 0): attack acts 4 (n 0, no shots) and 14-17 (n 1-4) */
static void em_atk04_g(EMW *em, EM02W *w, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        if ((u8)n != 0 && em02_rain_frame_g(em)) {
            Shell08_set_ang(em, 0, 0xC, em02_rain_ofs_g[(u8)n - 1][em->x06], 0x4000, 0);
            em->x06++;
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

/* G 0x5AD470 (G only): attack act 18, motion 0xF twice with five shots each */
static void em_atk18_g(EMW *em, EM02W *w, int n) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
    case 2:
        if ((u8)n != 0 && em02_rain_frame_g(em)) {
            Shell08_set_ang(em, 0, 0xC, em02_rain2_ofs_g[em->x06], 0x4000, 0);
            em->x06++;
        }
        if (em->x194 == 0) {
            em->x05++;
            if (em->x05 == 2) {
                em_char_set(em, 0xF, 0, 0);
            } else {
                em02_to_normal(em);
            }
        }
        break;
    }
}

/* G 0x5ACFB0 (G only): attack act 12, rise (motion 0x23, then 0x28 with fly_adjy2 type 4) and glide forward until
 * the distance (w->dist, em02_atk_act_set) is used up, land (0x24), recover (8) */
static void em_atk12_g(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x23, 0, 0);
        em->x388 = 0;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 2;
            em_char_set(em, 0x28, 0, 0);
            em02_fly_adjy2_init(em, 4);
        }
        break;
    case 2:
        em02_fly_adjy2(em);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    FLY_FLOOR(em);
}

/* G 0x5AD140 (G only): attack act 13, a climbing turn (0x25, fly_adjy2 type 5, pitch +0x1A8 per tick), the dive
 * (0x26: speed 80 down, shell04 0x1E) until 850 above the floor, the pull-out (0x27: pitch back after frame 15,
 * pushed out of the floor), the landing (0x24), recover (8) */
static void em_atk13_g(EMW *em, EM02W *w) {
    FLMAT m;
    f32 v[3];
    f32 o[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em_char_set(em, 0x25, 0, 0);
        em02_fly_adjy2_init(em, 5);
        em->ang[0] = 0;
        break;
    case 1:
        em->ang[0] += 0x1A8;
        em02_fly_adjy2(em);
        w->dist -= em->adj_z;
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x26, 0, 0);
            em->x388 = 2;
            em->rate_x = 0.0f;
            em->adj_y = 0.0f;
            em->adj_z = 80.0f;
            em->x3C0[0] = 0.0f;
            em->x3C0[1] = 0.0f;
            em->x3C0[2] = 9.0f;
            shell04_set(em, 0x1E);
        }
        break;
    case 2:
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        w->dist -= em->adj_z;
        if (em->pos[1] <= em->x5AC + 850.0f) {
            em->x05++;
            em_char_set(em, 0x27, 0, 0);
            em->x3C0[2] = -4.0f;
        }
        break;
    case 3:
        if (em_frame_check2(em, 0, 15.0f) == 0) {
            em->ang[0] -= 0x6A2;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        w->dist -= em->adj_z;
        w->spd[0] = 0;
        if (em->pos[1] < em->x5AC) {
            v[0] = (em->x5AC - em->pos[1]) / 3.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            cpRotMatrixYXZ2(w->spd, &m);
            flvecApplyMat33(o, v, (f32 *)&m);
            em->pos[0] += o[0];
            em->pos[1] += o[1];
            em->pos[2] += o[2];
        }
        em->pos[1] -= 50.0f;
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x24, 0, 0);
            em->ang[0] = 0;
            em->x388 = 0;
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 5:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    FLY_FLOOR(em);
}
#endif

/* Shared by actions 5, 10 and 11: mode 0/1/2 picks the shell angle (move03 passes it as a third argument). */
static void em_atk05_00581500(EMW *em, EM02W *w, int mode) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x1F, 0, 0);
        em->work08 = 0x5A;
        break;
    case 1:
        em02_senkai_player(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        em02_senkai_player(em);
        if (em_frame_check(em, 30.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            switch ((u8)mode) {
            case 0:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
                break;
            case 1:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x2AAB, 0);
                break;
            case 2:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x1E94, 0);
                break;
            }
            w->x1A = w->x1A - 1;
        }
        if (w->x1A <= 0 && em_frame_check(em, 76.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xA, 0xA, 0x1E);
        }
        if (em->x194 == 0) {
            em_char_set(em, 0x10, 4, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_atk06_00581770(EMW *em, EM02W *w) {
    f32 v[4];

    em->x8BB = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 5, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 7, 0, 0);
            shell04_set(em, 2);
        }
        break;
    case 2:
        if (w->has_tgt != 0) {
            EM02_TURN(em);
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_atk07_00581930(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x21, 0, 0);
        em02_fly_adjy2_init(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 70.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 2:
        if (em_frame_check(em, 94.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
        }
        em02_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
}

static void em_atk08_00581A90(EMW *em, EM02W *w) {
    em->x8BB = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 5, 0, 0);
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

static void em_atk09_00581B50(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x1F, 0, 0);
        em->work08 = 0x5A;
        break;
    case 1:
        em02_senkai_player(em);
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        em02_senkai_player(em);
        if (em_frame_check(em, 30.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            w->x1A = w->x1A - 1;
            switch (w->x1A) {
            case 2:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
                break;
            case 1:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x2AAB, 0);
                break;
            case 0:
                Shell08_set_ang(em, 0x33, 0xB, 0, 0x1E94, 0);
                break;
            }
        }
        if (w->x1A <= 0 && em_frame_check(em, 76.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xA, 0xA, 0x1E);
        }
        if (em->x194 == 0) {
            em_char_set(em, 0x10, 4, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_fly(em, 0);
        }
        break;
    }
    FLY_FLOOR(em);
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

DMG_SIMPLE(em_dmg00_00581DC0, 0x14)
DMG_SIMPLE(em_dmg01_00581E50, 0x15)

static void em_dmg02_00581EE0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 50.0f) == 0) {
            em->ang[1] += 0x28E;
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_dmg03_00581FA0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        em_cmd_reset(em);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (180.0f + em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x1A, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
#ifdef VERSION_MHG
            em_char_set(em, 8, 4, 0);       /* G (0x5AD8C0): blended in over 4 frames */
#else
            em_char_set(em, 8, 0, 0);
#endif
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_dmg04_00582100(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1B, 0, 0);
        em->work08 = 0x78;
        em_cmd_reset(em);
        game_w.flag1B3 |= 2;
        em->x762 = 3;
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 0x1C, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x762 = 0;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_dmg05_00582220(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x22, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 296.0f, 0)) {
            em->x05++;
            em_char_set(em, 0x1B, 0xC, 0x124);
        }
        break;
    case 2:
        em_mahi_eff_set(em, 2);
        if (--em->work08 <= 0) {
            em->x05++;
            em02_act_set(em, 0, 6, 4);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em02_act_set(em, 0, 6, 4);
        }
        break;
    }
}

static void em_dmg06_00582350(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x17, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check2(em, 0, 50.0f) == 0) {
            em->ang[1] -= 0x28E;
        }
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_dmg07_00582410(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x14, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
}

static void em_dmg08_005824A0(EMW *em, EM02W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        em_cmd_reset(em);
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (180.0f + em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x1A, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_rate_clear(em);
            em02_to_normal(em);
        }
        break;
    }
}

static void em_die00_00582600(EMW *em, EM02W *w) {
    em->x40E = 5;
    em->x888 = 0;
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x18, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
            em02_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        em02_hagi_move(em);
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em02_hagi_clr(em);
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em02_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die01_00582760(EMW *em, EM02W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x1D, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        game_w.flag1B3 |= 2;
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
            em02_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        em02_hagi_move(em);
        break;
    case 3:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em02_hagi_clr(em);
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em02_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die02_005828E0(EMW *em, EM02W *w) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x1A, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
#ifdef VERSION_MHG
        if (em_frame_check(em, 100.0f, 0)) {        /* G (0x5AE200): the body is down at frame 100 */
#else
        if (em_frame_check(em, 134.0f, 0)) {
#endif
            em->x05++;
            em->act_spd = 0.0f;
            em->work08 = 0;
            em->x388 = 3;
            Quest_enemy_die(em);
            em02_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        em->act_spd = 0.0f;
        em02_hagi_move(em);
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05++;
        }
        break;
    case 4:
        em02_hagi_move(em);
        break;
    case 5:
        if (--em->work08 <= 0) {
            em->x04++;
            em->x01 = 0;
            em02_hagi_clr(em);
        } else {
            em->x798 = (f32)em->work08 / 150.0f;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em02_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_move00_00582B40(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_act00_0057FA50(em, w); break;
    case 1: em_act01_0057FB30(em, w); break;
    case 2: em_act02_0057FBC0(em, w); break;
    case 3: em_act03_0057FC50(em, w); break;
    case 4: em_act04_0057FCD0(em, w); break;
    case 5: em_act05_0057FDF0(em, w); break;
    case 6: em_act06_0057FEC0(em, w); break;
    }
}

static void em_move01_00582BF0(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_mv00_0057FF80(em, w); break;
    case 1: em_mv01_005800C0(em, w); break;
    }
}

static void em_move02_00582C40(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_fly00_005803B0(em, w); break;
    case 1: em_fly01_005804B0(em, w); break;
    case 2: em_fly02_00580680(em, w); break;
    case 3: em_fly03_00580800(em, w); break;
    case 4: em_fly04_00580900(em, w); break;
    case 5: em_fly05_00580A50(em, w); break;
    case 6: em_fly06_00580B80(em, w); break;
    case 7: em_fly07_00580CD0(em, w); break;
    case 8: em_fly08_00580E00(em, w); break;
    case 9: em_fly09_00580F10(em, w); break;
    case 10: em_fly10_00581040(em, w); break;
#ifdef VERSION_MHG
    case 11: em_fly11_g(em, w); break;
#endif
    }
}

static void em_move03_00582D30(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_atk00_005811C0(em, w); break;
    case 1: em_atk01_00581240(em, w); break;
    case 2: em_atk02_005812C0(em, w); break;
    case 3: em_atk03_005813A0(em, w); break;
#ifdef VERSION_MHG
    case 4: em_atk04_g(em, w, 0); break;
#else
    case 4: em_atk04_00581480(em, w); break;
#endif
    case 5: em_atk05_00581500(em, w, 0); break;
    case 6: em_atk06_00581770(em, w); break;
    case 7: em_atk07_00581930(em, w); break;
    case 8: em_atk08_00581A90(em, w); break;
    case 9: em_atk09_00581B50(em, w); break;
    case 10: em_atk05_00581500(em, w, 1); break;
    case 11: em_atk05_00581500(em, w, 2); break;
#ifdef VERSION_MHG
    case 12: em_atk12_g(em, w); break;
    case 13: em_atk13_g(em, w); break;
    case 14: em_atk04_g(em, w, 1); break;
    case 15: em_atk04_g(em, w, 2); break;
    case 16: em_atk04_g(em, w, 3); break;
    case 17: em_atk04_g(em, w, 4); break;
    case 18: em_atk18_g(em, w, 4); break;
#endif
    }
}

static void em_move04_00582E30(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_dmg00_00581DC0(em, w); break;
    case 1: em_dmg01_00581E50(em, w); break;
    case 2: em_dmg02_00581EE0(em, w); break;
    case 3: em_dmg03_00581FA0(em, w); break;
    case 4: em_dmg04_00582100(em, w); break;
    case 5: em_dmg05_00582220(em, w); break;
    case 6: em_dmg06_00582350(em, w); break;
    case 7: em_dmg07_00582410(em, w); break;
    case 8: em_dmg08_005824A0(em, w); break;
    }
}

static void em_move05_00582F00(EMW *em, EM02W *w) {
    switch (em->x15) {
    case 0: em_die00_00582600(em, w); break;
    case 1: em_die01_00582760(em, w); break;
    case 2: em_die02_005828E0(em, w); break;
    }
}

static void em_demo00_00582F70(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x14, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x20, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 9, 0, 0);
            em02_fly_adjy2_init(em, 0);
        }
        break;
    case 3:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 4:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em_char_set(em, 0xA, 0, 0);
            w->x18 = 0;
        }
        break;
    case 5:
        em02_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 10.0f;
        if (!(em->pos[1] < 7500.0f)) {
            em->x05++;
        }
        break;
    default:
        break;
    }
}

static void em_demo01_00583120(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->pos[0] = 17300.0f;
        em->pos[1] = 0.0f;
        em->pos[2] = 31370.0f;
        em->tgt_pos[0] = 11980.0f;
        em->tgt_pos[1] = 5860.0f;
        em->tgt_pos[2] = 15200.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->ang[2] = 0;
        em->tgt_pos[0] = 17300.0f;
        em->tgt_pos[1] = 5860.0f;
        em->tgt_pos[2] = 31370.0f;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x96;
        em_rate_clear(em);
        break;
    case 1:
        if (--em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 9, 0, 0);
            em02_fly_adjy2_init(em, 0);
            w->x18 = 0;
        }
        break;
    case 2:
        if (em_frame_check(em, 134.0f, 0)) {
            em->x05++;
            em->x388 = 2;
            em02_fly_adjy2(em);
        }
        break;
    case 3:
        if (em02_fly_adjy2(em)) {
            em->x05++;
            em_char_set(em, 0xA, 0, 0);
            w->spd[0] = 0;
            w->spd[1] = em->ang[1];
            w->spd[2] = 0;
        }
        break;
    case 4:
        em02_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 20.0f;
        if (!(em->pos[1] < em->tgt_pos[1])) {
            em->x05++;
            w->turn = 0x100;
            em->tgt_pos[0] = 11980.0f;
            em->tgt_pos[1] = 5860.0f;
            em->tgt_pos[2] = 15200.0f;
            em_rate_clear(em);
            em->adj_z = 50.0f;
        }
        break;
    case 5:
        em02_senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 10.0f * em->adj_z) {
            em->x05++;
            em_rate_clear(em);
            em->adj_y = -10.0f;
        }
        break;
    case 6:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (!(700.0f + em->x5AC <= em->pos[1])) {
            em->x05++;
            em->adj_y = -20.0f;
        }
        break;
    case 7:
        w->spd[1] = em->ang[1];
        speed_add(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0xC, 0, 0);
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->x388 = 0;
            Em_set_quake_sub(em, 1);
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x20, 0, 0);
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xD, 0, 0);
        }
        break;
    case 11:
        if (em_frame_check(em, 102.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 12:
        if (Event_flag_ck(0x1B) == 1) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    default:
        break;
    }
}

static void em_demo02_00583610(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x1F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x10, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 30.0f, 0)) {
            Eft17_set(em, 0x33, 1, 1);
            Eft17_set(em, 0x33, 2, 1);
            Shell08_set_ang(em, 0x33, 0xB, 0, 0x38E4, 0);
        }
        if (em_frame_check(em, 76.0f, 0)) {
            em->x05++;
            em_char_set(em, 0xA, 0xA, 0x1E);
        }
        break;
    case 3:
        em02_fly_adjy(em, 1);
        em->pos[1] = em->pos[1] + 10.0f;
        if (!(em->pos[1] < 7500.0f)) {
            em->x05++;
        }
        break;
    }
}

#ifdef VERSION_MHG
/* G 0x5AF150 (G only): demo 3, the other dragons' entrance on stage 0x58 (event quest 60029, the GALLERY's scene 13):
 * the dragon is put far off, flies in to (9896, 15107) and on to (12021, 7469), lands and waits for event flag 0x24.
 * From G's asm (jump table 0x6D8410 in code order); agent E, 21 Oct 2026, not byte-checked. */
static void em_demo_turn_g(EMW *em)
{
    em->ang[0] = 0;
    em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
    em->ang[2] = 0;
}

static void em_demo03_g(EMW *em, EM02W *w) {
    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->pos[0] = 15779.0f;
        em->pos[1] = 571.0f;
        em->pos[2] = 3495.0f;
        em->tgt_pos[0] = 9896.0f;
        em->tgt_pos[1] = 46.0f;
        em->tgt_pos[2] = 15107.0f;
        em_demo_turn_g(em);
        em_char_set(em, 0x1D, 0, 0x27E);
        em->work08 = 0x5A;
        break;
    case 1:
        if (--em->work08 > 0) {
            break;
        }
        em->x05++;
        em_char_set(em, 0x1C, 0, 0);
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0x12, 0);
            em->work08 = 0x1E;
        }
        break;
    case 4:
        if (--em->work08 > 0) {
            break;
        }
        em->x05++;
        em_char_set(em, 0x1E, 0, 0);
        break;
    case 5:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
            em->work08 = 0xB4;
        }
        break;
    case 6:
        if (--em->work08 > 0) {
            break;
        }
        em->x05++;
        em_char_set(em, 0x20, 0, 0);
        em->x8B6 = 1;
        em->x8B4 = 0x708;
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x23, 0, 0);
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 2;
            em_char_set(em, 0x28, 0, 0);
            em02_fly_adjy2_init(em, 4);
            em->x8B6 = 0;
            em->x8B4 = 0;
        }
        break;
    case 9:
        em02_fly_adjy2(em);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) {
            em->x05++;
            em->pos[0] = 12021.0f;
            em->pos[1] = 100.0f;
            em->pos[2] = 7469.0f;
            em_demo_turn_g(em);
        }
        break;
    case 10:
        em02_fly_adjy2(em);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 12:
        if (Event_flag_ck(0x24) == 1) {
            em->x05++;
            em02_to_normal(em);
        }
        break;
    }
    (void)w;
}
#endif

static void em_move06_005837B0(EMW *em, EM02W *w) {
    em->act_spd = 1.0f;
    switch (em->x15) {
    case 0: em_demo00_00582F70(em, w); break;
    case 1: em_demo01_00583120(em, w); break;
    case 2: em_demo02_00583610(em, w); break;
#ifdef VERSION_MHG
    case 3: em_demo03_g(em, w); break;
#endif
    }
}

void em02_main_sub(EMW *em, EM02W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0: em_move00_00582B40(em, w); break;
    case 1: em_move01_00582BF0(em, w); break;
    case 2: em_move02_00582C40(em, w); break;
    case 3: em_move03_00582D30(em, w); break;
    case 4: em_move04_00582E30(em, w); break;
    case 5: em_move05_00582F00(em, w); break;
    case 6: em_move06_005837B0(em, w); break;
    case 7: em_move06_005837B0(em, w); break;
    }
    if (em->pos[0] <= 0.0f || em->pos[2] <= 0.0f) {
        em_dur_set(em, 0);
    }
}

#define UV_RESET(i) \
    do { \
        uv[i][0] = 0.0f; \
        uv[i][1] = 0.0f; \
        tm[i] = 0xFFFF; \
        ty[i] = 0xFF; \
    } while (0)

#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

void em02_uvmove(EMW *em) {
    int i;

    for (i = 0; i < 4; i++) {
        if (em->uvtm[i] != 0xFFFF) {
            em->uvtm[i] += 2;
        }
        switch (em->uvty[i]) {
        case 0:
            UVR(i);
            break;
        case 1:
            if (em->uvtm[i] >= 0x3E) {
                UVR(i);
            } else {
                int k = ((u32)em->uvtm[i] >> 1) + 1;

                em->uv[i][0] = 0.125f * (f32)(k % 8);
                em->uv[i][1] = 0.25f * (f32)(k / 8 % 4);
            }
            break;
        case 2:
            em->uv[i][0] = 0.125f;
            em->uv[i][1] = 0.0f;
            em->uvtm[i] = 0xFFFF;
            em->uvty[i] = 0xFF;
            break;
        case 3:
            if (em->uvtm[i] >= 0xC) {
                UVR(i);
            } else {
                int k = ((u32)em->uvtm[i] >> 1) + 2;

                em->uv[i][0] = 0.125f * (f32)(k % 4);
                em->uv[i][1] = 0.25f * (f32)(k / 4 % 4);
            }
            break;
        case 0xFF:
            break;
        }
    }
}

static void em_uvset(EMW *em, u32 frame, u16 idx, s8 type) {
    if (em->x19C == (f32)frame) {
        if (em->x1C4 == 0) {
            *(u8 *)((u16)idx + (u32)em + 0x5F8) = type;
            *(u16 *)(((u16)idx << 1) + (u32)em + 0x5F0) = 0;
        }
    }
}

static void move_default_00583BA0(EMW *em) {
    f32 (*uv)[3] = (f32 (*)[3])((u8 *)em + 0x5C0);
    u16 *tm = (u16 *)((u8 *)em + 0x5F0);
    u8 *ty = (u8 *)em + 0x5F8;

    uv[0][0] = 0.0f;
    uv[0][1] = 0.0f;
    tm[0] = 0xFFFF;
    ty[0] = 0xFF;
    uv[1][0] = 0.0f;
    uv[1][1] = 0.0f;
    tm[1] = 0xFFFF;
    ty[1] = 0xFF;
    uv[2][0] = 0.0f;
    uv[2][1] = 0.0f;
    tm[2] = 0xFFFF;
    ty[2] = 0xFF;
    uv[3][0] = 0.0f;
    uv[3][1] = 0.0f;
    tm[3] = 0xFFFF;
    ty[3] = 0xFF;
}

/* Sound and effect script per animation (sound_call(em, frame, se, joint) plays a sound at the joint once the
 * animation reaches the frame). Generated from the asm with tools/genef.py. */
static void ef_move_sub_00583BF0(EMW *em, EM02W *w) {

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005872F0(em, 6, 0x33, 0x34);
        sound_call_005872F0(em, 0x118, 0x54, 0x34);
        sound_call_005872F0(em, 0x30, 9, 0x22);
        sound_call_005872F0(em, 0x34, 9, 0x28);
        em_uvset(em, 0, 0, 0);
        em_uvset(em, 0, 1, 0);
        em_uvset(em, 0x75, 0, 1);
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 28.0f, 80.0f) || em_frame_check3(em, 0, 278.0f, 490.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3EA:
        sound_call_005872F0(em, 0x14, 0x2a, 0x34);
        sound_call_005872F0(em, 0x10, 1, 9);
        sound_call_005872F0(em, 0x56, 1, 5);
        sound_call_005872F0(em, 0xc, 0xe, 0xb);
        sound_call_005872F0(em, 0x52, 0xe, 0xb);
        em_uvset(em, 0, 0, 0);
        em_uvset(em, 0, 1, 0);
        em_uvset(em, 0x15, 1, 2);
        em_uvset(em, 0x19, 0, 1);
        em_uvset(em, 0x3d, 1, 3);
        quake_call_00587230(em, 0xe, 2);
        quake_call_00587230(em, 0x54, 2);
        if (em_frame_check(em, 2.0f, 0)) {
            shell04_set(em, 5);
        }
        if (em_frame_check(em, 54.0f, 0)) {
            shell04_set(em, 6);
        }
        if (em_frame_check(em, 130.0f, 0)) {
            shell04_set(em, 7);
        }
        if (em_frame_check(em, 116.0f, 0)) {
            Eft13_set_em_scl(em, 0xa, 5.0f, 0x14);
        }
        if (em_frame_check(em, 46.0f, 0)) {
            Eft13_set_em_scl(em, 6, 5.0f, 0x14);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 34.0f, 52.0f) || em_frame_check3(em, 0, 108.0f, 130.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3EB:
        sound_call_005872F0(em, 6, 0x1e, 0x34);
        sound_call_005872F0(em, 6, 0x16, 0);
        sound_call_005872F0(em, 0x60, 0x2a, 0x34);
        sound_call_005872F0(em, 0x1c, 1, 5);
        sound_call_005872F0(em, 0x5e, 1, 9);
        sound_call_005872F0(em, 0x3c, 0x1a, 0x11);
        sound_call_005872F0(em, 0x6c, 0x19, 0x11);
        em_uvset(em, 0, 0, 0);
        em_uvset(em, 0, 1, 2);
        em_uvset(em, 0x25, 1, 3);
        em_uvset(em, 0x31, 0, 1);
        em_uvset(em, 0x5a, 1, 2);
        em_uvset(em, 0x6d, 1, 3);
        em_uvset(em, 0x7f, 0, 1);
        em_uvset(em, 0x96, 1, 2);
        em_uvset(em, 0xb9, 1, 3);
        em_uvset(em, 0xf1, 0, 1);
        em_uvset(em, 0x104, 1, 2);
        em_uvset(em, 0x12f, 1, 3);
        em_uvset(em, 0x16e, 0, 1);
        em_uvset(em, 0x17c, 1, 2);
        em_uvset(em, 0x18b, 1, 3);
        quake_call_00587230(em, 0x1a, 2);
        quake_call_00587230(em, 0x60, 2);
        if (em_frame_check(em, 4.0f, 0)) {
            shell04_set(em, 8);
        }
        if (em_frame_check(em, 56.0f, 0)) {
            shell04_set(em, 9);
        }
#ifdef VERSION_MHG
        if (em_frame_check(em, 60.0f, 0)) {         /* G: frame 60 (MH1 114) */
#else
        if (em_frame_check(em, 114.0f, 0)) {
#endif
            shell04_set(em, 0x17);
        }
        if (em_frame_check(em, 20.0f, 0)) {
            Eft13_set_em_scl(em, 6, 1.2f, 7);
        }
        if (em_frame_check(em, 94.0f, 0)) {
            Eft13_set_em_scl(em, 0xa, 1.2f, 7);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 6.0f, 24.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 0);
                }
            }
            if (em_frame_check3(em, 0, 46.0f, 70.0f) || em_frame_check3(em, 0, 110.0f, 162.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3EC:
        sound_call_005872F0(em, 6, 0x1e, 0x34);
        sound_call_005872F0(em, 6, 0x16, 0);
        sound_call_005872F0(em, 0x60, 0x2a, 0x34);
        sound_call_005872F0(em, 0x1c, 1, 9);
        sound_call_005872F0(em, 0x5e, 1, 5);
        quake_call_00587230(em, 0x20, 2);
        quake_call_00587230(em, 0x5e, 2);
        if (em_frame_check(em, 4.0f, 0)) {
            shell04_set(em, 0xa);
        }
        if (em_frame_check(em, 56.0f, 0)) {
            shell04_set(em, 0xb);
        }
#ifdef VERSION_MHG
        if (em_frame_check(em, 60.0f, 0)) {         /* G: frame 60 (MH1 114) */
#else
        if (em_frame_check(em, 114.0f, 0)) {
#endif
            shell04_set(em, 0x17);
        }
        if (em_frame_check(em, 20.0f, 0)) {
            Eft13_set_em_scl(em, 0xa, 1.2f, 7);
        }
        if (em_frame_check(em, 94.0f, 0)) {
            Eft13_set_em_scl(em, 6, 1.2f, 7);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 6.0f, 24.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 1);
                }
            }
            if (em_frame_check3(em, 0, 46.0f, 70.0f) || em_frame_check3(em, 0, 110.0f, 162.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3ED:
        sound_call_005872F0(em, 0x12, 0x1e, 0x34);
        sound_call_005872F0(em, 0x44, 0x26, 0x34);
        sound_call_005872F0(em, 0x22, 3, 0x1d);
        sound_call_005872F0(em, 0x26, 6, 0x18);
        sound_call_005872F0(em, 0x70, 2, 0x1d);
        sound_call_005872F0(em, 0x1c, 9, 0x22);
        sound_call_005872F0(em, 0x20, 9, 0x28);
        sound_call_005872F0(em, 0x2e, 0x10, 0xb);
        sound_call_005872F0(em, 0x1c, 0x1a, 0xb);
        quake_call_00587230(em, 0x10, 2);
        quake_call_00587230(em, 0x1c, 2);
        quake_call_00587230(em, 0x2c, 2);
        quake_call_00587230(em, 0x70, 5);
        if (em_frame_check(em, 38.0f, 0)) {
            shell04_set(em, 0xc);
        }
        if (em_frame_check(em, 36.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 1.2f, 7);
        }
        if (em_frame_check(em, 38.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 66.0f, 112.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3EF:
        sound_call_005872F0(em, 0xa, 0x23, 0x34);
        sound_call_005872F0(em, 4, 0x11, 0x11);
        sound_call_005872F0(em, 0x24, 4, 9);
        sound_call_005872F0(em, 0x44, 4, 5);
        sound_call_005872F0(em, 0x62, 4, 9);
        sound_call_005872F0(em, 0x86, 4, 5);
        sound_call_005872F0(em, 0x1e, 2, 0x18);
        sound_call_005872F0(em, 0x3e, 2, 0x1d);
        sound_call_005872F0(em, 0x5c, 2, 0x18);
        sound_call_005872F0(em, 0x7e, 2, 0x1d);
        quake_call_00587230(em, 0x1a, 2);
        quake_call_00587230(em, 0x26, 2);
        quake_call_00587230(em, 0x3c, 2);
        quake_call_00587230(em, 0x58, 2);
        quake_call_00587230(em, 0x7e, 2);
        quake_call_00587230(em, 0x84, 2);
        if (em_frame_check(em, 24.0f, 0) || em_frame_check(em, 28.0f, 0) || em_frame_check(em, 86.0f, 0) || em_frame_check(em, 90.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 2.2f, 3);
        }
        if (em_frame_check(em, 36.0f, 0) || em_frame_check(em, 40.0f, 0) || em_frame_check(em, 96.0f, 0) || em_frame_check(em, 100.0f, 0)) {
            Eft13_set_em_scl(em, 0xa, 2.2f, 3);
        }
        if (em_frame_check(em, 58.0f, 0) || em_frame_check(em, 62.0f, 0) || em_frame_check(em, 126.0f, 0) || em_frame_check(em, 130.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 2.2f, 3);
        }
        if (em_frame_check(em, 66.0f, 0) || em_frame_check(em, 70.0f, 0) || em_frame_check(em, 130.0f, 0) || em_frame_check(em, 134.0f, 0)) {
            Eft13_set_em_scl(em, 6, 2.2f, 3);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 12.0f, 40.0f) || em_frame_check3(em, 0, 76.0f, 106.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3F0:
        sound_call_005872F0(em, 0x32, 0x1e, 0x34);
        sound_call_005872F0(em, 4, 0xf, 0x11);
        sound_call_005872F0(em, 4, 0x16, 0);
        sound_call_005872F0(em, 0x66, 4, 5);
        sound_call_005872F0(em, 0x86, 4, 9);
        sound_call_005872F0(em, 0x50, 0xd, 0x22);
        sound_call_005872F0(em, 0x54, 0xd, 0x28);
        quake_call_00587230(em, 0x5a, 2);
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 20.0f, 70.0f) || em_frame_check3(em, 0, 84.0f, 140.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3F1:
        sound_call_005872F0(em, 4, 0x22, 0x34);
        sound_call_005872F0(em, 0x34, 0x2a, 0x34);
        sound_call_005872F0(em, 0x16, 0x11, 0x11);
        sound_call_005872F0(em, 0x8c, 7, 9);
        sound_call_005872F0(em, 0xc, 0xa, 0x22);
        sound_call_005872F0(em, 0x12, 0xa, 0x28);
        sound_call_005872F0(em, 0x4c, 0xa, 0x22);
        sound_call_005872F0(em, 0x50, 0xa, 0x28);
        sound_call_005872F0(em, 0x7c, 0xa, 0x22);
        sound_call_005872F0(em, 0x82, 0xa, 0x28);
        sound_call_005872F0(em, 0xba, 0xa, 0x22);
        sound_call_005872F0(em, 0xbe, 0xa, 0x28);
        sound_call_005872F0(em, 0xf6, 0xa, 0x22);
        sound_call_005872F0(em, 0xfa, 0xa, 0x28);
        sound_call_005872F0(em, 0x138, 0xa, 0x22);
        sound_call_005872F0(em, 0x13c, 0xa, 0x28);
        sound_call_005872F0(em, 0x180, 0xa, 0x22);
        sound_call_005872F0(em, 0x184, 0xa, 0x28);
        if (em_frame_check(em, 196.0f, 0) || em_frame_check(em, 250.0f, 0) || em_frame_check(em, 316.0f, 0)) {
            shell04_set(em, 0x18);
        }
        if (em_frame_check(em, 38.0f, 0) || em_frame_check(em, 90.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em_frame_check(em, 138.0f, 0) || em_frame_check(em, 196.0f, 0) || em_frame_check(em, 256.0f, 0) || em_frame_check(em, 322.0f, 0) || em_frame_check(em, 394.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.2f, em, 1, 0);
            }
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 6.0f, 34.0f) || em_frame_check3(em, 0, 84.0f, 92.0f) || em_frame_check3(em, 0, 124.0f, 130.0f) || em_frame_check3(em, 0, 178.0f, 198.0f) || em_frame_check3(em, 0, 252.0f, 284.0f) || em_frame_check3(em, 0, 312.0f, 328.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3F2:
        sound_call_005872F0(em, 0x1e, 0xa, 0x22);
        sound_call_005872F0(em, 0x22, 0xa, 0x28);
        sound_call_005872F0(em, 0x64, 0xa, 0x22);
        sound_call_005872F0(em, 0x68, 0xa, 0x28);
        sound_call_005872F0(em, 0x2e, 0x18, 0x11);
        if (em_frame_check(em, 32.0f, 0)) {
            shell04_set(em, 0x19);
        }
        if (em_frame_check(em, 40.0f, 0) || em_frame_check(em, 110.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.2f, em, 1, 0);
            }
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 44.0f, 68.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3F3:
        sound_call_005872F0(em, 4, 0x31, 0x34);
        sound_call_005872F0(em, 0x1c, 0xa, 0x22);
        sound_call_005872F0(em, 0x20, 0xa, 0x28);
        sound_call_005872F0(em, 0x5a, 0xc, 0x22);
        sound_call_005872F0(em, 0x5e, 0xc, 0x28);
        sound_call_005872F0(em, 0x8c, 0xc, 0x22);
        sound_call_005872F0(em, 0x90, 0xc, 0x28);
        sound_call_005872F0(em, 0x2e, 0x1a, 0x11);
        if (em_frame_check(em, 122.0f, 0) || em_frame_check(em, 164.0f, 0)) {
            shell04_set(em, 0x1a);
        }
        if (em_frame_check(em, 42.0f, 0) || em_frame_check(em, 102.0f, 0) || em_frame_check(em, 156.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.2f, em, 1, 0);
            }
        }
        break;
    case 0x3F4:
        sound_call_005872F0(em, 0x14, 0x1e, 0x34);
        sound_call_005872F0(em, 0x7a, 0x23, 0x34);
        sound_call_005872F0(em, 0x10, 3, 5);
        sound_call_005872F0(em, 0x14, 5, 9);
        sound_call_005872F0(em, 0x1a, 8, 0);
        sound_call_005872F0(em, 0x2c, 0xa, 0x18);
        sound_call_005872F0(em, 0x2c, 6, 0x1d);
        sound_call_005872F0(em, 0x8e, 2, 0x1d);
        sound_call_005872F0(em, 0x9a, 4, 5);
        sound_call_005872F0(em, 0xb6, 3, 0x18);
        sound_call_005872F0(em, 0x10, 0xd, 0x22);
        sound_call_005872F0(em, 0x14, 0xd, 0x28);
        sound_call_005872F0(em, 0x2e, 0xa, 0x22);
        sound_call_005872F0(em, 0x32, 0xa, 0x28);
        sound_call_005872F0(em, 0x6c, 0xd, 0x22);
        sound_call_005872F0(em, 0x70, 0xd, 0x28);
        quake_call_00587230(em, 0x12, 2);
        quake_call_00587230(em, 0x28, 2);
        quake_call_00587230(em, 0x86, 5);
        if (em_frame_check(em, 26.0f, 0)) {
            shell04_set(em, 0xd);
        }
        if (em_frame_check(em, 40.0f, 0)) {
            shell04_set(em, 0xe);
        }
        if (em_frame_check(em, 42.0f, 0)) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if (em_frame_check(em, 190.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 68.0f, 118.0f) || em_frame_check3(em, 0, 160.0f, 252.0f) || em_frame_check3(em, 0, 298.0f, 330.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        break;
    case 0x3F5:
        sound_call_005872F0(em, 0x14, 0x1f, 0x34);
        sound_call_005872F0(em, 0x56, 0x20, 0x34);
        sound_call_005872F0(em, 0x5e, 0x1b, 0x34);
        sound_call_005872F0(em, 0xb0, 0x21, 0x34);
        sound_call_005872F0(em, 4, 0x17, 0);
        sound_call_005872F0(em, 0x60, 0x1a, 0x11);
        if (em_frame_check3(em, 0, 10.0f, 32.0f) || em_frame_check3(em, 0, 54.0f, 72.0f) || em_frame_check3(em, 0, 158.0f, 212.0f)) {
            if (!(*(u16 *)&game_w.x1E & 0x3)) {
                Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
            }
        }
        if (em_frame_check(em, 80.0f, 0)) {
            shell04_set(em, 0x11);
        }
        break;
    case 0x3F6:
        sound_call_005872F0(em, 4, 0x1f, 0x34);
        sound_call_005872F0(em, 0x58, 0x20, 0x34);
        sound_call_005872F0(em, 0x5e, 0x1b, 0x34);
        sound_call_005872F0(em, 0xb0, 0x21, 0x34);
        sound_call_005872F0(em, 4, 0x17, 0);
        sound_call_005872F0(em, 0x60, 0x1a, 0x11);
        if (em_frame_check3(em, 0, 10.0f, 32.0f) || em_frame_check3(em, 0, 54.0f, 72.0f) || em_frame_check3(em, 0, 158.0f, 212.0f)) {
            if (!(*(u16 *)&game_w.x1E & 0x3)) {
                Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
            }
        }
        if (em_frame_check(em, 80.0f, 0)) {
            shell04_set(em, 0x11);
        }
        break;
    case 0x3F7:
        sound_call_005872F0(em, 4, 0xf, 0xb);
        sound_call_005872F0(em, 0xb4, 0xf, 0xb);
        sound_call_005872F0(em, 0x1e, 0x1c, 0x34);
        sound_call_005872F0(em, 0x52, 0x23, 0x34);
        sound_call_005872F0(em, 0xee, 0x2a, 0x34);
        sound_call_005872F0(em, 0x56, 0x19, 0x33);
        sound_call_005872F0(em, 0x2c, 1, 9);
        sound_call_005872F0(em, 0x5e, 1, 5);
        sound_call_005872F0(em, 0xfa, 1, 5);
        sound_call_005872F0(em, 0x12c, 1, 5);
        quake_call_00587230(em, 0x26, 2);
        quake_call_00587230(em, 0x56, 2);
        quake_call_00587230(em, 0xf4, 2);
        quake_call_00587230(em, 0x124, 2);
        if (em_frame_check(em, 80.0f, 0)) {
            shell04_set(em, 3);
        }
        if (em_frame_check(em, 70.0f, 0)) {
            shell04_set(em, 0x16);
        }
        break;
    case 0x3F8:
        sound_call_005872F0(em, 0x14, 0x20, 0x34);
        sound_call_005872F0(em, 0x14, 0x1b, 0x34);
        sound_call_005872F0(em, 0xc, 0xc, 0x22);
        sound_call_005872F0(em, 0x10, 0xc, 0x28);
        sound_call_005872F0(em, 0x5c, 0xc, 0x22);
        sound_call_005872F0(em, 0x3c6, 0xc, 0x28);
        if (em_frame_check3(em, 0, 98.0f, 130.0f)) {
            if (!(*(u16 *)&game_w.x1E & 0x3)) {
                Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
            }
        }
        if (em_frame_check(em, 2.0f, 0)) {
            shell04_set(em, 0x12);
        }
        if (em_frame_check(em, 34.0f, 0) || em_frame_check(em, 112.0f, 0)) {
            shell04_set(em, 0x14);
        }
        break;
    case 0x3FA:
        sound_call_005872F0(em, 0x20, 0x22, 0x34);
        sound_call_005872F0(em, 0x94, 0x21, 0x34);
        sound_call_005872F0(em, 0x12, 4, 5);
        sound_call_005872F0(em, 0x90, 4, 5);
        sound_call_005872F0(em, 0x2c, 0x1a, 0x1d);
        sound_call_005872F0(em, 0x16, 0xf, 0xb);
        sound_call_005872F0(em, 0x50, 0xf, 0xb);
        quake_call_00587230(em, 0x14, 2);
        quake_call_00587230(em, 0x92, 2);
#ifdef VERSION_MHG
        if (em->kind != 2) {        /* G: the crimson dragon's own hits and dust (kind 0x24) */
            if (em_frame_check(em, 38.0f, 0)) {
                shell04_set(em, 0x1c);
            }
            if (em_frame_check(em, 34.0f, 0)) {
                shell04_set(em, 0x1d);
            }
            if (em_frame_check(em, 46.0f, 0)) {
                Eft20_set(0.3f, em, 0xa, 0x8b);
            }
            if (em_frame_check(em, 52.0f, 0)) {
                Eft20_set(0.3f, em, 0xa, 0x8b);
            }
            if (em_frame_check(em, 58.0f, 0)) {
                Eft20_set(0.3f, em, 0xa, 0x8b);
            }
            break;
        }
#endif
        if (em_frame_check(em, 48.0f, 0)) {
            shell04_set(em, 4);
        }
        break;
    case 0x3FB:
        sound_call_005872F0(em, 4, 0x1c, 0x34);
        sound_call_005872F0(em, 0x32, 0x1d, 0x34);
        sound_call_005872F0(em, 0x6c, 0x1e, 0x34);
        sound_call_005872F0(em, 0x38, 0x1a, 0x33);
        sound_call_005872F0(em, 0xe, 1, 5);
        sound_call_005872F0(em, 0xac, 4, 5);
        sound_call_005872F0(em, 4, 0x11, 0x11);
        sound_call_005872F0(em, 0x3e, 0x11, 0x11);
        sound_call_005872F0(em, 0x8e, 0x11, 0x11);
        quake_call_00587230(em, 0x44, 2);
        quake_call_00587230(em, 0x118, 2);
        if (em_frame_check(em, 60.0f, 0)) {
            shell04_set(em, 1);
        }
        break;
    case 0x3FC:
        sound_call_005872F0(em, 4, 0x4c, 0x34);
        sound_call_005872F0(em, 0x58, 0x29, 0x34);
        sound_call_005872F0(em, 0xaa, 0x26, 0x34);
        sound_call_005872F0(em, 0xa, 0x17, 0);
        sound_call_005872F0(em, 0x10, 1, 9);
        sound_call_005872F0(em, 0xb8, 4, 9);
        sound_call_005872F0(em, 0xce, 4, 5);
        sound_call_005872F0(em, 0x14, 0x11, 0xb);
        sound_call_005872F0(em, 0x86, 0xf, 0xb);
        sound_call_005872F0(em, 0x12, 9, 0x22);
        sound_call_005872F0(em, 0x16, 9, 0x28);
        sound_call_005872F0(em, 0x7a, 9, 0x22);
        sound_call_005872F0(em, 0x7e, 9, 0x28);
        quake_call_00587230(em, 0x12, 2);
        quake_call_00587230(em, 0xbe, 2);
        break;
    case 0x3FD:
        sound_call_005872F0(em, 4, 0x4f, 0x34);
        sound_call_005872F0(em, 0x64, 0x28, 0x34);
        sound_call_005872F0(em, 0x1a, 1, 9);
        sound_call_005872F0(em, 0x36, 1, 5);
        sound_call_005872F0(em, 0xc4, 1, 9);
        sound_call_005872F0(em, 0xe2, 1, 5);
        sound_call_005872F0(em, 0x48, 6, 0x18);
        sound_call_005872F0(em, 0xa, 0x17, 0);
        sound_call_005872F0(em, 0x20, 9, 0x22);
        sound_call_005872F0(em, 0x24, 9, 0x28);
        sound_call_005872F0(em, 0xa0, 0xf, 0xb);
        quake_call_00587230(em, 0x1a, 2);
        quake_call_00587230(em, 0x2e, 2);
        quake_call_00587230(em, 0x42, 2);
        quake_call_00587230(em, 0xc2, 2);
        quake_call_00587230(em, 0x120, 2);
        if (em_frame_check(em, 64.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 1.2f, 7);
        }
        if (em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        break;
    case 0x3FE:
        sound_call_005872F0(em, 4, 0x4f, 0x34);
        sound_call_005872F0(em, 0x78, 0x29, 0x34);
        sound_call_005872F0(em, 0xc2, 0x26, 0x34);
        sound_call_005872F0(em, 0x16, 0x14, 9);
        sound_call_005872F0(em, 0x1e, 3, 5);
        sound_call_005872F0(em, 0x38, 3, 0x18);
        sound_call_005872F0(em, 0x3c, 6, 0x18);
        sound_call_005872F0(em, 0x20, 9, 0x22);
        sound_call_005872F0(em, 0x24, 9, 0x28);
        sound_call_005872F0(em, 0x4a, 0x10, 0xb);
        sound_call_005872F0(em, 0x14, 0xf, 0xb);
        sound_call_005872F0(em, 0x46, 0x18, 0x11);
        sound_call_005872F0(em, 0xa8, 1, 9);
        sound_call_005872F0(em, 0x9c, 0x18, 0x11);
        sound_call_005872F0(em, 0x106, 0, 5);
        quake_call_00587230(em, 0x20, 2);
        quake_call_00587230(em, 0x3a, 2);
        quake_call_00587230(em, 0xaa, 2);
        if (em_frame_check(em, 52.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        if (em_frame_check(em, 58.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 1.2f, 7);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 0xa, 1.2f, 7);
        }
        break;
    case 0x3FF:
        sound_call_005872F0(em, 4, 0x4f, 0x34);
        sound_call_005872F0(em, 0x78, 0x29, 0x34);
        sound_call_005872F0(em, 0xc2, 0x26, 0x34);
        sound_call_005872F0(em, 0x1e, 6, 9);
        sound_call_005872F0(em, 0x22, 0x14, 5);
        sound_call_005872F0(em, 0xa2, 1, 5);
        sound_call_005872F0(em, 0x40, 3, 0x18);
        sound_call_005872F0(em, 0x42, 6, 0x18);
        sound_call_005872F0(em, 0x1a, 9, 0x22);
        sound_call_005872F0(em, 0x1e, 9, 0x28);
        sound_call_005872F0(em, 4, 0x10, 0xb);
        sound_call_005872F0(em, 0x46, 0xf, 0xb);
        sound_call_005872F0(em, 0x1a, 0x1a, 0x11);
        sound_call_005872F0(em, 0x40, 0x18, 0xb);
        sound_call_005872F0(em, 0xf8, 0, 5);
        quake_call_00587230(em, 0x20, 2);
        quake_call_00587230(em, 0x44, 2);
        quake_call_00587230(em, 0xa0, 2);
        if (em_frame_check(em, 52.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 1.2f, 7);
        }
        if (em_frame_check(em, 58.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        if (em_frame_check(em, 162.0f, 0)) {
            Eft13_set_em_scl(em, 6, 1.2f, 7);
        }
        break;
    case 0x400:
        sound_call_005872F0(em, 4, 0x49, 0x34);
        sound_call_005872F0(em, 0x70, 0x35, 0x34);
        sound_call_005872F0(em, 0x196, 0x35, 0x34);
        sound_call_005872F0(em, 0x280, 0x29, 0x34);
        sound_call_005872F0(em, 0x12, 3, 9);
        sound_call_005872F0(em, 0x82, 1, 5);
        sound_call_005872F0(em, 0x112, 0x12, 9);
        sound_call_005872F0(em, 0x136, 6, 0);
        sound_call_005872F0(em, 0x5c, 0xa, 0x22);
        sound_call_005872F0(em, 0x58, 0xa, 0x28);
        sound_call_005872F0(em, 0xea, 0xa, 0x22);
        sound_call_005872F0(em, 0xee, 0xa, 0x28);
        sound_call_005872F0(em, 0x1a2, 0x19, 0);
        sound_call_005872F0(em, 0x198, 0x16, 0);
        sound_call_005872F0(em, 0x58, 0x1a, 0x11);
        sound_call_005872F0(em, 0xd8, 0x11, 0xb);
        sound_call_005872F0(em, 0xe6, 0x1a, 0x11);
        sound_call_005872F0(em, 0x254, 0x11, 0xb);
        quake_call_00587230(em, 0x14, 2);
        quake_call_00587230(em, 0x82, 2);
        quake_call_00587230(em, 0x11c, 2);
        quake_call_00587230(em, 0x140, 2);
        quake_call_00587230(em, 0x15e, 2);
        quake_call_00587230(em, 0x238, 2);
        quake_call_00587230(em, 0x2ac, 2);
        if (em_frame_check(em, 314.0f, 0)) {
            Eft20_set(1.2f, em, 0, 0);
        }
        if (em_frame_check(em, 418.0f, 0)) {
            Eft20_set(0.9f, em, 1, 0);
        }
        break;
    case 0x401:
        sound_call_005872F0(em, 4, 0x4c, 0x34);
        sound_call_005872F0(em, 4, 0xe, 0x22);
        sound_call_005872F0(em, 8, 0xe, 0x28);
        sound_call_005872F0(em, 4, 0x18, 0x11);
        break;
    case 0x402:
        sound_call_005872F0(em, 0xe, 8, 0);
        sound_call_005872F0(em, 0x34, 6, 0x33);
        sound_call_005872F0(em, 0xe, 0x2b, 0x34);
#ifdef VERSION_MHG
        /* G: motion 0x402 is shorter; its own sound list */
        sound_call_005872F0(em, 0x78, 2, 0x18);
        sound_call_005872F0(em, 0x86, 2, 0x1d);
        sound_call_005872F0(em, 0x118, 4, 5);
        sound_call_005872F0(em, 0xdc, 4, 9);
        sound_call_005872F0(em, 0xa6, 0x15, 0);
        sound_call_005872F0(em, 0x8a, 0x21, 0x34);
        sound_call_005872F0(em, 0xcc, 2, 0x18);
#else
        sound_call_005872F0(em, 0x8a, 0x21, 0x34);
        sound_call_005872F0(em, 0x112, 0x2c, 0x34);
        sound_call_005872F0(em, 0x1ba, 0x2a, 0x34);
        sound_call_005872F0(em, 0xa6, 0x15, 0);
        sound_call_005872F0(em, 0x102, 0xf, 0x11);
        sound_call_005872F0(em, 0x1ac, 4, 9);
        sound_call_005872F0(em, 0x1fc, 4, 5);
        sound_call_005872F0(em, 0x1f2, 2, 0x1d);
        sound_call_005872F0(em, 0x19e, 2, 0x18);
#endif
        quake_call_00587230(em, 0xc, 2);
        quake_call_00587230(em, 0x20, 2);
        if (em_frame_check(em, 10.0f, 0)) {
            Eft20_set(1.2f, em, 0, 1);
        }
        break;
    case 0x403:
        sound_call_005872F0(em, 4, 0x49, 0x34);
        sound_call_005872F0(em, 0x52, 0x2f, 0x34);
        sound_call_005872F0(em, 0xac, 0x30, 0x34);
        sound_call_005872F0(em, 0x128, 0x56, 0x34);
        sound_call_005872F0(em, 0x24, 3, 0x1d);
        sound_call_005872F0(em, 0x24, 7, 0x18);
        sound_call_005872F0(em, 0x38, 6, 0x33);
        sound_call_005872F0(em, 0x86, 0x16, 0);
        sound_call_005872F0(em, 4, 0x11, 0xb);
        sound_call_005872F0(em, 0x18, 0x1a, 0x11);
        sound_call_005872F0(em, 0x60, 0x18, 0x11);
        sound_call_005872F0(em, 0x8e, 0x1a, 0x11);
        sound_call_005872F0(em, 0x138, 0x18, 0x11);
        quake_call_00587230(em, 0x2e, 5);
        if (em_frame_check(em, 46.0f, 0) || em_frame_check(em, 112.0f, 0) || em_frame_check(em, 320.0f, 0)) {
            Eft20_set(0.6f, em, 1, 0x80);
        }
        break;
    case 0x404:
        sound_call_005872F0(em, 4, 0x28, 0x34);
        sound_call_005872F0(em, 0x8c, 0x32, 0x34);
        sound_call_005872F0(em, 0x12e, 0x31, 0x34);
        sound_call_005872F0(em, 4, 0x15, 0);
        sound_call_005872F0(em, 0x8c, 0x17, 0);
        sound_call_005872F0(em, 0xdc, 3, 0x1d);
        sound_call_005872F0(em, 0xdc, 6, 0x18);
        quake_call_00587230(em, 0xdc, 2);
        if (em_frame_check(em, 216.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 1.2f, 7);
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        break;
    case 0x405:
        sound_call_005872F0(em, 4, 0x50, 0x34);
        sound_call_005872F0(em, 0x78, 0x35, 0x34);
        sound_call_005872F0(em, 0x162, 0x32, 0x34);
        sound_call_005872F0(em, 0x162, 0x2b, 0x34);
        sound_call_005872F0(em, 0x1ae, 0x35, 0x34);
        sound_call_005872F0(em, 4, 0x15, 0);
        sound_call_005872F0(em, 0x50, 0x18, 0x11);
        sound_call_005872F0(em, 0x13e, 0xf, 0xb);
        break;
    case 0x406:
        sound_call_005872F0(em, 8, 0x33, 0x34);
        sound_call_005872F0(em, 0xba, 0x21, 0x34);
        sound_call_005872F0(em, 0x16, 9, 0x22);
        sound_call_005872F0(em, 0x1a, 9, 0x28);
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 190.0f, 198.0f)) {
                Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
            }
        }
        break;
    case 0x407:
        sound_call_005872F0(em, 4, 0x1f, 0x34);
        sound_call_005872F0(em, 0x1c, 0xa, 0x22);
        sound_call_005872F0(em, 0x20, 0xa, 0x28);
        sound_call_005872F0(em, 0x6a, 0xa, 0x22);
        sound_call_005872F0(em, 0x6e, 0xa, 0x28);
        if (em->x8B6) {
            if (em_frame_check3(em, 0, 10.0f, 70.0f) || em_frame_check3(em, 0, 114.0f, 122.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
                }
            }
        }
        if (em_frame_check(em, 36.0f, 0) || em_frame_check(em, 122.0f, 0)) {
            shell04_set(em, 0x13);
        }
        break;
    case 0x408:
        sound_call_005872F0(em, 4, 0x55, 0x34);
        sound_call_005872F0(em, 0x3e, 0xc, 0x22);
        sound_call_005872F0(em, 0x42, 0xc, 0x28);
        sound_call_005872F0(em, 4, 0xf, 0x11);
        sound_call_005872F0(em, 0x78, 0x11, 0x11);
        sound_call_005872F0(em, 4, 0x18, 0xb);
        sound_call_005872F0(em, 0x34, 0x18, 0xb);
#ifdef VERSION_MHG
        if (!(em->mode == 6 && em->x15 == 0) && !(em->mode == 6 && em->x15 == 3)) {    /* G: not in demos 0 / 3 */
#else
        if (em->mode != 6 || em->x15 != 0) {
#endif
            if (em_frame_check(em, 66.0f, 0)) {
                shell04_set(em, 0x10);
                Eft15_set3(em, 5, 1.0f, 3);
            }
        }
        break;
    case 0x409:
        sound_call_005872F0(em, 4, 0x1f, 0x34);
        sound_call_005872F0(em, 0x50, 0x20, 0x34);
        sound_call_005872F0(em, 0x5e, 0x1b, 0x34);
        sound_call_005872F0(em, 0x82, 0x21, 0x34);
        sound_call_005872F0(em, 0x9c, 0x1e, 0x34);
        sound_call_005872F0(em, 0x38, 0xa, 0x22);
        sound_call_005872F0(em, 0x3c, 0xa, 0x28);
        sound_call_005872F0(em, 0x7a, 0xc, 0x22);
        sound_call_005872F0(em, 0x7e, 0xc, 0x28);
        sound_call_005872F0(em, 0xb4, 0xc, 0x22);
        sound_call_005872F0(em, 0xba, 0xc, 0x28);
        sound_call_005872F0(em, 0xf4, 0xc, 0x22);
        sound_call_005872F0(em, 0x94, 0xc, 0x28);
        sound_call_005872F0(em, 0x3e, 7, 5);
        sound_call_005872F0(em, 4, 0x17, 0);
        sound_call_005872F0(em, 4, 0xf, 0x11);
        sound_call_005872F0(em, 0x4c, 0x1a, 0x11);
        if (em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 0x1f, 8.0f, 7);
        }
        if (em_frame_check(em, 76.0f, 0) || em_frame_check(em, 252.0f, 0)) {
            shell04_set(em, 0x1b);
        }
        if (em_frame_check3(em, 0, 124.0f, 132.0f) || em_frame_check3(em, 0, 154.0f, 170.0f)) {
            if (!(*(u16 *)&game_w.x1E & 0x3)) {
                Eft20_set(1.0f, em, 0x1a, (s16)((u16)ran_suu(1) & 0x1));
            }
        }
        break;
    case 0x40A:
        sound_call_005872F0(em, 4, 0x32, 0x34);
        sound_call_005872F0(em, 0x96, 0x28, 0x34);
        sound_call_005872F0(em, 0x140, 0x33, 0x34);
        sound_call_005872F0(em, 0x1a4, 0x34, 0x34);
        sound_call_005872F0(em, 4, 0x17, 0);
        sound_call_005872F0(em, 0xf8, 6, 0x1d);
        sound_call_005872F0(em, 0x10a, 8, 0x33);
        sound_call_005872F0(em, 0xea, 0x19, 0x33);
        quake_call_00587230(em, 0xf2, 2);
        if (em_frame_check(em, 242.0f, 0)) {
            Eft13_set_em_scl(em, 0x1e, 1.2f, 7);
        }
        if (em_frame_check(em, 244.0f, 0)) {
            Eft13_set_em_scl(em, 0x19, 1.2f, 7);
        }
        break;
#ifdef VERSION_MHG
    /* G's new motions 0x40B-0x410 (em_char_set 0x23-0x28: attack acts 12 / 13; tools/genef.py GENEF_G=1) */
    case 0x40B:
        sound_call_005872F0(em, 0x12, 0xe, 0x22);
        sound_call_005872F0(em, 0x16, 0xe, 0x28);
        sound_call_005872F0(em, 0x46, 0xc, 0x22);
        sound_call_005872F0(em, 0x4a, 0xc, 0x28);
        sound_call_005872F0(em, 0x16, 5, 9);
        if (em_frame_check(em, 68.0f, 0)) {
            shell04_set(em, 0x1e);
        }
        if (em_frame_check(em, 34.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(3.0f, em, 0x13, 0);
            }
        }
        break;
    case 0x40C:
        sound_call_005872F0(em, 4, 0x3f, 0x33);
        sound_call_005872F0(em, 4, 0x40, 0x28);
        sound_call_005872F0(em, 8, 0x40, 0x22);
        sound_call_005872F0(em, 4, 0x22, 0x33);
        sound_call_005872F0(em, 0x26, 0xc, 0x22);
        sound_call_005872F0(em, 0x28, 0xc, 0x28);
        sound_call_005872F0(em, 0x4e, 0xa, 0x22);
        sound_call_005872F0(em, 0x52, 0xa, 0x28);
        sound_call_005872F0(em, 0x9e, 9, 0x22);
        sound_call_005872F0(em, 0xa2, 9, 0x28);
        sound_call_005872F0(em, 0x2c, 0x20, 0x34);
        sound_call_005872F0(em, 0x24, 0x12, 5);
        sound_call_005872F0(em, 0x3e, 5, 9);
        sound_call_005872F0(em, 0x9e, 1, 9);
        sound_call_005872F0(em, 0x54, 6, 0);
        if (em_frame_check3(em, 0, 30.0f, 70.0f)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0xa, 0x80);
                }
            }
        }
        if (em_frame_check(em, 96.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(2.0f, em, 0xb, 0);
            }
        }
        break;
    case 0x40D:
        sound_call_005872F0(em, 0x10, 0x21, 0x34);
        sound_call_005872F0(em, 0x18, 0xe, 0x22);
        sound_call_005872F0(em, 0x1c, 0xe, 0x28);
        sound_call_005872F0(em, 4, 0x3f, 0x33);
        sound_call_005872F0(em, 4, 0x40, 0x28);
        sound_call_005872F0(em, 8, 0x40, 0x22);
        if (em_frame_check(em, 52.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.5f, em, 1, 0);
            }
        }
        break;
    case 0x40E:
        sound_call_005872F0(em, 4, 0x3f, 0x33);
        sound_call_005872F0(em, 4, 0x40, 0x28);
        sound_call_005872F0(em, 8, 0x40, 0x22);
        sound_call_005872F0(em, 0x22, 0x41, 0x33);
        sound_call_005872F0(em, 0x22, 0x40, 0x28);
        sound_call_005872F0(em, 0x26, 0x40, 0x22);
        break;
    case 0x40F:
        sound_call_005872F0(em, 4, 0x3f, 0x33);
        sound_call_005872F0(em, 4, 0x40, 0x28);
        sound_call_005872F0(em, 8, 0x40, 0x22);
        sound_call_005872F0(em, 4, 0x22, 0x33);
        break;
    case 0x410:
        sound_call_005872F0(em, 4, 0x3f, 0x33);
        sound_call_005872F0(em, 4, 0x40, 0x28);
        sound_call_005872F0(em, 8, 0x40, 0x22);
        sound_call_005872F0(em, 0x22, 0x41, 0x33);
        sound_call_005872F0(em, 0x22, 0x40, 0x28);
        sound_call_005872F0(em, 0x26, 0x40, 0x22);
        if (em->pos[1] <= 1000.0f + em->x5AC) {
            if (!(*(u16 *)&game_w.x1E & 0x3)) {
                Eft20_set(0.8f, em, 0xa, 0x80);
            }
        }
        break;
#endif
    default:
        move_default_00583BA0(em);
        break;
    }
}

static void quake_call_00587230(EMW *em, int frame, int v) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_set_quake_sub(em, v);
    }
}

static void sound_call_sub_00587280(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
    Em_se_req2(em, se, 0, pos, 8, 0);
}

static void sound_call_005872F0(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, (f32)frame, 0)) {
        sound_call_sub_00587280(em, se, joint);
    }
}

void em02_effect_move(EMW *em) {
    EM02W *w = (EM02W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_00583BF0(em, w);
        break;
    }
    em02_uvmove(em);
}

void em02_local_init(EMW *em) {
    eft01_set((PLW *)em, 0);
}

/* A file static in the original; data tables point at it. */
void dummy_em_prog_005873C0(void) {
}
