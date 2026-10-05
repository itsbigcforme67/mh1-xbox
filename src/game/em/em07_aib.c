/* em07_ai, run 2: em_act00_0058FBF0 .. em_mv01_005903A0 (game.bin 0x0058FBF0-0x00590504). Matching functions of em07_ai_nm.c (that file holds the
 * whole code including the near-matches); see it for the description. */
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
    u8 _pad00;
    u8 eff;             /* 0x01 effect script step (em07_effect_move) */
    u8 _pad02[2];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    s8 x06;             /* 0x06 */
    u8 _pad07;
    s8 item_pt;         /* 0x08 pick point number of the item */
    s8 x09;             /* 0x09 */
    s16 x0A;            /* 0x0A */
    f32 xC;             /* 0x0C */
    f32 dist;           /* 0x10 distance to the target (500 with none) */
    u16 dang;           /* 0x14 angle left to turn */
    u8 has_tgt;         /* 0x16 */
    u8 x17;             /* 0x17 */
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
void sound_call_005937E0(EMW *em, int frame, int se, int joint, int vol);
void sound_call_mov(EMW *em, int f0, int f1, int se, int joint, int vol);
void sound_call_mov2(EMW *em, int f0, int f1, int se, int joint, int vol);
void quake_call_00593A90(EMW *em, int frame, int v);
void move_default_00593AE0(EMW *em);
void ef_move_sub_00593B30(EMW *em, EM07W *w);
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

void shell05_set(EMW *, int);
void shell05_set2(EMW *, f32 *, int);
void Quest_failed_set(EMW *);

void em07_main_sub(EMW *em);

#define UV_RESET(i)       \
    do {                  \
        uv[i][0] = 0.0f;  \
        uv[i][1] = 0.0f;  \
        tm[i] = 0xFFFF;   \
        ty[i] = 0xFF;     \
    } while (0)

void em_act00_0058FBF0(EMW *em) {
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

void em_act01_0058FC80(EMW *em) {
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

void em_act02_0058FD50(EMW *em) {
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
        em->x87F = 5;
        if (em->x194 == 0) {
            em->x05++;
            em->x87F = 0;
            em07_to_normal(em);
        }
        break;
    }
}

void em_act03_0058FE10(EMW *em) {
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

void em_act04_0058FEB0(EMW *em) {
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

void em_act05_0058FF30(EMW *em) {
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

void em_act06_0058FFD0(EMW *em) {
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

void em_act07_005900D0(EMW *em) {
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

void em_move00_00590180(EMW *em) {
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

void em_mv00_00590240(EMW *em) {
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

void em_mv01_005903A0(EMW *em) {
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
