/* em15 draft */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em15.h"

typedef struct FLYNEED {
    u8 _pad00[0x14];
    s32 x14;
} FLYNEED;
extern FLYNEED *em_hungry_tbl[];
typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 width;          /* 0x10 */
    f32 depth;          /* 0x14 */
    f32 floor_y;        /* 0x18 */
} STAGE_DATA;
STAGE_DATA *Stage_data_get(u8);
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
void target_kind_set(EMW *, f32 *);
void cmd_target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
void em_act_set(EMW *, int, u16);
void eft09_set(EMW *, int);
void em_cmd_reset(EMW *);
void Em_Sleep_End(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
void Eft04_set_time(EMW *, int, int, f32);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
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
void Eft20_set(f32, EMW *, int, int);
void Eft15_set3(EMW *, int, f32, int);
void Eft13_set_em(EMW *, int, int);
int em_frame_check3(EMW *, int, f32, f32);
void em15_act_set(EMW *em, int kind, u16 no, u16 arg);
s16 em_hp_vital_set2(EMW *, s16, s16);
void get_joint_pos_em(EMW *, int, f32 *);
void em_range_set(EMW *em, s8 no);
void NextStage_No_Set(EMW *);
void Em_Next_Stage_Pos(EMW *);
void NextStage_Dir_Set(EMW *, f32 *);
void em_area_move_init(EMW *em);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void em_search_data_set(EMW *em, u8 no);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_hungry_end(EMW *em);
void em_thirst_end(EMW *em);
void em_suimin_end(EMW *em);
void Em_Suimin_Start(EMW *em);
void em_hungry_add(EMW *em, s32 n);
void em_thirst_add(EMW *em, s32 n);
void em_hp_add(EMW *em, s16 n);
void em_ana_loop_cnt_set(EMW *em);
int em_sleep_hp_add(EMW *em, s16 n, s16 max, s16 step);
void em_niku_eat_set(EMW *em);
void Quest_enemy_capture();
extern s16 em15_stay_timer_tbl[];
extern s16 em15_runaway_timer_tbl[];

void em15_local_init(EMW *em);
void em15_init(EMW *em);
static void act_dist_select_005C2E90(EMW *em);
void em15_to_normal(EMW *em);
void em15_to_tenjo(EMW *em);
void em15_to_fly(EMW *em);
void em15_frame_reset(EMW *em, int i);
static void em_act00_005C3090(EMW *em, EM15W *w);
static void em_act01_005C3140(EMW *em, EM15W *w);
static void em_act02_005C31D0(EMW *em, EM15W *w);
static void em_act03_005C3280(EMW *em, EM15W *w);
static void em_act04_005C3330(EMW *em, EM15W *w);
static void em_act05_005C33B0(EMW *em, EM15W *w);
static void em_act06_005C3480(EMW *em, EM15W *w);
static void em_act07_005C3500(EMW *em, EM15W *w);
static void em_act08_005C35A0(EMW *em, EM15W *w);
static void em_act10_005C3630(EMW *em, EM15W *w);
static void em_act13_005C3730(EMW *em, EM15W *w);
static void em_act14_005C3840(EMW *em, EM15W *w);
static void em_act17_005C3BB0(EMW *em, EM15W *w);
static void em_act19_005C3C30(EMW *em, EM15W *w);
static void em_act40_005C3DB0(EMW *em, EM15W *w);
static void em_act18_005C3E70(EMW *em, EM15W *w);
static void em_act20_005C3FC0(EMW *em, EM15W *w);
static void em_act21_005C40D0(EMW *em, EM15W *w);
static void em_act22_005C4220(EMW *em, EM15W *w);
static void em_act23_005C42B0(EMW *em, EM15W *w);
static void em_act24_005C4380(EMW *em, EM15W *w);
static void em_act25_005C4450(EMW *em, EM15W *w);
static void em_act26_005C44D0(EMW *em, EM15W *w);
static void em_act27_005C4610(EMW *em, EM15W *w);
static void em_act28_005C4720(EMW *em, EM15W *w);
static void em_act29_005C47F0(EMW *em, EM15W *w);
static void em_act31_005C4930(EMW *em, EM15W *w);
static void em_mv00_005C4A70(EMW *em, EM15W *w);
static void em_mv03_005C4BD0(EMW *em, EM15W *w);
static void em_mv05_005C4ED0(EMW *em, EM15W *w);
static void em_mv06_005C51D0(EMW *em, EM15W *w);
static void em_mv07_005C5330(EMW *em, EM15W *w);
static void em_fly00_005C5470(EMW *em, EM15W *w);
static void em_fly01_005C5560(EMW *em, EM15W *w);
static void em_fly02_005C5790(EMW *em, EM15W *w);
static void em_fly03_005C5890(EMW *em, EM15W *w);
static void em_fly04_005C5B00(EMW *em, EM15W *w);
static void em_fly05_005C5C50(EMW *em, EM15W *w);
static void em_fly06_005C5DA0(EMW *em, EM15W *w);
static void em_fly07_005C5EC0(EMW *em, EM15W *w);
static void em_fly08_005C6000(EMW *em, EM15W *w);
static void em_fly09_005C6290(EMW *em, EM15W *w);
static void em_fly10_005C6430(EMW *em, EM15W *w);
static void em_fly11_005C6630(EMW *em, EM15W *w);
static void em_fly12_005C6860(EMW *em, EM15W *w);
static void em_fly13_005C6970(EMW *em, EM15W *w);
static void em_fly14_005C6C80(EMW *em, EM15W *w);
static void em_fly15_005C6D30(EMW *em, EM15W *w);
static void em_fly16_005C6E50(EMW *em, EM15W *w);
static void em_fly17_005C6F60(EMW *em, EM15W *w);
static void em_fly18_005C6F70(EMW *em, EM15W *w);
static void em_fly19_005C6F80(EMW *em, EM15W *w);
static void em_fly20_005C6F90(EMW *em, EM15W *w);
static void em_fly21_005C70D0(EMW *em, EM15W *w);
static void em_fly22_005C7210(EMW *em, EM15W *w);
static void em_fly23_005C7220(EMW *em, EM15W *w);
static void em_fly24_005C7440(EMW *em, EM15W *w);
static void em_fly25_005C75D0(EMW *em, EM15W *w);
void em_fly26(EMW *em, EM15W *w);
void em_fly27(EMW *em, EM15W *w);
void em_fly28(EMW *em, EM15W *w);
void em_fly29(EMW *em, EM15W *w);
void em_fly30(EMW *em, EM15W *w);
void em_fly31(EMW *em, EM15W *w);
void em_fly32(EMW *em, EM15W *w);
void em_fly33(EMW *em, EM15W *w);
void em_fly34(EMW *em, EM15W *w);
static void em_atk00_005C89A0(EMW *em, EM15W *w);
static void em_atk01_005C8AF0(EMW *em, EM15W *w);
static void em_atk02_005C8BB0(EMW *em, EM15W *w);
static void em_atk03_005C8C30(EMW *em, EM15W *w);
static void em_atk04_005C8CB0(EMW *em, EM15W *w);
static void em_atk05_005C8D30(EMW *em, EM15W *w);
static void em_atk06_005C8E00(EMW *em, EM15W *w);
static void em_atk07_005C8ED0(EMW *em, EM15W *w);
static void em_atk08_005C8F80(EMW *em, EM15W *w);
static void em_dmg00_005C90D0(EMW *em, EM15W *w);
static void em_dmg01_005C9160(EMW *em, EM15W *w);
static void em_dmg02_005C92D0(EMW *em, EM15W *w);
static void em_dmg03_005C9360(EMW *em, EM15W *w);
static void em_dmg04_005C93F0(EMW *em, EM15W *w);
static void em_dmg05_005C9510(EMW *em, EM15W *w);
static void em_dmg08_005C9630(EMW *em, EM15W *w);
static void em_dmg10_005C9770(EMW *em, EM15W *w);
static void em_dmg11_005C98B0(EMW *em, EM15W *w);
static void em_dmg12_005C99A0(EMW *em, EM15W *w);
static void em_dmg13_005C9AD0(EMW *em, EM15W *w);
static void em_dmg14_005C9C00(EMW *em, EM15W *w);
static void em_dmg15_005C9D20(EMW *em, EM15W *w);
static void em_dmg16_005C9E20(EMW *em, EM15W *w);
static void em_demo04_005C9F20(EMW *em, EM15W *w);
static void em_die00_005CA040(EMW *em, EM15W *w);
static void em_die01_005CA1D0(EMW *em, EM15W *w);
static void em_die02_005CA340(EMW *em, EM15W *w);
static void em_move00_005CA5B0(EMW *em, EM15W *w);
static void em_move01_005CA890(EMW *em, EM15W *w);
static void em_move02_005CA930(EMW *em, EM15W *w);
static void em_move03_005CABA0(EMW *em, EM15W *w);
static void em_move04_005CAC70(EMW *em, EM15W *w);
static void em_move05_005CAD90(EMW *em, EM15W *w);
static void em_move06_005CAE00(EMW *em, EM15W *w);
void em15_uvmove(EMW *em);
static void sound_call_sub_005CBA30(EMW *em, int se, int joint);
static void sound_call_005CBAA0(EMW *em, int frame, int se, int joint);
static void sound_call_parts_005CBB00(EMW *em, int frame, int se, int joint, u8 layer);
static void quake_call_005CBBA0(EMW *em, int frame, int arg);
void Em_set_quake_sub(EMW *, int);
static void move_default_005CBBF0(EMW *em);
static void ef_move_sub_005CBC40(EMW *em, EM15W *w);
void em15_effect_move(EMW *em);
static void ground_land_eff_set_005CF030(EMW *em);
void dummy_em_prog_005CF0E0(void);


void em15_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *) em, 0);
}

void em15_init(EMW *em) {
    EM15W *w = (EM15W *)em->ex;
    s16 temp_a3;
    s16 temp_v0;
    u8 temp_a0;
    u32 temp_a1;
    u8 temp_v1;

    em_char_set(em, 1, 0, 0);
    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
        em->x388 = 0;
        em15_act_set(em, 0, 1, 0);
        temp_v1 = game_w.stage;
        switch (temp_v1) {                          /* irregular */
        case 0x46:
            em->pos[0] = 14000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 10500.0f;
            break;
        case 0x49:
            em->pos[0] = 9500.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 1500.0f;
            em->pos[2] = 12000.0f;
            em->x388 = 2;
            em15_act_set(em, 0, 0x19, 0);
            break;
        case 0x4B:
            em->pos[0] = 8500.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 1500.0f;
            em->pos[2] = 10000.0f;
            em->x388 = 2;
            em15_act_set(em, 2, 0x19, 0);
            break;
        default:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f;
            break;
        }
    } else {
        em->x388 = 0;
        em15_act_set(em, 0, 1, 0);
    }
    temp_v0 = em_hp_vital_set2(em, 0x320, 0x4B0);
    em->x302 = temp_v0;
    em->x792 = temp_v0;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em15_stay_timer_tbl[em->stg];
    temp_a3 = em15_runaway_timer_tbl[em->stg];
    em->runaway_tm = temp_a3;
    w->tgt_ang = 0x4000;
    w->x2C = 0x100;
    w->x30 = 0x200;
    w->x34 = 0x100;
    w->x1C = 0;
    w->x44 = 0;
    temp_a0 = em->x948 & 1;
    em->x948 = temp_a0;
    if (temp_a0 == 0) {
        temp_a1 = em->kind;
        switch (temp_a1) {
        case 1:
        case 6:
        case 8:
        case 0xB:
        case 0xF:
        case 0xE:
        case 0x11:
        case 0x15:
        case 0x16:
        case 0x1A:
            em->ex[0xA3] = 0;
            eft09_set(em, temp_a1);
            break;
        case 0x14:
            break;
        }
    }
}

static void act_dist_select_005C2E90(EMW *em) {
    em->x839 = 1;
    if (em->x388 == 0) {
        em15_act_set(em, 0, 1, 0);
        return;
    }
    em15_act_set(em, 2, 2, 0);
}

void em15_to_normal(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em->x3F4 = 0;
    em->x839 = 1;
    if (em->x302 < ((s16)(0.2f * (f32) em->x792))) {
        em15_act_set(em, 0, 1, 0);
        return;
    }
    if (em->x888 == 0) {
        em15_act_set(em, 0, 1, 0);
        return;
    }
    em15_act_set(em, 0, 0x11, 0);
}

void em15_to_tenjo(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 2;
    em->x3F4 = 0;
    em->x839 = 1;
    M2C_FIELD(em, u8 *, 0x9EF) = 5;
    em15_act_set(em, 2, 0x19, 0);
}

void em15_to_fly(EMW *em) {
    em->x839 = 1;
    em->act_spd = 1.0f;
    em_act_set(em, 2, 0xE);
}

void em15_frame_reset(EMW *em, int i) {
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

static void em_act00_005C3090(EMW *em, EM15W *w) {
    s32 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x1E;
        break;
    case 1:
        if ((em->x8C3 == 0) && ((em->x194 == 0) || (temp_v1_2 = em->work08 - 1, em->work08 = temp_v1_2, (temp_v1_2 <= 0)))) {
            em->x05 += 1;
            act_dist_select_005C2E90(em);
        }
        break;
    }
}

static void em_act01_005C3140(EMW *em, EM15W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a2 + 1;
            act_dist_select_005C2E90(em);
        }
        break;
    }
}

static void em_act02_005C31D0(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x65, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_act_set(em, 0, 3, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 3, 4);
        }
        break;
    }
}

static void em_act03_005C3280(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x64, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_act_set(em, 0, 3, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 3, 4);
        }
        break;
    }
}

static void em_act04_005C3330(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x66, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act05_005C33B0(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
    em15_frame_reset(em, 0);
    em15_frame_reset(em, 2);
}

static void em_act06_005C3480(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x69, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act07_005C3500(EMW *em, EM15W *w) {
    s32 temp_v1_2;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 1, 0, 0);
        em->work08 = 0x1E;
        break;
    case 1:
        if (em->x8C3 == 0) {
            temp_v1_2 = em->work08 - 1;
            em->work08 = temp_v1_2;
            if (temp_v1_2 <= 0) {
                em->x05 += 1;
                act_dist_select_005C2E90(em);
            }
        }
        break;
    }
}

static void em_act08_005C35A0(EMW *em, EM15W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x33, 0, 0);
        em->work08 = 0x1E;
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act10_005C3630(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act13_005C3730(EMW *em, EM15W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x14, 0, 0);
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0xA, 0, 0);
        }
        if (em->x2E0 != 0x4B1) {
            em_char_set2(em, 0x579, 0xA, 0, 2);
        }
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em15_to_normal(em);
        }
        break;
    }
    em15_frame_reset(em, 0);
    em15_frame_reset(em, 2);
    sound_call_parts_005CBB00(em, 2, 0x20, 0x23, 1);
    sound_call_parts_005CBB00(em, 0x4A, 0x1F, 0x23, 1);
}

static void em_act14_005C3840(EMW *em, EM15W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if ((em_frame_check(em, 0, 56.0f) != 0) || (em_frame_check(em, 0, 108.0f) != 0) || (em_frame_check(em, 0, 126.0f) != 0) || (em_frame_check(em, 0, 138.0f) != 0) || (em_frame_check(em, 0, 170.0f) != 0)) {
            sound_call_sub_005CBA30(em, 0x26, 0x23);
            Eft20_set(5.0f, em, 7, 0);
        }
        if ((em_frame_check(em, 0, 210.0f) != 0) || (em_frame_check(em, 0, 218.0f) != 0)) {
            Eft20_set(2.5f, em, 7, 0);
        }
        if (em_frame_check(em, 0, 52.0f) != 0) {
            Eft20_set(1.0f, em, 8, 0);
        }
        if ((em_frame_check(em, 0, 106.0f) != 0) || (em_frame_check(em, 0, 130.0f) != 0)) {
            Eft20_set(1.0f, em, 6, 0);
        }
        if (em_frame_check(em, 0, 124.0f) != 0) {
            Eft20_set(1.0f, em, 6, 1);
        }
        if (em_frame_check(em, 0, 170.0f) != 0) {
            Eft20_set(1.0f, em, 0xF, 0);
        }
        if (em_frame_check(em, 0, 192.0f) != 0) {
            Eft20_set(1.0f, em, 0x10, 0);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x3B, 0, 0);
            em_hp_add(em, (s16)(0.05f * (f32) em->x792));
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em_thirst_end(em);
            em15_to_normal(em);
        }
        break;
    }
    em_thirst_add(em, 0xDE);
}

static void em_act17_005C3BB0(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act19_005C3C30(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em_frame_check(em, 0, 200.0f) != 0) {
            em->x05 += 1;
            if (em->x8C3 == 0) {
                em->x827 = 7;
                em->x828 = em->x951;
                em->x829 = em->x952;
                em->_pad9F4[0xC] = 1;
                em_niku_eat_set(em);
                em_hungry_add(em, 0x2710);
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em15_act_set(em, 0, 0x28, 4);
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
            em15_act_set(em, 0, 0x28, 4);
        }
        break;
    }
}

static void em_act40_005C3DB0(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x762 = 0;
        em_search_data_set(em, 0);
        em_range_set(em, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x3B, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act18_005C3E70(EMW *em, EM15W *w) {
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
        em->work08 = 0x2328;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x1F, 0, 0);
        }
        break;
    case 2:
        em_sleep_hp_add(em, 1, (s16)(0.5f * (f32) em->x792), 5);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_hinshi_end(em);
            em15_act_set(em, 0, 0x17, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x17, 4);
        }
        break;
    }
}

static void em_act20_005C3FC0(EMW *em, EM15W *w) {
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
            em15_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

static void em_act21_005C40D0(EMW *em, EM15W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x27, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        em->work08 = 0x2328;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        em_sleep_hp_add(em, 1, (s16)(0.5f * (f32) em->x792), 5);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em15_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

static void em_act22_005C4220(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act23_005C42B0(EMW *em, EM15W *w) {
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
            em15_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act24_005C4380(EMW *em, EM15W *w) {
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
            em15_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act25_005C4450(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act26_005C44D0(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4F, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x3B, 0, 0);
            em_hungry_add(em, em->hungry_max);
            em_hungry_end(em);
            em_hp_add(em, (s16)(0.05f * (f32) em->x792));
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em15_to_normal(em);
        }
        break;
    }
}

static void em_act27_005C4610(EMW *em, EM15W *w) {
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
            em15_act_set(em, 0, 0x1C, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x1C, 4);
        }
        break;
    }
}

static void em_act28_005C4720(EMW *em, EM15W *w) {
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
            em15_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act29_005C47F0(EMW *em, EM15W *w) {
    f32 v[3];
    s32 temp_v0;
    u8 temp_v1;

    em->x9EA = 5;
    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
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
            em->x05 += 1;
            em15_act_set(em, 4, 0xF, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 4, 0xF, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005CBA30(em, 0x57, 0x23);
    }
}

static void em_act31_005C4930(EMW *em, EM15W *w) {
    f32 v[3];
    s32 temp_v0;
    u8 temp_v1;

    em->x9EA = 5;
    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
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
            em->x05 += 1;
            em15_act_set(em, 4, 0x10, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 4, 0x10, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005CBA30(em, 0x57, 0x23);
    }
}

static void em_mv00_005C4A70(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_mv03_005C4BD0(EMW *em, EM15W *w) {
    f32 temp_f1;
    u32 var_s1;
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
            var_s1 = (u32)temp_f1;
            temp_s0 = (w->tgt_ang - (u16) em->ang[1]) & 0xFFFF;
            if ((em->x194 == 0) || (em_frame_check2(em, 0, 74.0f) != 0)) {
                if ((u32) ((temp_s0 + var_s1) & 0xFFFF) < (u32) (var_s1 * 2)) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em15_to_normal(em);
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
            if ((u32) ((temp_s0 + var_s1) & 0xFFFF) < (u32) (var_s1 * 2)) {
                em->ang[1] = w->tgt_ang;
            } else if (temp_s0 < 0x8000) {
                em->ang[1] = (em->ang[1] + var_s1) & 0xFFFF;
            } else {
                em->ang[1] = (em->ang[1] - var_s1) & 0xFFFF;
            }
        }
        break;
    }
}

static void em_mv05_005C4ED0(EMW *em, EM15W *w) {
    f32 temp_f1;
    u32 var_s1;
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
            var_s1 = (u32)temp_f1;
            temp_s0 = (w->tgt_ang - (u16) em->ang[1]) & 0xFFFF;
            if ((em->x194 == 0) || (em_frame_check2(em, 0, 48.0f) != 0)) {
                if ((u32) ((temp_s0 + var_s1) & 0xFFFF) < (u32) (var_s1 * 2)) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em15_to_normal(em);
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
            if ((u32) ((temp_s0 + var_s1) & 0xFFFF) < (u32) (var_s1 * 2)) {
                em->ang[1] = w->tgt_ang;
            } else if (temp_s0 < 0x8000) {
                em->ang[1] = (em->ang[1] + var_s1) & 0xFFFF;
            } else {
                em->ang[1] = (em->ang[1] - var_s1) & 0xFFFF;
            }
        }
        break;
    }
}

static void em_mv06_005C51D0(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_mv07_005C5330(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_fly00_005C5470(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em15_fly_adjy2_init(em, 2);
        break;
    case 1:
        if (em_frame_check(em, 0, 38.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em15_fly_adjy2(em);
        }
        break;
    case 2:
        if (em15_fly_adjy2(em) != 0) {
            em->x05 += 1;
            em15_to_fly(em);
        }
        break;
    }
}

static void em_fly01_005C5560(EMW *em, EM15W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    s32 temp_v1;
    u8 temp_a3;

    temp_a3 = em->x05;
    switch (temp_a3) {                              /* irregular */
    case 0:
        em->x05 = temp_a3 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em->ang[0] = 0;
        em->ang[2] = 0;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_y = -10.0f;
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
            em->x05 = temp_a3 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly02_005C5790(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 temp_v0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        em15_fly_adjy(em, 1);
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em15_act_set(em, 2, 1, 1);
        }
        em15_senkai_target(em);
        if (em->work08 == 0x12C) {
            em15_to_fly(em);
        }
        break;
    case 2:
        em15_fly_adjy(em, 1);
        em15_senkai_target(em);
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly03_005C5890(EMW *em, EM15W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em_rate_clear(em);
        em15_fly_adjy2_init(em, 2);
        break;
    case 1:
        if ((em_frame_check2(em, 0, 50.0f) != 0) && (em_frame_check2(em, 0, 114.0f) == 0)) {
            em15_senkai_target(em);
        }
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            em15_fly_adjy2(em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if ((em15_fly_adjy2(em) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
            em->x05 += 1;
            em_char_set(em, 0xB, 0, 0);
            em_rate_clear(em);
            temp_f1 = (em->x5AC - em->pos[1]) / 30.0f;
            em->adj_y = temp_f1;
            if (!(temp_f1 < 0.0f)) {
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
            em->x05 = temp_a2 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly04_005C5B00(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 var_s1;
    u8 temp_a1;

    var_s1 = 0;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em15_fly_adjy2_init(em, 3);
        em->adj_z = 20.0f;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            var_s1 = em15_fly_adjy2(em) & 0xFF;
        }
        if ((var_s1 != 0) && (em->pos[1] <= em->x5AC)) {
            em_char_set(em, 0x13, 0, 0);
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly05_005C5C50(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 var_s1;
    u8 temp_a1;

    var_s1 = 0;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em15_fly_adjy2_init(em, 3);
        em->adj_z = -20.0f;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            var_s1 = em15_fly_adjy2(em) & 0xFF;
        }
        if ((var_s1 != 0) && (em->pos[1] <= em->x5AC)) {
            em_char_set(em, 0x13, 0, 0);
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly06_005C5DA0(EMW *em, EM15W *w) {
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
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        em15_senkai_target(em);
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        temp_f0 = CalcDistanceXZ(em->pos, em->tgt_pos);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if ((temp_v1 <= 0) || (temp_f0 <= (10.0f * em->adj_z))) {
            em->x05 += 1;
            em_rate_clear(em);
            em15_to_fly(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly07_005C5EC0(EMW *em, EM15W *w) {
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
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        em15_senkai_target(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        em15_fly_adjy(em, 1);
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
            em15_to_fly(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}


static void em_fly08_005C6000(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 temp_a0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_4;
    u8 temp_a2;
    u8 temp_v1_3;
    FLYNEED *temp_a3;

    temp_a2 = em->x05;
    temp_a3 = em_hungry_tbl[em->kind];
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        em_char_set(em, 0xF, 0, 0);
        NextStage_No_Set(em);
        NextStage_Dir_Set(em, em->tgt_pos);
        break;
    case 1:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (AreaFieldInCheck(em->stg, em->pos) == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        em->x05 = temp_a2 + 1;
        temp_v1 = em->thirst;
        if (temp_v1 > 0xBB8) {
            em->thirst = temp_v1 - 0xBB8;
        } else {
            em->thirst = 0;
        }
        temp_a0 = temp_a3->x14;
        temp_v1_2 = em->hungry;
        if (temp_a0 < temp_v1_2) {
            em->hungry = temp_v1_2 - temp_a0;
        } else {
            em->hungry = 0;
        }
        break;
    case 3:
        em->x05 = temp_a2 + 1;
        em->work08 = 0x258;
        Em_Next_Stage_Pos(em);
        temp_v1_3 = em->x92F;
        if ((u16) em->x73A == temp_v1_3 || temp_v1_3 == 0xFF) {
            WyvernAreaMove(em);
            em15_act_set(em, 2, 9, 1);
        } else {
            if (em->x8C3 == 0) {
                em->x73A = (s16) temp_v1_3;
                em->x829 = temp_v1_3 & 0xFFFF;
                em->x827 = 3;
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em15_act_set(em, 2, 0xD, 1);
            WyvernAreaMove(em);
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            temp_v1_4 = em->work08 - 1;
            em->work08 = temp_v1_4;
            if (temp_v1_4 <= 0) {
                em->x05 = 3;
            }
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly09_005C6290(EMW *em, EM15W *w) {
    s32 temp_v1;
    f32 dd;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_z = 40.0f;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[1] = (s32) w->tgt_ang;
        em->x92F = 0xFF;
        em_area_move_init(em);
        break;
    case 1:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        dd = CalcDistanceXZ(em->pos, em->tgt_pos);
        em->work08 -= 1;
        if ((dd <= 500.0f) || (em->work08 < 0)) {
            em->x05 += 1;
            em->work08 = 0x258;
            em15_act_set(em, 2, 1, 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->work08 = 0x258;
                em15_act_set(em, 2, 9, 1);
            }
        }
        break;
    }
}

static void em_fly10_005C6430(EMW *em, EM15W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em->x388 = 2;
        em_rate_clear(em);
        NextStage_No_Set(em);
        NextStage_Dir_Set(em, em->tgt_pos);
        break;
    case 1:
        em15_senkai_target(em, 2);
        em15_fly_adjy(em, 1);
        em->pos[1] += 100.0f;
        temp_f1 = em->pos[1];
        if (!(temp_f1 < em->tgt_pos[1])) {
            em->x05 += 1;
        }
        break;
    case 2:
        em->x05 = temp_a1 + 1;
        em->adj_z = 40.0f;
        em_char_set(em, 0xF, 0, 0);
        /* fallthrough */
    case 3:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (AreaFieldInCheck(em->stg, em->pos) == 0) {
            em->x05 += 1;
        }
        break;
    case 4:
        em->x05 = temp_a1 + 1;
        Em_Next_Stage_Pos(em);
        WyvernAreaMove(em);
        if (em->stg == 0xF) {
            em->stg = 0x13;
        } else {
            em->stg = 0xF;
        }
        em15_to_fly(em);
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly11_005C6630(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0xF, 0, 0);
        if (!(em->adj_z <= 40.0f)) {
            em->adj_z = 40.0f;
        }
        break;
    case 1:
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) {
            if (!(em->adj_z <= 40.0f)) {
                em->adj_z = 40.0f;
            }
            em->x05 += 1;
            em->work08 = 0x12C;
            em15_act_set(em, 2, 1, 1);
        } else {
            w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
            w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
            em15_senkai_sub(em);
            w->spd[0] = (s32) em->ang[0];
            w->spd[1] = (s32) em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 += 1;
                em15_act_set(em, 2, 1, 1);
            }
        }
        break;
    case 2:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em15_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly12_005C6860(EMW *em, EM15W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        em15_fly_adjy(em, 1);
        em->pos[1] += 20.0f;
        temp_f1 = em->pos[1];
        if (!(temp_f1 < 10000.0f)) {
            em->x05 += 1;
            em_rate_clear(em);
            em15_to_fly(em);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly13_005C6970(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 temp_v1_2;
    u8 temp_a2;
    u8 temp_v1;
    STAGE_DATA *temp_v0;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_z = 40.0f;
        temp_v0 = Stage_data_get(em->stg);
        em->tgt_pos[0] = temp_v0->width / 2.0f;
        em->tgt_pos[1] = temp_v0->floor_y;
        em->tgt_pos[2] = temp_v0->depth / 2.0f;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[1] = (s32) w->tgt_ang;
        em_area_move_init(em);
        break;
    case 1:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_f1 = CalcDistanceXZ(em->pos, em->tgt_pos);
        em->work08 -= 1;
        if (temp_f1 <= 500.0f || em->work08 < 0) {
            em->x05 += 1;
            NextStage_No_Set(em);
            NextStage_Dir_Set(em, em->tgt_pos);
        }
        break;
    case 2:
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em15_senkai_sub(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (AreaFieldInCheck(em->stg, em->pos) == 0) {
            em->x05 += 1;
        }
        break;
    case 3:
        em->x05 = temp_a2 + 1;
        em->work08 = 0x258;
        Em_Next_Stage_Pos(em);
        temp_v1 = em->x92F;
        if ((u16) em->x73A == temp_v1 || temp_v1 == 0xFF) {
            WyvernAreaMove(em);
            em15_act_set(em, 2, 9, 1);
        } else {
            if (em->x8C3 == 0) {
                em->x73A = (s16) temp_v1;
                em->x829 = temp_v1 & 0xFFFF;
                em->x827 = 3;
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em15_act_set(em, 2, 0xD, 1);
            WyvernAreaMove(em);
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            temp_v1_2 = em->work08 - 1;
            em->work08 = temp_v1_2;
            if (temp_v1_2 <= 0) {
                em->x05 = 3;
            }
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly14_005C6C80(EMW *em, EM15W *w) {
    f32 temp_f1;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        em15_fly_adjy(em, 1);
        if (em->x194 == 0) {
            em->x05 += 1;
            em15_to_fly(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly15_005C6D30(EMW *em, EM15W *w) {
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
        if (temp_v1 > 0) {
            if (!(em->pos[1] <= (3000.0f + em->tgt_pos[1]))) {
                goto block_8;
            }
        } else {
block_8:
            em->x05 += 1;
            em_rate_clear(em);
            em15_to_fly(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly16_005C6E50(EMW *em, EM15W *w) {
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
        w->x30 = 0x100;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->tgt_ang = (u16) (w->tgt_ang - em->ang[1]);
        em->work08 = 0x12C;
        break;
    case 1:
        em15_fly_adjy(em, 1);
        temp_a0 = em15_senkai_target(em) & 0xFF;
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 > 0) {
            if (temp_a0 != 0) {
                goto block_8;
            }
        } else {
block_8:
            em->x05 += 1;
            em15_to_fly(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly17_005C6F60(EMW *em, EM15W *w) {

}

static void em_fly18_005C6F70(EMW *em, EM15W *w) {

}

static void em_fly19_005C6F80(EMW *em, EM15W *w) {

}

static void em_fly20_005C6F90(EMW *em, EM15W *w) {
    f32 temp_f1;
    s32 var_s1;
    u8 temp_a1;

    var_s1 = 0;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x54, 0, 0);
        em15_fly_adjy2_init(em, 0xC);
        break;
    case 1:
        if (em_frame_check2(em, 0, 44.0f) != 0) {
            em->x388 = 2;
            var_s1 = em15_fly_adjy2(em) & 0xFF;
        }
        if ((var_s1 != 0) && (em->pos[1] <= em->x5AC)) {
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            ground_land_eff_set_005CF030(em);
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly21_005C70D0(EMW *em, EM15W *w) {
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
        w->x30 = 0x100;
        em_char_set(em, 0xF, 0, 0);
        break;
    case 1:
        w->spd[1] = (s32) (Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF);
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        temp_f0 = flvecCalcDistance(em->pos, em->tgt_pos);
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
            em15_to_fly(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly22_005C7210(EMW *em, EM15W *w) {

}

static void em_fly23_005C7220(EMW *em, EM15W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x762 = 0;
        em_char_set(em, 0x12, 0xA, 0x1E);
        em15_fly_adjy2_init(em, 2);
        em->x388 = 2;
        em->x9EA = 0;
        em->x8BD = 1;
        break;
    case 1:
        em15_fly_adjy2(em);
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if ((em15_fly_adjy2(em) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
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
            em->x05 = temp_a2 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1_3 = em->x5AC;
    if (em->pos[1] < temp_f1_3) {
        em->pos[1] = temp_f1_3;
    }
}

static void em_fly24_005C7440(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em_char_set(em, 0x68, 0, 0);
        em->x388 = 0;
        break;
    case 1:
        if (em_frame_check(em, 0, 112.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em_rate_clear(em);
            em->adj_y = 100.0f;
            em->x3C0[1] = -5.0f;
            w->spd[0] = 0;
            w->spd[2] = 0;
        }
        break;
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->adj_y < 50.0f) {
            em->adj_y = 50.0f;
            em->x3C0[1] = 0.0f;
        }
        if (!(em->pos[1] <= (em->x7E4 - 580.0f))) {
            em->x05 += 1;
            em->pos[1] = em->x7E4;
            em_char_set(em, 0x76, 0, 0);
            em->_pad9EF[0] = 5;
            return;
        }
        break;
    case 3:
        em->_pad9EF[0] = 5;
        if (em->x194 == 0) {
            em->x05 += 1;
            em15_to_tenjo(em);
        }
        break;
    }
}

static void em_fly25_005C75D0(EMW *em, EM15W *w) {
    u8 temp_a1;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em_char_set(em, 0x67, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_tenjo(em);
        }
        break;
    }
}

void em_fly26(EMW *em, EM15W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0x6D, 0, 0);
        em_rate_clear(em);
        em->adj_y = -50.0f;
        em->x3C0[1] = -5.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em->_pad9EF[0] = 5;
        break;
    case 1:
        em->_pad9EF[0] = 5;
        if (em_frame_check(em, 0, 12.0f) != 0) {
            em->x05 += 1;
            em->pos[1] -= 580.0f;
            em->_pad9EF[0] = 0;
        }
        break;
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_f1 = em->x5AC;
        if (em->pos[1] <= temp_f1) {
            em->pos[1] = temp_f1;
            em->x05 += 1;
            em->x388 = 0;
            em_char_set(em, 0x75, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

void em_fly27(EMW *em, EM15W *w) {
    s32 temp_a2_2;
    u16 temp_a1;
    u32 temp_s0;
    u32 temp_v1;
    u8 temp_a2;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if (temp_v1 <= 0xE38 || temp_v1 >= 0xF1C8) {
            pl_flag_set((PLW *) em, 0x20000);
            em_char_set(em, 0x77, 0, 0);
        } else if (temp_v1 >= 0x8000) {
            em_char_set(em, 0x79, 0, 0);
        } else {
            em_char_set(em, 0x78, 0, 0);
        }
        break;
    case 1:
        if (em_frame_check2(em, 0, 8.0f) != 0) {
            em->x05 += 1;
        case 2:
            temp_a2_2 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_s0 = (temp_a1 - (temp_a2_2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + 0x2C8) & 0xFFFF) < 0x590) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em15_to_tenjo(em);
                    return;
                }
                if ((temp_s0 <= 0xE38) || (temp_s0 >= 0xF1C8)) {
                    pl_flag_set((PLW *) em, 0x20000);
                    em_char_set(em, 0x77, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *) em, 0x20000);
                if (temp_s0 >= 0x8000) {
                    em_char_set(em, 0x79, 0, 0);
                    return;
                }
                em_char_set(em, 0x78, 0, 0);
                return;
            }
            if ((u32) ((temp_s0 + 0x2C8) & 0xFFFF) < 0x590) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_s0 < 0x8000) {
                em->ang[1] = (temp_a2_2 + 0x2C8) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a2_2 - 0x2C8) & 0xFFFF;
        }
        break;
    }
}

void em_fly28(EMW *em, EM15W *w) {
    int d;
    f32 sp30[3];
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_a1;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x77, 0, 0);
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
        }
        mot_miration_ret(em, sp30);
        temp_f1 = w->dist - sp30[2];
        w->dist = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x05 += 1;
            em15_to_tenjo(em);
        }
        break;
    }
}

void em_fly29(EMW *em, EM15W *w) {
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v1;
    s8 temp_v0_2;
    u8 temp_a1;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x388 = 2;
        em_char_set(em, 0x77, 0, 0);
        em->x05 += 1;
        em->x07 = 0;
        em->x3F4 = 0;
        em->work08 = 0x12C;
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_8;
            }
        } else {
block_8:
            temp_v0_2 = w->x0A - 1;
            w->x0A = temp_v0_2;
            if ((s8)temp_v0_2 <= 0) {
                em->x05 += 1;
                em15_to_tenjo(em);
            } else {
                em->x883 += 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em_act_set(em, 2, 0x1D);
            }
        }
        temp_v1 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - em->ang[1]) & 0xFFFF;
        if (temp_v1 < 0x8001) {
            if (temp_v1 < 0x200) {
                em->ang[1] += temp_v1;
            } else {
                em->ang[1] += 0x200;
            }
        } else if (temp_v1 > 0xFE00) {
            em->ang[1] += temp_v1;
        } else {
            em->ang[1] -= 0x200;
        }
        break;
    }
}

void em_fly30(EMW *em, EM15W *w) {
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v1;
    s8 temp_v0_2;
    u8 temp_a1;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x388 = 2;
        em_char_set(em, 0x77, 0, 0);
        em->x05 += 1;
        em->x07 = 0;
        em->x3F4 = 0;
        em->work08 = 0x12C;
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_8;
            }
        } else {
block_8:
            temp_v0_2 = w->x0A - 1;
            w->x0A = temp_v0_2;
            if ((s8)temp_v0_2 <= 0) {
                em->x05 += 1;
                em15_to_tenjo(em);
            } else {
                em->x883 -= 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em_act_set(em, 2, 0x1E);
            }
        }
        temp_v1 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - em->ang[1]) & 0xFFFF;
        if (temp_v1 < 0x8001) {
            if (temp_v1 < 0x200) {
                em->ang[1] += temp_v1;
            } else {
                em->ang[1] += 0x200;
            }
        } else if (temp_v1 > 0xFE00) {
            em->ang[1] += temp_v1;
        } else {
            em->ang[1] -= 0x200;
        }
        break;
    }
}

void em_fly31(EMW *em, EM15W *w) {
    s32 temp_a2;
    s32 temp_v1_2;
    u16 temp_a1_2;
    u32 temp_a0;
    u32 temp_v1;
    u8 temp_a1;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        pl_flag_set((PLW *) em, 0x20000);
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if (temp_v1 <= 0xE38 || temp_v1 >= 0xF1C8) {
            em_char_set(em, 0x77, 0, 0);
        } else if (temp_v1 >= 0x8000) {
            em_char_set(em, 0x79, 0, 0);
        } else {
            em_char_set(em, 0x78, 0, 0);
        }
        break;
    case 1:
        if (em_frame_check2(em, 0, 8.0f) != 0) {
            em->x05 += 1;
        case 2:
            temp_a2 = em->ang[1];
            temp_a1_2 = w->tgt_ang;
            temp_a0 = (temp_a1_2 - (temp_a2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_a0 + 0x2C8) & 0xFFFF) < 0x590) {
                    em->x05 += 1;
                    em->work08 = 0x12C;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em15_act_set(em, 2, 0x1D, 1);
                    return;
                }
                if ((temp_a0 <= 0xE38) || (temp_a0 >= 0xF1C8)) {
                    em_char_set(em, 0x77, 0, 0);
                    return;
                }
                if (temp_a0 >= 0x8000) {
                    em_char_set(em, 0x79, 0, 0);
                    return;
                }
                em_char_set(em, 0x78, 0, 0);
                return;
            }
            if ((u32) ((temp_a0 + 0x2C8) & 0xFFFF) < 0x590) {
                em->ang[1] = (s32) temp_a1_2;
                return;
            }
            if (temp_a0 < 0x8000) {
                em->ang[1] = (temp_a2 + 0x2C8) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a2 - 0x2C8) & 0xFFFF;
        }
        break;
    case 3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em15_act_set(em, 2, 0x1D, 1);
            }
        }
        break;
    }
}

void em_fly32(EMW *em, EM15W *w) {
    int d;
    f32 sp30[3];
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a2;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x77, 0, 0);
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
        }
        mot_miration_ret(em, sp30);
        temp_f1 = w->dist - sp30[2];
        w->dist = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x05 += 1;
            em->x827 = 1;
            em->x828 = 0;
            em->x829 = (u8) em->x617;
            em->x881 = em->x827;
            em->x882 = em->x828;
            em->x883 = em->x829;
            cmd_target_kind_set(em, em->tgt_pos);
            em->work08 = 0x12C;
            em15_act_set(em, 2, 0x21, 1);
        }
        break;
    case 2:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em15_act_set(em, 2, 0x21, 1);
            }
        }
        break;
    }
}

void em_fly33(EMW *em, EM15W *w) {
    s32 temp_a2_2;
    s32 temp_v1_2;
    u16 temp_a1;
    u32 temp_s0;
    u32 temp_v1;
    u8 temp_a2;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if (temp_v1 <= 0xE38 || temp_v1 >= 0xF1C8) {
            pl_flag_set((PLW *) em, 0x20000);
            em_char_set(em, 0x77, 0, 0);
        } else if (temp_v1 >= 0x8000) {
            em_char_set(em, 0x79, 0, 0);
        } else {
            em_char_set(em, 0x78, 0, 0);
        }
        break;
    case 1:
        if (em_frame_check2(em, 0, 8.0f) != 0) {
            em->x05 += 1;
        case 2:
            temp_a2_2 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_s0 = (temp_a1 - (temp_a2_2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + 0x2C8) & 0xFFFF) < 0x590) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em->work08 = 0x12C;
                    em15_act_set(em, 2, 0x20, 1);
                    return;
                }
                if ((temp_s0 <= 0xE38) || (temp_s0 >= 0xF1C8)) {
                    pl_flag_set((PLW *) em, 0x20000);
                    em_char_set(em, 0x77, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *) em, 0x20000);
                if (temp_s0 >= 0x8000) {
                    em_char_set(em, 0x79, 0, 0);
                    return;
                }
                em_char_set(em, 0x78, 0, 0);
                return;
            }
            if ((u32) ((temp_s0 + 0x2C8) & 0xFFFF) < 0x590) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_s0 < 0x8000) {
                em->ang[1] = (temp_a2_2 + 0x2C8) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a2_2 - 0x2C8) & 0xFFFF;
        }
        break;
    case 3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em15_act_set(em, 2, 0x20, 1);
            }
        }
        break;
    }
}

void em_fly34(EMW *em, EM15W *w) {
    s32 temp_a2_2;
    s32 temp_v1_2;
    u16 temp_a1;
    u32 temp_s0;
    u32 temp_v1;
    u8 temp_a2;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if (temp_v1 <= 0xE38 || temp_v1 >= 0xF1C8) {
            pl_flag_set((PLW *) em, 0x20000);
            em_char_set(em, 0x77, 0, 0);
        } else if (temp_v1 >= 0x8000) {
            em_char_set(em, 0x79, 0, 0);
        } else {
            em_char_set(em, 0x78, 0, 0);
        }
        break;
    case 1:
        if (em_frame_check2(em, 0, 8.0f) != 0) {
            em->x05 += 1;
        case 2:
            temp_a2_2 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_s0 = (temp_a1 - (temp_a2_2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + 0x2C8) & 0xFFFF) < 0x590) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em->work08 = 0x12C;
                    em15_act_set(em, 2, 0x1C, 1);
                    return;
                }
                if ((temp_s0 <= 0xE38) || (temp_s0 >= 0xF1C8)) {
                    pl_flag_set((PLW *) em, 0x20000);
                    em_char_set(em, 0x77, 0, 0);
                    return;
                }
                pl_flag_clr((PLW *) em, 0x20000);
                if (temp_s0 >= 0x8000) {
                    em_char_set(em, 0x79, 0, 0);
                    return;
                }
                em_char_set(em, 0x78, 0, 0);
                return;
            }
            if ((u32) ((temp_s0 + 0x2C8) & 0xFFFF) < 0x590) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_s0 < 0x8000) {
                em->ang[1] = (temp_a2_2 + 0x2C8) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a2_2 - 0x2C8) & 0xFFFF;
        }
        break;
    case 3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em15_act_set(em, 2, 0x1C, 1);
            }
        }
        break;
    }
}

static void em_atk00_005C89A0(EMW *em, EM15W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6A, 0, 0);
        em15_fly_adjy2_init(em, 0xE);
        break;
    case 1:
        if (em_frame_check(em, 0, 78.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em15_fly_adjy2(em);
        }
        break;
    case 2:
        if ((em15_fly_adjy2(em) & 0xFF) && (em->pos[1] <= em->x5AC)) {
            em->x05 += 1;
            em->x388 = 0;
            em_char_set(em, 0x6B, 0, 0);
        }
        break;
    case 3:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_atk01_005C8AF0(EMW *em, EM15W *w) {
    u8 temp_a1;

    em->_pad9EF[0] = 5;
    em->pos[1] = em->x7E4;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x6E, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x6F, 0, 0);
        }
        break;
    case 2:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_tenjo(em);
        }
        break;
    }
}

static void em_atk02_005C8BB0(EMW *em, EM15W *w) {
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
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_atk03_005C8C30(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x23, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_atk04_005C8CB0(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x71, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_atk05_005C8D30(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2F, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 0, 254.0f) != 0) {
            em->x05 += 1;
            em_char_set(em, 0x73, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_atk06_005C8E00(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2B, 0, 0);
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_atk07_005C8ED0(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x72, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x73, 0, 0);
        }
        break;
    case 2:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_atk08_005C8F80(EMW *em, EM15W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6C, 0, 0);
        em15_fly_adjy2_init(em, 0xF);
        break;
    case 1:
        if (em_frame_check(em, 0, 30.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em15_fly_adjy2(em);
        }
        break;
    case 2:
        if ((em15_fly_adjy2(em) & 0xFF) && (em->pos[1] <= em->x5AC)) {
            em->x05 += 1;
            em->x388 = 0;
            em_char_set(em, 0x6B, 0, 0);
        }
        break;
    case 3:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_dmg00_005C90D0(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_dmg01_005C9160(EMW *em, EM15W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0x6D, 0, 0);
        em_rate_clear(em);
        em->adj_y = -50.0f;
        em->x3C0[1] = -5.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em->_pad9EF[0] = 5;
        em_cmd_reset(em);
        break;
    case 1:
        em->_pad9EF[0] = 5;
        if (em_frame_check(em, 0, 12.0f) != 0) {
            em->x05 += 1;
            em->pos[1] -= 580.0f;
            em->_pad9EF[0] = 0;
        }
        break;
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_f1 = em->x5AC;
        if (em->pos[1] <= temp_f1) {
            em->pos[1] = temp_f1;
            em->x05 += 1;
            em->x388 = 0;
            em_char_set(em, 0x75, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em15_to_normal(em);
        }
        break;
    }
}

static void em_dmg02_005C92D0(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_dmg03_005C9360(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
        }
        break;
    }
}

static void em_dmg04_005C93F0(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg05_005C9510(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg08_005C9630(EMW *em, EM15W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = -10.0f;
        em->x3C0[2] = 0.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
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
            em->x05 = temp_a2 + 1;
            em15_to_normal(em);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg10_005C9770(EMW *em, EM15W *w) {
    s8 temp_v0;
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
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
                em->x959 = 0;
                em_char_set(em, 0x60, 0, 0);
                em15_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em15_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg11_005C98B0(EMW *em, EM15W *w) {
    s32 temp_v0;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4C, 0, 0);
        Em_Mahi_Start(em);
        em->x8BD = 1;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em15_act_set(em, 0, 0x16, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 0, 0x16, 4);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg12_005C99A0(EMW *em, EM15W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x5F, 0, 0);
        em->x762 = 3;
        em->x8BD = 1;
        em->x9EA = 0x32;
        em->x88B = 1;
        Em_Sleep_Flag_Ck(em);
        break;
    case 1:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em15_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x8C3 == 0) {
            em15_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg13_005C9AD0(EMW *em, EM15W *w) {
    s8 temp_v0;
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x60, 0, 0);
        em->x8BD = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            temp_v0 = em->x95A - 1;
            em->x95A = temp_v0;
            if ((s8)temp_v0 <= 0) {
                em->x05 += 1;
                em->x959 = 0;
                em_char_set(em, 0x60, 0, 0);
                em15_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em15_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg14_005C9C00(EMW *em, EM15W *w) {
    s32 temp_v0;
    u8 temp_v1;

    em->x9EA = 5;
    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
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
            em->x05 += 1;
            if (em->x8B6 != 0) {
                em->x95A = 4;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em15_act_set(em, 4, 0xA, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 4, 0xA, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg15_005C9D20(EMW *em, EM15W *w) {
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        Em_Sleep_End(em);
        em->x88B = 1;
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
            em15_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg16_005C9E20(EMW *em, EM15W *w) {
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x62, 0, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
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
            em15_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em15_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_demo04_005C9F20(EMW *em, EM15W *w) {
    f32 v[3];
    s32 temp_a0;
    u8 temp_v1;

    em->x40C = 5;
    em->x9EA = 5;
    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            Quest_enemy_capture();
        }
        break;
    case 2:
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    temp_a0 = em->work08 + 1;
    em->work08 = temp_a0;
    if ((temp_a0 % 135) == 0) {
        sound_call_sub_005CBA30(em, 0x57, 0x23);
    }
}

static void em_die00_005CA040(EMW *em, EM15W *w) {
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
            em15_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die01_005CA1D0(EMW *em, EM15W *w) {
    s32 temp_v1;
    u8 temp_a0;

    em->x9EA = 5;
    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a0 + 1;
        em_char_set(em, 0x61, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
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
            em15_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die02_005CA340(EMW *em, EM15W *w) {
    s32 temp_v1;
    u8 temp_a0;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a0 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em->x3C0[1] = -10.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 0x1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x4E, 0, 0);
            em_rate_clear(em);
        }
        break;
    case 0x2:
        if (em_frame_check(em, 0, 60.0f) != 0) {
            em->x05 += 1;
            em->work08 = 0;
            em_char_set(em, 0x52, 0, 0);
            em->x388 = 3;
            Quest_enemy_die(em);
            return;
        }
        break;
    case 0x3:
        if (em->x194 == 0) {
            em->work08 = 0x1C2;
            em->x05 += 1;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            return;
        }
        break;
    case 0x4:
        Em_hagi_point_cnt_ck(em);
        break;
    case 0x5:
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
            em15_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_move00_005CA5B0(EMW *em, EM15W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em_act00_005C3090(em, w);
        break;
    case 1:
        em_act01_005C3140(em, w);
        break;
    case 2:
        em_act02_005C31D0(em, w);
        break;
    case 3:
        em_act03_005C3280(em, w);
        break;
    case 4:
        em_act04_005C3330(em, w);
        break;
    case 5:
        em_act05_005C33B0(em, w);
        break;
    case 6:
        em_act06_005C3480(em, w);
        break;
    case 7:
        em_act07_005C3500(em, w);
        break;
    case 8:
        em_act08_005C35A0(em, w);
        break;
    case 10:
        em_act10_005C3630(em, w);
        break;
    case 13:
        em_act13_005C3730(em, w);
        break;
    case 14:
        em_act14_005C3840(em, w);
        break;
    case 17:
        em_act17_005C3BB0(em, w);
        break;
    case 18:
        em_act18_005C3E70(em, w);
        break;
    case 19:
        em_act19_005C3C30(em, w);
        break;
    case 20:
        em_act20_005C3FC0(em, w);
        break;
    case 21:
        em_act21_005C40D0(em, w);
        break;
    case 22:
        em_act22_005C4220(em, w);
        break;
    case 23:
        em_act23_005C42B0(em, w);
        break;
    case 24:
        em_act24_005C4380(em, w);
        break;
    case 25:
        em_act25_005C4450(em, w);
        break;
    case 26:
        em_act26_005C44D0(em, w);
        break;
    case 27:
        em_act27_005C4610(em, w);
        break;
    case 28:
        em_act28_005C4720(em, w);
        break;
    case 29:
        em_act29_005C47F0(em, w);
        break;
    case 31:
        em_act31_005C4930(em, w);
        break;
    case 40:
        em_act40_005C3DB0(em, w);
        break;
    }
}

static void em_move01_005CA890(EMW *em, EM15W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_mv00_005C4A70(em, w);
        break;
    case 3:
        em_mv03_005C4BD0(em, w);
        break;
    case 5:
        em_mv05_005C4ED0(em, w);
        break;
    case 6:
        em_mv06_005C51D0(em, w);
        break;
    case 7:
        em_mv07_005C5330(em, w);
        break;
    }
}

static void em_move02_005CA930(EMW *em, EM15W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_fly00_005C5470(em, w);
        break;
    case 1:
        em_fly01_005C5560(em, w);
        break;
    case 2:
        em_fly02_005C5790(em, w);
        break;
    case 3:
        em_fly03_005C5890(em, w);
        break;
    case 4:
        em_fly04_005C5B00(em, w);
        break;
    case 5:
        em_fly05_005C5C50(em, w);
        break;
    case 6:
        em_fly06_005C5DA0(em, w);
        break;
    case 7:
        em_fly07_005C5EC0(em, w);
        break;
    case 8:
        em_fly08_005C6000(em, w);
        break;
    case 9:
        em_fly09_005C6290(em, w);
        break;
    case 10:
        em_fly10_005C6430(em, w);
        break;
    case 11:
        em_fly11_005C6630(em, w);
        break;
    case 12:
        em_fly12_005C6860(em, w);
        break;
    case 13:
        em_fly13_005C6970(em, w);
        break;
    case 14:
        em_fly14_005C6C80(em, w);
        break;
    case 15:
        em_fly15_005C6D30(em, w);
        break;
    case 16:
        em_fly16_005C6E50(em, w);
        break;
    case 17:
        em_fly17_005C6F60(em, w);
        break;
    case 18:
        em_fly18_005C6F70(em, w);
        break;
    case 19:
        em_fly19_005C6F80(em, w);
        break;
    case 20:
        em_fly20_005C6F90(em, w);
        break;
    case 21:
        em_fly21_005C70D0(em, w);
        break;
    case 22:
        em_fly22_005C7210(em, w);
        break;
    case 23:
        em_fly23_005C7220(em, w);
        break;
    case 24:
        em_fly24_005C7440(em, w);
        break;
    case 25:
        em_fly25_005C75D0(em, w);
        break;
    case 26:
        em_fly26(em, w);
        break;
    case 27:
        em_fly27(em, w);
        break;
    case 28:
        em_fly28(em, w);
        break;
    case 29:
        em_fly29(em, w);
        break;
    case 30:
        em_fly30(em, w);
        break;
    case 31:
        em_fly31(em, w);
        break;
    case 32:
        em_fly32(em, w);
        break;
    case 33:
        em_fly33(em, w);
        break;
    case 34:
        em_fly34(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move03_005CABA0(EMW *em, EM15W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_atk00_005C89A0(em, w);
        break;
    case 1:
        em_atk01_005C8AF0(em, w);
        break;
    case 2:
        em_atk02_005C8BB0(em, w);
        break;
    case 3:
        em_atk03_005C8C30(em, w);
        break;
    case 4:
        em_atk04_005C8CB0(em, w);
        break;
    case 5:
        em_atk05_005C8D30(em, w);
        break;
    case 6:
        em_atk06_005C8E00(em, w);
        break;
    case 7:
        em_atk07_005C8ED0(em, w);
        break;
    case 8:
        em_atk08_005C8F80(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move04_005CAC70(EMW *em, EM15W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_dmg00_005C90D0(em, w);
        break;
    case 1:
        em_dmg01_005C9160(em, w);
        break;
    case 2:
        em_dmg02_005C92D0(em, w);
        break;
    case 3:
        em_dmg03_005C9360(em, w);
        break;
    case 4:
        em_dmg04_005C93F0(em, w);
        break;
    case 5:
        em_dmg05_005C9510(em, w);
        break;
    case 8:
        em_dmg08_005C9630(em, w);
        break;
    case 10:
        em_dmg10_005C9770(em, w);
        break;
    case 11:
        em_dmg11_005C98B0(em, w);
        break;
    case 12:
        em_dmg12_005C99A0(em, w);
        break;
    case 13:
        em_dmg13_005C9AD0(em, w);
        break;
    case 14:
        em_dmg14_005C9C00(em, w);
        break;
    case 15:
        em_dmg15_005C9D20(em, w);
        break;
    case 16:
        em_dmg16_005C9E20(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move05_005CAD90(EMW *em, EM15W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_die00_005CA040(em, w);
        break;
    case 1:
        em_die01_005CA1D0(em, w);
        break;
    case 2:
        em_die02_005CA340(em, w);
        break;
    }
}

static void em_move06_005CAE00(EMW *em, EM15W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_demo04_005C9F20(em, w);
        break;
    case 1:
        em_demo04_005C9F20(em, w);
        break;
    case 2:
        em_demo04_005C9F20(em, w);
        break;
    case 3:
        em_demo04_005C9F20(em, w);
        break;
    case 4:
        em_demo04_005C9F20(em, w);
        break;
    }
}

#define M4(n) (em->mode == 4 && em->x15 == (n))
#define M0(n) (em->mode == 0 && em->x15 == (n))

void em15_main_sub(EMW *em, EM15W *w);
void em_hinshi_ck(EMW *em, f32 rate);
void em_hungry_ck(EMW *em);
void em_thirst_ck(EMW *em);
void em_sleep_ck(EMW *em);
u8 GetTenjoHit(f32 *, f32 *, u16 *);
void em_no_floor_ck(EMW *em);
void em_no_battle_area_ck(EMW *, int, int);
void em_sleep2_dmg_timer_set(EMW *em);
void em_dur_set(EMW *, int);
int em_hokaku_ck(EMW *em, f32 rate);
void em_cmd_ck(EMW *);

void em15_main(EMW *em) {
    EM15W *w = (EM15W *)em->ex;
    u8 dmg[4];
    u8 r;
    s16 q;

    em->x9F1 = 0;
    if (game_w.stage == em->stg) {
        em->x9F3 = GetTenjoHit(em->pos, &em->x7E4, &w->x40);
        switch (em->stg) {
        case 0x12:
        case 0x16:
        case 0x17:
        case 0x46:
        case 0x49:
        case 0x4B:
            em_no_floor_ck(em);
            break;
        default:
            em_no_floor_ck(em);
            break;
        }
    } else {
        switch (em->stg) {
        case 0x12:
        case 0x16:
        case 0x17:
        case 0x46:
        case 0x49:
        case 0x4B:
            em->x9F3 = 1;
            em->x7E4 = 1500.0f;
            if (em->mode == 0 || em->mode == 3) {
                em_no_battle_area_ck(em, 0, 1);
            }
            break;
        default:
            em->x9F3 = 0;
            em_no_floor_ck(em);
            break;
        }
    }
    em_mode_timer_sub(em);
    if (em->x8C2 != 1) {
        q = quest_w.no;
        switch (q) {
        case 0x28:
        case 0x29:
            em_hinshi_ck(em, 0.2f);
            em_thirst_ck(em);
            em_hungry_ck(em);
            em_sleep_ck(em);
            break;
        default:
            em_hinshi_ck(em, 0.2f);
            em_hungry_ck(em);
            em_sleep_ck(em);
            break;
        }
    }
    r = Em_Dmg_Sys(em, dmg);
    if (r != 0 && r != 0xE) {
        w->x44 = 0;
        w->x45 = 0;
    }
    switch (r) {
    case 0:
    case 9:
    case 11:
    case 14:
        break;
    case 1:
    case 2:
        if (em->x388 == 2) {
            em15_act_set(em, 5, 2, 2);
        } else if (em->x9EA != 0) {
            em15_act_set(em, 5, 1, 2);
        } else {
            em15_act_set(em, 5, 0, 2);
        }
        break;
    case 3:
    case 4:
        if (em->x9EA == 0) {
            pl_flag_clr((PLW *)em, 0x20000);
            if (dmg[0] == 0) {
                em->x95A = 0x10;
            } else if (em->x8B6 == 0) {
                em->x95A = 0xA;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em15_act_set(em, 4, 0xC, 2);
        }
        break;
    case 5:
        if (!M4(8) && !M4(1)) {
            if (M2C_FIELD(em, s8 *, 0x9EF) != 0) {
                em15_act_set(em, 4, 1, 2);
            } else {
                em15_act_set(em, 4, 8, 2);
            }
        }
        break;
    case 6:
        if (em->x9EA != 0) {
            em_mahi_dmg_timer_set(em);
            em15_act_set(em, 4, 0xE, 2);
        } else if (!M4(1) && !M4(0xB) && !M4(8)) {
            em_mahi_dmg_timer_set(em);
            em15_act_set(em, 4, 0xB, 2);
        }
        break;
    case 7:
        if (em->x9EA != 0) {
            em_sleep2_dmg_timer_set(em);
            if ((u8)em_hokaku_ck(em, 0.3f) == 1) {
                em15_act_set(em, 6, 4, 4);
            } else {
                em15_act_set(em, 0, 0x1F, 2);
            }
        } else if (!M0(0x1B) && !M4(1) && !M4(8)) {
            em_sleep2_dmg_timer_set(em);
            em15_act_set(em, 0, 0x1B, 2);
        }
        break;
    case 8:
        if (em->x9EA != 0) {
            em_sleep_dmg_timer_set(em);
            em15_act_set(em, 0, 0x1D, 2);
        } else if (!M0(0x14) && !M4(1) && !M4(8)) {
            em_sleep_dmg_timer_set(em);
            em15_act_set(em, 0, 0x14, 2);
        }
        break;
    case 10:
        switch (em->x15) {
        case 18:
            em15_act_set(em, 0, 0x17, 2);
            em->x839 = 0;
            break;
        case 20:
            em15_act_set(em, 0, 0x18, 2);
            em->x839 = 0;
            break;
        case 21:
            em15_act_set(em, 0, 0x18, 2);
            em->x839 = 0;
            break;
        case 27:
            em15_act_set(em, 0, 0x1C, 2);
            em->x839 = 0;
            break;
        case 29:
            em15_act_set(em, 4, 0xF, 2);
            em->x839 = 0;
            break;
        case 31:
            em15_act_set(em, 4, 0x10, 2);
            em->x839 = 0;
            break;
        }
        break;
    case 12:
        pl_flag_clr((PLW *)em, 0x20000);
        if (em->x388 == 2) {
            if (M2C_FIELD(em, s8 *, 0x9EF) != 0) {
                em15_act_set(em, 4, 1, 2);
            } else {
                em15_act_set(em, 4, 8, 2);
            }
        } else {
            switch ((u8)em->x38E) {
            case 0:
            case 7:
                em15_act_set(em, 4, 0, 2);
                break;
            case 5:
            case 6:
                em15_act_set(em, 4, 2, 2);
                break;
            case 1:
            case 2:
                em15_act_set(em, 4, 3, 2);
                break;
            default:
                if ((u8)em->x38E != 3) {
                    em15_act_set(em, 4, 5, 2);
                } else {
                    em15_act_set(em, 4, 4, 2);
                }
                break;
            }
        }
        break;
    case 13:
        if (em->x388 != 2) {
            em15_act_set(em, 4, 0, 2);
            em->x839 = 0;
        }
        break;
    }
    if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em15_main_sub(em, w);
    if (em->x6FF != 0) {
        em15_main_sub(em, w);
        em->x6FF = 0;
    }
}

void em15_main_sub(EMW *em, EM15W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0:
        em_move00_005CA5B0(em, w);
        break;
    case 1:
        em_move01_005CA890(em, w);
        break;
    case 2:
        em_move02_005CA930(em, w);
        break;
    case 3:
        em_move03_005CABA0(em, w);
        break;
    case 4:
        em_move04_005CAC70(em, w);
        break;
    case 5:
        em_move05_005CAD90(em, w);
        break;
    case 6:
        em_move06_005CAE00(em, w);
        break;
    case 7:
        em_move06_005CAE00(em, w);
        break;
    }
    if (em->pos[0] <= 0.0f || em->pos[2] <= 0.0f) {
        em_dur_set(em, 0);
    }
}

void em15_uvmove(EMW *em) {
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

static void sound_call_sub_005CBA30(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_005CBAA0(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_005CBA30(em, se, joint);
    }
}

static void sound_call_parts_005CBB00(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

static void quake_call_005CBBA0(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

static void move_default_005CBBF0(EMW *em) {
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

static void ef_move_sub_005CBC40(EMW *em, EM15W *w) {
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
        sound_call_005CBAA0(em, 0xFA, 0x34, 0x23);
        sound_call_005CBAA0(em, 0x172, 0x57, 0x23);
        break;
    case 0x3EB:
        sound_call_005CBAA0(em, 0x22, 0x35, 0x23);
        sound_call_005CBAA0(em, 0x16, 1, 0x14);
        sound_call_005CBAA0(em, 0x4C, 1, 0x1A);
        sound_call_005CBAA0(em, 0x8A, 1, 0x14);
        sound_call_005CBAA0(em, 0xBC, 1, 0x1A);
        quake_call_005CBBA0(em, 0x34, 1);
        quake_call_005CBBA0(em, 0x74, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0x14);
        }
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell16_set(em, 0x15);
        }
        if (em_frame_check(em, 0, 104.0f) != 0) {
            shell16_set(em, 0x16);
        }
        if (em_frame_check(em, 0, 162.0f) != 0) {
            shell16_set(em, 0x15);
        }
        if (em_frame_check(em, 0, 218.0f) != 0) {
            shell16_set(em, 0x17);
        }
        break;
    case 0x3ED:
        sound_call_005CBAA0(em, 0x18, 1, 0x14);
        sound_call_005CBAA0(em, 0x38, 1, 0x1A);
        quake_call_005CBBA0(em, 0x3C, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 5);
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell16_set(em, 6);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(*(u16 *)0x3F340E & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x3EE:
        sound_call_005CBAA0(em, 0x18, 1, 0x1A);
        sound_call_005CBAA0(em, 0x38, 1, 0x14);
        quake_call_005CBBA0(em, 0x3C, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell16_set(em, 7);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 8);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 0);
            return;
        }
        break;
    case 0x3EF:
        sound_call_005CBAA0(em, 6, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xE, 1, 0x1A);
        sound_call_005CBAA0(em, 0x26, 1, 0x14);
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell16_set(em, 9);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0xA);
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
        sound_call_005CBAA0(em, 6, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xE, 1, 0x14);
        sound_call_005CBAA0(em, 0x26, 1, 0x1A);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0xB);
        }
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell16_set(em, 0xC);
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
        sound_call_005CBAA0(em, 6, 0x2A, 0x23);
        sound_call_005CBAA0(em, 6, 4, 0x1A);
        sound_call_005CBAA0(em, 0x4C, 0, 0x14);
        sound_call_005CBAA0(em, 0x1C, 0x12, 0x14);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            shell16_set(em, 0x19);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0x1A);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            shell16_set(em, 0x1A);
            return;
        }
        break;
    case 0x3F3:
        sound_call_005CBAA0(em, 0xE, 0xB, 6);
        sound_call_005CBAA0(em, 0x12, 0xB, 0xC);
        sound_call_005CBAA0(em, 0x46, 0xB, 6);
        sound_call_005CBAA0(em, 0x42, 0xB, 0xC);
        sound_call_005CBAA0(em, 0x7A, 0xB, 6);
        sound_call_005CBAA0(em, 0x7E, 0xB, 0xC);
        if ((em_frame_check(em, 0, 52.0f) == 0) && (em_frame_check(em, 0, 104.0f) == 0)) {
            if (em_frame_check(em, 0, 162.0f) != 0) {
                goto block_137;
            }
        } else {
block_137:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft13_set_em_scl(em, 2, 5.0f, 7);
                return;
            }
        }
        break;
    case 0x3F7:
        sound_call_005CBAA0(em, 4, 0xC, 6);
        sound_call_005CBAA0(em, 8, 0xC, 0xC);
        sound_call_005CBAA0(em, 0x3A, 0xB, 6);
        sound_call_005CBAA0(em, 0x3E, 0xB, 0xC);
        sound_call_005CBAA0(em, 0x8E, 0xB, 6);
        sound_call_005CBAA0(em, 0x8A, 0xB, 0xC);
        sound_call_005CBAA0(em, 0xD4, 0xB, 6);
        sound_call_005CBAA0(em, 0xD8, 0xB, 0xC);
        if ((em_frame_check(em, 0, 8.0f) == 0) && (em_frame_check(em, 0, 86.0f) == 0)) {
            if (em_frame_check(em, 0, 160.0f) != 0) {
                goto block_143;
            }
        } else {
block_143:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 70.0f, 84.0f) == 0) && (em_frame_check3(em, 0, 146.0f, 156.0f) == 0)) {
                if (em_frame_check3(em, 0, 220.0f, 234.0f) != 0) {
                    goto block_150;
                }
            } else {
block_150:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3FA:
        sound_call_005CBAA0(em, 0x28, 7, 0x1A);
        sound_call_005CBAA0(em, 0x1A, 0xD, 6);
        sound_call_005CBAA0(em, 0x1E, 0xD, 0xC);
        sound_call_005CBAA0(em, 0x52, 0xB, 6);
        sound_call_005CBAA0(em, 0x56, 0xB, 0xC);
        sound_call_005CBAA0(em, 0x88, 0xB, 6);
        sound_call_005CBAA0(em, 0x8C, 0xB, 0xC);
        sound_call_005CBAA0(em, 0xB6, 0xB, 6);
        sound_call_005CBAA0(em, 0xBA, 0xB, 0xC);
        if (((em_frame_check(em, 0, 42.0f) != 0) || (em_frame_check(em, 0, 102.0f) != 0) || (em_frame_check(em, 0, 156.0f) != 0)) && (em->pos[1] <= (1000.0f + em->x5AC))) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if ((em->x8B6 != 0) && (em_frame_check(em, 0, 38.0f) != 0)) {
            Eft20_set(1.0f, em, 0x19, 5);
            Eft20_set(1.0f, em, 0x19, 4);
            return;
        }
        break;
    case 0x3FB:
        sound_call_005CBAA0(em, 2, 0xB, 6);
        sound_call_005CBAA0(em, 6, 0xB, 0xC);
        sound_call_005CBAA0(em, 0xC, 4, 0x1A);
        sound_call_005CBAA0(em, 4, 1, 0x14);
        sound_call_005CBAA0(em, 0xE, 9, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0xD);
        }
        if (em_frame_check(em, 0, 16.0f) != 0) {
            ground_land_eff_set_005CF030(em);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 18.0f, 44.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x407:
        sound_call_005CBAA0(em, 2, 0x2D, 0x23);
        sound_call_005CBAA0(em, 0xB6, 0x2D, 0x23);
        sound_call_005CBAA0(em, 0x16C, 0x2D, 0x23);
        v[1] = 10.0f;
        v[2] = 140.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v, 1.60000002f);
        break;
    case 0x408:
        sound_call_005CBAA0(em, 4, 0x57, 0x23);
        sound_call_005CBAA0(em, 0x46, 0, 0x1A);
        sound_call_005CBAA0(em, 0x8E, 3, 0x14);
        sound_call_005CBAA0(em, 4, 0x17, 0);
        sound_call_005CBAA0(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005CBAA0(em, 4, 0x57, 0x23);
        sound_call_005CBAA0(em, 0x46, 0x2D, 0x23);
        sound_call_005CBAA0(em, 0x7E, 3, 0x1A);
        sound_call_005CBAA0(em, 0xB6, 3, 0x14);
        sound_call_005CBAA0(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_005CBAA0(em, 0x24, 0x57, 0x23);
        v2[1] = 10.0f;
        v2[2] = 140.0f;
        v2[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.60000002f);
        break;
    case 0x40B:
        sound_call_005CBAA0(em, 0x14, 0x24, 0x23);
        sound_call_005CBAA0(em, 0x34, 0x22, 0x23);
        sound_call_005CBAA0(em, 0x32, 0x15, 0x22);
        sound_call_005CBAA0(em, 0x2C, 0, 0x14);
        sound_call_005CBAA0(em, 0xA2, 0, 0x1A);
        if (em_frame_check(em, 0, 40.0f) != 0) {
            shell16_set(em, 0xE);
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 28.0f, 64.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
            if (em_frame_check3(em, 0, 98.0f, 102.0f) != 0) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x40D:
        sound_call_005CBAA0(em, 0x1E, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xC, 0xC, 0x1A);
        sound_call_005CBAA0(em, 0x1C, 2, 0x1A);
        sound_call_005CBAA0(em, 0x2E, 3, 0x14);
        sound_call_005CBAA0(em, 0x48, 1, 0x1A);
        break;
    case 0x40E:
        sound_call_005CBAA0(em, 4, 0x57, 0x23);
        sound_call_005CBAA0(em, 0x48, 0x16, 0);
        sound_call_005CBAA0(em, 0xA4, 9, 0);
        sound_call_005CBAA0(em, 0x90, 3, 0xC);
        sound_call_005CBAA0(em, 0x88, 0xE, 6);
        sound_call_005CBAA0(em, 0xB2, 4, 0x22);
        if (em_frame_check(em, 0, 148.0f) != 0) {
            shell16_set(em, 0xF);
            return;
        }
        break;
    case 0x40F:
        sound_call_005CBAA0(em, 2, 0x57, 0x23);
        sound_call_005CBAA0(em, 2, 0x17, 0);
        sound_call_005CBAA0(em, 0x40, 3, 0x1A);
        sound_call_005CBAA0(em, 0x66, 0x10, 0x1A);
        break;
    case 0x413:
        sound_call_005CBAA0(em, 8, 0x23, 0x23);
        sound_call_005CBAA0(em, 0xE, 0xF, 6);
        sound_call_005CBAA0(em, 0x1E, 0x14, 0x2A);
        sound_call_005CBAA0(em, 0x2A, 0, 0x1A);
        sound_call_005CBAA0(em, 0x48, 1, 0x14);
        quake_call_005CBBA0(em, 0x2C, 1);
        quake_call_005CBBA0(em, 0x49, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell16_set(em, 0x11);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x417:
        sound_call_005CBAA0(em, 0x32, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x74, 0x28, 0x23);
        sound_call_005CBAA0(em, 0xA8, 0x29, 0x23);
        sound_call_005CBAA0(em, 0xAC, 0x18, 0x23);
        sound_call_005CBAA0(em, 0xAC, 0x37, 0x23);
        sound_call_005CBAA0(em, 0x20, 1, 0x1A);
        sound_call_005CBAA0(em, 0x32, 0x16, 0x14);
        sound_call_005CBAA0(em, 0x38, 0x36, 0x23);
        sound_call_005CBAA0(em, 0x38, 0x38, 0x23);
        if (em_frame_check(em, 0, 160.0f) != 0) {
            shell16_set(em, 0x25);
        }
        if (em_frame_check(em, 0, 58.0f) != 0) {
            Eft04_set_time(em, 3, (s32)((114.0f / (2.0f * em->act_spd))), 1.0f);
        }
        if (em_frame_check(em, 0, 172.0f) != 0) {
            Eft04_set(em, 4);
        }
        if (em_frame_check(em, 0, 176.0f) != 0) {
            Shell08_set_ang(em, 0x22, 6, 0, 0, 0);
            Shell08_set_ang(em, 0x22, 6, 0, 0, 0xE39);
            Shell08_set_ang(em, 0x22, 6, 0, 0, 0xF1C8);
            return;
        }
        break;
    case 0x41B:
        sound_call_005CBAA0(em, 4, 0x23, 0x23);
        sound_call_005CBAA0(em, 0x72, 0x34, 0x23);
        sound_call_005CBAA0(em, 0xBA, 0x34, 0x23);
        sound_call_005CBAA0(em, 4, 0x17, 0);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 10.0f, 90.0f) != 0) || (em_frame_check3(em, 0, 122.0f, 260.0f) != 0) || (em_frame_check2(em, 0, 296.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x41E:
        sound_call_005CBAA0(em, 0x4E, 0x5B, 0x23);
        sound_call_005CBAA0(em, 8, 0, 0x14);
        sound_call_005CBAA0(em, 0x28, 1, 0x1A);
        sound_call_005CBAA0(em, 0x88, 0, 0x14);
        sound_call_005CBAA0(em, 0xB6, 1, 0x1A);
        sound_call_005CBAA0(em, 0xFA, 0x12, 0x14);
        sound_call_005CBAA0(em, 0x138, 1, 0x1A);
        sound_call_005CBAA0(em, 0x54, 0x16, 0);
        sound_call_005CBAA0(em, 0xBE, 0x16, 0);
        if (em_frame_check(em, 0, 78.0f) != 0) {
            shell16_set(em, 0x13);
            Eft15_set3(em, 5, 1.0f, 4);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 86.0f, 298.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x423:
        sound_call_005CBAA0(em, 0xC, 0x16, 0);
        sound_call_005CBAA0(em, 6, 0x22, 0x23);
        sound_call_005CBAA0(em, 0x68, 3, 0x1A);
        break;
    case 0x424:
        sound_call_005CBAA0(em, 6, 0x53, 0x23);
        sound_call_005CBAA0(em, 0x48, 0x13, 0x2A);
        sound_call_005CBAA0(em, 0x22, 6, 0x1A);
        sound_call_005CBAA0(em, 0x32, 3, 0x14);
        sound_call_005CBAA0(em, 0x90, 0, 0x14);
        sound_call_005CBAA0(em, 0x16, 0xD, 6);
        sound_call_005CBAA0(em, 0x2C, 0xC, 0xC);
        if (em_frame_check(em, 0, 32.0f) != 0) {
            Eft13_set_em(em, 0x1B, 7);
        }
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft13_set_em(em, 0x16, 7);
            return;
        }
        break;
    case 0x425:
        sound_call_005CBAA0(em, 6, 0x53, 0x23);
        sound_call_005CBAA0(em, 0xA, 0x12, 0x1A);
        sound_call_005CBAA0(em, 0x26, 6, 0x1A);
        sound_call_005CBAA0(em, 0x4E, 4, 0x14);
        sound_call_005CBAA0(em, 0x6E, 3, 0x14);
        sound_call_005CBAA0(em, 0x5A, 0x15, 0);
        sound_call_005CBAA0(em, 0xAA, 3, 0x1A);
        if (em_frame_check(em, 0, 24.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
            return;
        }
        break;
    case 0x426:
        sound_call_005CBAA0(em, 4, 0x4F, 0x23);
        sound_call_005CBAA0(em, 0x56, 0x1F, 0x23);
        sound_call_005CBAA0(em, 0x10, 3, 0x14);
        sound_call_005CBAA0(em, 4, 0xD, 6);
        sound_call_005CBAA0(em, 8, 0xD, 0xC);
        sound_call_005CBAA0(em, 0x62, 0x13, 0x2A);
        sound_call_005CBAA0(em, 0x7E, 3, 0x1A);
        break;
    case 0x427:
        sound_call_005CBAA0(em, 4, 0x52, 0x23);
        sound_call_005CBAA0(em, 4, 0x13, 0);
        sound_call_005CBAA0(em, 0x4C, 0x1F, 0x23);
        sound_call_005CBAA0(em, 0xE, 0, 0x1A);
        break;
    case 0x428:
        sound_call_005CBAA0(em, 4, 0x52, 0x23);
        sound_call_005CBAA0(em, 0x6C, 0x20, 0x23);
        sound_call_005CBAA0(em, 4, 0x13, 0);
        sound_call_005CBAA0(em, 4, 0x12, 0x1A);
        sound_call_005CBAA0(em, 0x7A, 1, 0x1A);
        sound_call_005CBAA0(em, 0x96, 0, 0x14);
        break;
    case 0x42B:
        sound_call_005CBAA0(em, 0x46, 0x2D, 0x23);
        sound_call_005CBAA0(em, 0x46, 0x17, 0);
        break;
    case 0x42C:
        sound_call_005CBAA0(em, 0x1C, 2, 0x1A);
        sound_call_005CBAA0(em, 0x30, 4, 0x14);
        sound_call_005CBAA0(em, 0xDC, 0x10, 0x1A);
        sound_call_005CBAA0(em, 0x1E, 0x16, 0);
        sound_call_005CBAA0(em, 0x108, 9, 0);
        sound_call_005CBAA0(em, 0x108, 7, 0);
        sound_call_005CBAA0(em, 0x114, 0x17, 0);
        sound_call_005CBAA0(em, 0x180, 0x16, 0);
        sound_call_005CBAA0(em, 0x18, 0x52, 0x23);
        sound_call_005CBAA0(em, 0x8C, 0x28, 0x23);
        sound_call_005CBAA0(em, 0xB4, 0x2C, 0x23);
        sound_call_005CBAA0(em, 0x192, 0x2F, 0x23);
        sound_call_005CBAA0(em, 0x174, 0xF, 6);
        sound_call_005CBAA0(em, 0x176, 0xE, 0xC);
        if (em_frame_check(em, 0, 268.0f) != 0) {
            Eft20_set(0.899999976f, em, 0, 0);
            return;
        }
        break;
    case 0x42D:
        sound_call_005CBAA0(em, 4, 0x52, 0x23);
        sound_call_005CBAA0(em, 4, 0x13, 0x2A);
        sound_call_005CBAA0(em, 0x18, 9, 0);
        sound_call_005CBAA0(em, 0x18, 0x11, 0);
        quake_call_005CBBA0(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x80);
            return;
        }
        break;
    case 0x42E:
    case 0x433:
        sound_call_005CBAA0(em, 8, 0x16, 0);
        sound_call_005CBAA0(em, 4, 0x1F, 0x23);
        break;
    case 0x42F:
        sound_call_005CBAA0(em, 4, 0x16, 0x23);
        sound_call_005CBAA0(em, 0x1E, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x30, 1, 0x14);
        sound_call_005CBAA0(em, 0x46, 1, 0x1A);
        break;
    case 0x430:
        sound_call_005CBAA0(em, 4, 0x16, 0x23);
        sound_call_005CBAA0(em, 0x1E, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x30, 1, 0x1A);
        sound_call_005CBAA0(em, 0x46, 1, 0x14);
        break;
    case 0x432:
        sound_call_005CBAA0(em, 4, 0x52, 0x23);
        sound_call_005CBAA0(em, 4, 0x13, 0x2A);
        sound_call_005CBAA0(em, 0x18, 9, 0);
        sound_call_005CBAA0(em, 0x18, 0x11, 0);
        quake_call_005CBBA0(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x81);
            return;
        }
        break;
    case 0x434:
        sound_call_005CBAA0(em, 0x10, 0x5A, 0x23);
        sound_call_005CBAA0(em, 0x10, 0, 0x1A);
        break;
    case 0x435:
        sound_call_005CBAA0(em, 4, 0x2B, 0x23);
        sound_call_005CBAA0(em, 0x14, 0xB, 0xC);
        sound_call_005CBAA0(em, 0x3A, 0x14, 0);
        break;
    case 0x436:
        sound_call_005CBAA0(em, 0xA, 9, 0);
        sound_call_005CBAA0(em, 0xA, 7, 0x2A);
        sound_call_005CBAA0(em, 0xC, 0x11, 0);
        sound_call_005CBAA0(em, 0x10, 0x2C, 0x23);
        if (em_frame_check(em, 0, 8.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0);
            return;
        }
        break;
    case 0x437:
        sound_call_005CBAA0(em, 0xC, 0x23, 0x23);
        sound_call_005CBAA0(em, 0x24, 0x22, 0x23);
        sound_call_005CBAA0(em, 0x66, 0x22, 0x23);
        sound_call_005CBAA0(em, 0xA4, 0x22, 0x23);
        if ((em_frame_check(em, 0, 48.0f) == 0) && (em_frame_check(em, 0, 106.0f) == 0)) {
            if (em_frame_check(em, 0, 174.0f) != 0) {
                goto block_252;
            }
        } else {
block_252:
            shell16_set(em, 0x10);
            return;
        }
        break;
    case 0x43A:
        sound_call_005CBAA0(em, 0x2E, 0x2A, 0x23);
        sound_call_005CBAA0(em, 4, 0x16, 0);
        break;
    case 0x43C:
        sound_call_005CBAA0(em, 0x16, 0xD, 6);
        sound_call_005CBAA0(em, 0x1A, 0xD, 0xC);
        sound_call_005CBAA0(em, 0x34, 0xA, 6);
        sound_call_005CBAA0(em, 0x38, 0xE, 0xC);
        sound_call_005CBAA0(em, 0x4E, 0xE, 6);
        sound_call_005CBAA0(em, 0x4A, 0xA, 0xC);
        sound_call_005CBAA0(em, 0x58, 0xC, 6);
        sound_call_005CBAA0(em, 0x5C, 0xC, 0xC);
        sound_call_005CBAA0(em, 0x20, 5, 0x1A);
        sound_call_005CBAA0(em, 0x1C, 0, 0x14);
        if ((em_frame_check(em, 0, 42.0f) == 0) && (em_frame_check(em, 0, 60.0f) == 0)) {
            if (em_frame_check(em, 0, 80.0f) != 0) {
                goto block_259;
            }
        } else {
block_259:
            Eft20_set(1.0f, em, 1, 0);
            return;
        }
        break;
    case 0x442:
        sound_call_005CBAA0(em, 0x28, 0x27, 0x23);
        sound_call_005CBAA0(em, 0x20, 0xD, 6);
        sound_call_005CBAA0(em, 0x24, 0xD, 0xC);
        sound_call_005CBAA0(em, 0x4A, 0xD, 6);
        sound_call_005CBAA0(em, 0x4E, 0xD, 0xC);
        sound_call_005CBAA0(em, 0x7C, 0xB, 6);
        sound_call_005CBAA0(em, 0x80, 0xB, 0xC);
        if ((em_frame_check(em, 0, 50.0f) != 0) && (em->pos[1] <= (1000.0f + em->x5AC))) {
            Eft20_set(1.0f, em, 9, 0);
            Eft20_set(1.0f, em, 0xE, 0);
        }
        if ((em_frame_check(em, 0, 102.0f) != 0) && (em->pos[1] <= (1000.0f + em->x5AC))) {
            Eft20_set(1.0f, em, 9, 0);
            Eft20_set(1.0f, em, 0xE, 0);
            return;
        }
        break;
    case 0x447:
        sound_call_005CBAA0(em, 4, 0x10, 0);
        sound_call_005CBAA0(em, 0xA, 0x12, 0);
        sound_call_005CBAA0(em, 0x14, 9, 0);
        sound_call_005CBAA0(em, 4, 0x4F, 0x23);
        sound_call_005CBAA0(em, 4, 0x2A, 0x23);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0x80);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 4.0f, 60.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x448:
        sound_call_005CBAA0(em, 0x16, 0x12, 0);
        sound_call_005CBAA0(em, 0x48, 0x12, 0);
        sound_call_005CBAA0(em, 4, 0xC, 6);
        sound_call_005CBAA0(em, 8, 0xC, 0xC);
        sound_call_005CBAA0(em, 0x30, 0x13, 0);
        sound_call_005CBAA0(em, 4, 0x28, 0x23);
        sound_call_005CBAA0(em, 4, 0x16, 0);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 12.0f, 24.0f) != 0) || (em_frame_check3(em, 0, 46.0f, 90.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x449:
        sound_call_005CBAA0(em, 0x22, 0x16, 6);
        sound_call_005CBAA0(em, 0x26, 0x16, 0xC);
        sound_call_005CBAA0(em, 0x62, 4, 0x22);
        sound_call_005CBAA0(em, 0x6C, 3, 0x22);
        sound_call_005CBAA0(em, 4, 0x2A, 0x23);
        sound_call_005CBAA0(em, 4, 0x2B, 0x23);
        break;
    case 0x44A:
        sound_call_005CBAA0(em, 4, 0x16, 6);
        sound_call_005CBAA0(em, 8, 0x16, 0xC);
        sound_call_005CBAA0(em, 0x6C, 3, 0x22);
        sound_call_005CBAA0(em, 4, 0x21, 0x23);
        sound_call_005CBAA0(em, 0x46, 0x20, 0x23);
        break;
    case 0x44C:
        sound_call_005CBAA0(em, 4, 0x31, 0x23);
        break;
    case 0x44D:
        sound_call_005CBAA0(em, 0x14, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x16, 1, 0x1A);
        sound_call_005CBAA0(em, 0x48, 1, 0x14);
        sound_call_005CBAA0(em, 0x4A, 0x11, 0);
        break;
    case 0x44E:
        sound_call_005CBAA0(em, 0x18, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x62, 0x21, 0x23);
        sound_call_005CBAA0(em, 0x16, 1, 0x1A);
        sound_call_005CBAA0(em, 0x42, 1, 0x14);
        sound_call_005CBAA0(em, 0x48, 0x16, 0);
        break;
    case 0x44F:
        sound_call_005CBAA0(em, 0x10, 0x31, 0x23);
        break;
    case 0x450:
        sound_call_005CBAA0(em, 0x18, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x1C, 1, 0x1A);
        sound_call_005CBAA0(em, 0x6E, 0x15, 0);
        sound_call_005CBAA0(em, 0x5C, 0xD, 6);
        sound_call_005CBAA0(em, 0x60, 0xD, 0xC);
        sound_call_005CBAA0(em, 0x70, 6, 0x1A);
        if (em_frame_check(em, 0, 110.0f) != 0) {
            Eft13_set_em_scl(em, 2, 3.5f, 7);
            return;
        }
        break;
    case 0x451:
        sound_call_005CBAA0(em, 0x40, 0x33, 0x23);
        sound_call_005CBAA0(em, 0x2C, 0x16, 0);
        if (em_frame_check(em, 0, 66.0f) != 0) {
            shell16_set(em, 2);
            Eft15_set3(em, 5, 1.0f, 5);
            return;
        }
        break;
    case 0x452:
        sound_call_005CBAA0(em, 4, 0x28, 0x23);
        sound_call_005CBAA0(em, 0x3C, 0x24, 0x23);
        sound_call_005CBAA0(em, 0x40, 0x15, 0x22);
        sound_call_005CBAA0(em, 0x42, 0x11, 0x1A);
        sound_call_005CBAA0(em, 0x44, 6, 0x1A);
        sound_call_005CBAA0(em, 0x3E, 0xD, 6);
        sound_call_005CBAA0(em, 0x42, 0xD, 0xC);
        if (em_frame_check(em, 0, 80.0f) != 0) {
            shell16_set(em, 0x1D);
        }
        if (em_frame_check(em, 0, 70.0f) != 0) {
            Eft20_set(1.0f, em, 2, 6);
            return;
        }
        break;
    case 0x453:
        sound_call_005CBAA0(em, 4, 0x2C, 0x23);
        sound_call_005CBAA0(em, 0x10, 8, 0x22);
        sound_call_005CBAA0(em, 0x8C, 0x16, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0x1E);
        }
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft13_set_em_scl(em, 2, 3.5f, 7);
            return;
        }
        break;
    case 0x454:
        sound_call_005CBAA0(em, 0xC, 0x24, 0x23);
        sound_call_005CBAA0(em, 0x1A, 6, 0x22);
        sound_call_005CBAA0(em, 0x16, 0x11, 0x1A);
        sound_call_005CBAA0(em, 0x1A, 6, 0x14);
        if (em_frame_check(em, 0, 32.0f) != 0) {
            shell16_set(em, 0x1F);
        }
        if (em_frame_check(em, 0, 20.0f) != 0) {
            Eft20_set(1.0f, em, 2, 6);
            return;
        }
        break;
    case 0x455:
        sound_call_005CBAA0(em, 4, 0x24, 0x23);
        sound_call_005CBAA0(em, 0xC, 0x15, 0);
        if (em_frame_check(em, 0, 24.0f) != 0) {
            shell16_set(em, 0x22);
            return;
        }
        break;
    case 0x456:
        sound_call_005CBAA0(em, 4, 0x16, 0);
        sound_call_005CBAA0(em, 0x32, 0x14, 0x22);
        sound_call_005CBAA0(em, 0x44, 0x21, 0x23);
        sound_call_005CBAA0(em, 0xF6, 0x31, 0x23);
        if ((em_frame_check2(em, 0, 200.0f) != 0) && !(GAME_X1E16 & 0xF)) {
            Shell08_set_ang(em, 0x22, 5, 0, 0, 0);
            return;
        }
        break;
    case 0x457:
        sound_call_005CBAA0(em, 0x24, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x32, 0x16, 0);
        break;
    case 0x458:
        sound_call_005CBAA0(em, 0x28, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x8C, 0x21, 0x23);
        sound_call_005CBAA0(em, 0xB4, 0x27, 0x23);
        sound_call_005CBAA0(em, 0xEC, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xB2, 0x15, 0x22);
        sound_call_005CBAA0(em, 0x40, 1, 0x1A);
        sound_call_005CBAA0(em, 0x96, 1, 0x14);
        sound_call_005CBAA0(em, 0xB0, 0x12, 0x1A);
        if (em_frame_check(em, 0, 180.0f) != 0) {
            shell16_set(em, 0x18);
        }
        if (em_frame_check(em, 0, 172.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 3.0f, 3);
        }
        if (em_frame_check(em, 0, 178.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 3.0f, 3);
            return;
        }
        break;
    case 0x459:
        sound_call_005CBAA0(em, 4, 0x20, 0x23);
        sound_call_005CBAA0(em, 0x40, 0x22, 0x23);
        sound_call_005CBAA0(em, 0x64, 0x21, 0x23);
        sound_call_005CBAA0(em, 0x40, 1, 0x14);
        sound_call_005CBAA0(em, 0x88, 1, 0x14);
        if (em_frame_check(em, 0, 66.0f) != 0) {
            shell16_set(em, 1);
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 40.0f, 72.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
            if (em_frame_check3(em, 0, 102.0f, 106.0f) != 0) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x45A:
        sound_call_005CBAA0(em, 0x3A, 0x28, 0x23);
        sound_call_005CBAA0(em, 0x70, 0x56, 0x23);
        sound_call_005CBAA0(em, 0x14, 1, 0x1A);
        sound_call_005CBAA0(em, 0x26, 0x14, 0x22);
        sound_call_005CBAA0(em, 0x58, 0x15, 0x23);
        sound_call_005CBAA0(em, 4, 0x36, 0);
        sound_call_005CBAA0(em, 0x74, 0x37, 0);
        sound_call_005CBAA0(em, 0xC2, 0x36, 0);
        sound_call_005CBAA0(em, 0xC2, 0x37, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            Eft04_set(em, 0);
        }
        if (em_frame_check(em, 0, 114.0f) != 0) {
            shell16_set(em, 0x1B);
            return;
        }
        break;
    case 0x45B:
        sound_call_005CBAA0(em, 0x34, 1, 0x1A);
        sound_call_005CBAA0(em, 4, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xC, 0x16, 0x22);
        break;
    case 0x45D:
        sound_call_005CBAA0(em, 4, 0x2C, 0x23);
        sound_call_005CBAA0(em, 0x10, 8, 0x22);
        sound_call_005CBAA0(em, 0x92, 0x16, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell16_set(em, 0x1C);
        }
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft13_set_em_scl(em, 2, 3.5f, 7);
            return;
        }
        break;
    case 0x45E:
        sound_call_005CBAA0(em, 0xE, 0x1C, 0x1A);
        sound_call_005CBAA0(em, 0x14, 0x1C, 0x14);
        sound_call_005CBAA0(em, 0x62, 0x1C, 0x2A);
        sound_call_005CBAA0(em, 0x44, 0x1B, 0xC);
        sound_call_005CBAA0(em, 0x60, 0x1B, 6);
        break;
    case 0x45F:
        sound_call_005CBAA0(em, 0x16, 0x1B, 6);
        sound_call_005CBAA0(em, 2, 0x1B, 0xC);
        sound_call_005CBAA0(em, 0x20, 0x1C, 0x1A);
        sound_call_005CBAA0(em, 0xA, 0x1C, 0x14);
        break;
    case 0x460:
        sound_call_005CBAA0(em, 0xA, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xA, 0x1B, 0xC);
        sound_call_005CBAA0(em, 0x38, 0x1B, 6);
        sound_call_005CBAA0(em, 0x20, 0x1C, 0x14);
        sound_call_005CBAA0(em, 0x2A, 0x1C, 0x1A);
        sound_call_005CBAA0(em, 0x4C, 0x1C, 0x14);
        break;
    case 0x461:
        sound_call_005CBAA0(em, 0xA, 0x20, 0x23);
        sound_call_005CBAA0(em, 0xA, 0x1B, 0xC);
        sound_call_005CBAA0(em, 0x38, 0x1B, 6);
        sound_call_005CBAA0(em, 0x20, 0x1C, 0x14);
        sound_call_005CBAA0(em, 0x2A, 0x1C, 0x1A);
        sound_call_005CBAA0(em, 0x4C, 0x1C, 0x14);
        break;
    default:
        move_default_005CBBF0(em);
        break;
    }
}

void em15_effect_move(EMW *em) {
    EM15W *w = (EM15W *)em->ex;
    u8 temp_a2;

    temp_a2 = w->eff;
    switch (temp_a2) {                              /* irregular */
    case 0:
        w->eff = temp_a2 + 1;
        break;
    case 1:
        ef_move_sub_005CBC40(em, w);
        break;
    }
    em15_uvmove(em);
}

static void ground_land_eff_set_005CF030(EMW *em) {
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

void dummy_em_prog_005CF0E0(void) {

}


