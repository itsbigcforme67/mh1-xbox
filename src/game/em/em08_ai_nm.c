/* em08 AI - game.bin 0x0059A280-0x005A7380: monster kind 8: init, action steps em_act, walk/fly/swim states, attacks, damage
 * reactions, death, event demos, main, uvmove, sound/effect script and the hire (cat helper) moves. Field meanings are
 * guesses. The whole file is in this _nm file; the matching runs are em08_ai*.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"

/* Raw access to EMW fields that em.h does not name yet. */
#define EMF(em, T, o) (*(T *)((u8 *)(em) + (o)))

/* em08's part of the per-monster work at EMW+0x444. */
typedef struct EM08W {
    u8 _pad00[4];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    u8 _pad06;
    u8 x07;             /* 0x07 */
    u8 _pad08[2];
    u16 dang;           /* 0x0A angle left to turn */
    u8 _pad0C;
    u8 has_tgt;         /* 0x0D */
    u8 _pad0E;
    u8 xF;              /* 0x0F */
    s16 x10;            /* 0x10 */
    u8 _pad12[2];
    s32 x14;            /* 0x14 */
    s32 x18;            /* 0x18 */
    s32 vel[3];         /* 0x1C speed vector */
    s32 x28;            /* 0x28 */
    s32 x2C;            /* 0x2C */
    s32 x30;            /* 0x30 */
    f32 dist;           /* 0x34 distance left to walk */
    s8 x38;             /* 0x38 */
    s8 x39;             /* 0x39 */
    s8 x3A;             /* 0x3A */
    u8 _pad3B[7];
    s16 tm[4];          /* 0x42 hire (helper cat) per-slot timers */
    struct {
        u8 a;
        u8 b;
    } st[4];            /* 0x4A hire slot states */
    struct {
        u16 a;
        u16 b;
    } ang[4];           /* 0x52 hire slot angles */
    s16 cnt[4];         /* 0x62 */
    s8 tmr[4];          /* 0x6A */
} EM08W;

typedef struct HIRE_E {
    u16 t;
    u16 v;
} HIRE_E;



extern GAME_W game_w;


void Eft19_set(EMW *, int, int);
void eft09_set(EMW *);
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
int em08_fly_adjy2(EMW *);
void em08_fly_adjy2_init(EMW *, int);
void Eft08_set(f32 *, int, int, f32);
void shell21_set(EMW *, int);
void shell23_set(EMW *, int);
void vib_set_pl(PLW *, int);
int Code_Make(int, int, int, int);
extern u16 hire_down_angx_003887E8[4];
extern s16 hire_start_timer_tbl0_003887C8[4];
extern s16 hire_start_timer_tbl1_003887D0[4];
extern s16 hire_remove_timer_tbl0_003887D8[4];
extern s16 hire_remove_timer_tbl1_003887E0[4];
extern HIRE_E *hire_normal_add_tbl_00659250[4];
extern HIRE_E *hire_down_add_tbl_00659300[4];
void em08_effect_move(EMW *em);
void hire_req_set_005A6F90(EMW *em, EM08W *w, u8 req);
void atk_shell_set(EMW *em, int no);
void em08_vib_set(EMW *em);
void ground_land_eff_set_005A7050(EMW *em);
void swim_eff_set_005A7070(f32 scale, EMW *em);
void swim_eff_set2_005A7120(f32 scale, EMW *em);
void em21_target_ang_calc(EMW *em, int arg1);
void em08_uvmove(EMW *em);
static void sound_call_sub_005A3CA0(EMW *em, int se, int joint);
static void sound_call_005A3D10(EMW *em, int frame, int se, int joint);
static void quake_call_005A3DB0(EMW *em, int frame, int v);
static void move_default_005A3E00(EMW *em);
static void ef_move_sub_005A3E50(EMW *em, EM08W *w);

extern s16 em08_stay_timer_tbl[];
extern s16 em08_runaway_timer_tbl[];
void em08_act_set(EMW *em, int kind, u16 no, u16 arg);
void em08_to_normal(EMW *em);
void em08_to_swim(EMW *em);
int em08_act_sub(EMW *em, int arg1);
void em08_main_sub(EMW *em, EM08W *w);
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
void em_uvmove(EMW *em);
void net_send_em(EMW *, int, int);
int Quest_clear_ck(int);
void RedDragonEscapeCamera(EMW *);
void Eft02_set3(f32, EMW *, int, int, int, f32 *);
void Eft13_set_pos2(f32, EMW *, f32 *, int);
void Eft10_set(f32, EMW *, int, int);
void Shell22_set3(EMW *, int, int);
void shell05_set4(EMW *, int, int);
void bridge_eff_set(EMW *);
void toride_eff_set(EMW *);


void em08_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *) em, 0);
}

void em08_init(EMW *em) {
    EM08W *w = (EM08W *)em->ex;
    u8 t;

    em_char_set(em, 1, 0, 0);
    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
        switch (game_w.stage) {
        case 0x36:
            em->pos[0] = 6000.0f + (1500.0f * (f32)(u32)em->x13);
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f;
            em->ang[1] = 0x4000;
            em->x388 = 4;
            em08_act_set(em, 2, 0, 0);
            break;
        default:
            em->pos[0] = 5000.0f + (1500.0f * (f32)(u32)em->x13);
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            em->x388 = 0;
            em08_act_set(em, 2, 0, 0);
            break;
        }
    } else {
        switch (game_w.stage) {
        case 0x36:
            em->x388 = 4;
            em08_act_set(em, 2, 0, 0);
            break;
        case 0x2D:
            em->x388 = 4;
            em08_act_set(em, 2, 0, 0);
            break;
        default:
            em->x388 = 4;
            em08_act_set(em, 2, 0, 0);
            break;
        }
    }
    w->x10 = 0;
    em->x839 = 1;
    em->x88B = 1;
    if (em->kind == 8) {
        em->x765 = 1;
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x578, 0x258);
    } else {
        em->x8C3 = 0;
        em->x765 = 0;
        em->x792 = em->x302 = em_hp_vital_set2(em, 0x8C, 0x64);
    }
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em08_stay_timer_tbl[em->stg];
    em->runaway_tm = em08_runaway_timer_tbl[em->stg];
    w->dang = 0x4000;
    w->x28 = 0x100;
    w->x2C = 0x200;
    w->x30 = 0x100;
    w->x18 = 0x2000;
    w->x14 = 0;
    w->x38 = 0;
    w->x39 = 0;
    w->x3A = 0;
    if (em->kind == 8) {
        em->x7E0 = 450.0f;
    } else {
        em->x7E0 = 250.0f;
    }
    t = em->x948 & 1;
    em->x948 = t;
    if (t == 0 && em->kind != 0x14) {
        switch (em->kind) {
        case 1:
        case 6:
        case 8:
        case 0xB:
        case 0xF:
        case 0xE:
        case 0x11:
        case 0x15:
        case 0x16:
        case 0x22:
            em->ex[0xA3] = 0;
            eft09_set(em);
            break;
        }
    }
}

void em08_to_normal(EMW *em) {
    EMF(em, s32, 0x930) = 0x3F800000;
    EMF(em, s8, 0x388) = 0;
    EMF(em, s8, 0x3F4) = 0;
    EMF(em, s8, 0x839) = 1;
    if (em->x302 < (s16)(0.1f * (f32)em->x792)) {
        em08_act_set(em, 0, 1, 0);
        return;
    }
    em08_act_set(em, 0, 1, 0);
}

void em08_to_swim(EMW *em) {
    EMF(em, s32, 0x930) = 0x3F800000;
    EMF(em, s8, 0x388) = 4;
    EMF(em, s8, 0x3F4) = 0;
    em->x839 = 1;
    em08_act_set(em, 2, 0, 0);
}

int em08_act_sub(EMW *em, int arg1) {
    if (((s32 (*)[20])&em->x194)[arg1][0] == 0 || arg1 == 0xFF) {
        em08_to_normal(em);
        return 1;
    }
    return 0;
}

#define EM08_TURN(em, tgt)                                                \
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

#define EM08_TURNN(em, tgt, st)                                           \
    do {                                                                  \
        int d;                                                            \
        d = (u16)((u16)(tgt) - (em)->ang[1]);                             \
        if (d <= 0x8000) {                                                \
            if (d < (st)) {                                               \
                (em)->ang[1] += d;                                        \
            } else {                                                      \
                (em)->ang[1] += (st);                                     \
            }                                                             \
        } else if (d > 0x10000 - (st)) {                                  \
            (em)->ang[1] += d;                                            \
        } else {                                                          \
            (em)->ang[1] -= (st);                                         \
        }                                                                 \
    } while (0)

static void em_act00_0059A7B0(EMW *em, EM08W *w) {
    int t;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
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
        t = em->work08 - 1;
        em->work08 = t;
        if (t <= 0) {
            em->x839 = 1;
        }
        break;
    }
}

static void em_act01_0059A880(EMW *em, EM08W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0, 0, 0);
        }
        if (em->x2DE != 0x4B1) {
            em_char_set2(em, 0x4B1, 0, 0, 1);
        }
        if (em->x2E0 != 0x579) {
            em_char_set2(em, 0x579, 0, 0, 2);
        }
        break;
    case 1:
        if (em->x8C3 == 0 && em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_act02_0059A960(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0x5C);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_act03_0059AA10(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1U);
        em_range_set(em, 1);
        break;
    case 1:
        if ((em_frame_check(em, 56.0f, 0) != 0) || (em_frame_check(em, 108.0f, 0) != 0) || (em_frame_check(em, 126.0f, 0) != 0) || (em_frame_check(em, 138.0f, 0) != 0) || (em_frame_check(em, 170.0f, 0) != 0)) {
            sound_call_sub_005A3CA0(em, 0x26, 0x23);
            Eft20_set(5.0f, em, 7, 0);
        }
        if ((em_frame_check(em, 210.0f, 0) != 0) || (em_frame_check(em, 218.0f, 0) != 0)) {
            Eft20_set(2.5f, em, 7, 0);
        }
        if (em_frame_check(em, 52.0f, 0) != 0) {
            Eft20_set(1.0f, em, 8, 0);
        }
        if ((em_frame_check(em, 106.0f, 0) != 0) || (em_frame_check(em, 130.0f, 0) != 0)) {
            Eft20_set(1.0f, em, 6, 0);
        }
        if (em_frame_check(em, 124.0f, 0) != 0) {
            Eft20_set(1.0f, em, 6, 1);
        }
        if (em_frame_check(em, 170.0f, 0) != 0) {
            Eft20_set(1.0f, em, 0xF, 0);
        }
        if (em_frame_check(em, 192.0f, 0) != 0) {
            Eft20_set(1.0f, em, 0x10, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
            em_hp_add(em, (s16)(0.05f * (f32) em->x792));
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x762 = 0;
            em_search_data_set(em, 0U);
            em_range_set(em, 0);
            em_thirst_end(em);
            em08_to_normal(em);
        }
        break;
    }
    em_thirst_add(em, 0xDE);
}

static void em_act04_0059AD80(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x79, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 1;
            em_char_set(em, 0x7A, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x7C, 0, 0);
            break;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_act05_0059AE70(EMW *em, EM08W *w) {
    s32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x22, 0, 0);
            break;
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 6, 4);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 6, 4);
        }
        break;
    }
}

static void em_act06_0059AF80(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_act_set(em, 0, 7, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act07_0059B050(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_act08_0059B0D0(EMW *em, EM08W *w) {
    f32 v[4];
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 9, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 9, 4);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005A3CA0(em, 0x57, 0x23);
    }
}

static void em_act09_0059B210(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em08_act_set(em, 4, 0xD, 3);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_act10_0059B300(EMW *em, EM08W *w) {
    f32 v[4];
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 0xB, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 0xB, 4);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005A3CA0(em, 0x57, 0x23);
    }
}

static void em_act11_0059B440(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em08_act_set(em, 4, 0xD, 3);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_act12_0059B530(EMW *em, EM08W *w) {
    s32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x22, 0, 0);
            break;
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 0xD, 4);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 0xD, 4);
        }
        break;
    }
}

static void em_act13_0059B640(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em->x88B = 1;
        em_range_set(em, 0);
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_act_set(em, 0, 7, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act14_0059B710(EMW *em, EM08W *w) {
    f32 va[4];
    f32 vb[4];
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 1;
        em->x3F4 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x89, 0, 0);
            break;
        }
        break;
    case 3:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 0xF, 4);
        }
        va[1] = 10.0f;
        va[2] = 140.0f;
        va[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, va, 1.6f);
        if (((s32) em->work08 % 135) == 0) {
            sound_call_sub_005A3CA0(em, 0x57, 0x23);
            break;
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 0xF, 4);
        }
        vb[1] = 10.0f;
        vb[2] = 140.0f;
        vb[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vb, 1.6f);
        if (((s32) em->work08 % 135) == 0) {
            sound_call_sub_005A3CA0(em, 0x57, 0x23);
        }
        break;
    }
}

static void em_act15_0059B920(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x7C, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_act_set(em, 0, 7, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act16_0059B9F0(EMW *em, EM08W *w) {
    f32 va[4];
    f32 vb[4];
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x89, 0, 0);
            break;
        }
        break;
    case 3:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 0x11, 4);
        }
        va[1] = 10.0f;
        va[2] = 140.0f;
        va[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, va, 1.6f);
        if (((s32) em->work08 % 135) == 0) {
            sound_call_sub_005A3CA0(em, 0x57, 0x23);
            break;
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 0x11, 4);
        }
        vb[1] = 10.0f;
        vb[2] = 140.0f;
        vb[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vb, 1.6f);
        if (((s32) em->work08 % 135) == 0) {
            sound_call_sub_005A3CA0(em, 0x57, 0x23);
        }
        break;
    }
}

static void em_act17_0059BC00(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x7C, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_act_set(em, 0, 7, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act18_0059BCD0(EMW *em, EM08W *w) {
    s32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x20, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Suimin_Start(em);
        em->work08 = 0x2328;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x1F, 0, 0);
            break;
        }
        break;
    case 2:
        em_sleep_hp_add(em, 1, (s16)(0.3f * (f32) em->x792), 6);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            em_hinshi_end(em);
            em08_act_set(em, 0, 0x13, 4);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 0x13, 4);
        }
        break;
    }
}

static void em_act19_0059BE30(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x21, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        em_suimin_end(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_act_set(em, 0, 7, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act20_0059BF00(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1U);
        em_range_set(em, 1);
        break;
    case 1:
        if (em_frame_check(em, 200.0f, 0) != 0) {
            em->x05++;
            if (em->x8C3 == 0) {
                em->x827 = 7;
                em->x828 = em->x951;
                em->x829 = em->x952;
                cmd_target_kind_set(em, em->tgt_pos);
                em->_pad9F4[0xC] = 1;
                em_niku_eat_set(em);
                em_hungry_add(em, 0x2710);
            }
            em08_act_set(em, 0, 0x28, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->x827 = 7;
            em->x828 = em->x951;
            em->x829 = em->x952;
            cmd_target_kind_set(em, em->tgt_pos);
            em->_pad9F4[0xC] = 1;
            em_niku_eat_set(em);
            em_hungry_add(em, 0x2710);
            em08_act_set(em, 0, 0x28, 4);
        }
        break;
    }
}

static void em_act40_0059C080(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x762 = 0;
        em_search_data_set(em, 0U);
        em_range_set(em, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x3B, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_act21_0059C140(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_mv00_0059C1D0(EMW *em, EM08W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM08_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_mv01_0059C310(EMW *em, EM08W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 4, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM08_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em_char_set(em, 0x85, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_mv02_0059C480(EMW *em, EM08W *w) {
    u32 d;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        d = (u16)(w->dang - em->ang[1]);
        if (d <= 0xE38U || d >= 0xF1C8U) {
            pl_flag_set((PLW *)em, 0x20000);
            em_char_set(em, 3, 0, 0);
        } else if (d >= 0x8000U) {
            em_char_set(em, 6, 0, 0);
        } else {
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            d = (u16)(w->dang - (u16)em->ang[1]);
            if (em->x194 == 0) {
                if ((u32)((d + 0x1D4) & 0xFFFF) < 0x3A8U) {
                    em->x05++;
                    pl_flag_clr((PLW *)em, 0x20000);
                    em08_to_normal(em);
                } else if (d <= 0xE38U || d >= 0xF1C8U) {
                    pl_flag_set((PLW *)em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                } else {
                    pl_flag_clr((PLW *)em, 0x20000);
                    if (d >= 0x8000U) {
                        em_char_set(em, 6, 0, 0);
                    } else {
                        em_char_set(em, 5, 0, 0);
                    }
                }
                break;
            }
            if ((u32)((d + 0x1D4) & 0xFFFF) < 0x3A8U) {
                em->ang[1] = w->dang;
            } else if (d < 0x8000U) {
                em->ang[1] = (em->ang[1] + 0x1D4) & 0xFFFF;
            } else {
                em->ang[1] = (em->ang[1] - 0x1D4) & 0xFFFF;
            }
        }
        break;
    }
}

static void em_mv03_0059C6D0(EMW *em, EM08W *w) {
    f32 v[4];

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            EM08_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        mot_miration_ret(em, v);
        w->dist = w->dist - v[2];
        if (w->dist <= 0.0f) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_fly00_0059C810(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x64, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly01_0059C8A0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x6D, 0, 0);
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly02_0059C930(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x72, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_to_swim(em);
        }
        em->ang[1] += 0x1F0;
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

static void em_fly03_0059CA30(EMW *em, EM08W *w) {
    f32 temp_f1;
    u32 spd;
    s32 temp_a3;
    u16 temp_a1;
    u32 temp_a2;
    u32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->dang - em->ang[1]) & 0xFFFF;
        if ((temp_v1 <= 0x2AA8U) || (temp_v1 >= 0xD558U)) {
            em_char_set(em, 0x65, 0, 0);
        } else if (temp_v1 >= 0x8000U) {
            em_char_set(em, 0x6A, 0, 0);
        } else {
            em_char_set(em, 0x6B, 0, 0);
        }
        swim_eff_set2_005A7120(8.0f, em);
        break;
    case 1:
        if (em->x1C4 == 0) {
            spd = (u32)((32768.0f / (em->x1A8 / 2.0f)) * em->act_spd);
            temp_a3 = em->ang[1];
            temp_a1 = w->dang;
            temp_a2 = (temp_a1 - (temp_a3 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_a2 + spd) & 0xFFFF) < (u32) (spd * 2)) {
                    em->x05++;
                    em08_to_swim(em);
                    return;
                }
                if ((temp_a2 <= 0x2AA8U) || (temp_a2 >= 0xD558U)) {
                    em_char_set(em, 0x65, 0, 0);
                    return;
                }
                if (temp_a2 >= 0x8000U) {
                    em_char_set(em, 0x6A, 0, 0);
                    return;
                }
                em_char_set(em, 0x6B, 0, 0);
                return;
            }
            if ((u32) ((temp_a2 + spd) & 0xFFFF) < (u32) (spd * 2)) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_a2 < 0x8000U) {
                em->ang[1] = (temp_a3 + spd) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a3 - spd) & 0xFFFF;
        } else {
            break;
        }
        break;
    }
}

static void em_fly04_0059CCE0(EMW *em, EM08W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    s32 temp_a0;
    s32 temp_v1;
    s32 var_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x65, 0, 0);
        em_rate_clear(em);
        em->adj_z = 5.0f;
        em->x3C0[2] = 1.0f;
        break;
    case 1:
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        if (!(em->adj_z < 50.0f)) {
            em->adj_z = 50.0f;
        }
        if (w->has_tgt != 0) {
            EM08_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
        }
        w->dist = w->dist - em->adj_z;
        if (w->dist <= 0.0f) {
            em->x05 = 0xA;
            em08_to_swim(em);
        }
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x66, 0, 0);
            break;
        }
        break;
    case 2:
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        if (!(em->adj_z < 50.0f)) {
            em->adj_z = 50.0f;
        }
        w->dist = w->dist - em->adj_z;
        if (w->dist <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly05_0059CF30(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em_rate_clear(em);
        em->adj_z = 30.0f;
        em->x3C0[2] = 10.0f;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            xang_calc_target(em, w->vel, 0.0f, 0.0f);
            w->vel[1] = (s32) em->ang[1];
            w->vel[2] = 0;
            speed_add(em, w->vel);
            if (!(em->adj_z < 100.0f)) {
                em->adj_z = 100.0f;
            }
            if (w->has_tgt != 0) {
                EM08_TURN(em, Em_Calc_angY(em->pos, em->tgt_pos));
            }
            w->dist = w->dist - em->adj_z;
            if (w->dist <= 0.0f) {
                em->x05++;
                em08_to_swim(em);
            }
            swim_eff_set_005A7070(8.0f, em);
        }
        break;
    }
}

static void em_fly06_0059D0F0(EMW *em, EM08W *w) {
    u8 temp_a1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x6E, 0, 0);
        em08_fly_adjy2_init(em, 0);
        break;
    case 1:
        if (em08_fly_adjy2(em) != 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly07_0059D190(EMW *em, EM08W *w) {
    f32 temp_f1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x68, 0, 0);
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->adj_y = 2.0f;
        em->x3C0[1] = 4.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (!(em->adj_y <= 50.0f)) {
            em->adj_y = 50.0f;
            em->x3C0[1] = 0.0f;
        }
        temp_f1 = em->x7E4 - em->x7E0;
        if (!(em->pos[1] <= temp_f1)) {
            em->pos[1] = temp_f1;
            em->x05++;
            em_char_set(em, 0x86, 0, 0);
            swim_eff_set_005A7070(8.0f, em);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly08_0059D2E0(EMW *em, EM08W *w) {
    f32 temp_f1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x69, 0, 0);
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->adj_y = -2.0f;
        em->x3C0[1] = -4.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (em->adj_y < -50.0f) {
            em->adj_y = -50.0f;
            em->x3C0[1] = 0.0f;
        }
        temp_f1 = em->x5AC;
        if (em->pos[1] < temp_f1) {
            em->pos[1] = temp_f1;
            em->x05++;
            em_char_set(em, 0x87, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly09_0059D420(EMW *em, EM08W *w) {
    f32 temp_f1;
    u32 spd;
    s32 temp_a3;
    s32 temp_v1_2;
    u16 temp_a1;
    u32 temp_a2_2;
    u32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->dang - em->ang[1]) & 0xFFFF;
        if ((temp_v1 <= 0x2AA8U) || (temp_v1 >= 0xD558U)) {
            em_char_set(em, 0x65, 0, 0);
        } else if (temp_v1 >= 0x8000U) {
            em_char_set(em, 0x6A, 0, 0);
        } else {
            em_char_set(em, 0x6B, 0, 0);
        }
        swim_eff_set2_005A7120(8.0f, em);
        break;
    case 1:
        if (em->x1C4 == 0) {
            spd = (u32)((32768.0f / (em->x1A8 / 2.0f)) * em->act_spd);
            temp_a3 = em->ang[1];
            temp_a1 = w->dang;
            temp_a2_2 = (temp_a1 - (temp_a3 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_a2_2 + spd) & 0xFFFF) < (u32) (spd * 2)) {
                    em->x05++;
                    em->work08 = 0x12C;
                    em08_act_set(em, 2, 0xA, 1);
                    return;
                }
                if ((temp_a2_2 <= 0x2AA8U) || (temp_a2_2 >= 0xD558U)) {
                    em_char_set(em, 0x65, 0, 0);
                    return;
                }
                if (temp_a2_2 >= 0x8000U) {
                    em_char_set(em, 0x6A, 0, 0);
                    return;
                }
                em_char_set(em, 0x6B, 0, 0);
                return;
            }
            if ((u32) ((temp_a2_2 + spd) & 0xFFFF) < (u32) (spd * 2)) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_a2_2 < 0x8000U) {
                em->ang[1] = (temp_a3 + spd) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a3 - spd) & 0xFFFF;
            break;
        }
        break;
    case 2:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em08_act_set(em, 2, 0xA, 1);
            }
        }
        break;
    }
}

static void em_fly10_0059D720(EMW *em, EM08W *w) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_v0_2;

    switch (em->x05) {
    case 0:
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em->x05++;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
            break;
        }
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_9;
            }
        } else {
block_9:
            temp_v0_2 = w->x07 - 1;
            w->x07 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em->x05++;
                em08_to_swim(em);
            } else {
                em->x883 += 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f);
                em_act_set(em, 2, 0xA);
            }
        }
        EM08_TURNN(em, Em_Calc_angY(em->pos, em->tgt_pos), 0x200);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

static void em_fly11_0059D960(EMW *em, EM08W *w) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_v0_2;

    switch (em->x05) {
    case 0:
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em->x05++;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
            break;
        }
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_9;
            }
        } else {
block_9:
            temp_v0_2 = w->x07 - 1;
            w->x07 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em->x05++;
                em08_to_swim(em);
            } else {
                em->x883 -= 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f);
                em_act_set(em, 2, 0xB);
            }
        }
        EM08_TURNN(em, Em_Calc_angY(em->pos, em->tgt_pos), 0x200);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

static void em_fly12_0059DBA0(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x72, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_to_swim(em);
        }
        em->ang[1] += 0x1F0;
        em->ang[1] = (s32) (u16) EMF(em, s32, 0xA4);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

static void em_fly13_0059DCA0(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x73, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_to_swim(em);
        }
        em->ang[1] -= 0x1F0;
        em->ang[1] = (s32) (u16) EMF(em, s32, 0xA4);
        xang_calc_target(em, w->vel, 0.0f, 0.0f);
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        swim_eff_set_005A7070(8.0f, em);
        break;
    }
}

static void em_fly14_0059DDA0(EMW *em, EM08W *w) {
    u8 temp_a3;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->adj_y = 132.0f;
        em->adj_z = 60.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] < (em->x7E4 - em->x7E0))) {
            temp_a3 = em->x05;
            em->x05 = temp_a3 + 1;
            em->x388 = 2;
            em->adj_y = 50.0f;
            em->x3C0[1] = -5.0f;
            swim_eff_set2_005A7120(8.0f, em);
            em08_vib_set(em);
            Em_set_quake_sub(em, 3);
            break;
        }
        break;
    case 2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
        }
        break;
    }
}

static void em_fly15_0059DF70(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x79, 0, 0);
        em_rate_clear(em);
        em->pos[1] = em->x7E4 - em->x7E0;
        break;
    case 1:
        if (em_frame_check(em, 10.0f, 0) != 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly16_0059E020(EMW *em, EM08W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x78, 0, 0);
        em_rate_clear(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 2;
            em_char_set(em, 0x77, 0, 0);
            em->adj_y = 50.0f;
            em->adj_z = 80.0f;
            em->x3C0[1] = -4.0f;
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 0x2:
        w->vel[0] = 0;
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add_g(em, w->vel);
        if (em->pos[1] <= (em->x7E4 - em->x7E0)) {
            em->x388 = 4;
        }
        if (em->adj_y < 0.0f) {
            if (*(u8 *)0x3F3404 == em->stg) {
                if (em->x7E9 != 0) {
                    if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                        em->x05++;
                        em->x388 = 4;
                        em->pos[1] = em->x7E4 - em->x7E0;
                        em->adj_y = 0.0f;
                        em->x3C0[1] = 0.0f;
                        em_char_set(em, 0x67, 0xA, 0);
                        w->dist = 1000.0f;
                        em->work08 = 0x3C;
                        em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                        swim_eff_set2_005A7120(8.0f, em);
                        return;
                    }
                } else if (em->pos[1] < em->x5AC) {
                    em->x05 = 0x63;
                    em->pos[1] = em->x5AC;
                    em08_act_set(em, 0, 4, 4);
                    swim_eff_set2_005A7120(8.0f, em);
                    return;
                }
            } else if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                em->x05++;
                em->x388 = 4;
                em->pos[1] = em->x7E4 - em->x7E0;
                em->adj_y = 0.0f;
                em->x3C0[1] = 0.0f;
                em_char_set(em, 0x67, 0xA, 0);
                w->dist = 1000.0f;
                em->work08 = 0x3C;
                em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                swim_eff_set2_005A7120(8.0f, em);
                return;
            }
        }
        break;
    case 0x3:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f || em->adj_z <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
            break;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
        }
        break;
    }
}

static void em_fly17_0059E400(EMW *em, EM08W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->adj_y = 100.0f;
        em->adj_z = 60.0f;
        em->x3C0[1] = -8.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 0x1:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] <= (em->x7E4 - em->x7E0))) {
            em->x388 = 2;
            swim_eff_set2_005A7120(8.0f, em);
            em->x05++;
            em08_vib_set(em);
            Em_set_quake_sub(em, 3);
            break;
        }
        break;
    case 0x2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (em->pos[1] < (em->x7E4 - em->x7E0)) {
            em->x388 = 4;
        }
        if (em->adj_y < 0.0f) {
            if (*(u8 *)0x3F3404 == em->stg) {
                if (em->x7E9 != 0) {
                    if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                        em->x05++;
                        em->x388 = 4;
                        em->pos[1] = em->x7E4 - em->x7E0;
                        em->adj_y = 0.0f;
                        em->x3C0[1] = 0.0f;
                        em_char_set(em, 0x67, 0xA, 0);
                        w->dist = 1000.0f;
                        em->work08 = 0x3C;
                        em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                        swim_eff_set2_005A7120(8.0f, em);
                        return;
                    }
                } else if (em->pos[1] < em->x5AC) {
                    em->x05 = 0x63;
                    em->pos[1] = em->x5AC;
                    em08_act_set(em, 0, 4, 4);
                    swim_eff_set2_005A7120(8.0f, em);
                    return;
                }
            } else if (em->pos[1] < (em->x7E4 - em->x7E0)) {
                em->x05++;
                em->pos[1] = em->x7E4 - em->x7E0;
                em->x388 = 4;
                em->adj_y = 0.0f;
                em->x3C0[1] = 0.0f;
                em_char_set(em, 0x67, 0xA, 0);
                w->dist = 1000.0f;
                em->work08 = 0x3C;
                em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
                swim_eff_set2_005A7120(8.0f, em);
                return;
            }
        }
        break;
    case 0x3:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f || em->adj_z <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
            break;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
        }
        break;
    }
}

static void em_fly18_0059E820(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x78, 0, 0);
        em_rate_clear(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 2;
            em_char_set(em, 0x77, 0, 0);
            em->adj_y = 50.0f;
            em->adj_z = 100.0f;
            em->x3C0[1] = -4.0f;
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 2:
        w->vel[0] = 0;
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add_g(em, w->vel);
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05++;
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em08_act_set(em, 0, 4, 4);
            swim_eff_set2_005A7120(8.0f, em);
        }
        break;
    }
}

static void em_fly19_0059E9C0(EMW *em, EM08W *w) {
    s32 temp_v1_2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x82, 0, 0);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05++;
            em->x88B = 1;
            em08_act_set(em, 2, 0x18, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 2, 0x18, 4);
        }
        break;
    }
}

static void em_fly20_0059EA90(EMW *em, EM08W *w) {
    s32 temp_v1_2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x82, 0, 0);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05++;
            em->x88B = 1;
            em08_act_set(em, 2, 0x19, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->x88B = 1;
            em08_act_set(em, 2, 0x19, 4);
        }
        break;
    }
}

static void em_fly21_0059EB70(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly22_0059EC00(EMW *em, EM08W *w) {
    f32 temp_f3;
    s32 temp_v1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->work08 = 0x96;
        em->tgt_pos[0] = 6000.0f;
        em->tgt_pos[1] = -1000.0f;
        em->tgt_pos[2] = 10000.0f;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->x3C0[1] = -2.0f;
        em->adj_y = ((em->tgt_pos[1] - em->pos[1]) / (f32)em->work08) - ((em->x3C0[1] * (f32)em->work08) / 2.0f);
        em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / (f32) em->work08;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            em->pos[1] = -1000.0f;
            em08_act_set(em, 2, 0, 4);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->pos[1] = -1000.0f;
            em08_act_set(em, 2, 0, 4);
        }
        break;
    }
}

static void em_fly23_0059EDA0(EMW *em, EM08W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    u8 temp_a2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x762 = 0;
        em_char_set(em, 0x12, 0xA, 0x1E);
        em08_fly_adjy2_init(em, 3);
        em->x388 = 2;
        swim_eff_set2_005A7120(8.0f, em);
        break;
    case 1:
        em08_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05++;
        }
        break;
    case 2:
        if ((em08_fly_adjy2(em) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
            em->x05++;
            em_char_set(em, 0xB, 0, 0);
            em_rate_clear(em);
            temp_f1 = em->x5AC;
            if (em->pos[1] < temp_f1) {
                em->pos[1] = temp_f1;
            }
            temp_f1_2 = (em->x5AC - em->pos[1]) / 30.0f;
            em->adj_y = temp_f1_2;
            if (!(temp_f1_2 < 0.0f)) {
                em->adj_y = -10.0f;
            }
        }
        break;
    case 3:
        w->vel[0] = 0;
        w->vel[1] = (s32) em->ang[1];
        w->vel[2] = 0;
        speed_add(em, w->vel);
        if (em->pos[1] <= em->x5AC) {
            em->x05++;
            em_char_set(em, 0x13, 0, 0);
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            w->x10 = 0x96;
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
    temp_f1_3 = em->x5AC;
    if (em->pos[1] < temp_f1_3) {
        em->pos[1] = temp_f1_3;
    }
}

static void em_fly24_0059EFD0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_fly25_0059F070(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_atk00_0059F110(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_atk01_0059F190(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x2F, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 78.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0x71C);
            if (em->kind == 8) {
                Shell08_set_ang(em, 0x22, 0x7, 0, 0x71C, 0);
            } else {
                Shell08_set_ang(em, 0x22, 0x7, 0x1, 0x71C, 0);
            }
        }
        if (em_frame_check(em, 80.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0x71C);
        }
        if (em_frame_check(em, 82.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0x71C);
        }
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_atk02_0059F2F0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2B, 0, 0);
        em_action_timer_calc(em, 0);
        break;
    case 1:
        em->ang[1] -= 0x200;
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em_char_set(em, 0x2B, 0, 0);
            break;
        }
        break;
    case 2:
        em->ang[1] -= 0x200;
        if (EMF(em, s32, 0x1E4) == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_atk03_0059F3E0(EMW *em, EM08W *w) {
    em08_to_swim(em);
}

static void em_atk04_0059F3F0(EMW *em, EM08W *w) {
    em08_to_swim(em);
}

static void em_atk05_0059F400(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x70, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if (em_frame_check(em, 58.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0);
            if (em->kind == 8) {
                Shell08_set_ang(em, 0x22, 0x7, 0, 0xE39, 0);
            } else {
                Shell08_set_ang(em, 0x22, 0x7, 0x1, 0xE39, 0);
            }
        }
        if (em_frame_check(em, 26.0f, 0) != 0) {
            swim_eff_set2_005A7120(8.0f, em);
        }
        if (em_frame_check(em, 98.0f, 0) != 0) {
            swim_eff_set2_005A7120(8.0f, em);
        }
        if (em_frame_check(em, 60.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0);
        }
        if (em_frame_check(em, 62.0f, 0) != 0) {
            Eft17_set_ang(em, 0x22, 0xC, 0);
        }
        if (em->x194 == 0) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_atk06_0059F5C0(EMW *em, EM08W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a1;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x6C, 0, 0);
        em->x388 = 4;
        em_rate_clear(em);
        em->adj_z = 100.0f;
        em->adj_y = 80.0f;
        em->x3C0[1] = -4.5f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] <= (em->x7E4 - em->x7E0))) {
            temp_a1 = em->x05;
            em->x05 = temp_a1 + 1;
            em->x388 = 2;
            swim_eff_set2_005A7120(8.0f, em);
            em08_vib_set(em);
            Em_set_quake_sub(em, 3);
            break;
        }
    default:
        break;
    case 2:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] <= em->x7E4)) {
            em->x05++;
            em->adj_y = 0.0f;
            em->work08 = 0x1E;
            break;
        }
        break;
    case 3:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05++;
            break;
        }
        break;
    case 4:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->pos[1] <= (em->x7E4 - em->x7E0)) && (em->adj_y < 0.0f)) {
            em->x05++;
            em->x388 = 4;
            em->adj_y = 0.0f;
            em->x3C0[1] = 0.0f;
            em_char_set(em, 0x67, 0xA, 0);
            w->dist = 1000.0f;
            em->work08 = 0x3C;
            em->x3C0[2] = (w->dist - ((f32)em->work08 * em->adj_z)) / (f32)((em->work08 * em->work08) / 2);
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 5:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        w->dist -= em->adj_z;
        if (w->dist <= 0.0f || em->adj_z <= 0.0f) {
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_atk07_0059F890(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x88, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_atk08_0059F910(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0x71, 0, 0);
        em_rate_clear(em);
        em->adj_y = 16.0f;
        em->adj_z = 100.0f;
        em->x3C0[1] = -0.8f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        em08_vib_set(em);
        Em_set_quake_sub(em, 3);
        break;
    case 1:
        if (em_frame_check(em, 54.0f, 0) != 0) {
            em->work08 = 0x12;
            em->x3C0[2] = -em->adj_z / (f32) em->work08;
        }
        if ((em_frame_check2(em, 0, 84.0f) != 0) && (em->pos[1] <= (em->x7E4 - em->x7E0))) {
            em->adj_y = 0.0f;
            em->x3C0[1] = 0.0f;
        }
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (em->adj_z <= 0.0f) {
            em->adj_z = 0.0f;
            em->x3C0[2] = 0.0f;
        }
        if (em->x194 == 0) {
            em->x388 = 4;
            em->x05++;
            em08_to_swim(em);
        }
        break;
    }
}

static void em_dmg00_0059FAB0(EMW *em, EM08W *w) {
    u8 temp_a2;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3C, 0, 0);
        break;
    case 1:
        if (((s32 (*)[20])&em->x194)[em->x07][0] == 0) {
            em->x05++;
            em08_to_normal(em);
            break;
        }
        if (EMF(em, s32, 0x1E4) == 0) {
            em_char_set(em, 1, 0, 0);
        }
        if (EMF(em, s32, 0x234) == 0) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    }
}

static void em_dmg01_0059FB90(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x42, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg02_0059FC20(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg03_0059FCB0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x40, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg04_0059FD40(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4B, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x7C, 0, 0);
            break;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg05_0059FE40(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, (s16)(em->x07 ? 0x45 : 0x4A), 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0;
            em_char_set(em, (s16)(em->x07 ? 0x46 : 0x4B), 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, (s16)(em->x07 ? 0x81 : 0x80), 0, 0);
            break;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0;
            em_char_set(em, (s16)(em->x07 ? 0x47 : 0x7C), 0, 0);
            break;
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg06_0059FFE0(EMW *em, EM08W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em->x388 = 2;
        em->x3C0[1] = -3.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[2] = 0.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        } else {
            em->x388 = 4;
        }
        if (em->pos[1] < em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em08_act_set(em, 4, 0x10, 2);
        }
        break;
    }
}

static void em_dmg07_005A00F0(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em->x388 = 0;
        em_cmd_reset(em);
        Em_Mahi_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x8A, 0, 0);
            break;
        }
        break;
    case 3:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 4, 0x13, 3);
        }
        em_mahi_eff_set(em, 2);
        break;
    case 4:
        if (em->x8C3 == 0) {
            em08_act_set(em, 4, 0x13, 3);
        }
        em_mahi_eff_set(em, 2);
        break;
    }
}

static void em_dmg08_005A0250(EMW *em, EM08W *w) {
    s8 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x60, 0, 0);
        Em_Mahi_End(em);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            temp_v0 = em->x95A - 1;
            em->x95A = temp_v0;
            if (((s8)temp_v0) <= 0) {
                em->x05++;
                em->x959 = 0;
                em->x8BD = 0;
                em08_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg09_005A0350(EMW *em, EM08W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x88B = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em->x388 = 2;
        em->x3C0[1] = -3.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[2] = 0.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        } else {
            em->x388 = 4;
        }
        if (em->pos[1] < em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em08_act_set(em, 4, 0x10, 2);
        }
        break;
    }
}

static void em_dmg10_005A0470(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x88B = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg11_005A0500(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4C, 0, 0);
        Em_Mahi_Start(em);
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 0, 0x15, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 0, 0x15, 4);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg12_005A05E0(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x5F, 0, 0);
        em->x762 = 3;
        em->x8BD = 1;
        em->x88B = 1;
        Em_Sleep_Flag_Ck(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_act_set(em, 4, 0xD, 3);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg13_005A06C0(EMW *em, EM08W *w) {
    s8 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em_char_set(em, 0x60, 0, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            temp_v0 = em->x95A - 1;
            em->x95A = temp_v0;
            if (((s8)temp_v0) <= 0) {
                em->x05++;
                em->x959 = 0;
                em->x8BD = 0;
                em08_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg14_005A07B0(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 0x61, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em08_act_set(em, 4, 8, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 4, 8, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg15_005A08C0(EMW *em, EM08W *w) {
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x7E, 0, 0);
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->x8B9 = 0;
        em->work08 = 0x1E;
        em->x3C0[1] = -10.0f;
        em->adj_y = ((em->tgt_pos[1] - em->pos[1]) / (f32)em->work08) - ((em->x3C0[1] * (f32)em->work08) / 2.0f);
        em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / (f32) em->work08;
        w->vel[0] = 0;
        w->vel[2] = 0;
        em->x762 = 3;
        em_cmd_reset(em);
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        }
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
    default:
        break;
    case 2:
        if (em->x194 == 0) {
            em->x302 -= 0x64;
            if (em->x302 <= 0) {
                em->x302 = 0;
                em->x05 = 0x63;
                em08_act_set(em, 5, 3, 2);
                return;
            }
            em->x05++;
            em_char_set(em, 0x4B, 0, 0);
            break;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x7C, 0, 0);
            break;
        }
        break;
    case 5:
        if (em->x194 == 0) {
            em->x05++;
            em->x762 = 0;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg16_005A0B60(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x79, 0xC, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x7C, 0, 0);
            break;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg17_005A0C40(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 100.0f;
        em->x3C0[1] = -2.75f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        w->vel[1] = (s32) em->ang[1];
        if (!(em->pos[1] < (em->x7E4 - em->x7E0))) {
            em->x388 = 2;
            em->adj_y = 80.0f;
            speed_add_g(em, w->vel);
            em->x05++;
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        em->x388 = 4;
        speed_add(em, w->vel);
        break;
    case 2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->pos[1] < em->x7E4) && (em->adj_y < 0.0f)) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
    default:
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x4B, 0, 0);
            break;
        }
        break;
    case 4:
    case 5:
        if (em->x194 == 0) {
            em->x05++;
            break;
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x47, 0, 0);
            break;
        }
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_dmg18_005A0E60(EMW *em, EM08W *w) {
    s32 temp_v0;

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x82, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05++;
            em08_act_set(em, 2, 0x15, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em08_act_set(em, 2, 0x15, 4);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg19_005A0F50(EMW *em, EM08W *w) {

    switch (em->x05) {
    case 0:
        em->x05++;
        em->x3F4 = 0;
        em_char_set(em, 0x7C, 0, 0);
        em->x388 = 1;
        em_cmd_reset(em);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_demo00_005A0FF0(EMW *em, EM08W *w) {
    f32 temp_f1;
    f32 temp_f1_3;
    f32 temp_f1_4;
    f32 temp_f1_5;
    f32 var_a3;
    u32 spd;
    s32 temp_t0;
    u16 temp_a1;
    f32 temp_f1_2;
    u16 temp_a1_2;
    u32 temp_a2_3;
    u32 temp_a3;
    s32 temp_a2_2;
    u8 temp_a3_2;

    em->x9E1 = 5;
    em->x40C = 5;
    switch (em->x05) {
    case 0:
        em->x05 = 3;
        em->x388 = 4;
        em->pos[0] = 11900.0f;
        em->pos[1] = -em->x7E0;
        em->pos[2] = 5220.0f;
        em->tgt_pos[0] = 16700.0f;
        em->tgt_pos[1] = -em->x7E0;
        em->tgt_pos[2] = 8450.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->ang[2] = 0;
        w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
        em_rate_clear(em);
        em->adj_z = 50.0f;
        em_char_set(em, 0x66, 0, 0);
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 1:
        em21_target_ang_calc(em, 0x40);
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        w->dist = w->dist - em->adj_z;
        if (w->dist <= 0.0f) {
            em->x05++;
            em->tgt_pos[0] = 16700.0f;
            em->tgt_pos[1] = -em->x7E0;
            em->tgt_pos[2] = 8450.0f;
            w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            em_char_set(em, 0x6B, 0, 0);
            break;
        }
    default:
        break;
    case 2:
        spd = (u32)((32768.0f / (em->x1A8 / 2.0f)) * em->act_spd);
        temp_a2_2 = em->ang[1];
        temp_a1 = w->dang;
        temp_a3 = (temp_a1 - (temp_a2_2 & 0xFFFF)) & 0xFFFF;
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x66, 0, 0);
            break;
        }
        if ((u32) ((temp_a3 + spd) & 0xFFFF) < (u32) (spd * 2)) {
            em->ang[1] = (s32) temp_a1;
            break;
        }
        if (temp_a3 < 0x8000U) {
            em->ang[1] = (temp_a2_2 + spd) & 0xFFFF;
            break;
        }
        em->ang[1] = (temp_a2_2 - spd) & 0xFFFF;
        break;
    case 3:
        em21_target_ang_calc(em, 0x40);
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        w->dist = w->dist - em->adj_z;
        if (w->dist <= 0.0f) {
            em->x05++;
            em->tgt_pos[0] = 12728.0f;
            em->tgt_pos[1] = -em->x7E0;
            em->tgt_pos[2] = 13160.0f;
            w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
            w->dist = CalcDistanceXZ(em->pos, em->tgt_pos);
            em_char_set(em, 0x6A, 0, 0);
        }
        swim_eff_set_005A7070(8.0f, em);
        break;
    case 4:
        spd = (u32)((32768.0f / (em->x1A8 / 2.0f)) * em->act_spd);
        temp_t0 = em->ang[1];
        temp_a1_2 = w->dang;
        temp_a2_3 = (temp_a1_2 - (temp_t0 & 0xFFFF)) & 0xFFFF;
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x66, 0, 0);
            break;
        }
        if ((u32) ((temp_a2_3 + spd) & 0xFFFF) < (u32) (spd * 2)) {
            em->ang[1] = (s32) temp_a1_2;
            break;
        }
        if (temp_a2_3 < 0x8000U) {
            em->ang[1] = (temp_t0 + spd) & 0xFFFF;
            break;
        }
        em->ang[1] = (temp_t0 - spd) & 0xFFFF;
        break;
    case 5:
        em21_target_ang_calc(em, 0x40);
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        w->dist = w->dist - em->adj_z;
        if (w->dist <= 0.0f) {
            em->x05++;
            em->tgt_pos[0] = 14380.0f;
            em->tgt_pos[1] = 0.0f;
            em->tgt_pos[2] = 14930.0f;
            em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
            em_char_set(em, 0x77, 0, 0);
            em->adj_y = 132.0f;
            em->adj_z = 60.0f;
        }
        swim_eff_set_005A7070(8.0f, em);
        break;
    case 6:
        w->vel[1] = (s32) em->ang[1];
        speed_add(em, w->vel);
        if (!(em->pos[1] < (em->x7E4 - em->x7E0))) {
            temp_a3_2 = em->x05;
            em->x05 = temp_a3_2 + 1;
            em->x388 = 2;
            em->adj_y = 50.0f;
            em->x3C0[1] = -5.0f;
            swim_eff_set2_005A7120(8.0f, em);
            break;
        }
        break;
    case 7:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05++;
            em->pos[1] = em->x5AC;
            swim_eff_set2_005A7120(8.0f, em);
            em->x388 = 0;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 1;
            em_char_set(em, 0x7A, 0, 0);
            break;
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05++;
            em->x388 = 0;
            em_char_set(em, 0x7C, 0, 0);
            break;
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x19, 0, 0);
            break;
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 1, 0, 0);
            break;
        }
        break;
    case 12:
        if (Event_flag_ck(0x11) == 1) {
            em->x05++;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_demo01_005A1700(EMW *em, EM08W *w) {

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
        if (Event_flag_ck(0x11) == 1) {
            em->x05++;
            em->x01 = 1;
            em08_to_normal(em);
        }
        break;
    }
}

static void em_die00_005A17A0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em_char_set(em, 0x44, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em_frame_check(em, 212.0f, 0) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x2:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 4;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x4:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die01_005A19D0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em_char_set(em, 0x61, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x2:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 4;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x4:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die02_005A1BD0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 100.0f;
        em->x3C0[1] = -2.75f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 0x1:
        w->vel[1] = (s32) em->ang[1];
        if (!(em->pos[1] < (em->x7E4 - em->x7E0))) {
            em->x388 = 2;
            em->adj_y = 80.0f;
            speed_add_g(em, w->vel);
            em->x05++;
            break;
        }
        em->x388 = 4;
        speed_add(em, w->vel);
        break;
    case 0x2:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if ((em->pos[1] < em->x7E4) && (em->adj_y < 0.0f)) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
        break;
    case 0x3:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x44, 0, 0);
            em->x3F4 = 0;
            em->x388 = 3;
            Quest_enemy_die(em);
            break;
        }
        break;
    case 0x4:
        if (em_frame_check(em, 212.0f, 0) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x5:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 7;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x6:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x7:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die03_005A1F70(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;

    if (em->kind == 0x22) {
        em->x40C = 0xA;
    }
    em->x40E = 0xA;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em_char_set(em, 0x4B, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x80, 0, 0);
            break;
        }
        break;
    case 0x2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x7F, 0, 0);
        }
    case 0x3:
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x4:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 6;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x5:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x6:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die04_005A21F0(EMW *em, EM08W *w) {
    switch (em->x05) {
    case 0:
        if (Quest_enemy_revival_ck() == 1) {
            Quest_enemy_revival_set(em);
            em_status_init(em);
            em08_init(em);
            em_cmd_reset(em);
            em->x839 = 0;
            em->mode = 5;
            em->x15 = 4;
            em->x388 = 4;
            em_act_set(em, 2, 0);
        } else {
            em->x04 += 1;
        }
        break;
    }
}

static void em_die05_005A22A0(EMW *em, EM08W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;
    u8 var_v1;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    switch (em->x05) {
    case 0x0:
        em->x05++;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em->x388 = 2;
        em->x3C0[1] = -3.0f;
        em->x3C0[0] = 0.0f;
        em->x3C0[2] = 0.0f;
        w->vel[0] = 0;
        w->vel[2] = 0;
        break;
    case 0x1:
        w->vel[1] = (s32) em->ang[1];
        speed_add_g(em, w->vel);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        } else {
            em->x388 = 4;
        }
        if (em->pos[1] < em->x5AC) {
            em->x05++;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            break;
        }
        break;
    case 0x2:
        if (em->x194 == 0) {
            em->x05++;
            em_char_set(em, 0x44, 0, 0);
            em->x3F4 = 0;
            em->x388 = 3;
            Quest_enemy_die(em);
            break;
        }
        break;
    case 0x3:
        if (em_frame_check(em, 212.0f, 0) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->x05++;
            em->work08 = 0x384;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            break;
        }
        break;
    case 0x4:
        Em_hagi_point_cnt_ck(em);
        if (em->kind != 8) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 = 6;
                Em_hagi_point_clr(em);
                return;
            }
        }
        break;
    case 0x5:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            break;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x6:
        temp_f1 = em->x798 - 0.016666668f;
        em->x798 = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x01 = 0;
            em_act_set(em, 5, 4);
            break;
        }
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em08_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_move00_005A25C0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_act00_0059A7B0(em, w);
        break;
    case 1:
        em_act01_0059A880(em, w);
        break;
    case 2:
        em_act02_0059A960(em, w);
        break;
    case 3:
        em_act03_0059AA10(em, w);
        break;
    case 4:
        em_act04_0059AD80(em, w);
        break;
    case 5:
        em_act05_0059AE70(em, w);
        break;
    case 6:
        em_act06_0059AF80(em, w);
        break;
    case 7:
        em_act07_0059B050(em, w);
        break;
    case 8:
        em_act08_0059B0D0(em, w);
        break;
    case 9:
        em_act09_0059B210(em, w);
        break;
    case 10:
        em_act10_0059B300(em, w);
        break;
    case 11:
        em_act11_0059B440(em, w);
        break;
    case 12:
        em_act12_0059B530(em, w);
        break;
    case 13:
        em_act13_0059B640(em, w);
        break;
    case 14:
        em_act14_0059B710(em, w);
        break;
    case 15:
        em_act15_0059B920(em, w);
        break;
    case 16:
        em_act16_0059B9F0(em, w);
        break;
    case 17:
        em_act17_0059BC00(em, w);
        break;
    case 18:
        em_act18_0059BCD0(em, w);
        break;
    case 19:
        em_act19_0059BE30(em, w);
        break;
    case 20:
        em_act20_0059BF00(em, w);
        break;
    case 21:
        em_act21_0059C140(em, w);
        break;
    case 40:
        em_act40_0059C080(em, w);
        break;
    }
}

static void em_move01_005A2830(EMW *em, EM08W *w) {
    u8 temp_a2;

    temp_a2 = EMF(em, u8, 0x15);
    switch (temp_a2) {
    case 0:
        em_mv00_0059C1D0(em, w);
        break;
    case 1:
        em_mv01_0059C310(em, w);
        break;
    case 2:
        em_mv02_0059C480(em, w);
        break;
    case 3:
        em_mv03_0059C6D0(em, w);
        break;
    }
}

static void em_move02_005A28C0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_fly00_0059C810(em, w);
        break;
    case 1:
        em_fly01_0059C8A0(em, w);
        break;
    case 2:
        em_fly02_0059C930(em, w);
        break;
    case 3:
        em_fly03_0059CA30(em, w);
        break;
    case 4:
        em_fly04_0059CCE0(em, w);
        break;
    case 5:
        em_fly05_0059CF30(em, w);
        break;
    case 6:
        em_fly06_0059D0F0(em, w);
        break;
    case 7:
        em_fly07_0059D190(em, w);
        break;
    case 8:
        em_fly08_0059D2E0(em, w);
        break;
    case 9:
        em_fly09_0059D420(em, w);
        break;
    case 10:
        em_fly10_0059D720(em, w);
        break;
    case 11:
        em_fly11_0059D960(em, w);
        break;
    case 12:
        em_fly12_0059DBA0(em, w);
        break;
    case 13:
        em_fly13_0059DCA0(em, w);
        break;
    case 14:
        em_fly14_0059DDA0(em, w);
        break;
    case 15:
        em_fly15_0059DF70(em, w);
        break;
    case 16:
        em_fly16_0059E020(em, w);
        break;
    case 17:
        em_fly17_0059E400(em, w);
        break;
    case 18:
        em_fly18_0059E820(em, w);
        break;
    case 19:
        em_fly19_0059E9C0(em, w);
        break;
    case 20:
        em_fly20_0059EA90(em, w);
        break;
    case 21:
        em_fly21_0059EB70(em, w);
        break;
    case 22:
        em_fly22_0059EC00(em, w);
        break;
    case 23:
        em_fly23_0059EDA0(em, w);
        break;
    case 24:
        em_fly24_0059EFD0(em, w);
        break;
    case 25:
        em_fly25_0059F070(em, w);
    default:
        break;
    }
}

static void em_move03_005A2AA0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_atk00_0059F110(em, w);
        break;
    case 1:
        em_atk01_0059F190(em, w);
        break;
    case 2:
        em_atk02_0059F2F0(em, w);
        break;
    case 3:
        em_atk03_0059F3E0(em, w);
        break;
    case 4:
        em_atk04_0059F3F0(em, w);
        break;
    case 5:
        em_atk05_0059F400(em, w);
        break;
    case 6:
        em_atk06_0059F5C0(em, w);
        break;
    case 7:
        em_atk07_0059F890(em, w);
        break;
    case 8:
        em_atk08_0059F910(em, w);
    default:
        break;
    }
}

static void em_move04_005A2B70(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_dmg00_0059FAB0(em, w);
        break;
    case 1:
        em_dmg01_0059FB90(em, w);
        break;
    case 2:
        em_dmg02_0059FC20(em, w);
        break;
    case 3:
        em_dmg03_0059FCB0(em, w);
        break;
    case 4:
        em_dmg04_0059FD40(em, w);
        break;
    case 5:
        em_dmg05_0059FE40(em, w);
        break;
    case 6:
        em_dmg06_0059FFE0(em, w);
        break;
    case 7:
        em_dmg07_005A00F0(em, w);
        break;
    case 8:
        em_dmg08_005A0250(em, w);
        break;
    case 9:
        em_dmg09_005A0350(em, w);
        break;
    case 10:
        em_dmg10_005A0470(em, w);
        break;
    case 11:
        em_dmg11_005A0500(em, w);
        break;
    case 12:
        em_dmg12_005A05E0(em, w);
        break;
    case 13:
        em_dmg13_005A06C0(em, w);
        break;
    case 14:
        em_dmg14_005A07B0(em, w);
        break;
    case 15:
        em_dmg15_005A08C0(em, w);
        break;
    case 16:
        em_dmg16_005A0B60(em, w);
        break;
    case 17:
        em_dmg17_005A0C40(em, w);
        break;
    case 18:
        em_dmg18_005A0E60(em, w);
        break;
    case 19:
        em_dmg19_005A0F50(em, w);
    default:
        break;
    }
}

static void em_move05_005A2CF0(EMW *em, EM08W *w) {
    u8 temp_v1;

    temp_v1 = EMF(em, u8, 0x15);
    switch (temp_v1) {
    case 0:
        em_die00_005A17A0(em, w);
        break;
    case 1:
        em_die01_005A19D0(em, w);
        break;
    case 2:
        em_die02_005A1BD0(em, w);
        break;
    case 3:
        em_die03_005A1F70(em, w);
        break;
    case 4:
        em_die04_005A21F0(em, w);
        break;
    case 5:
        em_die05_005A22A0(em, w);
    default:
        break;
    }
}

static void em_move06_005A2D90(EMW *em, EM08W *w) {
    u8 temp_a2;

    temp_a2 = EMF(em, u8, 0x15);
    switch (temp_a2) {
    case 0:
        em_demo00_005A0FF0(em, w);
        break;
    case 1:
        em_demo01_005A1700(em, w);
        break;
    }
}

void em08_main(EMW *em) {
    EM08W *w = (EM08W *)em->ex;
    u8 dmg[4];
    u8 flag;
    u8 k;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;

    flag = 0;
    em_mode_timer_sub(em);
    em_no_floor_ck(em);
    if ((em->x8C2 != 1 || em->x8B6 == 0) && em->kind == 8) {
        em_hinshi_ck(em, 0.1f);
        em_thirst_ck(em);
        em_sleep_ck(em);
    }
    switch (Em_Dmg_Sys(em, dmg)) {
    case 1:
    case 2:
        if (em->x9EA != 0) {
            em08_act_set(em, 5, 1, 2);
        } else {
            switch (em->x388) {
            case 4:
            case 1:
                em08_act_set(em, 5, 2, 2);
                break;
            case 2:
                em08_act_set(em, 5, 5, 2);
                break;
            default:
            case 0:
                em08_act_set(em, 5, 0, 2);
                break;
            }
        }
        break;
    case 3:
    case 4:
        Em_Sleep_Flag_Ck(em);
        if (dmg[0] == 0) {
            em->x95A = 0x10;
        } else if (em->x8B6 == 0) {
            em->x95A = 0xA;
        } else {
            em->x95A = 6;
        }
        em_ana_loop_cnt_set(em);
        em08_act_set(em, 4, 0xC, 2);
        break;
    case 15:
        if (em->x388 == 4) {
            em08_act_set(em, 4, 0x11, 2);
        }
        break;
    case 5:
        if (!(em->mode == 4 && em->x15 == 6) && !(em->mode == 4 && em->x15 == 0x11)) {
            em08_act_set(em, 4, 6, 2);
        }
        break;
    case 6:
        flag = 1;
        if (em->x9EA != 0) {
            em_mahi_dmg_timer_set(em);
            em08_act_set(em, 4, 0xE, 2);
        } else if (!(em->mode == 4 && em->x15 == 0xB) && !(em->mode == 4 && em->x15 == 6) &&
                   !(em->mode == 4 && em->x15 == 7) && em->x388 == 1) {
            em_mahi_dmg_timer_set(em);
            em08_act_set(em, 4, 7, 2);
        } else if (!(em->mode == 4 && em->x15 == 0xB) && !(em->mode == 4 && em->x15 == 6) &&
                   !(em->mode == 4 && em->x15 == 7) && em->x388 == 0) {
            em_mahi_dmg_timer_set(em);
            em08_act_set(em, 4, 0xB, 2);
        } else if (!(em->mode == 4 && em->x15 == 0xF) && !(em->mode == 4 && em->x15 == 6) &&
                   !(em->mode == 4 && em->x15 == 0x11) && em->x388 == 4) {
            em_mahi_dmg_timer_set(em);
            em08_act_set(em, 4, 0x11, 2);
        }
        break;
    case 7:
        if (em->x9EA != 0) {
            em_sleep2_dmg_timer_set(em);
            em08_act_set(em, 0, 0xA, 2);
        } else if (!(em->mode == 2 && em->x15 == 0x14) && !(em->mode == 4 && em->x15 == 6) &&
                   !(em->mode == 4 && em->x15 == 0x11) && em->x388 == 4) {
            em08_act_set(em, 4, 0x11, 2);
        } else if (!(em->mode == 0 && em->x15 == 0x10) && !(em->mode == 4 && em->x15 == 6) && em->x388 == 1) {
            em_sleep2_dmg_timer_set(em);
            em08_act_set(em, 0, 0x10, 2);
        } else if (!(em->mode == 0 && em->x15 == 0xC) && !(em->mode == 4 && em->x15 == 6) && em->x388 == 0) {
            em_sleep2_dmg_timer_set(em);
            em08_act_set(em, 0, 0xC, 2);
        }
        break;
    case 8:
        if (!(em->mode == 2 && em->x15 == 0x13) && !(em->mode == 4 && em->x15 == 6) &&
            !(em->mode == 4 && em->x15 == 0x11) && em->x388 == 4) {
            em08_act_set(em, 4, 0x11, 2);
        } else if (!(em->mode == 0 && em->x15 == 0xE) && !(em->mode == 4 && em->x15 == 6) && em->x388 == 1) {
            em_sleep_dmg_timer_set(em);
            em08_act_set(em, 0, 0xE, 2);
        } else if (!(em->mode == 0 && em->x15 == 5) && !(em->mode == 4 && em->x15 == 6) && em->x388 == 0) {
            em_sleep_dmg_timer_set(em);
            em08_act_set(em, 0, 5, 2);
        }
        break;
    case 10:
        switch (em->x15) {
        case 5:
            em08_act_set(em, 0, 6, 2);
            em->x839 = 0;
            break;
        case 8:
            em08_act_set(em, 0, 9, 2);
            em->x839 = 0;
            break;
        case 10:
            em08_act_set(em, 0, 0xB, 2);
            em->x839 = 0;
            break;
        case 12:
            em08_act_set(em, 0, 0xD, 2);
            em->x839 = 0;
            break;
        case 14:
            em08_act_set(em, 0, 0xF, 2);
            em->x839 = 0;
            break;
        case 18:
            em08_act_set(em, 0, 0x13, 2);
            em->x839 = 0;
            break;
        }
        break;
    case 12:
        flag = 1;
        switch (em->x388) {
        case 0:
        case 3:
            k = em->x38E;
            switch (k) {
            case 0:
            case 7:
                em08_act_set(em, 4, 0, 2);
                break;
            case 5:
            case 6:
                em08_act_set(em, 4, 2, 2);
                break;
            case 1:
            case 2:
                em08_act_set(em, 4, 3, 2);
                break;
            default:
                if (em->hagi[k & 0xFF].cnt >= 2) {
                    em08_act_set(em, 4, 5, 2);
                    if (em->x38E != 3) {
                        em->x07 = 0;
                    } else {
                        em->x07 = 1;
                    }
                } else {
                    em08_act_set(em, 4, 1, 2);
                }
                break;
            }
            break;
        case 1:
            em08_act_set(em, 4, 4, 2);
            break;
        case 2:
            em08_act_set(em, 4, 6, 2);
            break;
        case 4:
            em08_act_set(em, 4, 0x11, 2);
            break;
        }
        break;
    case 13:
        flag = 1;
        if (em->x388 != 2) {
            em08_act_set(em, 4, 0, 2);
            em->x839 = 0;
        }
        break;
    case 14:
        flag = 1;
        break;
    }
    if (flag && em->x94E == 0) {
        em->x88B = 1;
    }
    if ((*(s16 *)0x3C7448 == 0x9A) && (em->stg == 0x38) && (Event_flag_ck(0x11) == 0)) {
        if ((*(u8 *)0x3F360F == 1) && (em->mode != 6)) {
            if (em->kind == 8) {
                em08_act_set(em, 6, 0, 1);
            } else {
                em08_act_set(em, 6, 1, 1);
            }
        }
    } else if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em08_main_sub(em, w);
    if (em->x6FF != 0) {
        em08_main_sub(em, w);
        em->x6FF = 0;
    }
    if ((em->x7E9 != 0) && (*(u8 *)0x3F3404 == em->stg)) {
        if (em->x388 == 4) {
            temp_f1 = em->x7E4 - em->x7E0;
            if (!(em->pos[1] <= temp_f1)) {
                em->pos[1] = temp_f1;
            }
            temp_f1_2 = em->x7E4 - 1000.0f;
            if (!(em->x5AC < temp_f1_2)) {
                em->x5AC = temp_f1_2;
            }
            temp_f1_3 = em->x5AC;
            if (em->pos[1] < temp_f1_3) {
                em->pos[1] = temp_f1_3;
            }
        }
        if (em->x388 == 0) {
            temp_f1_4 = em->x7E4;
            em->x5AC = temp_f1_4;
            if (em->pos[1] < temp_f1_4) {
                em->pos[1] = temp_f1_4;
            }
        }
    }
}

void em08_main_sub(EMW *em, EM08W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0:
        em_move00_005A25C0(em, w);
        break;
    case 1:
        em_move01_005A2830(em, w);
        break;
    case 2:
        em_move02_005A28C0(em, w);
        break;
    case 3:
        em_move03_005A2AA0(em, w);
        break;
    case 4:
        em_move04_005A2B70(em, w);
        break;
    case 5:
        em_move05_005A2CF0(em, w);
        break;
    case 6:
        em_move06_005A2D90(em, w);
        break;
    case 7:
        em_move06_005A2D90(em, w);
        break;
    }
    if (em->pos[0] <= 0.0f || em->pos[2] <= 0.0f) {
        em_dur_set(em, 0);
    }
}


#define UV_RESET(i)       \
    do {                  \
        uv[i][0] = 0.0f;  \
        uv[i][1] = 0.0f;  \
        tm[i] = 0xFFFF;   \
        ty[i] = 0xFF;     \
    } while (0)

#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

void em08_uvmove(EMW *em) {
    int i;

    for (i = 0; i < 4; i++) {
        if (em->uvtm[i] != 0xFFFF) {
            em->uvtm[i] += 1;
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

static void sound_call_sub_005A3CA0(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
    Em_se_req2(em, se, 0, pos, 7, 0);
}

static void sound_call_005A3D10(EMW *em, int frame, int se, int joint) {
    f32 pos[3];

    if (em_frame_check(em, (f32)frame, 0)) {
        flmatGetTrans(pos, (u8 *)em->mdl->bone + joint * 400);
        Em_se_req2(em, se, 0, pos, 7, 0);
    }
}

static void quake_call_005A3DB0(EMW *em, int frame, int v) {
    if (em_frame_check(em, (f32)frame, 0)) {
        Em_set_quake_sub(em, v);
    }
}

static void move_default_005A3E00(EMW *em) {
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

static void ef_move_sub_005A3E50(EMW *em, EM08W *w) {
    f32 va[4];
    f32 vb[4];
    f32 vc[4];

    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_005A3D10(em, 0x40, 0x20, 0x23);
        sound_call_005A3D10(em, 0xc8, Code_Make(0x2d, 2, 0x2d, 2), 0x23);
        sound_call_005A3D10(em, 0x19a, Code_Make(0x21, 4, 0x21, 4), 0x23);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x3EB:
        sound_call_005A3D10(em, 0x34, 1, 0x14);
        sound_call_005A3D10(em, 0x74, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 2);
        }
        if (em_frame_check(em, 50.0f, 0)) {
            atk_shell_set(em, 3);
        }
        if (em_frame_check(em, 120.0f, 0)) {
            atk_shell_set(em, 4);
        }
        break;
    case 0x3EC:
        sound_call_005A3D10(em, 0xa, 0x2e, 0x23);
        sound_call_005A3D10(em, 0x1c, 6, 0x14);
        sound_call_005A3D10(em, 0x34, 6, 0x1a);
        sound_call_005A3D10(em, 0x4c, 6, 0x14);
        sound_call_005A3D10(em, 0x64, 6, 0x1a);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 22.0f, 0)) {
            atk_shell_set(em, 0x13);
        }
        if (em_frame_check(em, 16.0f, 0) || em_frame_check(em, 56.0f, 0) || em_frame_check(em, 102.0f, 0)) {
            atk_shell_set(em, 0x14);
        }
        if (em_frame_check(em, 30.0f, 0) || em_frame_check(em, 78.0f, 0)) {
            atk_shell_set(em, 0x15);
        }
        if (em_frame_check(em, 18.0f, 0) || em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 0x1a, 4.0f, 3);
        }
        if (em_frame_check(em, 42.0f, 0) || em_frame_check(em, 90.0f, 0)) {
            Eft13_set_em_scl(em, 0x14, 4.0f, 3);
        }
        break;
    case 0x3ED:
        sound_call_005A3D10(em, 4, 0x21, 0x23);
        sound_call_005A3D10(em, 0x1a, 1, 0x14);
        sound_call_005A3D10(em, 0x38, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 5);
        }
        if (em_frame_check(em, 30.0f, 0)) {
            atk_shell_set(em, 6);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 2.0f, 78.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 1);
                }
            }
        }
        break;
    case 0x3EE:
        sound_call_005A3D10(em, 4, 0x21, 0x23);
        sound_call_005A3D10(em, 0x1a, 1, 0x14);
        sound_call_005A3D10(em, 0x38, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 30.0f, 0)) {
            atk_shell_set(em, 7);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 8);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 2.0f, 78.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 0);
                }
            }
        }
        break;
    case 0x3F2:
        sound_call_005A3D10(em, 6, 0x31, 0x23);
        sound_call_005A3D10(em, 6, 4, 0x1a);
        sound_call_005A3D10(em, 0x4c, 0, 0x14);
        sound_call_005A3D10(em, 0x1c, 0x12, 0x14);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 42.0f, 0)) {
            atk_shell_set(em, 0x11);
        }
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x12);
        }
        if (em_frame_check(em, 74.0f, 0)) {
            atk_shell_set(em, 0x12);
        }
        break;
    case 0x3F3:
        sound_call_005A3D10(em, 4, 0x20, 0x23);
        sound_call_005A3D10(em, 0xe, 0xb, 6);
        sound_call_005A3D10(em, 0x12, 0xb, 0xc);
        sound_call_005A3D10(em, 0x46, 0xb, 6);
        sound_call_005A3D10(em, 0x42, 0xb, 0xc);
        sound_call_005A3D10(em, 0x7a, 0xb, 6);
        sound_call_005A3D10(em, 0x7e, 0xb, 0xc);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x3F9:
        sound_call_005A3D10(em, 6, 0x27, 0x23);
        sound_call_005A3D10(em, 0x1c, 6, 0x14);
        sound_call_005A3D10(em, 0x38, 6, 0x1a);
        break;
    case 0x3FA:
        sound_call_005A3D10(em, 0x38, 7, 0x1a);
        sound_call_005A3D10(em, 0x1a, 0xd, 6);
        sound_call_005A3D10(em, 0x1e, 0xd, 0xc);
        sound_call_005A3D10(em, 0x52, 0xb, 6);
        sound_call_005A3D10(em, 0x56, 0xb, 0xc);
        sound_call_005A3D10(em, 0x88, 0xb, 6);
        sound_call_005A3D10(em, 0x8c, 0xb, 0xc);
        sound_call_005A3D10(em, 0xb6, 0xb, 6);
        sound_call_005A3D10(em, 0xba, 0xb, 0xc);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 42.0f, 0) || em_frame_check(em, 102.0f, 0) || em_frame_check(em, 156.0f, 0)) {
            if (em->pos[1] <= 1000.0f + em->x5AC) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        break;
    case 0x3FB:
        sound_call_005A3D10(em, 2, 0xb, 6);
        sound_call_005A3D10(em, 6, 0xb, 0xc);
        sound_call_005A3D10(em, 0xc, 4, 0x1a);
        sound_call_005A3D10(em, 4, 1, 0x14);
        sound_call_005A3D10(em, 0xe, 9, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0xd);
        }
        if (em_frame_check(em, 16.0f, 0)) {
            ground_land_eff_set_005A7050(em);
        }
        break;
    case 0x401:
        sound_call_005A3D10(em, 4, 0x1f, 0x23);
        sound_call_005A3D10(em, 0x6c, 0x22, 0x23);
        sound_call_005A3D10(em, 0xaa, 0x20, 0x23);
        sound_call_005A3D10(em, 0xd6, 0x22, 0x23);
        sound_call_005A3D10(em, 0x114, 0x27, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x407:
        sound_call_005A3D10(em, 2, 0x2d, 0x23);
        sound_call_005A3D10(em, 0xb6, 0x2d, 0x23);
        sound_call_005A3D10(em, 0x16c, 0x2d, 0x23);
        va[1] = 10.0f;
        va[2] = 140.0f;
        va[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, va, 1.6f);
        break;
    case 0x408:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x46, 0, 0x1a);
        sound_call_005A3D10(em, 0x8e, 3, 0x14);
        sound_call_005A3D10(em, 4, 0x1e, 0);
        sound_call_005A3D10(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x46, 0x2d, 0x23);
        sound_call_005A3D10(em, 0x7e, 3, 0x1a);
        sound_call_005A3D10(em, 0xb6, 3, 0x14);
        sound_call_005A3D10(em, 0x38, 0x1d, 0);
        break;
    case 0x40A:
        sound_call_005A3D10(em, 0x24, 0x2f, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        vb[1] = 10.0f;
        vb[2] = 140.0f;
        vb[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vb, 1.6f);
        break;
    case 0x40C:
        sound_call_005A3D10(em, 0x24, 0x24, 0x23);
        sound_call_005A3D10(em, 0x14, 0, 0x1a);
        sound_call_005A3D10(em, 0x28, 0, 0x14);
        sound_call_005A3D10(em, 0x3c, 0x15, 0x22);
        sound_call_005A3D10(em, 0x3c, 0xc, 0xc);
        sound_call_005A3D10(em, 0xc, 0xc, 6);
        sound_call_005A3D10(em, 0xa6, 0, 0x1a);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 50.0f, 0)) {
            atk_shell_set(em, 1);
        }
        break;
    case 0x40D:
        sound_call_005A3D10(em, 0x1e, 0x20, 0x23);
        sound_call_005A3D10(em, 0xc, 0xc, 0x1a);
        sound_call_005A3D10(em, 0x1c, 2, 0x1a);
        sound_call_005A3D10(em, 0x2e, 3, 0x14);
        sound_call_005A3D10(em, 0x48, 1, 0x1a);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x40E:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x48, 0x16, 0);
        sound_call_005A3D10(em, 0xa4, 9, 0);
        sound_call_005A3D10(em, 0x90, 3, 0xc);
        sound_call_005A3D10(em, 0x88, 0xe, 6);
        sound_call_005A3D10(em, 0xb2, 4, 0x22);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 148.0f, 0)) {
            atk_shell_set(em, 0xf);
        }
        vc[1] = 10.0f;
        vc[2] = 140.0f;
        vc[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, vc, 1.6f);
        if (em_frame_check(em, 156.0f, 0)) {
            Eft20_set(1.0f, em, 0, 0);
        }
        break;
    case 0x413:
        sound_call_005A3D10(em, 8, 0x23, 0x23);
        sound_call_005A3D10(em, 0xe, 0xf, 6);
        sound_call_005A3D10(em, 0x1e, 0x14, 0x2a);
        sound_call_005A3D10(em, 0x2a, 0, 0x1a);
        sound_call_005A3D10(em, 0x48, 1, 0x14);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 30.0f, 0)) {
            atk_shell_set(em, 0x10);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 6.0f, 38.0f)) {
                if (!(*(u16 *)&game_w.x1E & 0x3)) {
                    Eft20_set(1.0f, em, 0x1a, 1);
                }
            }
        }
        break;
    case 0x417:
        sound_call_005A3D10(em, 2, 0x12, 0);
        sound_call_005A3D10(em, 0x50, 0x18, 0x23);
        sound_call_005A3D10(em, 0x14, 0x28, 0x23);
        sound_call_005A3D10(em, 0x4a, 0x29, 0x23);
        sound_call_005A3D10(em, 0x4c, 0xd, 6);
        sound_call_005A3D10(em, 0x48, 0xd, 0xc);
        sound_call_005A3D10(em, 0xb4, 0x13, 6);
        sound_call_005A3D10(em, 0xbb, 0x13, 0xc);
        sound_call_005A3D10(em, 0xf0, 0x12, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 66.0f, 0)) {
            atk_shell_set(em, 0x19);
        }
        break;
    case 0x418:
        sound_call_005A3D10(em, 2, 0x22, 0x23);
        sound_call_005A3D10(em, 0x22, 0x22, 0x23);
        sound_call_005A3D10(em, 0x40, 0x22, 0x23);
        sound_call_005A3D10(em, 0x72, 0x20, 0x23);
        sound_call_005A3D10(em, 0x10, 1, 0x14);
        break;
    case 0x423:
        sound_call_005A3D10(em, 0xc, 0x1d, 0);
        sound_call_005A3D10(em, 6, 0x22, 0x23);
        sound_call_005A3D10(em, 0x68, 3, 0x1a);
        break;
    case 0x424:
        sound_call_005A3D10(em, 6, 0x2b, 0x23);
        sound_call_005A3D10(em, 0x94, 0x22, 0x23);
        sound_call_005A3D10(em, 0x48, 0x13, 0x2a);
        sound_call_005A3D10(em, 0x30, 6, 0x1a);
        sound_call_005A3D10(em, 0x30, 0x11, 0x14);
        sound_call_005A3D10(em, 0x30, 3, 0x14);
        sound_call_005A3D10(em, 0x74, 3, 0x14);
        sound_call_005A3D10(em, 0x94, 3, 0x1a);
        sound_call_005A3D10(em, 0xc0, 3, 0x1a);
        sound_call_005A3D10(em, 0x34, 0xc, 6);
        sound_call_005A3D10(em, 0x38, 0xd, 0xc);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 44.0f, 0)) {
            Eft13_set_em_scl(em, 0x1b, 1.0f, 7);
        }
        if (em_frame_check(em, 62.0f, 0)) {
            Eft13_set_em_scl(em, 0x16, 1.0f, 7);
        }
        if (em_frame_check(em, 80.0f, 0)) {
            Eft20_set(0.5f, em, 1, 0x80);
        }
        break;
    case 0x425:
        sound_call_005A3D10(em, 6, 0x2b, 0x23);
        sound_call_005A3D10(em, 0xa, 0x12, 0x1a);
        sound_call_005A3D10(em, 0x26, 6, 0x1a);
        sound_call_005A3D10(em, 0x4e, 4, 0x14);
        sound_call_005A3D10(em, 0x6e, 3, 0x14);
        sound_call_005A3D10(em, 0x5a, 0x15, 0);
        sound_call_005A3D10(em, 0xac, 3, 0x1a);
        if (em_frame_check(em, 24.0f, 0)) {
            Eft13_set_em_scl(em, 0x1a, 1.0f, 7);
        }
        break;
    case 0x426:
        sound_call_005A3D10(em, 4, 0x27, 0x23);
        sound_call_005A3D10(em, 0x56, 0x1f, 0x23);
        sound_call_005A3D10(em, 0x10, 3, 0x14);
        sound_call_005A3D10(em, 4, 0xd, 6);
        sound_call_005A3D10(em, 8, 0xd, 0xc);
        sound_call_005A3D10(em, 0x62, 0x13, 0x2a);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x427:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0);
        sound_call_005A3D10(em, 0xe, 1, 0x1a);
        sound_call_005A3D10(em, 0x4c, 0x1f, 0x23);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x428:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0);
        sound_call_005A3D10(em, 0x44, 0x20, 0x23);
        sound_call_005A3D10(em, 0x7a, 3, 0x1a);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x42A:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 268.0f, 0)) {
            Eft20_set(0.9f, em, 0, 0);
        }
        break;
    case 0x42B:
        sound_call_005A3D10(em, 0x46, 0x2d, 0x23);
        sound_call_005A3D10(em, 0x46, 0x1e, 0);
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x42C:
        sound_call_005A3D10(em, 0x1a, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x2e, 9, 0);
        sound_call_005A3D10(em, 4, 0x14, 0);
        sound_call_005A3D10(em, 0x1a, 0x2e, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 50.0f, 0)) {
            Eft20_set(0.8f, em, 0, 0);
        }
        break;
    case 0x42D:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x13, 0x2a);
        sound_call_005A3D10(em, 0x18, 9, 0);
        sound_call_005A3D10(em, 0x18, 0x11, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft20_set(1.0f, em, 0x12, 0x80);
        }
        break;
    case 0x42E:
        sound_call_005A3D10(em, 0x14, 8, 0);
        sound_call_005A3D10(em, 0x24, 4, 0x22);
        sound_call_005A3D10(em, 4, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x46, 8, 0);
        sound_call_005A3D10(em, 0x2a, 0x12, 0);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 16.0f, 0) || em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x42F:
        sound_call_005A3D10(em, 4, 0x20, 0x23);
        sound_call_005A3D10(em, 0x3e, 1, 0x14);
        sound_call_005A3D10(em, 0x38, 6, 0x1a);
        sound_call_005A3D10(em, 4, 0x1d, 0);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x432:
        sound_call_005A3D10(em, 4, 0x2a, 0);
        sound_call_005A3D10(em, 4, 0x13, 0x2a);
        sound_call_005A3D10(em, 0x18, 9, 0);
        sound_call_005A3D10(em, 0x18, 0x11, 0);
        hire_req_set_005A6F90(em, w, 1);
        if (em_frame_check(em, 26.0f, 0)) {
            Eft20_set(1.0f, em, 0x12, 0x81);
        }
        break;
    case 0x433:
        sound_call_005A3D10(em, 0x14, 8, 0);
        sound_call_005A3D10(em, 0x24, 4, 0x22);
        sound_call_005A3D10(em, 4, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x46, 8, 0);
        sound_call_005A3D10(em, 0x2a, 0x12, 0);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 16.0f, 0) || em_frame_check(em, 66.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x434:
        sound_call_005A3D10(em, 0x10, 0x2a, 0x23);
        sound_call_005A3D10(em, 0x66, 0x2b, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x437:
        sound_call_005A3D10(em, 0xc, 0x24, 0x23);
        sound_call_005A3D10(em, 0x24, 0x22, 0x23);
        sound_call_005A3D10(em, 0x66, 0x22, 0x23);
        sound_call_005A3D10(em, 0xa4, 0x22, 0x23);
        break;
    case 0x447:
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x448:
        hire_req_set_005A6F90(em, w, 1);
        break;
    case 0x44C:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x44D:
        sound_call_005A3D10(em, 4, 0x65, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x44E:
        sound_call_005A3D10(em, 4, 0x17, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x44F:
        sound_call_005A3D10(em, 4, 0x24, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 0x20, 0x15, 0x14);
        sound_call_005A3D10(em, 0x24, 0x15, 0x1a);
        sound_call_005A3D10(em, 0x46, 0x17, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 10.0f, 0)) {
            atk_shell_set(em, 0x1a);
        }
        break;
    case 0x450:
        sound_call_005A3D10(em, 0x14, 0x2e, 0x23);
        sound_call_005A3D10(em, 4, 0x15, 0x23);
        sound_call_005A3D10(em, 0x22, 0x30, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x451:
        sound_call_005A3D10(em, 4, 0x15, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x30, 0x2a);
        sound_call_005A3D10(em, 0x30, 0x30, 0x2a);
        sound_call_005A3D10(em, 4, 0x1d, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x452:
        sound_call_005A3D10(em, 4, 0x16, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x453:
        sound_call_005A3D10(em, 4, 0x16, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x454:
        sound_call_005A3D10(em, 4, 0x2e, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 0x30, 0x30, 0x2a);
        sound_call_005A3D10(em, 4, 0x16, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 10.0f, 0)) {
            atk_shell_set(em, 0xe);
        }
        break;
    case 0x457:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x458:
        sound_call_005A3D10(em, 0x34, 0x29, 0x23);
        sound_call_005A3D10(em, 0x34, 0x18, 0x23);
        sound_call_005A3D10(em, 0x6a, 0x16, 0);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 4, 0x19, 0);
        hire_req_set_005A6F90(em, w, 0);
        quake_call_005A3DB0(em, 0x1a, 3);
        if (em_frame_check(em, 26.0f, 0)) {
            em08_vib_set(em);
            atk_shell_set(em, 0x1d);
        }
        break;
    case 0x459:
        sound_call_005A3D10(em, 4, 0x33, 0x23);
        sound_call_005A3D10(em, 2, 0x16, 0x2a);
        if (em_frame_check(em, 28.0f, 0)) {
            atk_shell_set(em, 0x1b);
        }
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45A:
        sound_call_005A3D10(em, 4, 0x16, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45B:
        sound_call_005A3D10(em, 4, 0x16, 0x2a);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45C:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45D:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x45E:
        sound_call_005A3D10(em, 0x26, 0x24, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0x14);
        break;
    case 0x45F:
        sound_call_005A3D10(em, 4, 0x23, 0x23);
        sound_call_005A3D10(em, 0x42, 0x13, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x18);
        }
        break;
    case 0x460:
        sound_call_005A3D10(em, 4, 0x16, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x12, 0x14);
        sound_call_005A3D10(em, 0x1e, 0x15, 0);
        sound_call_005A3D10(em, 0x1e, 0x14, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x461:
        sound_call_005A3D10(em, 0xe, 0x10, 0);
        sound_call_005A3D10(em, 0xa, 9, 0);
        sound_call_005A3D10(em, 0x38, 8, 0x23);
        sound_call_005A3D10(em, 0x38, 0x12, 0x14);
        sound_call_005A3D10(em, 0x1e, 0x20, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 10.0f, 0)) {
            Eft20_set(0.7f, em, 0, 0x89);
        }
        break;
    case 0x462:
        sound_call_005A3D10(em, 4, 0x35, 0);
        sound_call_005A3D10(em, 4, 0x16, 0);
        sound_call_005A3D10(em, 0x32, 0x35, 0);
        sound_call_005A3D10(em, 0x46, 0x35, 0);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x17);
        }
        if (em_frame_check(em, 24.0f, 0) || em_frame_check(em, 42.0f, 0) || em_frame_check(em, 62.0f, 0) || em_frame_check(em, 82.0f, 0)) {
            Eft13_set_em_scl(em, 6, 8.0f + (f32)((u16)ran_suu(1) & 0x3FF) * 0.0005f, 3);
        }
        if (em_frame_check(em, 10.0f, 0) || em_frame_check(em, 32.0f, 0) || em_frame_check(em, 52.0f, 0) || em_frame_check(em, 74.0f, 0)) {
            Eft13_set_em_scl(em, 0xc, 8.0f + (f32)((u16)ran_suu(1) & 0x3FF) * 0.0005f, 3);
        }
        break;
    case 0x464:
        sound_call_005A3D10(em, 0x18, 0x12, 0x14);
        sound_call_005A3D10(em, 0x18, 0x15, 0);
        sound_call_005A3D10(em, 0x36, 0, 0x14);
        sound_call_005A3D10(em, 0x3c, 4, 0x1a);
        sound_call_005A3D10(em, 4, 0x16, 0);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x466:
        sound_call_005A3D10(em, 4, 0x2b, 0x23);
        sound_call_005A3D10(em, 0x5a, 0x30, 0x2a);
        hire_req_set_005A6F90(em, w, 3);
        break;
    case 0x467:
        sound_call_005A3D10(em, 0x1a, 0x2c, 0x23);
        sound_call_005A3D10(em, 0x2e, 8, 0);
        sound_call_005A3D10(em, 4, 0x14, 0);
        sound_call_005A3D10(em, 0x1a, 0x2e, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 42.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x468:
        sound_call_005A3D10(em, 4, 0x2c, 0x23);
        sound_call_005A3D10(em, 4, 0x10, 0);
        sound_call_005A3D10(em, 0x32, 9, 0);
        sound_call_005A3D10(em, 4, 0x1d, 0);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 42.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x469:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 4, 0x10, 0);
        sound_call_005A3D10(em, 0x32, 9, 0x23);
        sound_call_005A3D10(em, 4, 0x16, 0x23);
        hire_req_set_005A6F90(em, w, 3);
        if (em_frame_check(em, 46.0f, 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        break;
    case 0x46A:
        sound_call_005A3D10(em, 4, 0x1d, 0);
        sound_call_005A3D10(em, 0x86, 0x1d, 0);
        sound_call_005A3D10(em, 0x86, 0x2f, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x46B:
        sound_call_005A3D10(em, 4, 0x17, 0);
        sound_call_005A3D10(em, 0x20, 0x16, 0);
        sound_call_005A3D10(em, 0x6e, 0x15, 0x23);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x46C:
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x46D:
        sound_call_005A3D10(em, 0xa, 8, 0x14);
        sound_call_005A3D10(em, 4, 0x1f, 0x23);
        sound_call_005A3D10(em, 0xa, 0x12, 0x14);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 6.0f, 0) || em_frame_check(em, 10.0f, 0)) {
            Eft13_set_em_scl(em, 2, 7.0f, 3);
        }
        break;
    case 0x46E:
        sound_call_005A3D10(em, 4, 0x34, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x30, 0);
        sound_call_005A3D10(em, 4, 0x16, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 2.0f, 0)) {
            atk_shell_set(em, 0x1c);
        }
        break;
    case 0x46F:
        sound_call_005A3D10(em, 4, 0x34, 0x23);
        sound_call_005A3D10(em, 0x1e, 0x30, 0);
        sound_call_005A3D10(em, 4, 0x16, 0x22);
        hire_req_set_005A6F90(em, w, 0);
        break;
    case 0x470:
        sound_call_005A3D10(em, 4, 0x12, 0x1a);
        sound_call_005A3D10(em, 0x12, 6, 0x1a);
        sound_call_005A3D10(em, 0x66, 4, 0x14);
        sound_call_005A3D10(em, 0xd8, 1, 0x1a);
        sound_call_005A3D10(em, 0x108, 3, 0x14);
        sound_call_005A3D10(em, 0x38, 0x15, 0);
        sound_call_005A3D10(em, 0x38, 0x25, 0);
        sound_call_005A3D10(em, 4, 0x1f, 0x23);
        sound_call_005A3D10(em, 0x54, 0x1d, 0);
        hire_req_set_005A6F90(em, w, 0);
        if (em_frame_check(em, 60.0f, 0)) {
            atk_shell_set(em, 0x16);
        }
        break;
    case 0x471:
        sound_call_005A3D10(em, 4, 0x2f, 0x23);
        sound_call_005A3D10(em, 0x122, 0x2d, 0x23);
        break;
    case 0x472:
        sound_call_005A3D10(em, 4, 0x2a, 0x23);
        sound_call_005A3D10(em, 0x50, 0x29, 0x23);
        sound_call_005A3D10(em, 0x9e, 0x2c, 0x23);
        break;
    default:
        move_default_005A3E00(em);
        break;
    }
}

static void hire_move_sub2_005A69A0(EMW *em, EM08W *w, int i) {
    switch (w->st[i].b) {
    case 0:
        if (w->xF == 2 || w->xF == 3) {
            if (w->ang[i].a != hire_down_angx_003887E8[i]) {
                w->st[i].b = 2;
                w->tmr[i] = 0x14;
            }
        } else if (w->xF == 1) {
            if (w->ang[i].a != 0) {
                w->st[i].b = 1;
                w->tmr[i] = 0x14;
            }
        }
        break;
    case 1:
        if (--w->tmr[i] <= 0) {
            w->ang[i].a = 0;
            w->st[i].b = 0;
            return;
        }
        w->ang[i].a += ((0x10000 - w->ang[i].a) / w->tmr[i]) & 0xFFFF;
        break;
    case 2:
        if (--w->tmr[i] <= 0) {
            w->ang[i].a = hire_down_angx_003887E8[i];
            w->st[i].b = 0;
            return;
        }
        w->ang[i].a -= (((0x10000 - (hire_down_angx_003887E8[i] - w->ang[i].a)) & 0xFFFF) / w->tmr[i]) & 0xFFFF;
        break;
    }
}

static void hire_move_sub1_005A6B70(EMW *em, EM08W *w, int i) {
    switch (w->st[i].a) {
    case 0:
        if (w->xF == 2) {
            w->st[i].a++;
            w->tm[i] = hire_start_timer_tbl1_003887D0[i];
        } else if (w->xF == 1) {
            w->st[i].a++;
            w->tm[i] = hire_start_timer_tbl0_003887C8[i];
        }
        break;
    case 1:
        if (--w->tm[i] <= 0) {
            if (w->xF == 2) {
                w->st[i].a = 3;
            } else {
                w->st[i].a = 2;
            }
            w->tm[i] = 0;
            w->cnt[i] = 0;
        }
        break;
    case 2: {
        HIRE_E *tbl;
        HIRE_E *p;
        s16 cnt0;
        u16 cnt;
        u8 k;

        cnt0 = w->cnt[i];
        w->cnt[i] = cnt0 + 1;
        cnt = cnt0;
        tbl = hire_normal_add_tbl_00659250[i];
        p = tbl;
        k = 0;
        for (;;) {
            if (k && !p->t) {
                k |= 0x80;
                break;
            }
            if (p->t < cnt) {
                p++;
                k++;
                continue;
            }
            break;
        }
        if (k & 0x80) {
            if (w->xF == 2) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl1_003887E0[i];
            } else if (w->xF == 1) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl0_003887D8[i];
            } else {
                w->st[i].a = 0;
            }
            w->ang[i].b = tbl[k & 0x7F].v;
            return;
        }
        w->ang[i].b += tbl[k].v;
        break;
    }
    case 3: {
        HIRE_E *tbl;
        HIRE_E *p;
        s16 cnt0;
        u16 cnt;
        u8 k;

        cnt0 = w->cnt[i];
        w->cnt[i] = cnt0 + 1;
        cnt = cnt0;
        tbl = hire_down_add_tbl_00659300[i];
        p = tbl;
        k = 0;
        for (;;) {
            if (k && !p->t) {
                k |= 0x80;
                break;
            }
            if (p->t < cnt) {
                p++;
                k++;
                continue;
            }
            break;
        }
        if (k & 0x80) {
            if (w->xF == 2) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl1_003887E0[i];
            } else if (w->xF == 1) {
                w->st[i].a = 1;
                w->tm[i] = hire_remove_timer_tbl0_003887D8[i];
            } else {
                w->st[i].a = 0;
            }
            w->ang[i].b = tbl[k & 0x7F].v;
            return;
        }
        w->ang[i].b += tbl[k].v;
        break;
    }
    }
}

static void hire_move_005A6EB0(EMW *em, EM08W *w) {
    int i;

    i = 0;
    do {
        hire_move_sub1_005A6B70(em, w, i);
        hire_move_sub2_005A69A0(em, w, i);
        i++;
    } while (i < 4);
}

void em08_effect_move(EMW *em) {
    EM08W *w = (EM08W *)em->ex;
    u8 e = em->ex[0];

    switch (e) {
    case 0:
        em->ex[0] = e + 1;
        break;
    case 1:
        ef_move_sub_005A3E50(em, w);
        hire_move_005A6EB0(em, w);
        break;
    }
    em08_uvmove(em);
}

void hire_req_set_005A6F90(EMW *em, EM08W *w, u8 req) {
    w->xF = req;
    if (w->xF == 1 && em->x302 <= (s16)(0.1f * (f32)em->x792)) {
        w->xF = 2;
    }
    if (w->xF == 3 && em->x302 > (s16)(0.1f * (f32)em->x792)) {
        w->xF = 0;
    }
}

void ground_land_eff_set_005A7050(EMW *em) {
    Eft20_set(1.0f, em, 0xB, 0);
}

void swim_eff_set_005A7070(f32 scale, EMW *em) {
    f32 v[3];

    if (*(u8 *)0x3F3404 == em->stg && !(*(u16 *)0x3F340E & 3) &&
        !(em->pos[1] < (em->x7E4 - em->x7E0) - 100.0f)) {
        SetVector(v, em->pos[0], em->x7E4, em->pos[2]);
        Eft08_set(v, 2, 2, scale * EMF(em, f32, 0xB8));
    }
}

void swim_eff_set2_005A7120(f32 scale, EMW *em) {
    f32 v[3];

    if (*(u8 *)0x3F3404 == em->stg) {
        SetVector(v, em->pos[0], em->x7E4, em->pos[2]);
        Eft08_set(v, 5, 2, scale * EMF(em, f32, 0xB8));
        sound_call_sub_005A3CA0(em, 0x1A, 6);
    }
}

void em21_target_ang_calc(EMW *em, int arg1)
{
  int d;
  int a;
  int t;
  d = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
  a = em->ang[1];
  ;
  t = arg1;
  t = t & 0xFFFF;
  if (((d - a) & 0xFFFF) < 0x8001)
  {
    em->pos = em->pos;
    if (0, ((d - a) & 0xFFFF) < t)
    {
      em->ang[1] = a + ((d - a) & 0xFFFF);
    }
    else
    {
      em->ang[1] = a + t;
    }
  }
  else
    if ((0x10000 - t) < ((d - a) & 0xFFFF))
  {
    em->ang[1] = a + ((d - a) & 0xFFFF);
  }
  else
  {
    em->ang[1] = a - t;
  }
}

void atk_shell_set(EMW *em, int no) {
    if (em->kind == 8) {
        shell21_set(em, no);
    } else {
        shell23_set(em, no);
    }
}

void em08_vib_set(EMW *em) {
    PLW *pl;
    s8 i;

    pl = player_work;
    i = 0;
    if (0 < *(u8 *)0x3F34C3) {
        do {
            if (i == *(u8 *)0x3F34C1 && pl->be_flag != 0 && Pl_stg_ck_tw(em, pl) != 0 && pl->be_flag != 0 &&
                flvecCalcDistance(pl->pos, em->pos) <= 1000.0f) {
                vib_set_pl(pl, 3);
            }
            i++;
            pl = (PLW *)((u8 *)pl + 0xA00);
        } while (i < *(u8 *)0x3F34C3);
    }
}

/* A file static in the original; data tables point at it. */
void dummy_em_prog_005A7370(void) {
}
