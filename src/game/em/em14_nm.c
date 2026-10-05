/* em14 draft */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em14.h"

#define GAME_X1E16 (*(u16 *)((u8 *)&game_w + 0x1E))
typedef struct EML {
    s32 v;              /* 0x194 + i * 0x50: layer i is busy while non-zero (as EMW.x194) */
    u8 _pad[0x4C];
} EML;
#define EM_LYR(em, i) (((EML *)&(em)->x194)[i].v)
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))


f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_rate_clear(EMW *);
void em_char_set(EMW *, int, int, int);
void SetVector(f32 *, f32, f32, f32);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
void em_act_set(EMW *, int, u16);
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
void em_cmd_ck(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
void Em_Mahi_Start(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Eft13_set_em_scl(EMW *, int, f32, int);
void em_char_set2(EMW *, int, int, int, int);
void flmatGetTrans(f32 *, void *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void em_mahi_eff_set(EMW *, int);
u16 em_act_search(void *);
void em_action_timer_calc(EMW *, int);
int Event_flag_ck();
void em_dur_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void Eft15_set3(EMW *, int, f32, int);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
int em_frame_check3(EMW *, int, f32, f32);
void em14_act_set(EMW *em, int kind, u16 no, u16 arg);
int Em_stg_ck(EMW *);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_no_battle_area_ck(EMW *, int, int);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void Eft13_set_pos2(f32, EMW *, f32 *, int);
void get_joint_pos_em(EMW *, int, f32 *);
void em17_act_set(EMW *em, int kind, u16 no, u16 arg);
void em_range_set(EMW *em, s8 no);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_suimin_end(EMW *em);
void Em_Suimin_Start(EMW *em);
void em_no_floor_ck(EMW *em);
void em_hp_add(EMW *em, s16 n);
void em_ana_loop_cnt_set(EMW *em);
void em_tail_off_sub(EMW *em);
void em_sleep2_dmg_timer_set(EMW *em);
void Eft13_set_pos(f32, f32 *, int);
void AddVector(f32 *, f32 *, f32 *);
void Eft17_set_ex(f32 *, int, int, f32);
void vib_set(int, int);
void Quest_enemy_hagi_set();
extern s16 em14_stay_timer_tbl[];
extern s16 em14_runaway_timer_tbl[];

void em14_local_init(EMW *em);
void em14_init(EMW *em);
static void act_dist_select_005B5650(EMW *em);
void em14_to_normal(EMW *em, s16 a, s16 b);
void em14_to_swim(EMW *em);
void em14_to_fly(EMW *em, int flag);
void em14_frame_reset(EMW *em, int i);
static void em_act00_005B58E0(EMW *em, EM14W *w);
static void em_act01_005B59C0(EMW *em, EM14W *w);
static void em_act02_005B5AB0(EMW *em, EM14W *w);
static void em_act03_005B5B80(EMW *em, EM14W *w);
static void em_act04_005B5C10(EMW *em, EM14W *w);
static void em_act05_005B5CB0(EMW *em, EM14W *w);
static void em_act06_005B5DA0(EMW *em, EM14W *w);
static void em_act07_005B5E70(EMW *em, EM14W *w);
static void em_act08_005B5F10(EMW *em, EM14W *w);
static void em_act09_005B5F20(EMW *em, EM14W *w);
static void em_act10_005B5FC0(EMW *em, EM14W *w);
static void em_act11_005B60D0(EMW *em, EM14W *w);
static void em_act12_005B61A0(EMW *em, EM14W *w);
static void em_act13_005B61B0(EMW *em, EM14W *w);
static void em_act14_005B6280(EMW *em, EM14W *w);
static void em_act15_005B6290(EMW *em, EM14W *w);
static void em_act16_005B63C0(EMW *em, EM14W *w);
static void em_act17_005B64F0(EMW *em, EM14W *w);
static void em_act18_005B6570(EMW *em, EM14W *w);
static void em_act19_005B66D0(EMW *em, EM14W *w);
static void em_act20_005B66E0(EMW *em, EM14W *w);
static void em_act21_005B67F0(EMW *em, EM14W *w);
static void em_act22_005B6950(EMW *em, EM14W *w);
static void em_act23_005B69D0(EMW *em, EM14W *w);
static void em_act24_005B6AA0(EMW *em, EM14W *w);
static void em_act25_005B6B70(EMW *em, EM14W *w);
static void em_act26_005B6BF0(EMW *em, EM14W *w);
static void em_act27_005B6C00(EMW *em, EM14W *w);
static void em_act28_005B6D10(EMW *em, EM14W *w);
static void em_act29_005B6DE0(EMW *em, EM14W *w);
static void em_act33_005B6FD0(EMW *em, EM14W *w);
static void em_mv00_005B7060(EMW *em, EM14W *w);
static void em_mv01_005B71C0(EMW *em, EM14W *w);
static void em_mv02_005B71D0(EMW *em, EM14W *w);
static void em_mv03_005B72E0(EMW *em, EM14W *w);
static void em_mv04_005B75B0(EMW *em, EM14W *w);
static void em_mv05_005B76F0(EMW *em, EM14W *w);
static void em_mv06_005B79C0(EMW *em, EM14W *w);
static void em_mv07_005B7B20(EMW *em, EM14W *w);
static void em_fly00_005B7C60(EMW *em, EM14W *w);
static void em_fly01_005B7D30(EMW *em, EM14W *w);
static void em_fly02_005B7DC0(EMW *em, EM14W *w);
static void em_fly03_005B7EE0(EMW *em, EM14W *w);
static void em_fly04_005B8090(EMW *em, EM14W *w);
static void em_fly05_005B8260(EMW *em, EM14W *w);
static void em_fly06_005B83C0(EMW *em, EM14W *w);
static void em_fly07_005B84C0(EMW *em, EM14W *w);
static void em_fly08_005B85D0(EMW *em, EM14W *w);
static void em_fly09_005B86E0(EMW *em, EM14W *w);
static void em_fly10_005B8800(EMW *em, EM14W *w);
static void em_fly11_005B8890(EMW *em, EM14W *w);
static void em_fly12_005B89D0(EMW *em, EM14W *w);
static void em_fly14_005B8C10(EMW *em, EM14W *w);
static void em_fly23_005B8CD0(EMW *em, EM14W *w);
static void em_atk00_005B8F00(EMW *em, EM14W *w);
static void em_atk03_005B8FA0(EMW *em, EM14W *w);
static void em_atk06_005B9020(EMW *em, EM14W *w);
static void em_atk15_005B9110(EMW *em, EM14W *w);
static void em_atk16_005B9250(EMW *em, EM14W *w);
static void em_atk17_005B9390(EMW *em, EM14W *w);
static void em_atk26_005B94D0(EMW *em, EM14W *w);
static void em_atk27_005B9570(EMW *em, EM14W *w);
static void em_atk28_005B9610(EMW *em, EM14W *w);
static void em_atk29_005B9690(EMW *em, EM14W *w);
static void em_atk30_005B9730(EMW *em, EM14W *w);
static void em_atk31_005B97B0(EMW *em, EM14W *w);
static void em_dmg00_005B9830(EMW *em, EM14W *w);
static void em_dmg01_005B98C0(EMW *em, EM14W *w);
static void em_dmg02_005B9950(EMW *em, EM14W *w);
static void em_dmg03_005B99E0(EMW *em, EM14W *w);
static void em_dmg04_005B9A70(EMW *em, EM14W *w);
static void em_dmg05_005B9C40(EMW *em, EM14W *w);
static void em_dmg06_005B9D60(EMW *em, EM14W *w);
static void em_dmg07_005B9ED0(EMW *em, EM14W *w);
static void em_dmg08_005BA140(EMW *em, EM14W *w);
static void em_dmg09_005BA2A0(EMW *em, EM14W *w);
static void em_dmg10_005BA400(EMW *em, EM14W *w);
static void em_dmg11_005BA490(EMW *em, EM14W *w);
static void em_dmg13_005BA560(EMW *em, EM14W *w);
static void em_dmg14_005BA660(EMW *em, EM14W *w);
static void em_dmg15_005BA800(EMW *em, EM14W *w);
static void em_dmg16_005BA880(EMW *em, EM14W *w);
static void em_dmg17_005BA9A0(EMW *em, EM14W *w);
static void em_dmg18_005BAA90(EMW *em, EM14W *w);
static void em_demo00_005BAB80(EMW *em, EM14W *w);
static void em_die00_005BAEF0(EMW *em, EM14W *w);
static void em_die01_005BB090(EMW *em, EM14W *w);
static void em_die02_005BB290(EMW *em, EM14W *w);
static void em_move00_005BB460(EMW *em, EM14W *w);
static void em_move01_005BB690(EMW *em, EM14W *w);
static void em_move02_005BB760(EMW *em, EM14W *w);
static void em_move03_005BB900(EMW *em, EM14W *w);
static void em_move04_005BBA60(EMW *em, EM14W *w);
static void em_move05_005BBBC0(EMW *em, EM14W *w);
static void em_move06_005BBC30(EMW *em, EM14W *w);
void em14_main(EMW *em);
void em14_main_sub(EMW *em, EM14W *w);
void em14_uvmove(EMW *em);
static void sound_call_sub_005BCB30(EMW *em, int se, int joint);
static void sound_call_005BCBA0(EMW *em, int frame, int se, int joint);
static void sound_call_parts_005BCC00(EMW *em, int frame, int se, int joint, u8 layer);
static void quake_call_005BCCA0(EMW *em, int frame, int arg);
void Em_set_quake_sub(EMW *, int);
static void move_default_005BCCF0(EMW *em);
static void ef_move_sub_005BCD40(EMW *em, EM14W *w);
void em14_effect_move(EMW *em);
static void ground_land_eff_set_005C1120(EMW *em);
void em14_atk_end_sel(EMW *em, EM14W *w);
void dummy_em_prog_005C1250(void);


void em14_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *) em, 0);
}

void em14_init(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
    int hp;
    u8 temp_a0;
    u8 temp_v1_2;

    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
        switch (game_w.stage) {
        case 0x33:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 11000.0f;
            break;
        case 0x34:
            em->pos[0] = 12000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 6000.0f;
            break;
        case 0x35:
            em->pos[0] = 12000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 6000.0f;
            break;
        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            break;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 4;
    em14_act_set(em, 2, 1, 0);
    w->x06 = 0;
    if (em->kind == 0xE) {
        hp = em_hp_vital_set2(em, 0x7D0, 0x5DC);
        em->x302 = hp;
    } else {
        hp = em_hp_vital_set2(em, 0x640, 0x578);
        em->x302 = hp;
    }
    em->x792 = hp;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em14_stay_timer_tbl[em->stg];
    em->runaway_tm = em14_runaway_timer_tbl[em->stg];
    w->tgt_ang = 0x4000;
    w->x3C = 0x100;
    w->turn = 0x200;
    w->x44 = 0x100;
    w->x20 = 0x2000;
    w->x1C = 0;
    w->x1A = 0;
    w->x1B = 0;
    em->x734 = 3;
    M2C_FIELD(em, u8 *, 0x735) = 0;
    em->x7E0 = 1000.0f;
    temp_v1_2 = em->x948 & 1;
    em->x948 = temp_v1_2;
    if (temp_v1_2 == 0) {
        temp_a0 = em->kind;
        switch (temp_a0) {
        case 0xE:
        case 0x1A:
            em->ex[0xA3] = 0;
            eft09_set(em, 0x2000, 0x200, 0x100);
            break;
        }
    }
}

static void act_dist_select_005B5650(EMW *em) {
    switch (em->x734) {
    case 3:
        em->x839 = 1;
        em14_act_set(em, 0, 1, 0);
        break;
    }
}

void em14_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x734 != 0) {
        act_dist_select_005B5650(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        em14_act_set(em, 0, 1, 0);
        return;
    }
    if (em->char0 != 0x3E9) {
        em_char_set(em, 1, a, b);
    }
    if (em->x2DE != 0x44D) {
        em_char_set(em, 1, a, b);
    }
    if (em->x2E0 != 0x4B1) {
        em_char_set(em, 1, a, b);
    }
    em->x388 = 0;
    em->x3F4 = 0;
    em14_act_set(em, 0, 1, 0);
}

void em14_to_swim(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 4;
    em->x3F4 = 0;
    em->x839 = 1;
    em14_act_set(em, 2, 1, 0);
}

void em14_to_fly(EMW *em, int flag) {
    em->x839 = 1;
    em->act_spd = 1.0f;
    switch (flag & 0xFF) {
    case 0:
        em_act_set(em, 2, 0xE);
        break;
    }
}

void em14_frame_reset(EMW *em, int i) {
    if (EM_LYR(em, i) == 0) {
        switch (i) {
        case 0:
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
            break;
        case 1:
            em_char_set2(em, 0x4B1, 0xA, 0, 1);
            break;
        case 2:
            em_char_set2(em, 0x579, 0xA, 0, 2);
            break;
        }
    }
}

static void em_act00_005B58E0(EMW *em, EM14W *w) {
    s32 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
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
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x839 = 1;
        }
        break;
    }
}

extern u8 *em14_act_add[3];

static void em_act01_005B59C0(EMW *em, EM14W *w) {
    u16 temp_a2;
    u8 temp_a0;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (em->x8C3 == 0) {
            temp_a0 = em->x734;
            if (temp_a0 == 1 || temp_a0 == 2 || temp_a0 == 3) {
                if (em->x194 == 0) {
                    em->x05 += 1;
                    act_dist_select_005B5650(em);
                    return;
                }
            } else {
                temp_a2 = em_act_search(em14_act_add[M2C_FIELD(em, u8 *, 0x735)]) & 0xFFFF;
                if (temp_a2 != 1) {
                    em14_act_set(em, 0, temp_a2, 0);
                }
            }
        }
        break;
    }
}

static void em_act02_005B5AB0(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x72, 0, 0);
        em->work08 = 0xF0;
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x73, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act03_005B5B80(EMW *em, EM14W *w) {
    s32 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0xA;
        break;
    case 1:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            act_dist_select_005B5650(em);
        }
        break;
    }
}

static void em_act04_005B5C10(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em14_frame_reset(em, 1);
    em14_frame_reset(em, 2);
}

static void em_act05_005B5CB0(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x18, 0, 0);
        em_char_set2(em, 0x3E9, 0xA, 0, 0);
        em_char_set2(em, 0x579, 0xA, 0, 2);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em14_frame_reset(em, 0);
    em14_frame_reset(em, 2);
    sound_call_parts_005BCC00(em, 0x24, 0x2F, 0x23, 1);
}

static void em_act06_005B5DA0(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em14_frame_reset(em, 0);
    em14_frame_reset(em, 2);
    sound_call_parts_005BCC00(em, 0x3E, 0x1F, 0x23, 1);
    sound_call_parts_005BCC00(em, 4, 0x17, 0, 1);
}

static void em_act07_005B5E70(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1A, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em14_frame_reset(em, 0);
    em14_frame_reset(em, 2);
}

static void em_act08_005B5F10(EMW *em, EM14W *w) {

}

static void em_act09_005B5F20(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        if (em->x39A & 1) {
            em_char_set(em, 0x50, 0, 0);
            return;
        }
        em_char_set(em, 0x51, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act10_005B5FC0(EMW *em, EM14W *w) {
    u16 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x36, 0, 0);
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        temp_v1 = w->tgt_ang;
        if ((u32) ((((temp_v1 - M2C_FIELD(em, u16 *, 0xA4)) & 0xFFFF) + 0x200) & 0xFFFF) < 0x400) {
            em->ang[1] = (s32) temp_v1;
        }
        if (em_frame_check2(em, 0, 60.0f) != 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act11_005B60D0(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1C, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em14_frame_reset(em, 0);
    em14_frame_reset(em, 2);
    sound_call_parts_005BCC00(em, 2, 0x21, 0x23, 1);
    sound_call_parts_005BCC00(em, 0x6E, 0x1F, 0x23, 1);
}

static void em_act12_005B61A0(EMW *em, EM14W *w) {

}

static void em_act13_005B61B0(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x14, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em14_frame_reset(em, 0);
    em14_frame_reset(em, 2);
    sound_call_parts_005BCC00(em, 2, 0x20, 0x23, 1);
    sound_call_parts_005BCC00(em, 0x4A, 0x1F, 0x23, 1);
}

static void em_act14_005B6280(EMW *em, EM14W *w) {

}

static void em_act15_005B6290(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x50, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x51, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
            return;
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05 = temp_a2 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act16_005B63C0(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x51, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x50, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
            return;
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05 = temp_a2 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act17_005B64F0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x33, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act18_005B6570(EMW *em, EM14W *w) {
    s16 temp_a1_2;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x20, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        Em_Suimin_Start(em);
        em->work08 = 0x708;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x1F, 0, 0);
        }
        break;
    case 2:
        temp_a1_2 = em->x792;
        if (em->x302 < temp_a1_2) {
            em_hp_add(em, 1);
        } else {
            em_hp_add(em, temp_a1_2);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_hp_add(em, em->x792);
            em_hinshi_end(em);
            em14_act_set(em, 0, 0x17, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x17, 4);
        }
        break;
    }
}

static void em_act19_005B66D0(EMW *em, EM14W *w) {

}

static void em_act20_005B66E0(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

static void em_act21_005B67F0(EMW *em, EM14W *w) {
    s16 temp_a1_2;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x27, 0, 0);
        em->work08 = 0x258;
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        temp_a1_2 = em->x792;
        if (em->x302 < temp_a1_2) {
            em_hp_add(em, 1);
        } else {
            em_hp_add(em, temp_a1_2);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_hp_add(em, em->x792);
            em_hinshi_end(em);
            em14_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

static void em_act22_005B6950(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x50, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act23_005B69D0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x21, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        em_suimin_end(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act24_005B6AA0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act25_005B6B70(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act26_005B6BF0(EMW *em, EM14W *w) {

}

static void em_act27_005B6C00(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x26, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em14_act_set(em, 0, 0x1C, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x1C, 4);
        }
        break;
    }
}

static void em_act28_005B6D10(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x25, 0, 0);
        em->x88B = 1;
        em_range_set(em, 0);
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act29_005B6DE0(EMW *em, EM14W *w) {
    f32 v[3];
    s32 temp_v0;
    u16 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        temp_v1_2 = em->char0;
        if ((temp_v1_2 != 0x451) || (temp_v1_2 != 0x448)) {
            em_char_set(em, 0x69, 0, 0);
        }
        em->x388 = 4;
        em->x3F4 = 0;
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        if (em->char0 != 0x4AD) {
            em->x388 = 0;
            em->x05 += 1;
            em_char_set(em, 0x61, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 != 0) {
            em->x388 = 4;
        } else {
            em->x388 = 0;
        }
        if (em->x194 == 0) {
            em->x388 = 0;
            em->x05 += 1;
            em_char_set(em, 0x61, 0, 0);
        }
        break;
    case 2:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em14_act_set(em, 4, 0x12, 3);
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em14_act_set(em, 4, 0x12, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005BCB30(em, 0x57, 0x23);
    }
}

static void em_act33_005B6FD0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x3E, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv00_005B7060(EMW *em, EM14W *w) {
    int d;
    f32 sp30[3];
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            d = (u16)((u16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
            if (d <= 0x8000) {
                if (d <= 0x3F) {
                    em->ang[1] += d;
                } else {
                    em->ang[1] += 0x40;
                }
            } else if (d > 0xFFC0) {
                em->ang[1] += d;
            } else {
                em->ang[1] -= 0x40;
            }
            mot_miration_ret(em, sp30);
            temp_f1 = w->dist - sp30[2];
            w->dist = temp_f1;
            if (temp_f1 <= 0.0f) {
                em->work08 = 1;
            }
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv01_005B71C0(EMW *em, EM14W *w) {
    em14_to_normal(em, 0, 0);
}

static void em_mv02_005B71D0(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            em14_tossin_move(temp_a1);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
                goto block_11;
            }
            if (em14_sasari_ck(em) != 0) {
                em14_act_set(em, 0, 2, 1);
                return;
            }
            goto block_11;
        }
block_11:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv03_005B72E0(EMW *em, EM14W *w) {
    f32 temp_f1;
    u32 var_a3;
    s32 temp_a2;
    u16 temp_a1;
    u32 temp_s0;
    u32 temp_v1;
    u8 temp_a0;

    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if (temp_v1 <= 0xE38 || temp_v1 >= 0xF1C8) {
            pl_flag_set((PLW *)em, 0x20000);
            em_char_set(em, 3, 0, 0);
        } else if (temp_v1 >= 0x8000) {
            em_char_set(em, 6, 0, 0);
        } else {
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            temp_f1 = (16384.0f / (em->x1A8 / 2.0f)) * em->act_spd;
            var_a3 = (u32)temp_f1;
            temp_a2 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_s0 = (temp_a1 - (temp_a2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + var_a3) & 0xFFFF) < (u32) (var_a3 * 2)) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em14_to_normal(em, 0, 0);
                    return;
                }
                if ((temp_s0 <= 0xE38) || (temp_s0 >= 0xF1C8)) {
                    pl_flag_set((PLW *) em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *) em, 0x20000);
                if (temp_s0 >= 0x8000) {
                    em_char_set(em, 6, 0, 0);
                    return;
                }
                em_char_set(em, 5, 0, 0);
                return;
            }
            if ((u32) ((temp_s0 + var_a3) & 0xFFFF) < (u32) (var_a3 * 2)) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_s0 < 0x8000) {
                em->ang[1] = (temp_a2 + var_a3) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a2 - var_a3) & 0xFFFF;
        }
        break;
    }
}

static void em_mv04_005B75B0(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            em14_tossin_move(temp_a1);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
                goto block_11;
            }
            if (em14_sasari_ck(em) != 0) {
                em14_act_set(em, 0, 2, 1);
                return;
            }
            goto block_11;
        }
block_11:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_char_set(em, 0x64, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv05_005B76F0(EMW *em, EM14W *w) {
    f32 temp_f1;
    u32 var_a3;
    s32 temp_a2;
    u16 temp_a1;
    u32 temp_s0;
    u32 temp_v1;
    u8 temp_a0;

    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if (temp_v1 <= 0xE38 || temp_v1 >= 0xF1C8) {
            pl_flag_set((PLW *)em, 0x20000);
            em_char_set(em, 3, 0, 0);
        } else if (temp_v1 >= 0x8000) {
            em_char_set(em, 7, 0, 0);
        } else {
            em_char_set(em, 8, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 == 0) {
            temp_f1 = (16384.0f / (em->x1A8 / 2.0f)) * em->act_spd;
            var_a3 = (u32)temp_f1;
            temp_a2 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_s0 = (temp_a1 - (temp_a2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + var_a3) & 0xFFFF) < (u32) (var_a3 * 2)) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em14_to_normal(em, 0, 0);
                    return;
                }
                if ((temp_s0 <= 0xE38) || (temp_s0 >= 0xF1C8)) {
                    pl_flag_set((PLW *) em, 0x20000);
                    em_char_set(em, 3, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *) em, 0x20000);
                if (temp_s0 >= 0x8000) {
                    em_char_set(em, 7, 0, 0);
                    return;
                }
                em_char_set(em, 8, 0, 0);
                return;
            }
            if ((u32) ((temp_s0 + var_a3) & 0xFFFF) < (u32) (var_a3 * 2)) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_s0 < 0x8000) {
                em->ang[1] = (temp_a2 + var_a3) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a2 - var_a3) & 0xFFFF;
        }
        break;
    }
}

static void em_mv06_005B79C0(EMW *em, EM14W *w) {
    int d;
    f32 sp30[3];
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0xA, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            d = (u16)((u16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
            if (d <= 0x8000) {
                if (d <= 0x3F) {
                    em->ang[1] += d;
                } else {
                    em->ang[1] += 0x40;
                }
            } else if (d > 0xFFC0) {
                em->ang[1] += d;
            } else {
                em->ang[1] -= 0x40;
            }
            mot_miration_ret(em, sp30);
            temp_f1 = w->dist - sp30[2];
            w->dist = temp_f1;
            if (temp_f1 <= 0.0f) {
                em->work08 = 1;
            }
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv07_005B7B20(EMW *em, EM14W *w) {
    u16 temp_v1;
    u32 temp_a1_2;
    s32 temp_a0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x36, 0, 0);
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        temp_a0 = em->ang[1];
        temp_v1 = w->tgt_ang;
        temp_a1_2 = (temp_v1 - (temp_a0 & 0xFFFF)) & 0xFFFF;
        if ((u32) ((temp_a1_2 + 0x200) & 0xFFFF) < 0x400) {
            em->ang[1] = (s32) temp_v1;
        } else if (temp_a1_2 < 0x8000) {
            em->ang[1] = (temp_a0 + 0x200) & 0xFFFF;
        } else {
            em->ang[1] = (temp_a0 - 0x200) & 0xFFFF;
        }
        if (em_frame_check2(em, 0, 60.0f) != 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_fly00_005B7C60(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        if (em->char0 != 0x44F) {
            em_char_set(em, 0x67, 0, 0);
        }
        em->x8BD = 1;
        em->x388 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 100.0f) != 0) {
            em->x95C = 2;
            em->x7D6 = 2;
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em->x8BD = 0;
            em14_to_swim(em);
        }
        break;
    }
    em->x8BB = 2;
}

static void em_fly01_005B7D30(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a2 + 1;
            em14_to_swim(em);
        }
        break;
    }
}

static void em_fly02_005B7DC0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        if (em->char0 != 0x450) {
            em_char_set(em, 0x68, 0, 0);
            em->x388 = 4;
        } else if (em->x1C4 != 0) {
            em->x388 = 4;
        } else {
            em->x388 = 0;
        }
        em->x8BD = 1;
        break;
    case 1:
        if (em->x1C4 != 0) {
            em->x388 = 4;
        } else {
            em->x388 = 0;
            if (em_frame_check2(em, 0, 100.0f) == 0) {
                em->x95C = 2;
                em->x7D6 = 2;
            }
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em->x8BD = 0;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    em->x8BB = 2;
}

static void em_fly03_005B7EE0(EMW *em, EM14W *w) {
    f32 temp_f1;
    u32 var_a2;
    s32 temp_v1;
    u16 var_v1;
    u32 temp_a1;
    u8 temp_a0;

    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x28;
        break;
    case 1:
        temp_f1 = 409.6f * em->act_spd;
        var_a2 = (u32)temp_f1;
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        temp_a1 = (w->tgt_ang - M2C_FIELD(em, u16 *, 0xA4)) & 0xFFFF;
        if (temp_v1 <= 0) {
            if ((u32) ((temp_a1 + var_a2) & 0xFFFF) < (u32) (var_a2 * 2)) {
                em->x05 += 1;
                em14_to_swim(em);
                return;
            }
            em->work08 = 0x28;
            return;
        }
        if ((u32) ((temp_a1 + var_a2) & 0xFFFF) < (u32) (var_a2 * 2)) {
            var_v1 = w->tgt_ang;
        } else if (temp_a1 < 0x8000) {
            var_v1 = (em->ang[1] + var_a2) & 0xFFFF;
        } else {
            var_v1 = (em->ang[1] - var_a2) & 0xFFFF;
        }
        em->ang[1] = (s32) var_v1;
        break;
    }
}

static void em_fly04_005B8090(EMW *em, EM14W *w) {
    f32 sp40[3];
    PLW *temp_s0;
    s16 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;
    u8 temp_t0;

    temp_t0 = *(u8 *)0x3F34C1;
    temp_a1 = em->x05;
    temp_s0 = &player_work[temp_t0];
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0x64);
        w->x4E = 2;
        break;
    case 1:
        temp_v1 = w->x4E - 1;
        w->x4E = temp_v1;
        if ((s16)temp_v1 == 0) {
            Em_set_quake_sub(em, 5);
            w->x4E = 8;
        }
        if (w->has_tgt != 0) {
            if (em_frame_check2(em, 0, 150.0f) == 0) {
                em14_tossin_move(em);
            } else {
                mot_miration_ret(em, sp40);
                w->dist = (f32) (w->dist - sp40[2]);
            }
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        if (!(*(u16 *)0x3F340E & 7) && (Pl_stg_ck_tw(em, temp_s0) != 0) && (Pl_stg_ck(temp_s0) != 0) && (flvecCalcDistance(em->pos, temp_s0->pos) < 2000.0f)) {
            vib_set(0, 2);
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em14_to_swim(em);
        }
        break;
    }
}

static void em_fly05_005B8260(EMW *em, EM14W *w) {
    int d;
    f32 sp30[3];
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        em_char_set(em, 3, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            d = (u16)((u16)Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
            if (d <= 0x8000) {
                if (d <= 0x3F) {
                    em->ang[1] += d;
                } else {
                    em->ang[1] += 0x40;
                }
            } else if (d > 0xFFC0) {
                em->ang[1] += d;
            } else {
                em->ang[1] -= 0x40;
            }
            mot_miration_ret(em, sp30);
            temp_f1 = w->dist - sp30[2];
            w->dist = temp_f1;
            if (temp_f1 <= 0.0f) {
                em->work08 = 1;
            }
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em14_to_swim(em);
        }
        break;
    }
}

static void em_fly06_005B83C0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em14_fly_adjy2_init(em, 2);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 0, 70.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em14_fly_adjy2(em);
        }
        break;
    case 2:
        if (em14_fly_adjy2(em) != 0) {
            em->x05 += 1;
            em14_to_fly(em, 0);
        }
        break;
    }
}

static void em_fly07_005B84C0(EMW *em, EM14W *w) {
    f32 temp_f1;
    s32 temp_a0;
    s32 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->turn = 0x100;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        w->x18 = 0;
        em->work08 = 0x96;
        break;
    case 1:
        em14_fly_adjy(em, 1);
        temp_a0 = em14_senkai_target(em) & 0xFF;
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 > 0) {
            if (temp_a0 != 0) {
                goto block_9;
            }
        } else {
block_9:
            em->x05 += 1;
            em14_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly08_005B85D0(EMW *em, EM14W *w) {
    s16 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0x64);
        w->x4E = 2;
        break;
    case 1:
        temp_v1 = w->x4E - 1;
        w->x4E = temp_v1;
        if ((s16)temp_v1 == 0) {
            Em_set_quake_sub(em, 5);
            w->x4E = 8;
        }
        if (w->has_tgt != 0) {
            em14_tossin_move(em);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
            }
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em14_to_swim(em);
        }
        break;
    }
}

static void em_fly09_005B86E0(EMW *em, EM14W *w) {
    f32 sp30[3];
    f32 temp_f1;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em_rate_clear(em);
        em->adj_y = 20.0f;
        break;
    case 1:
        w->spd[1] = (s32) (Em_Calc_angY(em->pos, sp30) & 0xFFFF);
        speed_add(em, w->spd);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if ((temp_v1 <= 0) || !(em->pos[1] <= (1000.0f + em->tgt_pos[1]))) {
            em->x05 += 1;
            em_rate_clear(em);
            em14_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly10_005B8800(EMW *em, EM14W *w) {
    s32 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x5A;
        break;
    case 1:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em14_to_swim(em);
        }
        break;
    }
}

static void em_fly11_005B8890(EMW *em, EM14W *w) {
    f32 temp_f0;
    f32 temp_f1;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        em14_senkai_target(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        em14_fly_adjy(em, 1);
        temp_f0 = CalcDistanceXZ(em->pos, em->tgt_pos);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 > 0) {
            if (temp_f0 <= (10.0f * em->adj_z)) {
                goto block_9;
            }
        } else {
block_9:
            em->x05 += 1;
            em_rate_clear(em);
            em14_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly12_005B89D0(EMW *em, EM14W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em->ang[0] = 0;
        em->ang[2] = 0;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_y = -10.0f;
        w->x18 = 0;
        break;
    case 1:
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (!((630.0f + em->x5AC) <= em->pos[1]) && ((em_frame_check(em, 0, 10.0f) != 0) || (em_frame_check(em, 0, 86.0f) != 0) || (em_frame_check(em, 0, 160.0f) != 0))) {
            em->x05 += 1;
            em_char_set(em, 0xB, 0, 0);
            temp_f1 = (em->x5AC - em->pos[1]) / 30.0f;
            em->adj_y = temp_f1;
            if (!(temp_f1 < 0.0f)) {
                em->adj_y = -10.0f;
            }
            em->work08 = 0x1E;
        }
        break;
    case 2:
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 < 0) {
            em->x05 += 1;
            em_char_set(em, 0x13, 0, 0);
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->x388 = 0;
            Em_set_quake_sub(em, 1);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly14_005B8C10(EMW *em, EM14W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly23_005B8CD0(EMW *em, EM14W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x762 = 0;
        em_char_set(em, 0x12, 0xA, 0x60);
        em14_fly_adjy2_init(em, 2);
        em->x388 = 2;
        em->x8BD = 1;
        break;
    case 1:
        em14_fly_adjy2(temp_a2);
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if ((em14_fly_adjy2(temp_a2) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
            em->x05 += 1;
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
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em_char_set(em, 0x13, 0, 0);
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            w->x06 = 0x96;
            em->x05 += 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_3 = em->x5AC;
    if (em->pos[1] < temp_f1_3) {
        em->pos[1] = temp_f1_3;
    }
}

static void em_atk00_005B8F00(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em14_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk03_005B8FA0(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 1;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em14_atk_end_sel(em, w);
        }
        break;
    }
}

static void em_atk06_005B9020(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2B, 0, 0);
        em_action_timer_calc(em, 0);
        break;
    case 1:
        em->ang[1] -= 0x200;
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 += 1;
            em_char_set(em, 0x2B, 0, 0);
        }
        break;
    case 2:
        em->ang[1] -= 0x200;
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 += 1;
            em14_atk_end_sel(em, w);
        }
        break;
    }
}

static void em_atk15_005B9110(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            em14_tossin_move(temp_a1);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
                goto block_11;
            }
            if (em14_sasari_ck(em) != 0) {
                em14_act_set(em, 0, 2, 1);
                return;
            }
            goto block_11;
        }
block_11:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em_char_set(em, 0x64, 0, 0);
            em->x05 += 1;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk16_005B9250(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            em14_tossin_move(temp_a1);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
                goto block_11;
            }
            if (em14_sasari_ck(em) != 0) {
                em14_act_set(em, 0, 2, 1);
                return;
            }
            goto block_11;
        }
block_11:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em_char_set(em, 0x6A, 0, 0);
            em->x05 += 1;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk17_005B9390(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x11, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            em14_tossin_move(temp_a1);
            if (w->dist <= 0.0f) {
                em->work08 = 1;
                goto block_11;
            }
            if (em14_sasari_ck(em) != 0) {
                em14_act_set(em, 0, 2, 1);
                return;
            }
            goto block_11;
        }
block_11:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em_char_set(em, 0x6B, 0, 0);
            em->x05 += 1;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk26_005B94D0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em14_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x6A, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk27_005B9570(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em14_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x6B, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk28_005B9610(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6C, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk29_005B9690(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em14_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x6D, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk30_005B9730(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk31_005B97B0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x70, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg00_005B9830(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3C, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg01_005B98C0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x42, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg02_005B9950(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg03_005B99E0(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x40, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg04_005B9A70(EMW *em, EM14W *w) {
    f32 temp_f1;
    u32 var_a0;
    u8 temp_a1;

    em->x40C = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x41, 0, 0);
        em->x948 |= 1;
        em->x958 += 1;
        Em_Sleep_Flag_Ck(em);
        break;
    case 1:
        if ((em_frame_check(em, 0, 6.0f) != 0) && ((s8) em->ex[0xA3] != 0) && (em->kind != 0x14)) {
            em->ex[0xA3] = 0;
        }
        if ((em_frame_check2(em, 0, 52.0f) != 0) && (em_frame_check2(em, 0, 122.0f) == 0)) {
            temp_f1 = 936.0f * em->act_spd;
            var_a0 = (u32)temp_f1;
            em->ang[1] -= var_a0 & 0xFFFF;
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em->ang[1] -= 8;
            em14_act_set(em, 4, 0xF, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 4, 0xF, 3);
        }
        break;
    }
}

static void em_dmg05_005B9C40(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x45, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0;
            em_char_set(em, 0x46, 0, 0);
        }
        break;
    case 2:
        em->work08 += 1;
        if (em->work08 >= 0x78) {
            em->x05 += 1;
            em_char_set(em, 0x47, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg06_005B9D60(EMW *em, EM14W *w) {
    s8 temp_v0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 4;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x69, 0, 0);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x1C4 != 0) {
            em->x388 = 4;
        } else {
            em->x388 = 0;
        }
        if (em->x194 == 0) {
            em->x388 = 0;
            em_char_set(em, 0x60, 0, 0);
            em->x95A = 4;
            em_ana_loop_cnt_set(em);
            em->x05 += 1;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            temp_v0 = em->x95A - 1;
            em->x95A = temp_v0;
            if ((s8)temp_v0 <= 0) {
                em->x05 += 1;
                em->x8BD = 0;
                em14_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em14_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg07_005B9ED0(EMW *em, EM14W *w) {
    FLMAT m50;
    f32 v[3];
    f32 v2[3];
    f32 var_f0;
    u8 temp_a0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x71, 0, 0);
        w->x1A = (u8) (w->x1A + 1);
        if (em->kind == 0xE) {
            temp_a0 = w->x1A;
            if (temp_a0 != 1) {
                if (temp_a0 == 2) {
                    goto block_8;
                }
            } else {
block_8:
                if (temp_a0 == 1) {
                    var_f0 = -90.0f;
                } else {
                    var_f0 = 90.0f;
                }
                v[0] = var_f0;
                v[1] = 40.0f;
                v[2] = 80.0f;
                flmatCopy(&m50, get_joint_wmat_em(em, 0x27));
                flvecApplyMat33_2(v, &m50);
                v2[0] = m50[3][0] + v[0];
                v2[1] = m50[3][1] + v[1];
                v2[2] = m50[3][2] + v[2];
                Eft02_set3(em, 0, 0xA, 0, v2, 1.0f);
            }
            if ((s32) w->x1A >= 2) {
                Quest_enemy_hagi_set(em, 0x40);
                return;
            }
        } else {
            if (w->x1A == 1) {
                v[0] = 0.0f;
                v[1] = 30.0f;
                v[2] = 100.0f;
                flmatCopy(&m50, get_joint_wmat_em(em, 0x27));
                flvecApplyMat33_2(v, &m50);
                v2[0] = m50[3][0] + v[0];
                v2[1] = m50[3][1] + v[1];
                v2[2] = m50[3][2] + v[2];
                Eft02_set3(em, 0, 0xA, 0, v2, 1.0f);
            }
            if ((s32) w->x1A > 0) {
                Quest_enemy_hagi_set(em, 0x80);
            }
            break;
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg08_005BA140(EMW *em, EM14W *w) {
    u8 temp_a3;

    temp_a3 = em->x05;
    switch (temp_a3) {                              /* irregular */
    case 0:
        em->x05 = temp_a3 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a3 + 1;
            em_char_set(em, 0x47, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a3 + 1;
            em14_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg09_005BA2A0(EMW *em, EM14W *w) {
    u8 temp_a3;

    temp_a3 = em->x05;
    switch (temp_a3) {                              /* irregular */
    case 0:
        em->x05 = temp_a3 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em->x88B = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a3 + 1;
            em_char_set(em, 0x47, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a3 + 1;
            em14_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg10_005BA400(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg11_005BA490(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4C, 0, 0);
        Em_Mahi_Start(em);
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em14_act_set(em, 0, 0x21, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 0, 0x21, 4);
        }
        break;
    }
}

static void em_dmg13_005BA560(EMW *em, EM14W *w) {
    s8 temp_v0;
    u8 temp_a2;

    em->x9EA = 5;
    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em_char_set(em, 0x60, 0, 0);
        em_cmd_reset(em);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            temp_v0 = em->x95A - 1;
            em->x95A = temp_v0;
            if ((s8)temp_v0 <= 0) {
                em->x05 += 1;
                em->x959 = 0;
                em->x8BD = 0;
                em14_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg14_005BA660(EMW *em, EM14W *w) {
    s32 temp_v0;
    u16 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        temp_v1_2 = em->char0;
        if ((temp_v1_2 != 0x451) || (temp_v1_2 != 0x448)) {
            em_char_set(em, 0x69, 0, 0);
        }
        em->x3F4 = 0;
        em_cmd_reset(em);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em->x388 = 4;
        if (em->char0 != 0x4AD) {
            em->x388 = 0;
            em->x05 += 1;
            em_char_set(em, 0x61, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 != 0) {
            em->x388 = 4;
        } else {
            em->x388 = 0;
        }
        if (em->x194 == 0) {
            em->x388 = 0;
            em->x05 += 1;
            em_char_set(em, 0x61, 0, 0);
        }
        break;
    case 2:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em14_act_set(em, 4, 0x11, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg15_005BA800(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_cmd_reset(em);
        em_tail_off_sub(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg16_005BA880(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4A, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0;
            em_char_set(em, 0x4B, 0, 0);
        }
        break;
    case 2:
        em->work08 += 1;
        if (em->work08 >= 0x78) {
            em->x05 += 1;
            em_char_set(em, 0x48, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em14_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg17_005BA9A0(EMW *em, EM14W *w) {
    s8 temp_v0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em_char_set(em, 0x60, 0, 0);
        Em_Mahi_End(em);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            temp_v0 = em->x95A - 1;
            em->x95A = temp_v0;
            if ((s8)temp_v0 <= 0) {
                em->x05 += 1;
                em14_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg18_005BAA90(EMW *em, EM14W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em14_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em14_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_demo00_005BAB80(EMW *em, EM14W *w) {
    s32 temp_v1;
    s32 temp_v1_2;
    u8 temp_a2;

    em->x9E1 = 5;
    em->x40C = 5;
    temp_a2 = em->x05;
    switch (temp_a2) {
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->pos[0] = 4905.0f;
        em->pos[1] = 310.0f;
        em->pos[2] = 9335.0f;
        em->ang[0] = 0;
        em->ang[1] = 0x3A00;
        em->ang[2] = 0;
        em_char_set(em, 0x1F, 0, 0);
        em->work08 = 0x12C;
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_char_set(em, 0x21, 0, 0);
            return;
        }
    default:
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x51, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x67, 0, 0);
            return;
        }
        break;
    case 4:
        if (em_frame_check2(em, 0, 100.0f) != 0) {
            em->x95C = 2;
            em->x7D6 = 2;
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em->work08 = 0xF0;
            return;
        }
        break;
    case 5:
    case 7:
    case 9:
        em->x95C = 2;
        em->x7D6 = 2;
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0;
            em->x05 += 1;
            em->x388 = 0;
            em->pos[0] = 9800.0f;
            em->pos[1] = 10.0f;
            em->pos[2] = 9335.0f;
            em->ang[1] = 0x400;
            em_char_set2(em, 0x450, 0, 0, 0);
            em_char_set2(em, 0x518, 0, 0, 1);
            em_char_set2(em, 0x5E0, 0, 0, 2);
            return;
        }
        break;
    case 6:
    case 8:
        if (em_frame_check(em, 0, 40.0f) != 0) {
            em->x05 += 1;
        }
        if (em_frame_check2(em, 0, 100.0f) == 0) {
            em->x95C = 2;
            em->x7D6 = 2;
            return;
        }
        break;
    case 10:
        if (em_frame_check2(em, 0, 100.0f) == 0) {
            em->x95C = 2;
            em->x7D6 = 2;
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x6C, 0, 0);
            return;
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x33, 0, 0);
            return;
        }
        break;
    case 12:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
            return;
        }
        break;
    case 13:
        if (Event_flag_ck(0x12) == 1) {
            em->x05 += 1;
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_die00_005BAEF0(EMW *em, EM14W *w) {
    s32 temp_v1;
    u8 temp_a0;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a0 + 1;
        em_char_set(em, 0x44, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em_frame_check(em, 0, 212.0f) != 0) {
            Em_set_quake_sub(em, 2);
        }
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05 += 1;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 0x2:
        Em_hagi_point_cnt_ck(em);
        break;
    case 0x3:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            return;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x63:
        if (em->x194 == 0) {
            em->x388 = 0;
            em14_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die01_005BB090(EMW *em, EM14W *w) {
    f32 temp_f1;
    s32 temp_v1_2;
    u16 temp_v1;
    u8 temp_a2;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        temp_v1 = em->char0;
        if ((temp_v1 != 0x451) || (temp_v1 != 0x448)) {
            em_char_set(em, 0x69, 0, 0);
        }
        em->x3F4 = 0;
        em->x388 = 4;
        Quest_enemy_die(em);
        if (em->char0 != 0x4AD) {
            em->x388 = 3;
            em->x05 += 1;
            em_char_set(em, 0x61, 0, 0);
        }
        break;
    case 1:
        if (em->x1C4 != 0) {
            em->x388 = 4;
        } else {
            em->x388 = 3;
        }
        if (em->x194 == 0) {
            em->x388 = 3;
            em->x05 += 1;
            em_char_set(em, 0x61, 0, 0);
            return;
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05 += 1;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            return;
        }
        break;
    case 3:
        Em_hagi_point_cnt_ck(em);
        break;
    case 4:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            return;
        }
        temp_f1 = (f32) em->work08 / 150.0f;
        em->x798 = temp_f1;
        if (!(temp_f1 <= 1.0f)) {
            em->x798 = 1.0f;
        }
        break;
    }
}

static void em_die02_005BB290(EMW *em, EM14W *w) {
    u8 temp_a0;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        break;
    case 1:
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 60.0f) != 0) {
            em->x05 += 1;
            em->work08 = 0;
            em_char_set(em, 0x52, 6, 0);
            em->x388 = 3;
            Quest_enemy_die(em);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05 += 1;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            return;
        }
        break;
    case 4:
        Em_hagi_point_cnt_ck(em);
        break;
    }
}

static void em_move00_005BB460(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_act00_005B58E0(em, w);
        break;
    case 1:
        em_act01_005B59C0(em, w);
        break;
    case 2:
        em_act02_005B5AB0(em, w);
        break;
    case 3:
        em_act03_005B5B80(em, w);
        break;
    case 4:
        em_act04_005B5C10(em, w);
        break;
    case 5:
        em_act05_005B5CB0(em, w);
        break;
    case 6:
        em_act06_005B5DA0(em, w);
        break;
    case 7:
        em_act07_005B5E70(em, w);
        break;
    case 8:
        em_act08_005B5F10(em, w);
        break;
    case 9:
        em_act09_005B5F20(em, w);
        break;
    case 10:
        em_act10_005B5FC0(em, w);
        break;
    case 11:
        em_act11_005B60D0(em, w);
        break;
    case 12:
        em_act12_005B61A0(em, w);
        break;
    case 13:
        em_act13_005B61B0(em, w);
        break;
    case 14:
        em_act14_005B6280(em, w);
        break;
    case 15:
        em_act15_005B6290(em, w);
        break;
    case 16:
        em_act16_005B63C0(em, w);
        break;
    case 17:
        em_act17_005B64F0(em, w);
        break;
    case 18:
        em_act18_005B6570(em, w);
        break;
    case 19:
        em_act19_005B66D0(em, w);
        break;
    case 20:
        em_act20_005B66E0(em, w);
        break;
    case 21:
        em_act21_005B67F0(em, w);
        break;
    case 22:
        em_act22_005B6950(em, w);
        break;
    case 23:
        em_act23_005B69D0(em, w);
        break;
    case 24:
        em_act24_005B6AA0(em, w);
        break;
    case 25:
        em_act25_005B6B70(em, w);
        break;
    case 26:
        em_act26_005B6BF0(em, w);
        break;
    case 27:
        em_act27_005B6C00(em, w);
        break;
    case 28:
        em_act28_005B6D10(em, w);
        break;
    case 29:
        em_act29_005B6DE0(em, w);
        break;
    case 33:
        em_act33_005B6FD0(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move01_005BB690(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_mv00_005B7060(em, w);
        break;
    case 1:
        em_mv01_005B71C0(em, w);
        break;
    case 2:
        em_mv02_005B71D0(em, w);
        break;
    case 3:
        em_mv03_005B72E0(em, w);
        break;
    case 4:
        em_mv04_005B75B0(em, w);
        break;
    case 5:
        em_mv05_005B76F0(em, w);
        break;
    case 6:
        em_mv06_005B79C0(em, w);
        break;
    case 7:
        em_mv07_005B7B20(em, w);
        break;
    case 8:
        em_mv04_005B75B0(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move02_005BB760(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_fly00_005B7C60(em, w);
        break;
    case 1:
        em_fly01_005B7D30(em, w);
        break;
    case 2:
        em_fly02_005B7DC0(em, w);
        break;
    case 3:
        em_fly03_005B7EE0(em, w);
        break;
    case 4:
        em_fly04_005B8090(em, w);
        break;
    case 5:
        em_fly05_005B8260(em, w);
        break;
    case 6:
        em_fly06_005B83C0(em, w);
        break;
    case 7:
        em_fly07_005B84C0(em, w);
        break;
    case 8:
        em_fly08_005B85D0(em, w);
        break;
    case 9:
        em_fly09_005B86E0(em, w);
        break;
    case 10:
        em_fly10_005B8800(em, w);
        break;
    case 11:
        em_fly11_005B8890(em, w);
        break;
    case 12:
        em_fly12_005B89D0(em, w);
        break;
    case 14:
        em_fly14_005B8C10(em, w);
        break;
    case 23:
        em_fly23_005B8CD0(em, w);
        break;
    }
}

static void em_move03_005BB900(EMW *em, EM14W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_atk00_005B8F00(em, w);
        break;
    case 3:
        em_atk03_005B8FA0(em, w);
        break;
    case 6:
        em_atk06_005B9020(em, w);
        break;
    case 15:
        em_atk15_005B9110(em, w);
        break;
    case 16:
        em_atk16_005B9250(em, w);
        break;
    case 17:
        em_atk17_005B9390(em, w);
        break;
    case 26:
        em_atk26_005B94D0(em, w);
        break;
    case 27:
        em_atk27_005B9570(em, w);
        break;
    case 28:
        em_atk28_005B9610(em, w);
        break;
    case 29:
        em_atk29_005B9690(em, w);
        break;
    case 30:
        em_atk30_005B9730(em, w);
        break;
    case 31:
        em_atk31_005B97B0(em, w);
        break;
    }
}

static void em_move04_005BBA60(EMW *em, EM14W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_dmg00_005B9830(em, w);
        break;
    case 1:
        em_dmg01_005B98C0(em, w);
        break;
    case 2:
        em_dmg02_005B9950(em, w);
        break;
    case 3:
        em_dmg03_005B99E0(em, w);
        break;
    case 4:
        em_dmg04_005B9A70(em, w);
        break;
    case 5:
        em_dmg05_005B9C40(em, w);
        break;
    case 6:
        em_dmg06_005B9D60(em, w);
        break;
    case 7:
        em_dmg07_005B9ED0(em, w);
        break;
    case 8:
        em_dmg08_005BA140(em, w);
        break;
    case 9:
        em_dmg09_005BA2A0(em, w);
        break;
    case 10:
        em_dmg10_005BA400(em, w);
        break;
    case 11:
        em_dmg11_005BA490(em, w);
        break;
    case 13:
        em_dmg13_005BA560(em, w);
        break;
    case 14:
        em_dmg14_005BA660(em, w);
        break;
    case 15:
        em_dmg15_005BA800(em, w);
        break;
    case 16:
        em_dmg16_005BA880(em, w);
        break;
    case 17:
        em_dmg17_005BA9A0(em, w);
        break;
    case 18:
        em_dmg18_005BAA90(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move05_005BBBC0(EMW *em, EM14W *w) {
    u8 temp_a2;

    em->x40E = 5;
    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_die00_005BAEF0(em, w);
        break;
    case 1:
        em_die01_005BB090(em, w);
        break;
    case 2:
        em_die02_005BB290(em, w);
        break;
    }
}

static void em_move06_005BBC30(EMW *em, EM14W *w) {
    switch (em->x15) {
    case 0:
        em_demo00_005BAB80(em, w);
        break;
    }
}

void em14_main(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
    u8 sp3C;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    s16 temp_v0_3;
    u32 temp_v0_4;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a0_5;
    u8 temp_v0;
    u8 temp_v0_2;
    u8 temp_v0_5;
    u8 temp_v0_6;
    u8 temp_v0_7;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 temp_v1_4;

    if (em->x8B6 != 0) {
        temp_v1 = em->x8B7;
        switch (temp_v1) {                          /* switch 1; irregular */
        case 0:                                     /* switch 1 */
            temp_v0 = (s8) w->x1B + 1;
            w->x1B = temp_v0;
            if ((s8)temp_v0 >= 0x3D) {
                w->x1B = 0x3C;
                em->x8B7 += 1;
            }
            break;
        case 1:                                     /* switch 1 */
            w->x1B = 0x3C;
            break;
        }
    } else {
        em->x8B7 = 0;
        temp_v0_2 = (s8) w->x1B - 1;
        w->x1B = temp_v0_2;
        if ((s8)temp_v0_2 < 0) {
            w->x1B = 0;
        }
    }
    em_mode_timer_sub(em);
    em_no_floor_ck(em);
    if (em->x388 == 4) {
        em_no_battle_area_ck(em, 0, 3);
    } else {
        em_no_battle_area_ck(em, 0, 1);
    }
    if (em14_suna_ck(em) == 1) {
        em14_to_normal(em, 0, 0);
    }
    temp_v0_3 = w->x06;
    if (temp_v0_3 != 0) {
        w->x06 = (s16) (temp_v0_3 - 1);
    }
    temp_v0_4 = Em_Dmg_Sys(em, &sp3C) & 0xFF;
    switch (temp_v0_4) {                            /* switch 2 */
    case 1:                                         /* switch 2 */
    case 2:                                         /* switch 2 */
        temp_v1_2 = em->x388;
        if (temp_v1_2 == 2) {
            em14_act_set(em, 5, 2, 2);
        } else if ((temp_v1_2 != 4) && ((temp_a0 = em->mode, (temp_a0 != 4)) || (em->x15 != 6))) {
            if (temp_a0 == 4) {
                if (em->x15 != 0xD) {
                    goto block_31;
                }
                goto block_40;
            }
block_31:
            if (temp_a0 == 4) {
                if (em->x15 != 0xE) {
                    goto block_34;
                }
                goto block_40;
            }
block_34:
            if (temp_a0 == 4) {
                if (em->x15 != 0x11) {
                    goto block_37;
                }
                goto block_40;
            }
block_37:
            if ((temp_a0 == 4) && (em->x15 == 0x12)) {
                goto block_40;
            }
            em14_act_set(em, 5, 0, 2);
        } else {
block_40:
            em14_act_set(em, 5, 1, 2);
        }
        break;
    case 15:                                        /* switch 2 */
    case 16:                                        /* switch 2 */
        if (em->x388 == 4) {
            if (em->mode == 4) {
                if (em->x15 != 6) {
                    goto block_48;
                }
            } else {
block_48:
                em14_act_set(em, 4, 6, 2);
            }
        }
        break;
    case 5:                                         /* switch 2 */
        if ((em->mode != 4) || (em->x15 != 8)) {
            em14_act_set(em, 4, 8, 2);
        }
        break;
    case 6:                                         /* switch 2 */
        temp_v0_5 = em->mode;
        if ((temp_v0_5 != 4) || (em->x15 != 0xE)) {
            if ((em->x388 != 4) && ((temp_v0_5 != 4) || (em->x15 != 6)) && ((temp_v0_5 != 0) || (em->x15 != 0x1D))) {
                if (temp_v0_5 == 4) {
                    if (em->x15 != 0xD) {
                        goto block_64;
                    }
                    goto block_70;
                }
block_64:
                if (temp_v0_5 == 4) {
                    if (em->x15 != 0x11) {
                        goto block_67;
                    }
                    goto block_70;
                }
block_67:
                if ((temp_v0_5 == 4) && (em->x15 == 0x12)) {
                    goto block_70;
                }
                if (temp_v0_5 == 4) {
                    if (em->x15 != 0xB) {
                        goto block_75;
                    }
                } else {
block_75:
                    if (temp_v0_5 == 4) {
                        if (em->x15 != 8) {
                            goto block_78;
                        }
                    } else {
block_78:
                        em_mahi_dmg_timer_set(em);
                        em14_act_set(em, 4, 0xB, 2);
                    }
                }
            } else {
block_70:
                em_mahi_dmg_timer_set(em);
                em14_act_set(em, 4, 0xE, 2);
            }
        }
        break;
    case 7:                                         /* switch 2 */
        temp_a0_2 = em->mode;
        if (temp_a0_2 == 0) {
            if (em->x15 != 0x1D) {
                goto block_83;
            }
        } else {
block_83:
            if ((em->x388 != 4) && ((temp_a0_2 != 4) || (em->x15 != 6))) {
                if (temp_a0_2 == 4) {
                    if (em->x15 != 0xE) {
                        goto block_89;
                    }
                    goto block_98;
                }
block_89:
                if (temp_a0_2 == 4) {
                    if (em->x15 != 0xD) {
                        goto block_92;
                    }
                    goto block_98;
                }
block_92:
                if (temp_a0_2 == 4) {
                    if (em->x15 != 0x11) {
                        goto block_95;
                    }
                    goto block_98;
                }
block_95:
                if ((temp_a0_2 == 4) && (em->x15 == 0x12)) {
                    goto block_98;
                }
                if ((temp_a0_2 != 0) || (em->x15 != 0x1B)) {
                    if (temp_a0_2 == 4) {
                        if (em->x15 != 8) {
                            goto block_105;
                        }
                    } else {
block_105:
                        em_sleep2_dmg_timer_set(em);
                        em14_act_set(em, 0, 0x1B, 2);
                    }
                }
            } else {
block_98:
                em_sleep2_dmg_timer_set(em);
                em14_act_set(em, 0, 0x1D, 2);
            }
        }
        break;
    case 8:                                         /* switch 2 */
        temp_a0_3 = em->mode;
        if (temp_a0_3 == 0) {
            if (em->x15 != 0x1D) {
                goto block_110;
            }
        } else {
block_110:
            if ((em->x388 != 4) && ((temp_a0_3 != 4) || (em->x15 != 6))) {
                if (temp_a0_3 == 4) {
                    if (em->x15 != 0xE) {
                        goto block_116;
                    }
                    goto block_125;
                }
block_116:
                if (temp_a0_3 == 4) {
                    if (em->x15 != 0xD) {
                        goto block_119;
                    }
                    goto block_125;
                }
block_119:
                if (temp_a0_3 == 4) {
                    if (em->x15 != 0x11) {
                        goto block_122;
                    }
                    goto block_125;
                }
block_122:
                if ((temp_a0_3 == 4) && (em->x15 == 0x12)) {
                    goto block_125;
                }
                if ((temp_a0_3 != 0) || (em->x15 != 0x14)) {
                    if (temp_a0_3 == 4) {
                        if (em->x15 != 8) {
                            goto block_132;
                        }
                    } else {
block_132:
                        em_sleep_dmg_timer_set(em);
                        em14_act_set(em, 0, 0x14, 2);
                    }
                }
            } else {
block_125:
                em_sleep2_dmg_timer_set(em);
                em14_act_set(em, 0, 0x1D, 2);
            }
        }
        break;
    case 10:                                        /* switch 2 */
        temp_v1_3 = em->x15;
        switch (temp_v1_3) {                        /* switch 3; irregular */
        case 20:                                    /* switch 3 */
            em14_act_set(em, 0, 0x18, 2);
            em->x839 = 0;
            break;
        case 27:                                    /* switch 3 */
            em14_act_set(em, 0, 0x1C, 2);
            em->x839 = 0;
            break;
        case 29:                                    /* switch 3 */
            em14_act_set(em, 4, 0x12, 2);
            em->x839 = 0;
            break;
        }
        break;
    case 11:                                        /* switch 2 */
        em14_act_set(em, 4, 4, 2);
        break;
    case 12:                                        /* switch 2 */
        pl_flag_clr((PLW *) em, 0x20000);
        temp_v0_6 = em->x388;
        if (temp_v0_6 == 2) {
            em14_act_set(em, 4, 8, 2);
        } else if (temp_v0_6 == 4) {
            em14_act_set(em, 4, 6, 2);
        } else {
            temp_a0_4 = (u8) em->x38E;
            switch (temp_a0_4) {                    /* switch 4 */
            case 0:                                 /* switch 4 */
            case 7:                                 /* switch 4 */
                em14_act_set(em, 4, 0, 2);
                break;
            case 6:                                 /* switch 4 */
                if (em->kind == 0xE) {
                    temp_v1_4 = w->x1A;
                    if (temp_v1_4 == 0) {
                        if ((s32) M2C_FIELD(((temp_a0_4 * 8) + em), u8 *, 0x30A) <= 0) {
                            goto block_160;
                        }
                        goto block_163;
                    }
block_160:
                    if ((temp_v1_4 == 1) && ((s32) M2C_FIELD((((temp_a0_4 & 0xFF) * 8) + em), u8 *, 0x30A) >= 2)) {
block_163:
                        em14_act_set(em, 4, 7, 2);
                    } else {
                        em14_act_set(em, 4, 2, 2);
                    }
                } else if ((w->x1A == 0) && ((s32) M2C_FIELD(((temp_a0_4 * 8) + em), u8 *, 0x30A) >= 2)) {
                    em14_act_set(em, 4, 7, 2);
                } else {
                    em14_act_set(em, 4, 2, 2);
                }
                break;
            case 5:                                 /* switch 4 */
                em14_act_set(em, 4, 2, 2);
                break;
            case 1:                                 /* switch 4 */
            case 2:                                 /* switch 4 */
                em14_act_set(em, 4, 3, 2);
                break;
            default:                                /* switch 4 */
                if ((s32) M2C_FIELD((((temp_a0_4 & 0xFF) * 8) + em), u8 *, 0x30A) >= 2) {
                    em14_act_set(em, 4, 5, 2);
                    if ((u8) em->x38E != 3) {
                        em14_act_set(em, 4, 5, 2);
                    } else {
                        em14_act_set(em, 4, 0x10, 2);
                    }
                } else {
                    em14_act_set(em, 4, 1, 2);
                }
                break;
            }
        }
        break;
    case 13:                                        /* switch 2 */
        temp_v0_7 = em->x388;
        if ((temp_v0_7 != 2) && (temp_v0_7 != 4)) {
            em14_act_set(em, 4, 0, 2);
            em->x839 = 0;
        }
        break;
    }
    if ((quest_w.no == 0xAB) && (em->stg == 0x35) && (Event_flag_ck(0x12) == 0)) {
        if ((*(u8 *)0x3F360F == 1) && (em->mode != 6)) {
            em14_act_set(em, 6, 0, 1);
        }
    } else if (em->x734 != 3) {

    } else if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em14_main_sub(em, w);
    if (em->x6FF != 0) {
        em14_main_sub(em, w);
        em->x6FF = 0;
    }
    if ((em->x7E9 != 0) && (game_w.stage == em->stg)) {
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
        temp_a0_5 = em->x388;
        if (temp_a0_5 != 0) {
            if (temp_a0_5 == 3) {
                goto block_211;
            }
        } else {
block_211:
            temp_f1_4 = em->x7E4;
            em->x5AC = temp_f1_4;
            if (em->pos[1] < temp_f1_4) {
                em->pos[1] = temp_f1_4;
            }
        }
    }
}

void em14_main_sub(EMW *em, EM14W *w) {
    u8 temp_v1;

    em->mode_old = em->mode;
    em->x15_old = em->x15;
    temp_v1 = em->mode;
    switch (temp_v1) {
    case 0:
        em_move00_005BB460(em, w);
        break;
    case 1:
        em_move01_005BB690(em, w);
        break;
    case 2:
        em_move02_005BB760(em, w);
        break;
    case 3:
        em_move03_005BB900(em, w);
        break;
    case 4:
        em_move04_005BBA60(em, w);
        break;
    case 5:
        em_move05_005BBBC0(em, w);
        break;
    case 6:
        em_move06_005BBC30(em, w);
        break;
    case 7:
        em_move06_005BBC30(em, w);
        break;
    }
    if ((em->pos[0] <= 0.0f) || (em->pos[2] <= 0.0f)) {
        em_dur_set(em, 0);
    }
}

void em14_uvmove(EMW *em) {
    EMW *var_t1;
    EMW *var_t2;
    s32 temp_t7;
    s32 temp_t7_2;
    s32 var_t3;
    s32 var_t5;
    s32 var_t5_2;
    u16 temp_t4;
    u16 temp_t5;
    u16 temp_t5_2;
    u8 temp_t4_3;
    void *temp_t4_2;

    var_t3 = 0;
    var_t2 = em;
    var_t1 = em;
    do {
        temp_t4 = M2C_FIELD(var_t2, u16 *, 0x5F0);
        if (temp_t4 != 0xFFFF) {
            M2C_FIELD(var_t2, u16 *, 0x5F0) = (u16) (temp_t4 + 1);
        }
        temp_t4_2 = em + var_t3;
        temp_t4_3 = M2C_FIELD(temp_t4_2, u8 *, 0x5F8);
        switch (temp_t4_3) {                        /* irregular */
        case 0xFF:
            break;
        case 0x0:
            M2C_FIELD(var_t1, f32 *, 0x5C0) = 0.0f;
            M2C_FIELD(var_t1, f32 *, 0x5C4) = 0.0f;
            M2C_FIELD(var_t2, u16 *, 0x5F0) = 0xFFFF;
            M2C_FIELD(temp_t4_2, u8 *, 0x5F8) = 0xFF;
            break;
        case 0x1:
            temp_t5 = M2C_FIELD(var_t2, u16 *, 0x5F0);
            if ((s32) temp_t5 >= 0x3E) {
                M2C_FIELD(var_t1, f32 *, 0x5C0) = 0.0f;
                M2C_FIELD(var_t1, f32 *, 0x5C4) = 0.0f;
                M2C_FIELD(var_t2, u16 *, 0x5F0) = 0xFFFF;
                M2C_FIELD(temp_t4_2, u8 *, 0x5F8) = 0xFF;
            } else {
                temp_t7 = (temp_t5 >> 1) + 1;
                var_t5 = temp_t7 >> 3;
                M2C_FIELD(var_t1, f32 *, 0x5C0) = (f32) (0.125f * (f32) (temp_t7 % 8));
                if (temp_t7 < 0) {
                    var_t5 = (s32) (temp_t7 + 7) >> 3;
                }
                M2C_FIELD(var_t1, f32 *, 0x5C4) = (f32) (0.25f * (f32) (var_t5 % 4));
            }
            break;
        case 0x2:
            M2C_FIELD(var_t1, f32 *, 0x5C0) = 0.125f;
            M2C_FIELD(var_t1, f32 *, 0x5C4) = 0.0f;
            M2C_FIELD(var_t2, u16 *, 0x5F0) = 0xFFFF;
            M2C_FIELD(temp_t4_2, u8 *, 0x5F8) = 0xFF;
            break;
        case 0x3:
            temp_t5_2 = M2C_FIELD(var_t2, u16 *, 0x5F0);
            if ((s32) temp_t5_2 >= 0xC) {
                M2C_FIELD(var_t1, f32 *, 0x5C0) = 0.0f;
                M2C_FIELD(var_t1, f32 *, 0x5C4) = 0.0f;
                M2C_FIELD(var_t2, u16 *, 0x5F0) = 0xFFFF;
                M2C_FIELD(temp_t4_2, u8 *, 0x5F8) = 0xFF;
            } else {
                temp_t7_2 = (temp_t5_2 >> 1) + 2;
                var_t5_2 = temp_t7_2 >> 2;
                M2C_FIELD(var_t1, f32 *, 0x5C0) = (f32) (0.125f * (f32) (temp_t7_2 % 4));
                if (temp_t7_2 < 0) {
                    var_t5_2 = (s32) (temp_t7_2 + 3) >> 2;
                }
                M2C_FIELD(var_t1, f32 *, 0x5C4) = (f32) (0.25f * (f32) (var_t5_2 % 4));
            }
            break;
        }
        var_t3 += 1;
        var_t2 += 2;
        var_t1 += 0xC;
    } while (var_t3 < 4);
}

static void sound_call_sub_005BCB30(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_005BCBA0(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_005BCB30(em, se, joint);
    }
}

static void sound_call_parts_005BCC00(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

static void quake_call_005BCCA0(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

static void move_default_005BCCF0(EMW *em) {
    M2C_FIELD(em, s32 *, 0x5C0) = 0;
    M2C_FIELD(em, s32 *, 0x5C4) = 0;
    M2C_FIELD(em, u16 *, 0x5F0) = 0xFFFF;
    M2C_FIELD(em, u8 *, 0x5F8) = 0xFF;
    M2C_FIELD(em, s32 *, 0x5CC) = 0;
    M2C_FIELD(em, s32 *, 0x5D0) = 0;
    M2C_FIELD(em, u16 *, 0x5F2) = 0xFFFF;
    M2C_FIELD(em, u8 *, 0x5F9) = 0xFF;
    M2C_FIELD(em, s32 *, 0x5D8) = 0;
    M2C_FIELD(em, s32 *, 0x5DC) = 0;
    M2C_FIELD(em, u16 *, 0x5F4) = 0xFFFF;
    M2C_FIELD(em, u8 *, 0x5FA) = 0xFF;
    M2C_FIELD(em, s32 *, 0x5E4) = 0;
    M2C_FIELD(em, s32 *, 0x5E8) = 0;
    M2C_FIELD(em, u16 *, 0x5F6) = 0xFFFF;
    M2C_FIELD(em, u8 *, 0x5FB) = 0xFF;
}

static void ef_move_sub_005BCD40(EMW *em, EM14W *w) {
    f32 sp50[3];
    FLMAT m50;
    f32 v3[3];
    f32 v[3];
    f32 v2[3];
    s16 temp_v1;
    u16 temp_a0;

    temp_a0 = em->char0;
    if (temp_a0 != w->anim) {
        w->anim = (s16) temp_a0;
    }
    temp_v1 = w->anim;
    switch (temp_v1) {                              /* irregular */
    case 0x3E9:
        break;
    case 0x3EB:
        if (em->x388 != 4) {
            sound_call_005BCBA0(em, 0x34, 1, 0x14);
            sound_call_005BCBA0(em, 0x74, 1, 0x1A);
            quake_call_005BCCA0(em, 0x34, 1);
            quake_call_005BCCA0(em, 0x74, 1);
            if (em_frame_check(em, 0, 2.0f) != 0) {
                shell18_set(em, 2);
            }
            if (em_frame_check(em, 0, 50.0f) != 0) {
                shell18_set(em, 3);
            }
            if (em_frame_check(em, 0, 120.0f) != 0) {
                shell18_set(em, 4);
            }
        }
        break;
    case 0x3ED:
        if (em->x388 != 4) {
            sound_call_005BCBA0(em, 0x19, 1, 0x14);
            sound_call_005BCBA0(em, 0x38, 1, 0x1A);
            quake_call_005BCCA0(em, 0x3C, 1);
            if (em_frame_check(em, 0, 2.0f) != 0) {
                shell18_set(em, 5);
            }
            if (em_frame_check(em, 0, 30.0f) != 0) {
                shell18_set(em, 6);
            }
            if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(*(u16 *)0x3F340E & 3)) {
                Eft20_set(1.0f, em, 0x1A, 1);
                return;
            }
        }
        break;
    case 0x3EE:
        if (em->x388 != 4) {
            sound_call_005BCBA0(em, 0x19, 1, 0x1A);
            sound_call_005BCBA0(em, 0x38, 1, 0x14);
            quake_call_005BCCA0(em, 0x3C, 1);
            if (em_frame_check(em, 0, 30.0f) != 0) {
                shell18_set(em, 7);
            }
            if (em_frame_check(em, 0, 2.0f) != 0) {
                shell18_set(em, 8);
            }
            if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, 0);
                return;
            }
        }
        break;
    case 0x3EF:
        sound_call_005BCBA0(em, 6, 0x20, 0x23);
        sound_call_005BCBA0(em, 0xE, 1, 0x1A);
        sound_call_005BCBA0(em, 0x26, 1, 0x14);
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell18_set(em, 9);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0xA);
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 2.0f, 18.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, 1);
            }
            if ((em_frame_check3(em, 0, 20.0f, 70.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, 0);
                return;
            }
        }
        break;
    case 0x3F0:
        sound_call_005BCBA0(em, 6, 0x20, 0x23);
        sound_call_005BCBA0(em, 0xE, 1, 0x14);
        sound_call_005BCBA0(em, 0x26, 1, 0x1A);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0xB);
        }
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell18_set(em, 0xC);
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 2.0f, 18.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, 0);
            }
            if ((em_frame_check3(em, 0, 20.0f, 70.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, 1);
                return;
            }
        }
        break;
    case 0x3F2:
        sound_call_005BCBA0(em, 6, 0x2B, 0x23);
        sound_call_005BCBA0(em, 6, 4, 0x1A);
        sound_call_005BCBA0(em, 0x4C, 0, 0x14);
        sound_call_005BCBA0(em, 0x1C, 0x12, 0x14);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            shell18_set(em, 0x19);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0x1A);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            shell18_set(em, 0x1A);
            return;
        }
        break;
    case 0x3F3:
        sound_call_005BCBA0(em, 0xE, 0xB, 6);
        sound_call_005BCBA0(em, 0x12, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x46, 0xB, 6);
        sound_call_005BCBA0(em, 0x42, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x7A, 0xB, 6);
        sound_call_005BCBA0(em, 0x7E, 0xB, 0xC);
        if ((em_frame_check(em, 0, 52.0f) == 0) && (em_frame_check(em, 0, 104.0f) == 0)) {
            if (em_frame_check(em, 0, 162.0f) != 0) {
                goto block_132;
            }
        } else {
block_132:
            Eft13_set_em_scl(em, 2, 5.0f, 7);
            return;
        }
        break;
    case 0x3F4:
        sound_call_005BCBA0(em, 6, 0xC, 6);
        sound_call_005BCBA0(em, 4, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x40, 0xC, 6);
        sound_call_005BCBA0(em, 0x3C, 0xC, 0xC);
        break;
    case 0x3F5:
        sound_call_005BCBA0(em, 0xE, 0xF, 6);
        sound_call_005BCBA0(em, 0x12, 0xF, 0xC);
        sound_call_005BCBA0(em, 0x2C, 0xF, 6);
        sound_call_005BCBA0(em, 0x28, 0xF, 0xC);
        sound_call_005BCBA0(em, 0x42, 0xF, 6);
        sound_call_005BCBA0(em, 0x46, 0xF, 0xC);
        if ((em_frame_check(em, 0, 26.0f) != 0) || (em_frame_check(em, 0, 52.0f) != 0)) {
            Eft13_set_em_scl(em, 1, 5.0f, 7);
            return;
        }
        break;
    case 0x3F7:
        sound_call_005BCBA0(em, 4, 0xC, 6);
        sound_call_005BCBA0(em, 8, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x3A, 0xB, 6);
        sound_call_005BCBA0(em, 0x3E, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x8E, 0xB, 6);
        sound_call_005BCBA0(em, 0x8A, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xD4, 0xB, 6);
        sound_call_005BCBA0(em, 0xD8, 0xB, 0xC);
        if ((em_frame_check(em, 0, 8.0f) == 0) && (em_frame_check(em, 0, 86.0f) == 0)) {
            if (em_frame_check(em, 0, 160.0f) != 0) {
                goto block_141;
            }
        } else {
block_141:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 70.0f, 84.0f) == 0) && (em_frame_check3(em, 0, 146.0f, 156.0f) == 0)) {
                if (em_frame_check3(em, 0, 220.0f, 234.0f) != 0) {
                    goto block_148;
                }
            } else {
block_148:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3F9:
        if (em->x388 == 4) {
            if ((game_w.stage == em->stg) && !(GAME_X1E16 & 3)) {
                SetVector(sp50, em->pos[0], em->x7E4, em->pos[2]);
                Eft13_set_pos(20.0f, sp50, 0x14);
                return;
            }
        } else {
            sound_call_005BCBA0(em, 4, 0x27, 0x23);
            sound_call_005BCBA0(em, 4, 0x16, 0);
            sound_call_005BCBA0(em, 4, 0xB, 6);
            sound_call_005BCBA0(em, 8, 0xB, 0xC);
            sound_call_005BCBA0(em, 0x1A, 1, 0x14);
            sound_call_005BCBA0(em, 0x62, 0x1E, 0x14);
            sound_call_005BCBA0(em, 0x78, 6, 0x14);
            sound_call_005BCBA0(em, 0x80, 0x38, 0x14);
            sound_call_005BCBA0(em, 0x90, 6, 0x1A);
            sound_call_005BCBA0(em, 0x96, 0x38, 0x14);
            if (em_frame_check(em, 0, 106.0f) != 0) {
                shell18_set(em, 0xE);
            }
            if ((em_frame_check(em, 0, 100.0f) != 0) || (em_frame_check(em, 0, 154.0f) != 0)) {
                Eft20_set(1.0f, em, 2, 0);
            }
            if ((em_frame_check(em, 0, 88.0f) != 0) || (em_frame_check(em, 0, 132.0f) != 0)) {
                Eft20_set(1.0f, em, 3, 0);
            }
            if (em->x8B6 != 0) {
                if ((em_frame_check(em, 0, 106.0f) != 0) || (em_frame_check(em, 0, 132.0f) != 0) || (em_frame_check(em, 0, 158.0f) != 0)) {
                    Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 1) + 2));
                }
                if ((em_frame_check3(em, 0, 4.0f, 50.0f) != 0) && !(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3FA:
        sound_call_005BCBA0(em, 4, 0x23, 0x23);
        sound_call_005BCBA0(em, 0xC, 4, 0x14);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x38, 0xF, 6);
        sound_call_005BCBA0(em, 0x3C, 0xF, 0xC);
        sound_call_005BCBA0(em, 0x8A, 0xB, 6);
        sound_call_005BCBA0(em, 0x8E, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xBE, 0xB, 6);
        sound_call_005BCBA0(em, 0xC2, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xEE, 0xB, 6);
        sound_call_005BCBA0(em, 0xF2, 0xB, 0xC);
        if ((em_frame_check(em, 0, 74.0f) == 0) && (em_frame_check(em, 0, 140.0f) == 0) && (em_frame_check(em, 0, 194.0f) == 0)) {
            if (em_frame_check(em, 0, 240.0f) != 0) {
                goto block_176;
            }
        } else {
block_176:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 10.0f, 64.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
            if ((em_frame_check3(em, 0, 136.0f, 140.0f) == 0) && (em_frame_check3(em, 0, 188.0f, 192.0f) == 0)) {
                if (em_frame_check3(em, 0, 136.0f, 234.0f) != 0) {
                    goto block_186;
                }
            } else {
block_186:
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x3FB:
        sound_call_005BCBA0(em, 2, 0xB, 6);
        sound_call_005BCBA0(em, 6, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xC, 4, 0x1A);
        sound_call_005BCBA0(em, 4, 1, 0x14);
        sound_call_005BCBA0(em, 0xE, 9, 0);
        sound_call_005BCBA0(em, 0x3A, 0x16, 0);
        sound_call_005BCBA0(em, 0x74, 0, 0x14);
        sound_call_005BCBA0(em, 0x96, 1, 0x14);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0xD);
        }
        if (em_frame_check(em, 0, 16.0f) != 0) {
            ground_land_eff_set_005C1120(em);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check(em, 0, 14.0f) != 0) {
                Eft20_set(1.0f, em, 0x19, 2);
                Eft20_set(1.0f, em, 0x19, 3);
            }
            if ((em_frame_check3(em, 0, 18.0f, 44.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x405:
        sound_call_005BCBA0(em, 0x38, 0x1F, 0x23);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 4, 0xF, 0xC);
        sound_call_005BCBA0(em, 8, 0xF, 6);
        sound_call_005BCBA0(em, 0x78, 0xE, 0xC);
        sound_call_005BCBA0(em, 0x7C, 0xE, 6);
        break;
    case 0x407:
        sound_call_005BCBA0(em, 2, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0xB6, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0x16C, 0x2D, 0x23);
        v[1] = 10.0f;
        v[2] = 140.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v, 1.60000002f);
        break;
    case 0x408:
        sound_call_005BCBA0(em, 4, 0x2F, 0x23);
        sound_call_005BCBA0(em, 0x6C, 0x1F, 0x23);
        sound_call_005BCBA0(em, 0x46, 0, 0x1A);
        sound_call_005BCBA0(em, 0x8E, 3, 0x14);
        sound_call_005BCBA0(em, 4, 0x17, 0);
        sound_call_005BCBA0(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005BCBA0(em, 4, 0x2F, 0x23);
        sound_call_005BCBA0(em, 0x46, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0x7E, 3, 0x1A);
        sound_call_005BCBA0(em, 0xB6, 3, 0x14);
        sound_call_005BCBA0(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_005BCBA0(em, 0x24, 0x2F, 0x23);
        v2[1] = 10.0f;
        v2[2] = 140.0f;
        v2[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.60000002f);
        break;
    case 0x40C:
        sound_call_005BCBA0(em, 0x24, 0x24, 0x23);
        sound_call_005BCBA0(em, 0x14, 0, 0x1A);
        sound_call_005BCBA0(em, 0x28, 0, 0x14);
        sound_call_005BCBA0(em, 0x3C, 0x15, 0x22);
        sound_call_005BCBA0(em, 0x3C, 0xC, 0xC);
        sound_call_005BCBA0(em, 0xC, 0xC, 6);
        sound_call_005BCBA0(em, 0xA6, 0, 0x1A);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell18_set(em, 1);
        }
        if (em_frame_check(em, 0, 46.0f) != 0) {
            shell18_set(em, 0x1F);
            return;
        }
        break;
    case 0x40D:
        sound_call_005BCBA0(em, 0x1E, 0x20, 0x23);
        sound_call_005BCBA0(em, 0xC, 0xC, 0x1A);
        sound_call_005BCBA0(em, 0x1C, 2, 0x1A);
        sound_call_005BCBA0(em, 0x2E, 3, 0x14);
        sound_call_005BCBA0(em, 0x48, 1, 0x1A);
        break;
    case 0x40E:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x57, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x75, 0x23);
        }
        sound_call_005BCBA0(em, 0x48, 0x16, 0);
        sound_call_005BCBA0(em, 0xA4, 9, 0);
        sound_call_005BCBA0(em, 0x90, 3, 0xC);
        sound_call_005BCBA0(em, 0x88, 0xE, 6);
        sound_call_005BCBA0(em, 0xB2, 4, 0x22);
        if (em_frame_check(em, 0, 148.0f) != 0) {
            shell18_set(em, 0xF);
        }
        if (em_frame_check(em, 0, 156.0f) != 0) {
            Eft20_set(1.0f, em, 0, 0);
            return;
        }
        break;
    case 0x40F:
        sound_call_005BCBA0(em, 2, 0x2F, 0x23);
        sound_call_005BCBA0(em, 2, 0x17, 0);
        sound_call_005BCBA0(em, 0x40, 3, 0x1A);
        sound_call_005BCBA0(em, 0x66, 0x10, 0x1A);
        break;
    case 0x413:
        sound_call_005BCBA0(em, 8, 0x23, 0x23);
        sound_call_005BCBA0(em, 0xE, 0xF, 6);
        sound_call_005BCBA0(em, 0x1E, 0x14, 0x2A);
        sound_call_005BCBA0(em, 0x2A, 0, 0x1A);
        sound_call_005BCBA0(em, 0x48, 1, 0x14);
        quake_call_005BCCA0(em, 0x2C, 1);
        quake_call_005BCCA0(em, 0x49, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell18_set(em, 0x11);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x41B:
        sound_call_005BCBA0(em, 4, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x26, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0x38, 0x12, 0x1A);
        sound_call_005BCBA0(em, 0x58, 6, 0x1A);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x96, 0x56, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x96, 0x74, 0x23);
        }
        sound_call_005BCBA0(em, 0xEE, 0x16, 0);
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft20_set(0.800000012f, em, 2, 2);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0x1B);
        }
        if (em_frame_check(em, 0, 168.0f) != 0) {
            shell18_set(em, 0x1B);
        }
        if (em_frame_check(em, 0, 56.0f) != 0) {
            shell18_set(em, 0x1C);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 156.0f, 248.0f) == 0) {
                if (em_frame_check3(em, 0, 276.0f, 346.0f) != 0) {
                    goto block_246;
                }
            } else {
block_246:
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x41E:
        sound_call_005BCBA0(em, 2, 0x12, 0);
        sound_call_005BCBA0(em, 0x1C, 4, 0x1A);
        sound_call_005BCBA0(em, 0x38, 4, 0x14);
        sound_call_005BCBA0(em, 0x1C, 5, 0x1A);
        sound_call_005BCBA0(em, 0x78, 4, 0x14);
        sound_call_005BCBA0(em, 0x108, 4, 0x1A);
        sound_call_005BCBA0(em, 4, 0x14, 0x23);
        sound_call_005BCBA0(em, 6, 0x23, 0x23);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x4E, 0x56, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x4E, 0x74, 0x23);
        }
        sound_call_005BCBA0(em, 0xA, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x10, 0xC, 6);
        if (em_frame_check(em, 0, 102.0f) != 0) {
            shell18_set(em, 0x12);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 16.0f, 26.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x424:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 6, 0x53, 0x23);
        } else {
            sound_call_005BCBA0(em, 6, 0x71, 0x23);
        }
        sound_call_005BCBA0(em, 0x48, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x30, 6, 0x1A);
        sound_call_005BCBA0(em, 0x30, 0x11, 0x14);
        sound_call_005BCBA0(em, 0x30, 3, 0x14);
        sound_call_005BCBA0(em, 0x74, 3, 0x14);
        sound_call_005BCBA0(em, 0x94, 3, 0x1A);
        sound_call_005BCBA0(em, 0x34, 0xC, 6);
        sound_call_005BCBA0(em, 0x38, 0xD, 0xC);
        sound_call_005BCBA0(em, 0xC2, 3, 0x1A);
        if (em_frame_check(em, 0, 44.0f) != 0) {
            Eft13_set_em_scl(em, 0x1B, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 62.0f) != 0) {
            Eft13_set_em_scl(em, 0x16, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 80.0f) != 0) {
            Eft20_set(0.600000024f, em, 1, 0x80);
            return;
        }
        break;
    case 0x425:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 6, 0x53, 0x23);
        } else {
            sound_call_005BCBA0(em, 6, 0x71, 0x23);
        }
        sound_call_005BCBA0(em, 0xA, 0x12, 0x1A);
        sound_call_005BCBA0(em, 0x26, 6, 0x1A);
        sound_call_005BCBA0(em, 0x4E, 4, 0x14);
        sound_call_005BCBA0(em, 0x6E, 3, 0x14);
        sound_call_005BCBA0(em, 0x5A, 0x15, 0);
        sound_call_005BCBA0(em, 0xAA, 3, 0x1A);
        if (em_frame_check(em, 0, 24.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
            return;
        }
        break;
    case 0x426:
        sound_call_005BCBA0(em, 4, 0x27, 0x23);
        sound_call_005BCBA0(em, 0x56, 0x1F, 0x23);
        sound_call_005BCBA0(em, 0x10, 3, 0x14);
        sound_call_005BCBA0(em, 4, 0xD, 6);
        sound_call_005BCBA0(em, 8, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x62, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x7E, 3, 0x1A);
        break;
    case 0x427:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0);
        sound_call_005BCBA0(em, 0x4C, 0x1F, 0x23);
        break;
    case 0x428:
        sound_call_005BCBA0(em, 4, 0x12, 0x1A);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0);
        sound_call_005BCBA0(em, 0x44, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x8E, 3, 0x14);
        break;
    case 0x429:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x54, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x72, 0x23);
        }
        sound_call_005BCBA0(em, 0xA8, 0x2A, 0x23);
        sound_call_005BCBA0(em, 4, 0x27, 0x2C);
        sound_call_005BCBA0(em, 0xFC, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x130, 0x27, 0x23);
        sound_call_005BCBA0(em, 0x3E, 4, 0x2C);
        sound_call_005BCBA0(em, 0x12, 0x12, 0x14);
        sound_call_005BCBA0(em, 0x2C, 6, 0x1A);
        sound_call_005BCBA0(em, 0x3A, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0xD2, 3, 0x1A);
        sound_call_005BCBA0(em, 0x142, 3, 0x1A);
        sound_call_005BCBA0(em, 0x3E, 9, 0);
        sound_call_005BCBA0(em, 0x4A, 9, 0);
        sound_call_005BCBA0(em, 0x64, 0x12, 0);
        sound_call_005BCBA0(em, 0xA2, 0x12, 0);
        sound_call_005BCBA0(em, 0xE, 0xC, 6);
        sound_call_005BCBA0(em, 0x12, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x11E, 0xD, 6);
        sound_call_005BCBA0(em, 0x122, 0xD, 0xC);
        if (em_frame_check(em, 0, 20.0f) != 0) {
            Eft20_set(1.0f, em, 3, 0x80);
        }
        if ((em_frame_check(em, 0, 128.0f) != 0) || (em_frame_check(em, 0, 164.0f) != 0)) {
            Eft20_set(0.600000024f, em, 3, 0x80);
        }
        if (em_frame_check(em, 0, 60.0f) != 0) {
            Eft20_set(1.0f, em, 1, 0x80);
            return;
        }
        break;
    case 0x42A:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0);
        break;
    case 0x42B:
        sound_call_005BCBA0(em, 0x46, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0x46, 0x17, 0);
        break;
    case 0x42C:
        sound_call_005BCBA0(em, 0x1C, 2, 0x1A);
        sound_call_005BCBA0(em, 0x30, 4, 0x14);
        sound_call_005BCBA0(em, 0xDC, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0x1E, 0x16, 0);
        sound_call_005BCBA0(em, 0x108, 9, 0);
        sound_call_005BCBA0(em, 0x108, 7, 0);
        sound_call_005BCBA0(em, 0x114, 0x17, 0);
        sound_call_005BCBA0(em, 0x180, 0x16, 0);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x18, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x18, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 0x8C, 0x2C, 0x23);
        sound_call_005BCBA0(em, 0x192, 0x2B, 0x23);
        sound_call_005BCBA0(em, 0x174, 0xF, 6);
        sound_call_005BCBA0(em, 0x176, 0xE, 0xC);
        if (em_frame_check(em, 0, 268.0f) != 0) {
            Eft20_set(0.899999976f, em, 0, 0);
            return;
        }
        break;
    case 0x42D:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x18, 9, 0);
        sound_call_005BCBA0(em, 0x18, 0x11, 0);
        quake_call_005BCCA0(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x80);
            return;
        }
        break;
    case 0x42E:
    case 0x433:
        sound_call_005BCBA0(em, 8, 0x16, 0);
        sound_call_005BCBA0(em, 0x32, 0x1F, 0x23);
        break;
    case 0x42F:
        sound_call_005BCBA0(em, 4, 0x16, 0x23);
        sound_call_005BCBA0(em, 0x1E, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x30, 1, 0x14);
        sound_call_005BCBA0(em, 0x46, 1, 0x1A);
        break;
    case 0x430:
        sound_call_005BCBA0(em, 4, 0x16, 0x23);
        sound_call_005BCBA0(em, 0x1E, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x30, 1, 0x1A);
        sound_call_005BCBA0(em, 0x46, 1, 0x14);
        break;
    case 0x431:
        sound_call_005BCBA0(em, 4, 0x1E, 0x22);
        sound_call_005BCBA0(em, 4, 0x1D, 6);
        sound_call_005BCBA0(em, 8, 0x1D, 0xC);
        sound_call_005BCBA0(em, 0xA, 0x2A, 0x23);
        sound_call_005BCBA0(em, 0x4C, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x18, 0x12, 0);
        sound_call_005BCBA0(em, 0x1E, 0x10, 0);
        sound_call_005BCBA0(em, 0x1E, 9, 0);
        sound_call_005BCBA0(em, 0x22, 7, 0);
        quake_call_005BCCA0(em, 0x1A, 2);
        if ((em_frame_check2(em, 0, 28.0f) != 0) && (em_frame_check2(em, 0, 52.0f) == 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0xA, 0x80);
            return;
        }
        break;
    case 0x432:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x18, 9, 0);
        sound_call_005BCBA0(em, 0x18, 0x11, 0);
        quake_call_005BCCA0(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x81);
            return;
        }
        break;
    case 0x434:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x10, 0x5A, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x10, 0x78, 0x23);
        }
        sound_call_005BCBA0(em, 0x6E, 0x37, 0x23);
        em_mahi_eff_set(em, 2);
        break;
    case 0x435:
        sound_call_005BCBA0(em, 4, 0x2B, 0x23);
        sound_call_005BCBA0(em, 0x14, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x3A, 0x14, 0);
        break;
    case 0x436:
        sound_call_005BCBA0(em, 0xA, 9, 0);
        sound_call_005BCBA0(em, 0xA, 7, 0x2A);
        sound_call_005BCBA0(em, 0xC, 0x11, 0);
        sound_call_005BCBA0(em, 0x10, 0x2B, 0x23);
        if (em_frame_check(em, 0, 8.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0);
            return;
        }
        break;
    case 0x438:
        sound_call_005BCBA0(em, 2, 0x16, 0x22);
        sound_call_005BCBA0(em, 0x64, 0x17, 0x22);
        sound_call_005BCBA0(em, 0x26, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0xC8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x1C, 3, 0x1A);
        sound_call_005BCBA0(em, 0x140, 3, 0x1A);
        break;
    case 0x439:
        sound_call_005BCBA0(em, 2, 0x16, 0x22);
        sound_call_005BCBA0(em, 0x64, 0x17, 0x22);
        sound_call_005BCBA0(em, 0x26, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0xC8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x1C, 3, 0x14);
        sound_call_005BCBA0(em, 0x140, 3, 0x14);
        break;
    case 0x43A:
        sound_call_005BCBA0(em, 4, 0x37, 0x23);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        break;
    case 0x43B:
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 4, 0x17, 0);
        sound_call_005BCBA0(em, 0xA, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0xA, 0, 0x1A);
        sound_call_005BCBA0(em, 0x1C, 0, 0x14);
        sound_call_005BCBA0(em, 0x34, 0, 0x1A);
        sound_call_005BCBA0(em, 0x58, 0, 0x14);
        sound_call_005BCBA0(em, 0x80, 0, 0x1A);
        sound_call_005BCBA0(em, 0xC6, 0, 0x14);
        sound_call_005BCBA0(em, 4, 0x2B, 0x23);
        sound_call_005BCBA0(em, 0xA8, 0x27, 0x23);
        sound_call_005BCBA0(em, 0xA2, 0xD, 6);
        sound_call_005BCBA0(em, 0xA6, 0xD, 0xC);
        break;
    case 0x442:
        sound_call_005BCBA0(em, 0x20, 0xD, 6);
        sound_call_005BCBA0(em, 0x24, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x4A, 0xD, 6);
        sound_call_005BCBA0(em, 0x4E, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x7C, 0xB, 6);
        sound_call_005BCBA0(em, 0x80, 0xB, 0xC);
        break;
    case 0x448:
        sound_call_005BCBA0(em, 0x16, 0x12, 0);
        sound_call_005BCBA0(em, 0x48, 0x12, 0);
        sound_call_005BCBA0(em, 4, 0xC, 6);
        sound_call_005BCBA0(em, 8, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x30, 0x13, 0);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x50, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x6E, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x16, 0);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 12.0f, 24.0f) != 0) || (em_frame_check3(em, 0, 46.0f, 90.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x449:
        sound_call_005BCBA0(em, 0x22, 0x16, 6);
        sound_call_005BCBA0(em, 0x26, 0x16, 0xC);
        sound_call_005BCBA0(em, 0x62, 4, 0x22);
        sound_call_005BCBA0(em, 0x6C, 3, 0x22);
        sound_call_005BCBA0(em, 4, 0x2A, 0x23);
        sound_call_005BCBA0(em, 4, 0x2B, 0x23);
        break;
    case 0x44A:
        sound_call_005BCBA0(em, 4, 0x16, 6);
        sound_call_005BCBA0(em, 8, 0x16, 0xC);
        sound_call_005BCBA0(em, 0x6C, 3, 0x22);
        sound_call_005BCBA0(em, 4, 0x28, 0x23);
        sound_call_005BCBA0(em, 0x46, 0x20, 0x23);
        break;
    case 0x44C:
        sound_call_005BCBA0(em, 0x14, 6, 0x1A);
        sound_call_005BCBA0(em, 0x14, 0x10, 0x14);
        sound_call_005BCBA0(em, 0x1A, 0x11, 0x14);
        sound_call_005BCBA0(em, 0x8E, 0, 0x14);
        sound_call_005BCBA0(em, 0x9A, 0, 0x1A);
        sound_call_005BCBA0(em, 0x50, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x68, 0x16, 0);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            shell18_set(em, 0x16);
        }
        if (em_frame_check(em, 0, 18.0f) != 0) {
            Eft20_set(1.79999995f, em, 2, 6);
        }
        if ((em_frame_check3(em, 0, 40.0f, 80.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft13_set_em_scl(em, 2, 7.0f, 3);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check(em, 0, 22.0f) != 0) {
                Eft20_set(1.0f, em, 0x19, 2);
                Eft20_set(1.0f, em, 0x19, 3);
            }
            if ((em_frame_check3(em, 0, 72.0f, 116.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x44F:
        sound_call_005BCBA0(em, 0x10, 0x11, 0x1A);
        sound_call_005BCBA0(em, 0x4E, 0x10, 0x14);
        sound_call_005BCBA0(em, 0xE, 0x1D, 6);
        sound_call_005BCBA0(em, 0x2C, 0x1D, 0xC);
        sound_call_005BCBA0(em, 0x3E, 0x1D, 6);
        sound_call_005BCBA0(em, 0x68, 0x1E, 0);
        sound_call_005BCBA0(em, 0x82, 0x16, 0);
        quake_call_005BCCA0(em, 0x68, 4);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0x20);
        }
        if (em_frame_check(em, 0, 20.0f) != 0) {
            Eft20_set(1.20000005f, em, 2, 0);
        }
        if (em_frame_check(em, 0, 82.0f) != 0) {
            Eft20_set(1.20000005f, em, 3, 0);
        }
        if (em_frame_check(em, 0, 104.0f) != 0) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if ((em_frame_check3(em, 0, 40.0f, 62.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft13_set_em_scl(em, 0xF, 5.0f, 3);
        }
        if ((em_frame_check(em, 0, 18.0f) != 0) || (em_frame_check(em, 0, 22.0f) != 0)) {
            Eft13_set_em_scl(em, 9, 5.0f, 3);
        }
        if ((em_frame_check(em, 0, 148.0f) != 0) || (em_frame_check(em, 0, 160.0f) != 0) || (em_frame_check(em, 0, 172.0f) != 0) || (em_frame_check(em, 0, 184.0f) != 0)) {
            Eft13_set_em_scl(em, 0x29, 6.0f, 3);
        }
        if (!(em->x948 & 1) && (em_frame_check(em, 0, 180.0f) != 0)) {
            Eft13_set_em_scl(em, 0x2A, 0.699999988f, 0x13);
            return;
        }
        break;
    case 0x450:
        sound_call_005BCBA0(em, 0xC, 0x1D, 0x22);
        sound_call_005BCBA0(em, 0xE, 0x1E, 0x22);
        sound_call_005BCBA0(em, 0x14, 0x2E, 0x23);
        sound_call_005BCBA0(em, 0xA8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x3C, 8, 0x14);
        sound_call_005BCBA0(em, 0x3C, 6, 0x14);
        sound_call_005BCBA0(em, 0x40, 2, 0x1A);
        sound_call_005BCBA0(em, 0xB2, 1, 0x14);
        sound_call_005BCBA0(em, 0xE8, 0, 0x1A);
        sound_call_005BCBA0(em, 0xAE, 0xC, 6);
        sound_call_005BCBA0(em, 0xB2, 0xC, 0xC);
        sound_call_005BCBA0(em, 0xC, 0x15, 0x22);
        quake_call_005BCCA0(em, 2, 5);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            shell18_set(em, 0x15);
        }
        if (em_frame_check(em, 0, 40.0f) != 0) {
            shell18_set(em, 0x21);
        }
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft13_set_em_scl(em, 0x22, 1.39999998f, 0x13);
            if (Em_stg_ck(em) != 0) {
                get_joint_pos_em(em, 0x22, sp50);
                sp50[1] = em->x5AC;
                Eft17_set_ex(sp50, (u16)em->ang[1], 8, 1.0f);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 12.0f, 36.0f) == 0) && (em_frame_check3(em, 0, 66.0f, 130.0f) == 0)) {
                if (em_frame_check3(em, 0, 168.0f, 204.0f) != 0) {
                    goto block_406;
                }
            } else {
block_406:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x451:
        sound_call_005BCBA0(em, 6, 7, 0x22);
        sound_call_005BCBA0(em, 6, 0x1D, 0x22);
        sound_call_005BCBA0(em, 0x44, 0xC, 6);
        sound_call_005BCBA0(em, 0x48, 0xC, 0xC);
        sound_call_005BCBA0(em, 4, 0x36, 0x23);
        sound_call_005BCBA0(em, 0xC, 0x15, 0x22);
        quake_call_005BCCA0(em, 0xC, 3);
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft13_set_em_scl(em, 2, 1.39999998f, 0x13);
            if (Em_stg_ck(em) != 0) {
                get_joint_pos_em(em, 2, sp50);
                sp50[1] = em->x5AC;
                Eft17_set_ex(sp50, (u16)em->ang[1], 8, 1.0f);
                return;
            }
        }
        break;
    case 0x452:
        sound_call_005BCBA0(em, 8, 1, 0x1A);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x8A, 0x16, 0);
        sound_call_005BCBA0(em, 0x12, 0x15, 0x22);
        sound_call_005BCBA0(em, 0x2C, 0x30, 0x22);
        sound_call_005BCBA0(em, 0x94, 0, 0x14);
        sound_call_005BCBA0(em, 0xB4, 0, 0x1A);
        sound_call_005BCBA0(em, 0x30, 0x1F, 0x23);
        if (em_frame_check(em, 0, 48.0f) != 0) {
            shell18_set(em, 0x18);
        }
        if (em_frame_check(em, 0, 50.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 5.0f, 3);
        }
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 5.0f, 3);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 86.0f, 152.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x453:
        sound_call_005BCBA0(em, 4, 0x34, 0x23);
        sound_call_005BCBA0(em, 0x32, 0x35, 0x23);
        sound_call_005BCBA0(em, 0x32, 0x15, 0x22);
        sound_call_005BCBA0(em, 0xA, 6, 0x1A);
        sound_call_005BCBA0(em, 0xA8, 0, 0x14);
        sound_call_005BCBA0(em, 0xD8, 0, 0x1A);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x74, 0xC, 6);
        sound_call_005BCBA0(em, 0x78, 0xC, 0xC);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell18_set(em, 0x10);
        }
        if (em_frame_check(em, 0, 10.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 44.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 5.0f, 3);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 8.0f, 24.0f) == 0) {
                if (em_frame_check3(em, 0, 136.0f, 172.0f) != 0) {
                    goto block_432;
                }
            } else {
block_432:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x454:
        sound_call_005BCBA0(em, 8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x10, 3, 0x14);
        sound_call_005BCBA0(em, 0x4A, 0, 0x1A);
        sound_call_005BCBA0(em, 0xC2, 0, 0x1A);
        sound_call_005BCBA0(em, 0x102, 0, 0x14);
        sound_call_005BCBA0(em, 0x3A, 0x14, 0x2A);
        sound_call_005BCBA0(em, 0x46, 0x15, 0x2C);
        sound_call_005BCBA0(em, 0x56, 8, 0x2C);
        sound_call_005BCBA0(em, 0x5A, 0x12, 0x2C);
        if (em_frame_check(em, 0, 70.0f) != 0) {
            shell18_set(em, 0x1D);
        }
        if (!(em->x948 & 1) && (em_frame_check(em, 0, 84.0f) != 0)) {
            Eft20_set(1.0f, em, 2, 7);
            return;
        }
        break;
    case 0x455:
        sound_call_005BCBA0(em, 4, 0x12, 0x1A);
        sound_call_005BCBA0(em, 4, 0x1F, 0x23);
        sound_call_005BCBA0(em, 0xA, 3, 0x1A);
        sound_call_005BCBA0(em, 0x70, 4, 0x14);
        sound_call_005BCBA0(em, 0xD8, 1, 0x1A);
        sound_call_005BCBA0(em, 0x108, 3, 0x14);
        sound_call_005BCBA0(em, 0x38, 0x1D, 0);
        sound_call_005BCBA0(em, 0x38, 0x14, 0);
        sound_call_005BCBA0(em, 0x54, 0x16, 0);
        if (em_frame_check(em, 0, 54.0f) != 0) {
            shell18_set(em, 0x1E);
        }
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 2.0f, 3);
        }
        if (em_frame_check(em, 0, 52.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 2.0f, 3);
        }
        if (em_frame_check(em, 0, 56.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 2.0f, 3);
            return;
        }
        break;
    case 0x457:
        sound_call_005BCBA0(em, 0xE, 2, 0x14);
        sound_call_005BCBA0(em, 0x13E, 4, 0x14);
        sound_call_005BCBA0(em, 0x170, 1, 0x1A);
        sound_call_005BCBA0(em, 4, 0x16, 0x23);
        sound_call_005BCBA0(em, 4, 0x1F, 0x23);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x64, 0x5B, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x64, 0x79, 0x23);
        }
        sound_call_005BCBA0(em, 0x130, 0x16, 0);
        if (em_frame_check(em, 0, 112.0f) != 0) {
            shell18_set(em, 0x13);
            Eft15_set3(em, 5, 1.0f, 6);
            return;
        }
        break;
    case 0x458:
        sound_call_005BCBA0(em, 0x32, 0x28, 0x23);
        sound_call_005BCBA0(em, 0x68, 0x27, 0x23);
        sound_call_005BCBA0(em, 0x22, 4, 0x14);
        sound_call_005BCBA0(em, 0x86, 0, 0x14);
        sound_call_005BCBA0(em, 0xB6, 1, 0x1A);
        sound_call_005BCBA0(em, 0x26, 0x15, 0x2A);
        sound_call_005BCBA0(em, 0x52, 0x15, 0x2A);
        sound_call_005BCBA0(em, 0x72, 0x15, 0x2A);
        sound_call_005BCBA0(em, 0x9C, 0x14, 0x2A);
        sound_call_005BCBA0(em, 0xC4, 0x13, 0x2A);
        if (em_frame_check(em, 0, 40.0f) != 0) {
            shell18_set(em, 0x14);
            return;
        }
        break;
    case 0x459:
        sound_call_005BCBA0(em, 4, 0x16, 0);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x5E, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x7C, 0x23);
        }
        sound_call_005BCBA0(em, 0x10E, 0x2B, 0x23);
        sound_call_005BCBA0(em, 4, 0x11, 0x14);
        sound_call_005BCBA0(em, 0x36, 4, 0x14);
        sound_call_005BCBA0(em, 0x86, 0, 0x14);
        sound_call_005BCBA0(em, 0xDC, 3, 0x14);
        sound_call_005BCBA0(em, 0x128, 3, 0x1A);
        break;
    case 0x45A:
        sound_call_005BCBA0(em, 4, 0x37, 0x23);
        sound_call_005BCBA0(em, 0x66, 0x34, 0x23);
        sound_call_005BCBA0(em, 0x22, 6, 0x1A);
        sound_call_005BCBA0(em, 0x1E, 0xF, 6);
        sound_call_005BCBA0(em, 0x22, 0xF, 0xC);
        quake_call_005BCCA0(em, 4, 3);
        quake_call_005BCCA0(em, 0x68, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            v3[1] = 30.0f;
            v3[0] = 0.0f;
            v3[2] = 100.0f;
            flvecApplyMat33_2(v3, get_joint_wmat_em(em, 0x27));
            get_joint_pos_em(em, 0x27, sp50);
            AddVector(sp50, sp50, v3);
            Eft02_set3(em, 0, 9, 0, sp50, 1.0f);
            Eft02_set3(em, 0, 9, 1, sp50, 1.0f);
            Eft13_set_pos2(10.0f, em, sp50, 0x22);
        }
        if (!(GAME_X1E16 & 0x1F)) {
            Eft21_set(em, 2);
            return;
        }
        break;
    case 0x45B:
        sound_call_005BCBA0(em, 4, 0x37, 0x23);
        sound_call_005BCBA0(em, 0x5C, 0x28, 0x23);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x52, 0xD, 6);
        sound_call_005BCBA0(em, 0x56, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x48, 0x1D, 0x22);
        sound_call_005BCBA0(em, 0x6C, 6, 0x1A);
        sound_call_005BCBA0(em, 0x8E, 5, 0x14);
        sound_call_005BCBA0(em, 0xA6, 0, 0x1A);
        if (em_frame_check(em, 0, 68.0f) != 0) {
            Eft21_set(em, 2);
            return;
        }
        break;
    default:
        move_default_005BCCF0(em);
        break;
    }
}

void em14_effect_move(EMW *em) {
    EM14W *w = (EM14W *)em->ex;
    u8 temp_a2;

    temp_a2 = w->eff;
    switch (temp_a2) {                              /* irregular */
    case 0:
        w->eff = temp_a2 + 1;
        break;
    case 1:
        ef_move_sub_005BCD40(em, w);
        break;
    }
    em14_uvmove(em);
}

static void ground_land_eff_set_005C1120(EMW *em) {
    f32 pos[3];

    if (game_w.stage == 0) {
        get_joint_pos_em(em, 0x14, pos);
        pos[1] = em->x5AC;
        if (pos[1] <= 46.0f) {
            eft11_set(em, pos, 1);
            get_joint_pos_em(em, 0x1A, pos);
            eft11_set(em, pos, 1);
        }
    } else {
        Eft20_set(1.0f, em, 0xB, 0);
    }
}

void em14_atk_end_sel(EMW *em, EM14W *w) {
    if (em->x734 == 3) {
        em14_to_normal(em, 0, 0);
        return;
    }
    if (((s32) em->x39A % 10) == 0) {
        em14_act_set(em, 0, 1, 1);
        return;
    }
    em14_to_normal(em, 0, 0);
}

void dummy_em_prog_005C1250(void) {

}


