/* em07 AI - game.bin 0x0058F4A0-0x00599EC0: monster kind 7: item/hagi pick points, init, action steps em_act00-07, walk
 * and turn states (em_mv00-19), attacks, damage reactions, death, event demos, em07_main, uvmove and the per-animation
 * sound/effect script (ef_move_sub). The setters are in em07.c (0x599ED0-). Field meanings are guesses. The whole file
 * is in this _nm file; the matching runs are em07_ai*.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))

/* em07's part of the per-monster work at EMW+0x444 (em07.c has the same start). */
typedef struct EM07W {
    u8 _pad00[2];
    s16 anim;           /* 0x02 animation the sound/effect script follows */
    u8 _pad04[4];
    s8 item_pt;         /* 0x08 pick point number of the item */
    s8 x09;             /* 0x09 */
    s16 x0A;            /* 0x0A */
    f32 xC;             /* 0x0C */
    f32 dist;           /* 0x10 distance to the target (500 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 has_tgt;         /* 0x16 */
    s8 x17;             /* 0x17 */
    s8 x18;             /* 0x18 */
    s8 x19;             /* 0x19 */
    u8 x1A;             /* 0x1A */
    s8 hagi[3];         /* 0x1B hagi pick point numbers, -1 none */
    s16 x1E;            /* 0x1E */
} EM07W;

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

extern s8 hagi_tbl_003886B0[3][2];
extern f32 hagi_r_tbl_00657788[3];
void em07_act_set(EMW *em, int kind, u16 no, u16 arg);
void em07_to_normal(EMW *em);
void get_joint_pos_em(EMW *, int, f32 *);
f32 *get_joint_wmat_em(EMW *, int);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void Ext_pick_point_st(int, int);
int Ext_pick_point_set(STIEM *, f32 *);
void Ext_pick_point_pos(int, f32 *);
int Ext_pick_point_cnt_ck(int);
void Ext_pick_point_clr(int);
int softdip_ck(int);
void eft01_set(PLW *, int);
void em07_item_pos_set(EMW *em, f32 *o);
void em07_hagi_set(EMW *em);
void em07_hagi_move(EMW *em);
void em07_hagi_clr(EMW *em);
static void sound_call_mov2(EMW *em, int frame, int se, int joint);

void em07_local_area_move_init(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 p[3];

    w->x18 = 0;
    if (w->item_pt != -1) {
        Ext_pick_point_st(w->item_pt, em->stg);
        em07_item_pos_set(em, p);
        Ext_pick_point_pos(w->item_pt, p);
    }
}

void em07_local_init(EMW *em) {
    eft01_set((PLW *)em, 2);
}

void em07_item_pos_set(EMW *em, f32 *o) {
    f32 in[3];
    f32 q[3];
    f32 p[3];

    in[1] = 250.0f;
    in[2] = 120.0f;
    in[0] = 0.0f;
    get_joint_pos_em(em, 3, p);
    flvecApplyMat33(q, in, get_joint_wmat_em(em, 3));
    o[0] = p[0] + q[0];
    o[1] = p[1] + q[1];
    o[2] = p[2] + q[2];
}

void em07_item_set(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    STIEM s;
    f32 pos[3];

    if (w->x09 <= 0) {
        w->item_pt = -1;
    }
    s.rad = 340.0f;
    if (quest_w.x14E == 0) {
        s.id = 0x9B;
    } else {
        s.id = 0x9C;
    }
    s.x14 = 2;
    s.cnt = w->x09;
    s.stg = em->stg;
    s.x19 = 1;
    s.x1A = 0;
    em07_item_pos_set(em, pos);
    w->item_pt = Ext_pick_point_set(&s, pos);
}

void em07_hagi_set(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    STIEM s;
    f32 pos[3];
    int i;

    if (quest_w.x14E == 0) {
        s.id = 0xE4;
    } else {
        s.id = 0xE5;
    }
    for (i = 0; i < 3; i++) {
        s.rad = hagi_r_tbl_00657788[i];
        s.stg = em->stg;
        s.x1A = 0;
        s.x19 = 1;
        get_joint_pos_em(em, hagi_tbl_003886B0[i][0], pos);
        pos[1] = em->x5AC;
        s.x14 = 2;
        s.cnt = hagi_tbl_003886B0[i][1];
        w->hagi[i] = Ext_pick_point_set(&s, pos);
    }
}

void em07_hagi_move(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 pos[3];
    int i;

    for (i = 0; i < 3; i++) {
        if (w->hagi[i] != -1) {
            get_joint_pos_em(em, hagi_tbl_003886B0[i][0], pos);
            pos[1] = em->x5AC;
            Ext_pick_point_pos(w->hagi[i], pos);
            if (Ext_pick_point_cnt_ck(w->hagi[i]) <= 0) {
                Ext_pick_point_clr(w->hagi[i]);
                w->hagi[i] = -1;
            }
        }
    }
}

void em07_hagi_clr(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    int i;

    for (i = 0; i < 3; i++) {
        if (w->hagi[i] != -1) {
            Ext_pick_point_clr(w->hagi[i]);
            w->hagi[i] = -1;
        }
    }
}

void em07_init(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    if (quest_w.no == 0) {
        switch (game_w.stage) {
        case 11:
            em->pos[0] = 33000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 30200.0f;
            em->ang[1] = 0xC000;
            break;
        case 12:
            em->pos[0] = 7400.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 26300.0f;
            em->ang[1] = 0x4000;
            break;
        case 13:
            em->pos[0] = 5000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 8000.0f;
            em->ang[1] = 0x8000;
            break;
        case 14:
            em->pos[0] = 2400.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 11000.0f;
            em->ang[1] = 0xE39;
            break;
        case 15:
            em->pos[0] = 7000.0f;
            em->pos[1] = 580.0f;
            em->pos[2] = 7000.0f;
            em->ang[1] = 0;
            break;
        case 28:
            em->pos[0] = 30800.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 32600.0f;
            em->ang[1] = 0x6C00;
            break;
        case 30:
            em->pos[0] = 23500.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 3500.0f;
            em->ang[1] = 0xBA06;
            break;
        default:
            em->pos[0] = 10000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f;
            em->ang[1] = 0x8000;
            break;
        }
    }
    em->x792 = em->x302 = em_hp_vital_set2(em, 0x2710, 0xD05);
    em->x734 = 3;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x888 = 1;
    em->x7EE = 0xF;
    em_char_set(em, 1, 0, 0);
    em_act_set(em, 0, 1);
    w->xC = 1500.0f;
    w->x17 = 0;
    w->x0A = 0;
    w->x18 = 0;
    w->x1E = 0x96;
    w->x09 = 3;
    em07_item_set(em);
}

void em07_to_normal(EMW *em) {
    em->act_spd = 1.0f;
    em->x3F4 = 0;
    em->x839 = 1;
    if (em->x388 == 0) {
        em->x388 = 0;
        em07_act_set(em, 0, 4, 0);
    } else {
        em->x388 = 1;
        em07_act_set(em, 0, 1, 0);
    }
}

int em07_act_sub(EMW *em, int idx) {
    if (*(s32 *)(idx * 0x50 + (char *)em + 0x194) == 0) {
        goto ok;
    }
    if (idx != 0xFF) {
        return 0;
    }
ok:
    if (em->x734 == 3) {
        em->x839 = 1;
        return 1;
    }
    em_act_set(em, 0, 1);
    return 1;
}

static void em_act00_0058FBF0(EMW *em) {
    if (softdip_ck(0x4D) == 0) {
        if (em->char0 != 0x3E9) {
            em_char_set(em, 1, 0xA, 0);
        }
        if (em->x2DE != 0x44D) {
            em_char_set(em, 1, 0xA, 0);
        }
        if (em->x2E0 != 0x4B1) {
            em_char_set(em, 1, 0xA, 0);
        }
    }
}

static void em_act01_0058FC80(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        if (em->x2DE != 0x4B1) {
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
        }
        if (em->x2E0 != 0x579) {
            em_char_set2(em, 0x579, 0xA, 0, 2);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_act02_0058FD50(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 6, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 7, 0, 0);
        }
        break;
    case 2:
        em->_pad87C[3] = 5;
        if (em->x194 == 0) {
            em->x05++;
            em->_pad87C[3] = 0;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_act03_0058FE10(EMW *em) {
    em->x8BB = 5;
    em->x8BD = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 8, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 0;
            em->x8BB = 0;
            em->x8BD = 0;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_act04_0058FEB0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xC, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_act05_0058FF30(EMW *em) {
    em->x8BB = 5;
    em->x8BD = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x8BB = 0;
            em->x8BD = 0;
            em->x388 = 1;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_act06_0058FFD0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        if (em->x2DE != 0x4B1) {
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
        }
        if (em->x2E0 != 0x579) {
            em_char_set2(em, 0x579, 0xA, 0, 2);
        }
        em->work08 = 0x1E;
        break;
    case 1:
        if (em->x194 == 0 || --em->work08 <= 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_act07_005900D0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xC, 0, 0);
        em->work08 = 0x1E;
        break;
    case 1:
        if (em->x194 == 0 || --em->work08 <= 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}
#define EM07_TURN(em, tgt)                                                \
    do {                                                                  \
        int d;                                                            \
        d = (u16)((u16)(tgt) - (em)->ang[1]);                             \
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


static void em_move00_00590180(EMW *em) {
    switch (em->x15) {
    case 0:
        em_act00_0058FBF0(em);
        break;
    case 1:
        em_act01_0058FC80(em);
        break;
    case 2:
        em_act02_0058FD50(em);
        break;
    case 3:
        em_act03_0058FE10(em);
        break;
    case 4:
        em_act04_0058FEB0(em);
        break;
    case 5:
        em_act05_0058FF30(em);
        break;
    case 6:
        em_act06_0058FFD0(em);
        break;
    case 7:
        em_act07_005900D0(em);
        break;
    }
}

static void em_mv00_00590240(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM07_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_mv01_005903A0(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM07_TURN(em, (u16)(Em_Calc_angY(em->pos, em->tgt_pos) + 0x8000));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist + v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_mv02_00590510(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        d = (u16)(w->dang - em->ang[1]);
        if (d < 0x11C8U || d >= 0xEE39U) {
            pl_flag_set((PLW *)em, 0x20000);
            em_char_set(em, 3, 0, 0);
        } else if (d >= 0x8000U) {
            em_char_set(em, 5, 0, 0);
        } else {
            em_char_set(em, 4, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            d = (u16)(w->dang - (u16)em->ang[1]);
            if (em->x194 == 0) {
                if ((u32)((d + 0x22) & 0xFFFF) < 0x44U) {
                    em->x05++;
                    pl_flag_clr((PLW *)em, 0x20000);
                    em07_to_normal(em);
                    return;
                }
                if (d < 0x11C8U || d >= 0xEE39U) {
                    pl_flag_set((PLW *)em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *)em, 0x20000);
                if (d >= 0x8000U) {
                    em_char_set(em, 5, 0, 0);
                } else {
                    em_char_set(em, 4, 0, 0);
                }
                return;
            }
            if (em_frame_check2(em, 0, 162.0f) != 0 && em_frame_check2(em, 0, 462.0f) == 0) {
                if ((u32)((d + 0x22) & 0xFFFF) < 0x44U) {
                    em->ang[1] = w->dang;
                } else if (d < 0x8000U) {
                    em->ang[1] = (em->ang[1] + 0x22) & 0xFFFF;
                } else {
                    em->ang[1] = (em->ang[1] - 0x22) & 0xFFFF;
                }
            }
        }
        break;
    }
}

static void em_mv03_00590790(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 4, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 162.0f) != 0 && em_frame_check2(em, 0, 462.0f) == 0) {
            em->ang[1] += 0x22;
        }
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_mv04_00590860(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x14, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 28.0f) != 0 && em_frame_check2(em, 0, 156.0f) == 0) {
                em->ang[1] += 0x80;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 1;
            }
        }
        break;
    }
}

static void em_mv05_00590940(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x15, 0, 0);
        break;
    case 1:
        em07_act_sub(em, 0);
        break;
    }
}

static void em_mv06_005909B0(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x17, 0, 0);
        break;
    case 1:
        if (em07_act_sub(em, 0) != 0) {
            w->x17 = 0;
        }
        break;
    }
}

static void em_mv07_00590A30(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x16, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 30.0f) != 0 && em_frame_check2(em, 0, 158.0f) == 0) {
                em->ang[1] -= 0x80;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 0;
            }
        }
        break;
    }
}

static void em_mv08_00590B10(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x1A, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 28.0f) != 0 && em_frame_check2(em, 0, 156.0f) == 0) {
                em->ang[1] -= 0x80;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 2;
            }
        }
        break;
    }
}

static void em_mv09_00590BF0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x1B, 0, 0);
        break;
    case 1:
        em07_act_sub(em, 0);
        break;
    }
}

static void em_mv10_00590C60(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x1D, 0, 0);
        break;
    case 1:
        if (em07_act_sub(em, 0) != 0) {
            w->x17 = 0;
        }
        break;
    }
}

static void em_mv11_00590CE0(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x1C, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 30.0f) != 0 && em_frame_check2(em, 0, 158.0f) == 0) {
                em->ang[1] += 0x80;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 0;
            }
        }
        break;
    }
}

static void em_mv12_00590DC0(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 82.0f) != 0 && em_frame_check2(em, 0, 210.0f) == 0) {
                em->ang[1] += 0x100;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 0;
            }
        }
        break;
    }
}

static void em_mv13_00590EA0(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x1F, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 82.0f) != 0 && em_frame_check2(em, 0, 210.0f) == 0) {
                em->ang[1] -= 0x100;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 0;
            }
        }
        break;
    }
}

static void em_mv14(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x18, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 204.0f) != 0 && em_frame_check2(em, 0, 334.0f) == 0) {
                em->ang[1] += 0x7E;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 0;
                em->ang[1] += 8;
            }
        }
        break;
    }
}

static void em_mv15(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x1E, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 204.0f) != 0 && em_frame_check2(em, 0, 334.0f) == 0) {
                em->ang[1] -= 0x7E;
            }
            if (em07_act_sub(em, 0) != 0) {
                w->x17 = 0;
                em->ang[1] -= 8;
            }
        }
        break;
    }
}

static void em_mv16(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x10, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM07_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

void shell05_set(EMW *, int);
void shell05_set2(EMW *, f32 *, int);
void Quest_failed_set(EMW *);

static void em_mv17(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 3, 0, 0);
        em_char_set(em, 0x42, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 34.0f, 0) != 0) {
            shell05_set(em, 0x42);
        }
        if (em_frame_check(em, 410.0f, 0) != 0) {
            shell05_set(em, 0x43);
        }
        if (w->has_tgt != 0) {
            EM07_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
    sound_call_mov2(em, 0xAA, 0x168, 0x4F);
}

static void em_mv18(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[3];
    int sp[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM07_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
        }
        break;
    case 2:
        mot_miration_ret(em, v);
        SetVector(&em->rate_x, v[0], v[1], -v[2]);
        sp[0] = 0;
        sp[1] = em->ang[1];
        sp[2] = 0;
        speed_add(em, sp);
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_mv19(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[3];
    int sp[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM07_TURN(em, (u16)(Em_Calc_angY(em->pos, em->tgt_pos) + 0x8000));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist + v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
        }
        break;
    case 2:
        mot_miration_ret(em, v);
        SetVector(&em->rate_x, v[0], v[1], -v[2]);
        sp[0] = 0;
        sp[1] = em->ang[1];
        sp[2] = 0;
        speed_add(em, sp);
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_move01_005917E0(EMW *em) {
    switch (em->x15) {
    case 0:
        em_mv00_00590240(em);
        break;
    case 1:
        em_mv01_005903A0(em);
        break;
    case 2:
        em_mv02_00590510(em);
        break;
    case 3:
        em_mv03_00590790(em);
        break;
    case 4:
        em_mv04_00590860(em);
        break;
    case 5:
        em_mv05_00590940(em);
        break;
    case 6:
        em_mv06_005909B0(em);
        break;
    case 7:
        em_mv07_00590A30(em);
        break;
    case 8:
        em_mv08_00590B10(em);
        break;
    case 9:
        em_mv09_00590BF0(em);
        break;
    case 10:
        em_mv10_00590C60(em);
        break;
    case 11:
        em_mv11_00590CE0(em);
        break;
    case 12:
        em_mv12_00590DC0(em);
        break;
    case 13:
        em_mv13_00590EA0(em);
        break;
    case 14:
        em_mv14(em);
        break;
    case 15:
        em_mv15(em);
        break;
    case 16:
        em_mv16(em);
        break;
    case 17:
        em_mv17(em);
        break;
    case 18:
        em_mv18(em);
        break;
    case 19:
        em_mv19(em);
        break;
    }
}

static void em_move02_00591960(EMW *em) {
}

static void em_atk00_00591970(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xD, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_atk01_005919F0(EMW *em) {
    f32 v[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xE, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        if (em_frame_check(em, 160.0f, 0) != 0) {
            if (em->stg == 0xB) {
                SetVector(v, 13300.0f, 1700.0f, 20100.0f);
                shell05_set2(em, v, 0x44);
                bridge_eff_set(em);
            }
            if (em->stg == 0xC) {
                SetVector(v, 7300.0f, 1700.0f, 11700.0f);
                shell05_set2(em, v, 0x45);
                bridge_eff_set(em);
            }
            if (em->stg == 0x1E) {
                SetVector(v, 14600.0f, 1700.0f, 23700.0f);
                shell05_set2(em, v, 0x46);
                bridge_eff_set(em);
            }
        }
        break;
    }
}

static void em_atk02_00591B80(EMW *em) {
    EM07W *w = (EM07W *)em->ex;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 210.0f, 0) != 0) {
            toride_eff_set(em);
            if (em->x8C3 == 0) {
                if (w->x19 < 0xA) {
                    w->x19++;
                    if (w->x19 >= 0xA) {
                        Quest_failed_set(em);
                    }
                    *(s8 *)0x3F360D = w->x19;
                }
            }
        }
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_atk03_00591C90(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xD, 0, 0);
        break;
    case 1:
        if (em_frame_check2(em, 0, 36.0f) != 0 && em_frame_check2(em, 0, 78.0f) == 0) {
            em->work08 = 0x15;
            em->ang[1] += (0x13E9 / em->work08) & 0xFFFF;
        }
        if (em_frame_check2(em, 0, 434.0f) != 0 && em_frame_check2(em, 0, 510.0f) == 0) {
            em->work08 = 0x26;
            em->ang[1] -= (0x13E9 / em->work08) & 0xFFFF;
        }
        if (em_frame_check(em, 510.0f, 0) != 0) {
            em->ang[1] += 0xA;
        }
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_atk04_00591E10(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 210.0f, 0) != 0) {
            toride_eff_set(em);
        }
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_atk05_00591EC0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 210.0f, 0) != 0) {
            *(u8 *)0x3F35A3 |= 1;
            toride_eff_set(em);
        }
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_move03_00591F80(EMW *em) {
    switch (em->x15) {
    case 0:
        em_atk00_00591970(em);
        break;
    case 1:
        em_atk01_005919F0(em);
        break;
    case 2:
        em_atk02_00591B80(em);
        break;
    case 3:
        em_atk03_00591C90(em);
        break;
    case 4:
        em_atk04_00591E10(em);
        break;
    case 5:
        em_atk05_00591EC0(em);
        break;
    }
}

static void em_dmg00_00592020(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x44, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 28.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x3C, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3D, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_dmg01_00592130(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_dmg02_005921C0(EMW *em) {
    int sp[3];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x3F, 0, 0);
        em_cmd_reset(em);
        em_rate_clear(em);
        em->adj_z = -26.913044f;
        break;
    case 1:
        if (em->x1C4 == 0) {
            sp[0] = 0;
            sp[1] = em->ang[1];
            sp[2] = 0;
            speed_add(em, sp);
            if (em_frame_check2(em, 0, 46.0f) != 0) {
                em->x05++;
            }
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_dmg03_005922B0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x44, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_dmg04_00592340(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 1;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_dmg05_005923C0(EMW *em) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xF, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 1;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_dmg06_00592490(EMW *em) {
    int t;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 1;
        em_char_set(em, 0x44, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 28.0f, 0) != 0) {
            em->x05++;
            em_char_set(em, 0x3C, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x12C;
        }
        break;
    case 3:
        t = em->work08 - 1;
        em->work08 = t;
        if (t <= 0) {
            em->x05++;
            em_char_set(em, 0x3D, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_move04_005925D0(EMW *em) {
    switch (em->x15) {
    case 0:
        em_dmg00_00592020(em);
        break;
    case 1:
        em_dmg01_00592130(em);
        break;
    case 2:
        em_dmg02_005921C0(em);
        break;
    case 3:
        em_dmg03_005922B0(em);
        break;
    case 4:
        em_dmg04_00592340(em);
        break;
    case 5:
        em_dmg05_005923C0(em);
        break;
    case 6:
        em_dmg06_00592490(em);
        break;
    }
}

static void em_die00_00592680(EMW *em) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 8, 0, 0);
        em->work08 = 0;
        em->x3F4 = 0;
        em->x388 = 1;
        Quest_enemy_die(em);
        break;
    case 1:
        if (em_frame_check(em, 450.0f, 0) != 0) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x43, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 3;
            em07_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 3:
        if (softdip_ck(0x42) != 0) {
            em->x05 = 0x63;
        } else {
            em07_hagi_move(em);
        }
        break;
    case 0x63:
        em->x05++;
        em07_hagi_clr(em);
        em->x302 = 0x3415;
        em07_to_normal(em);
        break;
    }
}

static void em_die01_005927F0(EMW *em) {
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x43, 0, 0);
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 3;
            em07_hagi_set(em);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        if (softdip_ck(0x42) != 0) {
            em->x05 = 0x63;
        } else {
            em07_hagi_move(em);
        }
        break;
    case 0x63:
        em->x05++;
        em->x302 = 0x3415;
        em07_hagi_clr(em);
        em07_to_normal(em);
        break;
    }
}

static void em_move05_00592900(EMW *em) {
    switch (em->x15) {
    case 0:
        em_die00_00592680(em);
        break;
    case 1:
        em_die01_005927F0(em);
        break;
    }
}

static void em_demo00_00592950(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[4];
    int t;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 1;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0xF0;
        em->pos[0] = 8150.0f;
        em->pos[1] = 0.0f;
        em->pos[2] = 9310.0f;
        em->tgt_pos[0] = 10660.0f;
        em->tgt_pos[1] = 0.0f;
        em->tgt_pos[2] = 9250.0f;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        w->x18 = 1;
        em->x839 = 0;
        break;
    case 1:
        t = em->work08 - 1;
        em->work08 = t;
        if (t <= 0) {
            em->x05++;
            em_char_set(em, 3, 0, 0);
        }
        break;
    case 2:
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em->pos[0] = 13600.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 10750.0f;
            em->ang[1] = 0x25E0;
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xC, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xD, 0, 0);
        }
        break;
    case 5:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0xF, 0, 0);
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 7:
        if (Event_flag_ck(0x18) == 1) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_demo01_00592BE0(EMW *em) {
    EM07W *w = (EM07W *)em->ex;
    f32 v[4];

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 1;
        em_char_set(em, 0x40, 0, 0);
        em->pos[0] = 6070.0f;
        em->pos[1] = 0.0f;
        em->pos[2] = 17350.0f;
        em->tgt_pos[0] = 3600.0f;
        em->tgt_pos[1] = 0.0f;
        em->tgt_pos[2] = 19500.0f;
        em->ang[1] = (Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - 0x4000;
        w->dist = CalcDistanceXZ(em->pos, em->tgt_pos) - 500.0f;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x41, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            if (em_frame_check2(em, 0, 64.0f) == 0) {
                em->ang[1] += 0x210;
            }
            if (em->x194 == 0) {
                em->x05++;
                em_char_set(em, 3, 0, 0);
            }
        }
        break;
    case 3:
        if (EMF(em, s32, 0x1E4) == 0 && em->x2DE == 0x4F2) {
            em_char_set2(em, 0x4B3, 0, 0, 1);
        }
        EM07_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->tgt_pos[0] = 430.0f;
            em->tgt_pos[1] = 0.0f;
            em->tgt_pos[2] = 23930.0f;
            em->x05++;
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        }
        break;
    case 4:
        EM07_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em_char_set(em, 3, 0, 0);
        }
        break;
    case 5:
        if (Event_flag_ck(0x19) == 1) {
            em->x05++;
            em07_to_normal(em);
        }
        break;
    }
}

static void em_move06_00592F90(EMW *em) {
    switch (em->x15) {
    case 0:
        em_demo00_00592950(em);
        break;
    case 1:
        em_demo01_00592BE0(em);
        break;
    }
}

