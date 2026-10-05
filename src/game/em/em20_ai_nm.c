/* em20 draft */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em20.h"

typedef struct FLYNEED {
    u8 _pad00[0x14];
    s32 x14;
} FLYNEED;
extern FLYNEED *em_thirst_tbl[];
typedef f32 (*EM_POSP)[3];
EM_POSP gp_ptr_ck(EMW *em, EM_STG_POS *p);
extern u16 gero_tbl[4][2];
typedef struct STAGE_DATA {
    u8 _pad00[0x10];
    f32 width;
    f32 depth;
    f32 floor_y;
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
void em_pl_pos_set(EMW *, u8, f32 *);
void World_calc2(u8, f32 *, f32 *);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
void em_act_set(EMW *, int, u16);
void eft09_set(EMW *, int);
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
void em_cmd_ck(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void Em_Mahi_Start(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
int Code_Make(int, int, int, int);
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
void em_rate_clear_g(EMW *);
int Event_flag_ck();
void em_dur_set(EMW *, int);
int Quest_clear_ck();
void em_ikari_add(EMW *, s16);
void Eft20_set(f32, EMW *, int, int);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
int em_frame_check3(EMW *, int, f32, f32);
void em20_act_set(EMW *em, int kind, u16 no, u16 arg);
void em20_ground_point_search(EMW *em);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_hinshi_ck(EMW *, f32);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void get_joint_pos_em(EMW *, int, f32 *);
void em_range_set(EMW *em, s8 no);
void NextStage_No_Set(EMW *);
void Em_Next_Stage_Pos(EMW *);
void NextStage_Dir_Set(EMW *, f32 *);
void em_area_move_init(EMW *em);
void xang_calc_pl(EMW *em, int *ang, f32 a, f32 b);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void em_egg_ck(EMW *em);
void em_hungry_ck(EMW *em);
void em_thirst_ck(EMW *em);
void em_search_data_set(EMW *em, u8 no);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_suimin_end(EMW *em);
void Em_Suimin_Start(EMW *em);
void em_hungry_add(EMW *em, s32 n);
void em_no_floor_ck(EMW *em);
void em_hp_add(EMW *em, s16 n);
int em_target_pl_samestage_ck(EMW *em);
void em_ana_loop_cnt_set(EMW *em);
int em_hokaku_ck(EMW *em, f32 rate);
int em_sleep_hp_add(EMW *em, s16 n, s16 max, s16 step);
void em_niku_eat_set(EMW *em);
void em_sleep2_dmg_timer_set(EMW *em);
void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl);
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
extern s16 em20_stay_timer_tbl[];
extern s16 em20_runaway_timer_tbl[];

void em20_local_init(EMW *em);
void em20_init(EMW *em);
static u16 *em_act_search2_005EC0A0(EMW *em, u16 *tbl);
static void act_dist_select_005EC0E0(EMW *em);
void em20_to_normal(EMW *em, s16 a, s16 b);
void em20_dmg_to_normal(EMW *em, s16 a, s16 b);
void em20_to_fly(EMW *em, int flag);
static void item_theft_005EC560(PLW *arg1);
void em20_frame_reset(EMW *em, int i);
static void em_act00_005EC740(EMW *em, EM20W *w);
static void em_act01_005EC820(EMW *em, EM20W *w);
static void em_act02_005EC910(EMW *em, EM20W *w);
static void em_act03_005EC9A0(EMW *em, EM20W *w);
static void em_act04_005ECA20(EMW *em, EM20W *w);
static void em_act05_005ECAC0(EMW *em, EM20W *w);
static void em_act06_005ECBB0(EMW *em, EM20W *w);
static void em_act07_005ECC80(EMW *em, EM20W *w);
static void em_act08_005ECD20(EMW *em, EM20W *w);
static void em_act09_005ECD30(EMW *em, EM20W *w);
static void em_act10_005ECDE0(EMW *em, EM20W *w);
static void em_act11_005ECEF0(EMW *em, EM20W *w);
static void em_act12_005ECFC0(EMW *em, EM20W *w);
static void em_act13_005ED040(EMW *em, EM20W *w);
static void em_act14_005ED110(EMW *em, EM20W *w);
static void em_act15_005ED4A0(EMW *em, EM20W *w);
static void em_act16_005ED5D0(EMW *em, EM20W *w);
static void em_act17_005ED700(EMW *em, EM20W *w);
static void em_act18_005ED780(EMW *em, EM20W *w);
static void em_act19_005ED940(EMW *em, EM20W *w);
static void em_act40_005EDAC0(EMW *em, EM20W *w);
static void em_act20_005EDB80(EMW *em, EM20W *w);
static void em_act21_005EDC90(EMW *em, EM20W *w);
static void em_act22_005EDE50(EMW *em, EM20W *w);
static void em_act23_005EDED0(EMW *em, EM20W *w);
static void em_act24_005EDFA0(EMW *em, EM20W *w);
static void em_act25_005EE070(EMW *em, EM20W *w);
static void em_act26_005EE0F0(EMW *em, EM20W *w);
static void em_act27_005EE230(EMW *em, EM20W *w);
static void em_act28_005EE340(EMW *em, EM20W *w);
static void em_act29_005EE410(EMW *em, EM20W *w);
static void em_act31_005EE550(EMW *em, EM20W *w);
static void em_act33_005EE690(EMW *em, EM20W *w);
void em_act34(EMW *em, EM20W *w);
static void em_mv00_005EE7A0(EMW *em, EM20W *w);
static void em_mv01_005EE900(EMW *em, EM20W *w);
static void em_mv02_005EE920(EMW *em, EM20W *w);
static void em_mv03_005EEA80(EMW *em, EM20W *w);
static void em_mv04_005EED50(EMW *em, EM20W *w);
static void em_mv05_005EEEB0(EMW *em, EM20W *w);
static void em_mv06_005EF180(EMW *em, EM20W *w);
static void em_mv07_005EF2E0(EMW *em, EM20W *w);
static void em_mv09_005EF420(EMW *em, EM20W *w);
static void em_mv10_005EF430(EMW *em, EM20W *w);
static void em_fly00_005EF5C0(EMW *em, EM20W *w);
static void em_fly01_005EF6C0(EMW *em, EM20W *w);
static void em_fly02_005EF900(EMW *em, EM20W *w);
static void em_fly03_005EFA10(EMW *em, EM20W *w);
static void em_fly04_005EFCA0(EMW *em, EM20W *w);
static void em_fly05_005EFE10(EMW *em, EM20W *w);
static void em_fly06_005EFF80(EMW *em, EM20W *w);
static void em_fly07_005F01A0(EMW *em, EM20W *w);
static void em_fly08_005F02E0(EMW *em, EM20W *w);
static void em_fly09_005F0580(EMW *em, EM20W *w);
static void em_fly10_005F0730(EMW *em, EM20W *w);
static void em_fly11_005F0940(EMW *em, EM20W *w);
static void em_fly12_005F0B90(EMW *em, EM20W *w);
static void em_fly13_005F0CF0(EMW *em, EM20W *w);
static void em_fly14_005F1050(EMW *em, EM20W *w);
static void em_fly15_005F1120(EMW *em, EM20W *w);
static void em_fly16_005F11E0(EMW *em, EM20W *w);
static void em_fly17_005F12F0(EMW *em, EM20W *w);
static void em_fly18_005F1530(EMW *em, EM20W *w);
static void em_fly19_005F1700(EMW *em, EM20W *w);
static void em_fly20_005F18F0(EMW *em, EM20W *w);
static void em_fly21_005F1A40(EMW *em, EM20W *w);
static void em_fly22_005F1B80(EMW *em, EM20W *w);
static void em_fly23_005F1CE0(EMW *em, EM20W *w);
static void em_fly24_005F1F20(EMW *em, EM20W *w);
static void em_atk00_005F2040(EMW *em, EM20W *w);
static void em_atk02_005F20E0(EMW *em, EM20W *w);
static void em_atk03_005F2350(EMW *em, EM20W *w);
static void em_atk04_005F23D0(EMW *em, EM20W *w);
static void em_atk06_005F24F0(EMW *em, EM20W *w);
static void em_atk07_005F25E0(EMW *em, EM20W *w);
static void em_atk08_005F2680(EMW *em, EM20W *w);
static void em_atk09_005F2FE0(EMW *em, EM20W *w);
static void em_atk10_005F30D0(EMW *em, EM20W *w);
static void em_atk11_005F3170(EMW *em, EM20W *w);
static void em_atk18_005F32A0(EMW *em, EM20W *w);
static void em_atk21_005F3440(EMW *em, EM20W *w);
static void em_atk26_005F3CE0(EMW *em, EM20W *w, int idx);
static void em_atk27_005F3E70(EMW *em, EM20W *w);
static void em_atk28_005F3F30(EMW *em, EM20W *w);
static void em_atk29_005F3FF0(EMW *em, EM20W *w);
static void em_atk30_005F4090(EMW *em, EM20W *w);
static void em_atk31_005F41D0(EMW *em, EM20W *w);
static void em_dmg00_005F4260(EMW *em, EM20W *w);
static void em_dmg01_005F42F0(EMW *em, EM20W *w);
static void em_dmg02_005F4380(EMW *em, EM20W *w);
static void em_dmg03_005F4410(EMW *em, EM20W *w);
static void em_dmg04_005F44A0(EMW *em, EM20W *w);
static void em_dmg05_005F44B0(EMW *em, EM20W *w);
static void em_dmg06_005F45D0(EMW *em, EM20W *w);
static void em_dmg07_005F46F0(EMW *em, EM20W *w);
static void em_dmg08_005F47E0(EMW *em, EM20W *w);
static void em_dmg09_005F4940(EMW *em, EM20W *w);
static void em_dmg10_005F4AD0(EMW *em, EM20W *w);
static void em_dmg11_005F4B60(EMW *em, EM20W *w);
static void em_dmg12_005F4C40(EMW *em, EM20W *w);
static void em_dmg13_005F4D70(EMW *em, EM20W *w);
static void em_dmg14_005F4EA0(EMW *em, EM20W *w);
static void em_dmg15_005F4FC0(EMW *em, EM20W *w);
static void em_dmg16_005F5090(EMW *em, EM20W *w);
static void em_dmg17_005F5160(EMW *em, EM20W *w);
static void em_dmg18_005F5290(EMW *em, EM20W *w);
static void em_dmg19_005F5390(EMW *em, EM20W *w);
static void em_dmg20_005F5490(EMW *em, EM20W *w);
static void em_demo00_005F55D0(EMW *em, EM20W *w);
static void em_demo04_005F5870(EMW *em, EM20W *w);
static void em_die00_005F5990(EMW *em, EM20W *w);
static void em_die01_005F5B30(EMW *em, EM20W *w);
static void em_die02_005F5CB0(EMW *em, EM20W *w);
static void em_move00_005F5F20(EMW *em, EM20W *w);
static void em_move01_005F6180(EMW *em, EM20W *w);
static void em_move02_005F6270(EMW *em, EM20W *w);
static void em_move03_005F6440(EMW *em, EM20W *w);
static void em_move04_005F6680(EMW *em, EM20W *w);
static void em_move05_005F6810(EMW *em, EM20W *w);
static void em_move06_005F6880(EMW *em, EM20W *w);
void em20_main(EMW *em);
void em20_main_sub(EMW *em, EM20W *w);
void em20_uvmove(EMW *em);
static void sound_call_sub_005F75F0(EMW *em, int se, int joint);
static void sound_call_005F7660(EMW *em, int frame, int se, int joint);
static void sound_call_parts_005F76C0(EMW *em, int frame, int se, int joint, u8 layer);
void Em_set_quake_sub(EMW *, int);
static void quake_call_005F7760(EMW *em, int frame, int arg);
static void move_default_005F77B0(EMW *em);
static void ef_move_sub_005F7800(EMW *em, EM20W *w);
void em20_effect_move(EMW *em);
static void ground_land_eff_set_005FC860(EMW *em);
static void takeoff_eff_set_005FC910(EMW *em);
static void takeon_eff_set_005FC980(EMW *em);
static void hover_eff_set2_005FCA20(EMW *em);
static s32 kyusyu_char_set_005FCA70(EMW *em);
void em20_atk_end_sel(EMW *em, EM20W *w);
static void kyusyu_senkai_ret_005FCBA0(EMW *em);
void em20_material_sub(EMW *em, int type, u8 *tbl);
void dummy_em_prog_005FCDB0(void);


void em20_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *) em, 0);
}

void em20_init(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
    int hp;
    u8 temp_a0;
    u8 temp_a1;

    if (quest_w.no == 0) {
        em->ang[1] = 0;
        switch (game_w.stage) {
        case 0:
            em->pos[0] = 8000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 6000.0f;
            break;
        case 15:
            em->pos[0] = 7900.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 1050.0f;
            em->pos[2] = 13900.0f;
            break;
        case 18:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 200.0f;
            em->pos[2] = 8000.0f;
            break;
        case 22:
            em->pos[0] = 9900.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 300.0f;
            em->pos[2] = 10350.0f;
            break;
        case 24:
            em->pos[0] = 10000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 5700.0f;
            em->ang[1] = 0x4000;
            break;
        case 27:
            em->pos[0] = 14300.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 7100.0f;
            break;
        case 33:
            em->pos[0] = 17300.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 18000.0f;
            break;
        case 37:
            em->pos[0] = 9750.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 300.0f;
            em->pos[2] = 7750.0f;
            break;
        case 40:
            em->pos[0] = 10500.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9500.0f;
            break;
        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            break;
        }
    }
    em_char_set(em, 1, 0, 0);
    em->x388 = 0;
    em20_act_set(em, 0, 1, 0);
    w->x06 = 0;
    w->x1B = 1;
    w->x08 = 0;
    if (em->kind == 0x14) {
        hp = em_hp_vital_set2(em, 0x1F4, 0x3E8);
        em->x302 = hp;
    } else {
        hp = em_hp_vital_set2(em, 0x12C, 0x2BC);
        em->x302 = hp;
    }
    em->x792 = hp;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em20_stay_timer_tbl[em->stg];
    em->runaway_tm = em20_runaway_timer_tbl[em->stg];
    w->dang = 0x4000;
    w->pitch_spd = 0x100;
    w->turn = 0x200;
    w->bank_spd = 0x100;
    w->bank_max = 0x2000;
    w->turn_left = 0;
    em->x734 = 3;
    M2C_FIELD(em, u8 *, 0x735) = 0;
    w->x48 = 0;
    w->x49 = 0;
    w->x4A = 0;
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

static u16 *em_act_search2_005EC0A0(EMW *em, u16 *tbl) {
    EM20W *w = (EM20W *)em->ex;
    u16 *p;
    u8 i;

    i = w->x19;
    w->x19 = i + 1;
    p = tbl + i * 2;
    if (*p == 0xFFFF) {
        p = tbl;
        w->x19 = 1;
    }
    return p;
}

extern u8 *em20_act_add[3];
extern u16 *em20_rail_add[2];
extern u16 *em20_rail_half_add[1];

static void act_dist_select_005EC0E0(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
    u16 *p;
    int k;
    u8 temp_a1;
    u8 temp_a2;

    temp_a1 = em->x734;
    temp_a2 = M2C_FIELD(em, u8 *, 0x735);
    switch (temp_a1) {
    case 0:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, em_act_search(em20_act_add[temp_a2]) & 0xFFFF, 1);
        }
        break;
    case 1:
        if (em->x8C3 == 0) {
            p = em_act_search2_005EC0A0(em, em20_rail_add[temp_a2]);
            k = p[0];
            if (k == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em20_act_set(em, k, p[1], 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            p = em_act_search2_005EC0A0(em, em20_rail_half_add[temp_a2]);
            k = p[0];
            if (k == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em20_act_set(em, k, p[1], 1);
        }
        break;
    case 3:
        em->x839 = 1;
        if (em->x388 == 0) {
            em20_act_set(em, 0, 1, 0);
        } else {
            em20_act_set(em, 2, 2, 0);
        }
        break;
    }
}

void em20_to_normal(EMW *em, s16 a, s16 b) {
    f32 k = 0.2f;

    if (em->x734 != 0) {
        act_dist_select_005EC0E0(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        if (em->x302 < (s16)((f32)em->x792 * k)) {
            em20_act_set(em, 0, 1, 0);
        } else if (em->x888 == 0) {
            em20_act_set(em, 0, 1, 0);
        } else {
            em20_act_set(em, 0, 0x11, 0);
        }
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
    em20_act_set(em, 0, 1, 0);
}

void em20_dmg_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x39A % 100 < 0xA) {
        em20_to_normal(em, 0, 0);
        em->x839 = 0;
        if (em->kind == 0x14) {
            em20_act_set(em, 3, 0x1E, 1);
            return;
        }
        if (em->x8B6 != 0) {
            em20_act_set(em, 3, 0x1F, 1);
            return;
        }
        em20_act_set(em, 3, 0x1E, 1);
        return;
    }
    em20_to_normal(em, a, b);
}

void em20_to_fly(EMW *em, int flag) {
    if (em->x734 != 3) {
        act_dist_select_005EC0E0(em);
        return;
    }
    em->x839 = 1;
    em->act_spd = 1.0f;
    switch (flag & 0xFF) {
    case 0:
        em_act_set(em, 2, 0xE);
        break;
    case 1:
        em_act_set(em, 2, 0xF);
        break;
    }
}

static void item_theft_005EC560(PLW *pl) {
    int list[20];
    int n;
    int i;
    int sel;
    u16 id;

    if (pl->id == game_w.master && Quest_clear_ck(1) == 0) {
        if (Pl_Skill_ck(pl, 0x2B) != 1) {
            n = 0;
            for (i = 0; i < 20; i++) {
                id = pl->item[i].id;
                if (id != 0) {
                    if (Item_data[id][0] == 0 && Item_data[id][2] < 4) {
                        list[n++] = i;
                    }
                }
            }
            if (n != 0) {
                sel = (s16)pl->item[list[(u16)ran_suu(1) % n]].id;
                Pl_item_stack(pl, sel & 0xFFFF, -1);
                set01_set(1, 6, sel);
            }
        }
    }
}

void em20_frame_reset(EMW *em, int i) {
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

static void em_act00_005EC740(EMW *em, EM20W *w) {
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

extern u8 *em20_act_add[3];

static void em_act01_005EC820(EMW *em, EM20W *w) {
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
            if (((u32) (temp_a0 - 1) < 2) || (temp_a0 == 3)) {
                if (em->x194 == 0) {
                    em->x05 += 1;
                    act_dist_select_005EC0E0(em);
                    return;
                }
            } else {
                temp_a2 = em_act_search(*(&em20_act_add + (em->_pad735[0] * 4))) & 0xFFFF;
                if (temp_a2 != 1) {
                    em20_act_set(em, 0, temp_a2, 0);
                }
            }
        } else {
            return;
        }
        break;
    }
}

static void em_act02_005EC910(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6A, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act03_005EC9A0(EMW *em, EM20W *w) {
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
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act04_005ECA20(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em20_frame_reset(em, 1);
    em20_frame_reset(em, 2);
}

static void em_act05_005ECAC0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em20_frame_reset(em, 0);
    em20_frame_reset(em, 2);
    sound_call_parts_005F76C0(em, 0x20, 0x2F, 0x23, 1);
}

static void em_act06_005ECBB0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em20_frame_reset(em, 0);
    em20_frame_reset(em, 2);
    sound_call_parts_005F76C0(em, 0x20, 0x20, 0x23, 1);
    sound_call_parts_005F76C0(em, 0xA0, 0x17, 0x23, 1);
}

static void em_act07_005ECC80(EMW *em, EM20W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_v1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em20_frame_reset(em, 0);
    em20_frame_reset(em, 2);
}

static void em_act08_005ECD20(EMW *em, EM20W *w) {

}

static void em_act09_005ECD30(EMW *em, EM20W *w) {
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
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act10_005ECDE0(EMW *em, EM20W *w) {
    u16 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x65, 0, 0);
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        temp_v1 = w->dang;
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act11_005ECEF0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em20_frame_reset(em, 0);
    em20_frame_reset(em, 2);
    sound_call_parts_005F76C0(em, 0x38, 0x21, 0x23, 1);
    sound_call_parts_005F76C0(em, 0x68, 0x1F, 0x23, 1);
}

static void em_act12_005ECFC0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act13_005ED040(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em20_frame_reset(em, 0);
    em20_frame_reset(em, 2);
    sound_call_parts_005F76C0(em, 2, 0x20, 0x23, 1);
    sound_call_parts_005F76C0(em, 0x4A, 0x1F, 0x23, 1);
}

static void em_act14_005ED110(EMW *em, EM20W *w) {
    s32 temp_a0;
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
        if ((em_frame_check(em, 0, 56.0f) != 0) || (em_frame_check(em, 0, 108.0f) != 0) || (em_frame_check(em, 0, 126.0f) != 0) || (em_frame_check(em, 0, 138.0f) != 0) || (em_frame_check(em, 0, 170.0f) != 0)) {
            sound_call_sub_005F75F0(em, 0x26, 0x23);
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
            em->x05 = temp_a1 + 1;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    em->thirst += 0xDE;
    temp_a0 = em->thirst_max;
    if (temp_a0 < em->thirst) {
        em->thirst = temp_a0;
    }
}

static void em_act15_005ED4A0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act16_005ED5D0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act17_005ED700(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act18_005ED780(EMW *em, EM20W *w) {
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
        if (em->kind == 6) {
            em->work08 = 0x1518;
            return;
        }
        em->work08 = 0x2328;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x1F, 0, 0);
        }
        break;
    case 2:
        if (em->kind == 6) {
            em_sleep_hp_add(em, 1, (s16)(0.35f * (f32) em->x792), 9);
        } else {
            em_sleep_hp_add(em, 1, (s16)(0.4f * (f32) em->x792), 5);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_hinshi_end(em);
            em20_act_set(em, 0, 0x17, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x17, 4);
        }
        break;
    }
}

static void em_act19_005ED940(EMW *em, EM20W *w) {
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
            em20_act_set(em, 0, 0x28, 4);
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
            em20_act_set(em, 0, 0x28, 4);
        }
        break;
    }
}

static void em_act40_005EDAC0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act20_005EDB80(EMW *em, EM20W *w) {
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
            em20_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

static void em_act21_005EDC90(EMW *em, EM20W *w) {
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
        if (em->kind == 6) {
            em->work08 = 0x1518;
            return;
        }
        em->work08 = 0x2328;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x22, 0, 0);
        }
        break;
    case 2:
        if (em->kind == 6) {
            em_sleep_hp_add(em, 1, (s16)(0.35f * (f32) em->x792), 9);
        } else {
            em_sleep_hp_add(em, 1, (s16)(0.4f * (f32) em->x792), 5);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em20_act_set(em, 0, 0x18, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x18, 4);
        }
        break;
    }
}

static void em_act22_005EDE50(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act23_005EDED0(EMW *em, EM20W *w) {
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
            em20_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act24_005EDFA0(EMW *em, EM20W *w) {
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
            em20_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act25_005EE070(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act26_005EE0F0(EMW *em, EM20W *w) {
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
            em->hungry = em->hungry_max;
            em_hp_add(em, (s16)(0.05f * (f32) em->x792));
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act27_005EE230(EMW *em, EM20W *w) {
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
            em20_act_set(em, 0, 0x1C, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x1C, 4);
        }
        break;
    }
}

static void em_act28_005EE340(EMW *em, EM20W *w) {
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
            em20_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act29_005EE410(EMW *em, EM20W *w) {
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
            em20_act_set(em, 4, 0x12, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 4, 0x12, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005F75F0(em, 0x57, 0x23);
    }
}

static void em_act31_005EE550(EMW *em, EM20W *w) {
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
            em20_act_set(em, 4, 0x13, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 4, 0x13, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005F75F0(em, 0x57, 0x23);
    }
}

static void em_act33_005EE690(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

void em_act34(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv00_005EE7A0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv01_005EE900(EMW *em, EM20W *w) {
    em->x05 += 1;
    em20_to_normal(em, 0, 0);
}

static void em_mv02_005EE920(EMW *em, EM20W *w) {
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
        em_char_set(em, 0x68, 0, 0);
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv03_005EEA80(EMW *em, EM20W *w) {
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
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->dang - em->ang[1]) & 0xFFFF;
        if (temp_v1 < 0xE39 || temp_v1 >= 0xF1C8) {
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
            temp_a1 = w->dang;
            temp_s0 = (temp_a1 - (temp_a2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + var_a3) & 0xFFFF) < (u32) (var_a3 * 2)) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em20_to_normal(em, 0, 0);
                    return;
                }
                if ((temp_s0 < 0xE39) || (temp_s0 >= 0xF1C8)) {
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
        } else {
            return;
        }
        break;
    }
}

static void em_mv04_005EED50(EMW *em, EM20W *w) {
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
        em_char_set(em, 0x11, 0, 0);
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv05_005EEEB0(EMW *em, EM20W *w) {
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
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->dang - em->ang[1]) & 0xFFFF;
        if (temp_v1 < 0xE39 || temp_v1 >= 0xF1C8) {
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
            temp_a1 = w->dang;
            temp_s0 = (temp_a1 - (temp_a2 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + var_a3) & 0xFFFF) < (u32) (var_a3 * 2)) {
                    em->x05 += 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em20_to_normal(em, 0, 0);
                    return;
                }
                if ((temp_s0 < 0xE39) || (temp_s0 >= 0xF1C8)) {
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
        } else {
            return;
        }
        break;
    }
}

static void em_mv06_005EF180(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv07_005EF2E0(EMW *em, EM20W *w) {
    s32 temp_a0;
    u16 temp_v1;
    u32 temp_a1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x36, 0, 0);
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        temp_a0 = em->ang[1];
        temp_v1 = w->dang;
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv09_005EF420(EMW *em, EM20W *w) {

}

static void em_mv10_005EF430(EMW *em, EM20W *w) {
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
        em_char_set(em, 0x68, 0, 0);
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
            em_char_set(em, 0x65, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_fly00_005EF5C0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em20_fly_adjy2_init(em, 2);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 0, 38.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em20_fly_adjy2(em);
        }
        break;
    case 2:
        if (em20_fly_adjy2(em) != 0) {
            em->x05 += 1;
            em20_to_fly(em, 0);
        }
        break;
    }
}

static void em_fly01_005EF6C0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly02_005EF900(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 temp_v0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        em20_fly_adjy(1, temp_a2);
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em20_act_set(em, 2, 1, 1);
        }
        em20_senkai_target(em);
        if (em->work08 == 0x12C) {
            em20_to_fly(em, 0);
        }
        break;
    case 2:
        em20_fly_adjy(1, temp_a2);
        em20_senkai_target(em);
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly03_005EFA10(EMW *em, EM20W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em_rate_clear(em);
        em20_fly_adjy2_init(em, 2);
        w->x18 = 0;
        break;
    case 1:
        if ((em_frame_check2(em, 0, 50.0f) != 0) && (em_frame_check2(em, 0, 114.0f) == 0)) {
            em20_senkai_target(em);
        }
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            em20_fly_adjy2(em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if ((em20_fly_adjy2(em) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
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
            w->x06 = 0x96;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly04_005EFCA0(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 var_s2;
    u8 temp_a1;

    var_s2 = 0;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em20_fly_adjy2_init(em, 3);
        em->adj_z = 20.0f;
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            var_s2 = em20_fly_adjy2(em) & 0xFF;
        }
        if ((var_s2 != 0) && (em->pos[1] <= em->x5AC)) {
            em_char_set(em, 0x13, 0, 0);
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            w->x06 = 0x96;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly05_005EFE10(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 var_s2;
    u8 temp_a1;

    var_s2 = 0;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em20_fly_adjy2_init(em, 3);
        em->adj_z = -20.0f;
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            var_s2 = em20_fly_adjy2(em) & 0xFF;
        }
        if ((var_s2 != 0) && (em->pos[1] <= em->x5AC)) {
            em_char_set(em, 0x13, 0, 0);
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            w->x06 = 0x96;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly06_005EFF80(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 temp_v0;
    u8 temp_a1;
    u8 temp_v0_2;
    u8 temp_v1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x388 = 2;
        em_char_set(em, 0xC, 0, 0);
        em->x05 += 1;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
        }
        w->x18 = 1;
        break;
    case 1:
        temp_v1 = em->x07;
        if ((temp_v1 == 0) && (em->x194 == 0)) {
            em->x07 = temp_v1 + 1;
            em_char_set(em, 0xE, 0, 0);
        }
        if ((CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) || (temp_v0 = em->work08 - 1, em->work08 = temp_v0, (temp_v0 <= 0))) {
            temp_v0_2 = w->x05 - 1;
            w->x05 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em20_to_fly(em, 1);
            } else {
                em->x883 += 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)(((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f));
                em_act_set(em, 2, 6);
            }
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 1, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly07_005F01A0(EMW *em, EM20W *w) {
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
        em20_senkai_target(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        em20_fly_adjy(em, 1);
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
            em20_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

extern FLYNEED *em_hungry_tbl[];


static void em_fly08_005F02E0(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_4;
    u8 temp_a2;
    u8 temp_v1_3;
    FLYNEED *temp_a3;

    temp_a2 = em->x05;
    temp_a3 = em_thirst_tbl[em->kind];
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_rate_clear(em);
        em->adj_z = 50.0f;
        em_char_set(em, 0xC, 0, 0);
        NextStage_Dir_Set(em, em->tgt_pos);
        w->x18 = 1;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 1, 0);
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
        temp_a0 = temp_a3->x14;
        temp_v1 = em->thirst;
        if (temp_a0 < temp_v1) {
            em->thirst = temp_v1 - temp_a0;
        } else {
            em->thirst = 0;
        }
        temp_a0_2 = em_hungry_tbl[em->kind]->x14;
        temp_v1_2 = em->hungry;
        if (temp_a0_2 < temp_v1_2) {
            em->hungry = temp_v1_2 - temp_a0_2;
        } else {
            em->hungry = 0;
        }
        break;
    case 3:
        em->x05 = temp_a2 + 1;
        em->work08 = 0x258;
        Em_Next_Stage_Pos(em);
        temp_v1_3 = em->x92F;
        if ((u16) em->x73A != temp_v1_3) {
            if (temp_v1_3 == 0xFF) {
                goto block_19;
            }
            if (em->x8C3 == 0) {
                em->x73A = (s16) temp_v1_3;
                em->x829 = temp_v1_3 & 0xFFFF;
                em->x827 = 3;
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em20_act_set(em, 2, 0xD, 1);
            WyvernAreaMove(em);
        } else {
block_19:
            WyvernAreaMove(em);
            em20_act_set(em, 2, 9, 1);
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

static void em_fly09_005F0580(EMW *em, EM20W *w) {
    s32 temp_v1;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_z = 80.0f;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[1] = (s32) w->dang;
        em->x92F = 0xFF;
        w->x18 = 1;
        em_area_move_init(em);
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 5, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        em->work08 -= 1;
        if ((CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) || (em->work08 < 0)) {
            em->x05 += 1;
            em->work08 = 0x258;
            em20_act_set(em, 2, 1, 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->work08 = 0x258;
                em20_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
}

static void em_fly10_005F0730(EMW *em, EM20W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em->x388 = 2;
        em_rate_clear(em);
        NextStage_No_Set(em);
        NextStage_Dir_Set(em, em->tgt_pos);
        w->x18 = 1;
        break;
    case 1:
        em20_senkai_target(temp_a1, 2);
        em20_fly_adjy(em, 1);
        temp_f1 = em->pos[1] + 100.0f;
        em->pos[1] = temp_f1;
        if (!(temp_f1 < em->tgt_pos[1])) {
            em->x05 += 1;
        }
        break;
    case 2:
        em->x05 = temp_a1 + 1;
        em->adj_z = 50.0f;
        em_char_set(em, 0xC, 0, 0);
        /* fallthrough */
    case 3:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 1, 0);
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
        em20_to_fly(em, 1);
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly11_005F0940(EMW *em, EM20W *w) {
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
        em_char_set(em, 0xC, 0, 0);
        if (!(em->adj_z <= 100.0f)) {
            em->adj_z = 100.0f;
        }
        w->x18 = 1;
        break;
    case 1:
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) {
            if (!(em->adj_z <= 50.0f)) {
                em->adj_z = 50.0f;
            }
            em->x05 += 1;
            em->work08 = 0x12C;
            em20_act_set(em, 2, 1, 1);
        } else {
            w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
            w->dang = (u16) (w->dang - em->ang[1]);
            em20_senkai_sub(em, 1, 0);
            w->spd[0] = (s32) em->ang[0];
            w->spd[1] = (s32) em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 += 1;
                em20_act_set(em, 2, 1, 1);
            }
        }
        break;
    case 2:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 1, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em20_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly12_005F0B90(EMW *em, EM20W *w) {
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
        em_char_set(em, 0x34, 0, 0);
        em20_fly_adjy2_init(em, 0xE);
        break;
    case 1:
        if (em_frame_check2(em, 0, 76.0f) != 0) {
            em->x388 = 2;
            var_s1 = em20_fly_adjy2(em) & 0xFF;
        }
        if ((var_s1 != 0) && (em->pos[1] <= em->x5AC)) {
            em_char_set(em, 0x39, 0, 0);
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            ground_land_eff_set_005FC860(em);
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly13_005F0CF0(EMW *em, EM20W *w) {
    f32 temp_f1;
    f32 var_f0;
    s32 temp_v1_2;
    u8 temp_a2;
    u8 temp_v1;
    STAGE_DATA *temp_v0;
    EM_POSP temp_v0_2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        w->turn = 0x100;
        em_char_set(em, 0xF, 0, 0);
        em_rate_clear(em);
        em->adj_z = 80.0f;
        temp_v0 = Stage_data_get(em->stg);
        em->tgt_pos[1] = temp_v0->floor_y;
        temp_v0_2 = gp_ptr_ck(em, em->area->x0);
        if (temp_v0_2 == 0) {
            em->tgt_pos[0] = temp_v0->width / 2.0f;
            var_f0 = temp_v0->depth / 2.0f;
        } else {
            em->tgt_pos[0] = (*temp_v0_2)[0];
            var_f0 = (*temp_v0_2)[2];
        }
        em->tgt_pos[2] = var_f0;
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        em->ang[1] = (s32) w->dang;
        w->x18 = 1;
        em_area_move_init(em);
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 5, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        em->work08 -= 1;
        if ((CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) || (em->work08 < 0)) {
            em->x05 += 1;
            NextStage_Dir_Set(em, em->tgt_pos);
        }
        break;
    case 2:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 1, 0);
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
        if ((u16) em->x73A != temp_v1) {
            if (temp_v1 == 0xFF) {
                goto block_21;
            }
            if (em->x8C3 == 0) {
                em->x73A = (s16) temp_v1;
                em->x829 = temp_v1 & 0xFFFF;
                em->x827 = 3;
                cmd_target_kind_set(em, em->tgt_pos);
            }
            em20_act_set(em, 2, 0xD, 1);
            WyvernAreaMove(em);
        } else {
block_21:
            WyvernAreaMove(em);
            em20_act_set(em, 2, 9, 1);
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

static void em_fly14_005F1050(EMW *em, EM20W *w) {
    f32 temp_f1;
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xF, 0, 0);
        w->x18 = 0;
        break;
    case 1:
        em20_fly_adjy(em, 1);
        if (em->x194 == 0) {
            em->x05 += 1;
            em20_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly15_005F1120(EMW *em, EM20W *w) {
    f32 temp_f1;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_char_set(em, 0xC, 0, 0);
        em_rate_clear_g(em);
        em->adj_y = 0.0f;
        em->rate_x = 0.0f;
        w->x18 = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em20_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly16_005F11E0(EMW *em, EM20W *w) {
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
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        w->x18 = 0;
        em->work08 = 0x96;
        break;
    case 1:
        em20_fly_adjy(em, 1);
        temp_a0 = em20_senkai_target(em) & 0xFF;
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 > 0) {
            if (temp_a0 != 0) {
                goto block_9;
            }
        } else {
block_9:
            em->x05 += 1;
            em20_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly17_005F12F0(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 temp_v0;
    u16 temp_a0;
    u8 temp_a1;
    u8 temp_v1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z < 50.0f) {
            em->x07 = 0;
            em->adj_z = 50.0f;
            em_char_set(em, 0xC, 0, 0);
        } else {
            em->x07 = 1;
            temp_a0 = em->char0;
            if ((temp_a0 != 0x3F4) && (temp_a0 != 0x3F6)) {
                em_char_set(em, 0xC, 0, 0);
            }
        }
        w->x18 = 1;
        break;
    case 1:
        temp_v1 = em->x07;
        if ((temp_v1 == 0) && (em->x194 == 0)) {
            em->x07 = temp_v1 + 1;
            em_char_set(em, 0xE, 0, 0);
        }
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_17;
            }
        } else {
block_17:
            em->x883 += 1;
            em->x883 &= 3;
            target_kind_set(em, em->tgt_pos);
            em->work08 = (s32)(((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f)) + 0x96;
            em->x05 += 1;
            em_act_set(em, 2, 0x11);
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub2(em, 1, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly18_005F1530(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x5A, 0, 0);
        em20_fly_adjy2_init(em, 9);
        w->x18 = 0;
        break;
    case 1:
        temp_v1 = em->ang[0];
        if (temp_v1 != 0) {
            if (temp_v1 < 0x8000) {
                em->ang[0] = temp_v1 - w->pitch_spd;
            } else {
                em->ang[0] = temp_v1 + w->pitch_spd;
            }
            em->ang[0] = (s32) (u16) M2C_FIELD(em, s32 *, 0xA0);
        }
        temp_v1_2 = em->ang[2];
        if (temp_v1_2 != 0) {
            if (temp_v1_2 < 0x8000) {
                em->ang[2] = temp_v1_2 - w->bank_spd;
            } else {
                em->ang[2] = temp_v1_2 + w->bank_spd;
            }
            em->ang[2] = (s32) (u16) M2C_FIELD(em, s32 *, 0xA8);
        }
        if (em20_fly_adjy2(em, temp_a1, 2) & 0xFF) {
            em->x05 += 1;
            em_rate_clear(em);
            em->work08 = 0x12C;
            em_char_set(em, 0xF, 0, 0);
            em20_act_set(em, 2, 1, 1);
        }
        break;
    case 2:
        temp_v1_3 = em->work08 - 1;
        em->work08 = temp_v1_3;
        if (temp_v1_3 <= 0) {
            em->work08 = 0x12C;
            em_char_set(em, 0xF, 0, 0);
            if (em->x8C3 == 0) {
                em20_act_set(em, 2, 1, 1);
            }
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly19_005F1700(EMW *em, EM20W *w) {
    f32 temp_f1;
    s32 temp_v0;
    u16 temp_a0;
    u8 temp_a1;
    u8 temp_v1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z < 50.0f) {
            em->x07 = 0;
            em->adj_z = 50.0f;
            em_char_set(em, 0xC, 0, 0);
        } else {
            em->x07 = 1;
            temp_a0 = em->char0;
            if ((temp_a0 != 0x3F4) && (temp_a0 != 0x3F6)) {
                em_char_set(em, 0xC, 0, 0);
            }
        }
        w->x18 = 1;
        break;
    case 1:
        temp_v1 = em->x07;
        if ((temp_v1 == 0) && (em->x194 == 0)) {
            em->x07 = temp_v1 + 1;
            em_char_set(em, 0xE, 0, 0);
        }
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_17;
            }
        } else {
block_17:
            em->x05 += 1;
            em_act_set(em, 2, 0xF);
        }
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub3(em, 1, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly20_005F18F0(EMW *em, EM20W *w) {
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
        em20_fly_adjy2_init(em, 0xC);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 36.0f) != 0) {
            em->x388 = 2;
            var_s1 = em20_fly_adjy2(em) & 0xFF;
        }
        if ((var_s1 != 0) && (em->pos[1] <= em->x5AC)) {
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            ground_land_eff_set_005FC860(em);
            Em_set_quake_sub(em, 2);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly21_005F1A40(EMW *em, EM20W *w) {
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
            em20_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly22_005F1B80(EMW *em, EM20W *w) {
    s32 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x5A, 0, 0);
        em20_fly_adjy2_init(em, 9);
        w->x18 = 0;
        break;
    case 1:
        temp_v1 = em->ang[0];
        if (temp_v1 != 0) {
            if (temp_v1 < 0x8000) {
                em->ang[0] = temp_v1 - w->pitch_spd;
            } else {
                em->ang[0] = temp_v1 + w->pitch_spd;
            }
            em->ang[0] = (s32) (u16) M2C_FIELD(em, s32 *, 0xA0);
        }
        temp_v1_2 = em->ang[2];
        if (temp_v1_2 != 0) {
            if (temp_v1_2 < 0x8000) {
                em->ang[2] = temp_v1_2 - w->bank_spd;
            } else {
                em->ang[2] = temp_v1_2 + w->bank_spd;
            }
            em->ang[2] = (s32) (u16) M2C_FIELD(em, s32 *, 0xA8);
        }
        if (em20_fly_adjy2(em, temp_a1, 2) & 0xFF) {
            em->x05 += 1;
            em_rate_clear(em);
            em20_to_fly(em, 0);
        }
        break;
    case 2:
        em20_fly_adjy2((EMW *) temp_a1, 2);
        break;
    }
}

static void em_fly23_005F1CE0(EMW *em, EM20W *w) {
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
        em20_fly_adjy2_init(em, 2);
        em->x388 = 2;
        em->x9EA = 0;
        em->x959 = 0;
        em->x8BD = 1;
        break;
    case 1:
        em20_fly_adjy2(temp_a2);
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if ((em20_fly_adjy2(temp_a2) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_3 = em->x5AC;
    if (em->pos[1] < temp_f1_3) {
        em->pos[1] = temp_f1_3;
    }
}

static void em_fly24_005F1F20(EMW *em, EM20W *w) {
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
            em20_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_atk00_005F2040(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em20_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk02_005F20E0(EMW *em, EM20W *w) {
    f32 temp_f1;
    u8 temp_a2;
    u8 temp_v1;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_v1 = em->x06;
        switch (temp_v1) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            em->x06 = temp_v1 + 1;
            em->x388 = 0;
            /* fallthrough */
        case 1:                                     /* switch 2 */
            if (em20_horm_main(em, 1, temp_a2) != 0) {
                em->x3F4 = 0;
                em->x05 += 1;
                em_char_set(em, 0x28, 0, 0);
                em20_fly_adjy2_init(em, 5);
            }
            break;
        }
        break;
    case 1:                                         /* switch 1 */
        if (em_frame_check(em, 0, 58.0f) != 0) {
            takeoff_eff_set_005FC910(em);
        }
        if (em_frame_check2(em, 0, 60.0f) != 0) {
            em->x388 = 2;
            em20_fly_adjy2(em);
            em->x05 += 1;
        }
        break;
    case 2:                                         /* switch 1 */
        w->dist = (f32) (w->dist - em->adj_z);
        em20_fly_adjy2(1, temp_a2);
        if (w->dist <= 0.0f) {
            em->x05 += 1;
            em->x3C0[1] = -10.0f;
        }
        if (w->dist <= 500.0f) {
            hover_eff_set2_005FCA20(em);
        }
        break;
    case 3:                                         /* switch 1 */
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        if (w->dist <= 500.0f) {
            hover_eff_set2_005FCA20(em);
        }
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x29, 0, 0);
            takeon_eff_set_005FC980(em);
        }
        break;
    case 4:                                         /* switch 1 */
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em20_atk_end_sel(em, w);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_atk03_005F2350(EMW *em, EM20W *w) {
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
            em20_atk_end_sel(em, w);
        }
        break;
    }
}

static void em_atk04_005F23D0(EMW *em, EM20W *w) {
    s32 temp_v1;
    s32 temp_v1_2;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x44, 0, 0);
        em->x40C = 5;
        break;
    case 1:
        em->x40C = 5;
        if (em_frame_check(em, 0, 350.0f) != 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em->work08 = 0x12C;
            em20_act_set(em, 3, 9, 1);
            return;
        }
        break;
    case 3:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em20_act_set(em, 3, 9, 1);
            }
        }
        break;
    }
}

static void em_atk06_005F24F0(EMW *em, EM20W *w) {
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
            em20_atk_end_sel(em, w);
        }
        break;
    }
}

static void em_atk07_005F25E0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6D, 0, 0);
        em->x88B = 1;
        em->x839 = 0;
        em_cmd_reset(em);
        em->x8BD = 1;
        break;
    case 1:
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk08_005F2680(EMW *em, EM20W *w) {
    f32 sp60[3];
    f32 sp50[3];
    f32 sp40[3];
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    f32 temp_f1_5;
    f32 temp_f1_6;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v1;
    s32 temp_v1_3;
    s32 temp_v1_6;
    s8 temp_s0_2;
    s8 temp_v1_5;
    u16 temp_v1_2;
    u8 temp_a1;
    u8 temp_v1_4;
    STAGE_DATA *temp_s0;

    temp_a1 = em->x05;
    temp_s0 = Stage_data_get(em->stg);
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 3, 1);
        temp_f1 = em->adj_z;
        if (temp_f1 > 100.0f) {
            em->adj_z = temp_f1 - 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em->x05 += 1;
            em->work08 = 0x258;
        }
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 2:
        if (em->x617 == -1) {
            kyusyu_senkai_ret_005FCBA0(em);
        } else {
            temp_f1_2 = em->adj_z;
            if (temp_f1_2 > 80.0f) {
                em->adj_z = temp_f1_2 - 1.0f;
            }
            temp_s0_2 = em->x617;
            em_pl_pos_set(em, temp_s0_2 & 0xFF, sp50);
            World_calc2(player_work[(s8)temp_s0_2].stg, sp50, sp60);
            w->dang = Em_Calc_angY(em->x754, sp60);
            w->dang = (u16) (w->dang - em->ang[1]);
            em20_senkai_sub(em, 3, 1);
            temp_v1_2 = w->dang;
            if (((s32) temp_v1_2 < 0x801) || ((s32) temp_v1_2 >= 0xF800)) {
                if (!(CalcDistanceXZ(em->pos, sp50) > 4000.0f)) {
                    kyusyu_senkai_ret_005FCBA0(em);
                } else {
                    em->x05 += 1;
                    w->turn = 0x100;
                    em_char_set(em, 0x2C, 0, 0);
                    em->work08 = 0x258;
                }
            }
            w->spd[0] = (s32) em->ang[0];
            w->spd[1] = (s32) em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 > 0) {
                if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                    goto block_23;
                }
            } else {
block_23:
                kyusyu_senkai_ret_005FCBA0(em);
            }
        }
        break;
    case 3:
        temp_v1_3 = em->ang[2];
        if (temp_v1_3 != 0) {
            if (temp_v1_3 < 0x8001) {
                em->ang[2] = temp_v1_3 - w->bank_spd;
            } else {
                em->ang[2] = temp_v1_3 + w->bank_spd;
            }
        }
        if (em_frame_check2(em, 0, 46.0f) != 0) {
            temp_f1_3 = em->adj_z;
            if (temp_f1_3 > 50.0f) {
                em->adj_z = temp_f1_3 - 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x258;
        }
        em20_senkai_player(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 0.1f;
        temp_v0_2 = em->work08 - 1;
        em->work08 = temp_v0_2;
        if (temp_v0_2 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_37;
            }
        } else {
block_37:
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 4:
        if (em->pos[1] <= (1000.0f + temp_s0->floor_y)) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0x258;
        }
        if (kyusyu_char_set_005FCA70(em) != 0) {
            temp_v1_4 = em->x05;
            if (temp_v1_4 == 4) {
                em->x05 = temp_v1_4 + 1;
                em->work08 = 0x258;
            }
        }
        if (em->char0 == 0x415) {
            em20_xang_set_pl(0, em, 2);
        } else {
            em20_xang_set_pl(0xC3480000, em, 0);
        }
        em20_senkai_player(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_pl(em, w->spd, -3000.0f, 0.0f);
        if (em->x74C != 0) {
            w->spd[0] = 0;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            temp_f1_4 = em->adj_y;
            if (temp_f1_4 < 0.0f) {
                em->pos[1] -= temp_f1_4;
            }
            em->pos[1] += 30.0f;
        }
        temp_v0_3 = em->work08 - 1;
        em->work08 = temp_v0_3;
        if (temp_v0_3 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_56;
            }
        } else {
block_56:
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 5:
        kyusyu_char_set_005FCA70(em);
        if ((em->char0 == 0x415) && (em->x194 == 0)) {
            em->x05 = 6;
            em_char_set(em, 0xE, 0, 0);
            em20_ground_point_search(em);
        }
        em20_senkai_player(em);
        em20_xang_set_pl(0x42C80000, em, 2);
        if ((em->char0 == 0x415) && (em_frame_check2(em, 0, 76.0f) != 0)) {
            xang_calc_pl(em, w->spd, 300.0f, 0.0f);
        } else {
            temp_v1_5 = em->x617;
            if (temp_v1_5 != -1) {
                em_pl_pos_set(em, temp_v1_5 & 0xFF, sp40);
                if (!((200.0f + sp40[1]) < em->pos[1])) {
                    xang_calc_pl(em, w->spd, 50.0f, 0.0f);
                } else {
                    xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
                }
            } else {
                xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
            }
        }
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        if (em->x74C != 0) {
            w->spd[0] = 0;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            temp_f1_5 = em->adj_y;
            if (temp_f1_5 < 0.0f) {
                em->pos[1] -= temp_f1_5;
            }
            em->pos[1] += 30.0f;
        }
        temp_v0_4 = em->work08 - 1;
        em->work08 = temp_v0_4;
        if (temp_v0_4 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_79;
            }
        } else {
block_79:
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 6:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_xang_set_pl(0, em, 1);
        em20_senkai_sub(em, 3, 1);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if ((1300.0f + temp_s0->floor_y) <= em->pos[1]) {
            em->x05 += 1;
            em->work08 = 0x12C;
        }
        break;
    case 7:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_xang_set_pl(0, em, 2);
        em20_senkai_sub(em, 3, 1);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em20_ground_point_search(em);
        }
        if (((em->ang[0] == 0) && (em->ang[2] == 0)) || (temp_v1_6 = em->work08 - 1, em->work08 = temp_v1_6, (temp_v1_6 <= 0))) {
            em->ang[0] = 0;
            em->ang[2] = 0;
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    }
    temp_f1_6 = em->x5AC;
    if (em->pos[1] < temp_f1_6) {
        em->pos[1] = temp_f1_6;
    }
}

static void em_atk09_005F2FE0(EMW *em, EM20W *w) {
    s32 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6E, 0, 0);
        Em_Mode_Chg(em, 0, 0);
        em->x40C = 5;
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em->work08 = 0x12C;
            em20_act_set(em, 3, 7, 1);
        }
        break;
    case 2:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em20_act_set(em, 3, 7, 1);
            }
        }
        break;
    }
}

static void em_atk10_005F30D0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em20_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x66, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk11_005F3170(EMW *em, EM20W *w) {
    PLW *temp_t0;
    u16 temp_a2;
    u8 temp_a3;

    temp_a3 = em->x05;
    switch (temp_a3) {                              /* irregular */
    case 0:
        if (em20_horm_main() != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x66, 0, 0);
            em->x19 = 0;
        }
        break;
    case 1:
        if (em->x19 != 0) {
            temp_t0 = em->x7A0;
            if (temp_t0 != 0) {
                temp_a2 = temp_t0->id;
                if (((s32) temp_a2 < 4) && (temp_t0 == &player_work[temp_a2]) && (em->x7A4->kind == 0x11) && (temp_t0->be_flag != 0)) {
                    em->x05 = temp_a3 + 1;
                    item_theft_005EC560(em->x7A0);
                }
            }
        }
        /* fallthrough */
    case 2:
        if (em->x194 == 0) {
            em->x05 = 3;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk18_005F32A0(EMW *em, EM20W *w) {
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
        em_char_set(em, 0x11, 0, 0);
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
            if (temp_f1 <= 500.0f) {
                em->work08 = 1;
            }
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em_char_set(em, 0x5B, 0, 0);
            shell17_set(em, 0x24);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk21_005F3440(EMW *em, EM20W *w) {
    f32 sp60[3];
    f32 sp50[3];
    f32 sp40[3];
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    f32 temp_f1_5;
    f32 temp_f1_6;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_3;
    s8 temp_s2;
    s8 temp_v1_5;
    u16 temp_v1_2;
    u8 temp_a1;
    u8 temp_v1_4;
    STAGE_DATA *temp_v0;

    temp_v0 = Stage_data_get(em->stg);
    temp_a1 = em->x05;
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 3, 1);
        temp_f1 = em->adj_z;
        if (temp_f1 > 100.0f) {
            em->adj_z = temp_f1 - 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em->x05 += 1;
            em->work08 = 0x258;
        }
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 2:
        if (em->x617 == -1) {
            kyusyu_senkai_ret_005FCBA0(em);
        } else {
            temp_f1_2 = em->adj_z;
            if (temp_f1_2 > 80.0f) {
                em->adj_z = temp_f1_2 - 1.0f;
            }
            temp_s2 = em->x617;
            em_pl_pos_set(em, temp_s2 & 0xFF, sp50);
            World_calc2(player_work[(s8)temp_s2].stg, sp50, sp60);
            w->dang = Em_Calc_angY(em->x754, sp60);
            w->dang = (u16) (w->dang - em->ang[1]);
            em20_senkai_sub(em, 3, 1);
            temp_v1_2 = w->dang;
            if (((s32) temp_v1_2 < 0x801) || ((s32) temp_v1_2 >= 0xF800)) {
                if (!(CalcDistanceXZ(em->pos, sp50) > 4000.0f)) {
                    kyusyu_senkai_ret_005FCBA0(em);
                } else {
                    em->x05 += 1;
                    w->turn = 0x100;
                    em_char_set(em, 0x2C, 0, 0);
                    em->work08 = 0x258;
                }
            }
            w->spd[0] = (s32) em->ang[0];
            w->spd[1] = (s32) em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            temp_v0_2 = em->work08 - 1;
            em->work08 = temp_v0_2;
            if (temp_v0_2 > 0) {
                if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                    goto block_24;
                }
            } else {
block_24:
                kyusyu_senkai_ret_005FCBA0(em);
            }
        }
        break;
    case 3:
        temp_v1_3 = em->ang[2];
        if (temp_v1_3 != 0) {
            if (temp_v1_3 < 0x8001) {
                em->ang[2] = temp_v1_3 - w->bank_spd;
            } else {
                em->ang[2] = temp_v1_3 + w->bank_spd;
            }
        }
        if (em_frame_check2(em, 0, 46.0f) != 0) {
            em20_xang_set_pl(0xC3480000, em, 0);
            temp_f1_3 = em->adj_z;
            if (temp_f1_3 > 50.0f) {
                em->adj_z = temp_f1_3 - 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x258;
        }
        em20_senkai_player(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 0.1f;
        temp_v0_3 = em->work08 - 1;
        em->work08 = temp_v0_3;
        if (temp_v0_3 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_38;
            }
        } else {
block_38:
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 4:
        if (em->pos[1] <= (1000.0f + temp_v0->floor_y)) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0x258;
        }
        if (kyusyu_char_set_005FCA70(em) != 0) {
            temp_v1_4 = em->x05;
            if (temp_v1_4 == 4) {
                em->x05 = temp_v1_4 + 1;
                em->work08 = 0x258;
            }
        }
        if (em->char0 == 0x415) {
            em20_xang_set_pl(0, em, 2);
        } else {
            em20_xang_set_pl(0xC3480000, em, 0);
        }
        em20_senkai_player(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_pl(em, w->spd, -3000.0f, 0.0f);
        if (em->x74C != 0) {
            w->spd[0] = 0;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            temp_f1_4 = em->adj_y;
            if (temp_f1_4 < 0.0f) {
                em->pos[1] -= temp_f1_4;
            }
            em->pos[1] += 30.0f;
        }
        temp_v0_4 = em->work08 - 1;
        em->work08 = temp_v0_4;
        if (temp_v0_4 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_57;
            }
        } else {
block_57:
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 5:
        kyusyu_char_set_005FCA70(em);
        if ((em->char0 == 0x415) && (em->x194 == 0)) {
            em->x05 = 6;
            em->x3C0[0] = 0.0f;
            em->x3C0[2] = -10.0f;
            em->x3C0[1] = -10.0f;
        }
        em20_senkai_player(em);
        em20_xang_set_pl(0x42C80000, em, 2);
        if ((em->char0 == 0x415) && (em_frame_check2(em, 0, 76.0f) != 0)) {
            xang_calc_pl(em, w->spd, 50.0f, 0.0f);
        } else {
            temp_v1_5 = em->x617;
            if (temp_v1_5 != -1) {
                em_pl_pos_set(em, temp_v1_5 & 0xFF, sp40);
                if (!((200.0f + sp40[1]) < em->pos[1])) {
                    xang_calc_pl(em, w->spd, 50.0f, 0.0f);
                } else {
                    xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
                }
            } else {
                xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
            }
        }
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        if (em->x74C != 0) {
            w->spd[0] = 0;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            temp_f1_5 = em->adj_y;
            if (temp_f1_5 < 0.0f) {
                em->pos[1] -= temp_f1_5;
            }
            em->pos[1] += 30.0f;
        }
        temp_v0_5 = em->work08 - 1;
        em->work08 = temp_v0_5;
        if (temp_v0_5 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_80;
            }
        } else {
block_80:
            kyusyu_senkai_ret_005FCBA0(em);
        }
        break;
    case 6:
        if (em->adj_z < 0.0f) {
            em->adj_z = 0.0f;
            em->x3C0[2] = 0.0f;
        }
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 0, 0);
        }
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_atk_end_sel(em, w);
        }
        break;
    }
    temp_f1_6 = em->x5AC;
    if (em->pos[1] < temp_f1_6) {
        em->pos[1] = temp_f1_6;
    }
}


static void em_atk26_005F3CE0(EMW *em, EM20W *w, int idx) {
    u8 temp_a0;

    temp_a0 = em->x05;
    switch (temp_a0) {
    case 0:
        em->x05 = temp_a0 + 1;
        em->x388 = 0;
    case 1:
        if (em20_horm_main(em) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x2F, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 2.0f * (f32)(u32)gero_tbl[idx][0]) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, gero_tbl[idx][1], 0);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, gero_tbl[idx][1], 0);
            }
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk27_005F3E70(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        /* fallthrough */
    case 1:
        if (em20_horm_main(em, temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x69, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk28_005F3F30(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        /* fallthrough */
    case 1:
        if (em20_horm_main(em, temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x6A, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk29_005F3FF0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6B, 0, 0);
        w->x08 = 1;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            w->x08 = 0;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}


static void em_atk30_005F4090(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x2F, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 0, 2.0f * (f32)(u32)gero_tbl[0][0]) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, gero_tbl[0][1], 0);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, gero_tbl[0][1], 0);
            }
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk31_005F41D0(EMW *em, EM20W *w) {
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
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg00_005F4260(EMW *em, EM20W *w) {
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
            em20_dmg_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg01_005F42F0(EMW *em, EM20W *w) {
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
            em20_dmg_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg02_005F4380(EMW *em, EM20W *w) {
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
            em20_dmg_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg03_005F4410(EMW *em, EM20W *w) {
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
            em20_dmg_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg04_005F44A0(EMW *em, EM20W *w) {

}

static void em_dmg05_005F44B0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg06_005F45D0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg07_005F46F0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em_char_set(em, 0x4D, 0, 0);
        if (!(em->adj_y <= -50.0f)) {
            em->adj_y = -50.0f;
        }
        em_cmd_reset(em);
        break;
    case 1:
        if (em_frame_check(em, 0, 14.0f) != 0) {
            em->x05 += 1;
            em20_to_fly(em, 1);
            em->adj_y = 0.0f;
        }
        speed_add(em, w->spd);
        break;
    }
}

static void em_dmg08_005F47E0(EMW *em, EM20W *w) {
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
            em20_dmg_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg09_005F4940(EMW *em, EM20W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        em->x88B = 0;
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
            em_char_set(em, 0x47, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em20_dmg_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg10_005F4AD0(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg11_005F4B60(EMW *em, EM20W *w) {
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
        em->x8BD = 1;
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em->x8BD = 0;
            em20_act_set(em, 0, 0x22, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 0, 0x22, 4);
        }
        break;
    }
}

static void em_dmg12_005F4C40(EMW *em, EM20W *w) {
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
            em20_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x8C3 == 0) {
            em20_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg13_005F4D70(EMW *em, EM20W *w) {
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
                em_char_set(em, 0x60, 0, 0);
                em20_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em20_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg14_005F4EA0(EMW *em, EM20W *w) {
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
            em20_act_set(em, 4, 0x11, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 4, 0x11, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg15_005F4FC0(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x6C, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x64, 0xA, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg16_005F5090(EMW *em, EM20W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x44, 0, 0);
        em->x40C = 5;
        break;
    case 1:
        em->x40C = 5;
        if (em_frame_check(em, 0, 350.0f) != 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        em->x40C = 5;
        if (em->x194 == 0) {
            em20_act_set(em, 3, 9, 4);
        }
        break;
    }
}

static void em_dmg17_005F5160(EMW *em, EM20W *w) {
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
                em_char_set(em, 0x60, 0, 0);
                em20_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em20_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg18_005F5290(EMW *em, EM20W *w) {
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
            em20_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg19_005F5390(EMW *em, EM20W *w) {
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
            em20_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em20_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg20_005F5490(EMW *em, EM20W *w) {
    FLMAT m40;
    f32 v[3];
    f32 v2[3];
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3F, 0, 0);
        w->x1B = 0;
        v[0] = 0.0f;
        v[1] = 90.0f;
        v[2] = 0.0f;
        flmatCopy(&m40, get_joint_wmat_em(em, 0x27));
        flvecApplyMat33_2(v, &m40);
        v2[0] = m40[3][0] + v[0];
        v2[1] = m40[3][1] + v[1];
        v2[2] = m40[3][2] + v[2];
        Eft02_set3(em, 0, 0xA, 0, v2, 1.0f);
        Quest_enemy_hagi_set(em, 0x100);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_dmg_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_demo00_005F55D0(EMW *em, EM20W *w) {
    f32 temp_f0;
    f32 temp_f1;
    s32 temp_v1;
    u8 temp_a2;

    em->x9E1 = 5;
    em->x40C = 5;
    temp_a2 = em->x05;
    switch (temp_a2) {
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em->pos[0] = 8600.0f;
        em->pos[1] = 1500.0f;
        em->pos[2] = 4300.0f;
        em->tgt_pos[0] = 8600.0f;
        em->tgt_pos[1] = 1500.0f;
        em->tgt_pos[2] = 15000.0f;
        em->ang[0] = 0;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->ang[2] = 0;
        em_char_set(em, 0xC, 0, 0);
        em_rate_clear(em);
        em->adj_z = 50.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em->work08 = 0x12C;
        em->ex[0x90] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        temp_f0 = CalcDistanceXZ(em->pos, em->tgt_pos);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 > 0) {
            if (temp_f0 <= 1000.0f) {
                goto block_8;
            }
        } else {
block_8:
            em->x05 += 1;
            em->ex[0x90] = 1;
            em->x388 = 0;
            em->pos[0] = 8000.0f;
            em->pos[1] = 0.0f;
            em->pos[2] = 13984.0f;
            em->tgt_pos[0] = 8000.0f;
            em->tgt_pos[1] = 0.0f;
            em->tgt_pos[2] = 15468.0f;
            em_char_set(em, 0x29, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x50, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x33, 0, 0);
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 5:
        if (Event_flag_ck(0x10) == 1) {
            em->x05 += 1;
            em->x9E1 = 0;
            em->x40C = 0;
            em20_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_demo04_005F5870(EMW *em, EM20W *w) {
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
        sound_call_sub_005F75F0(em, 0x57, 0x23);
    }
}

static void em_die00_005F5990(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die01_005F5B30(EMW *em, EM20W *w) {
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
            em20_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die02_005F5CB0(EMW *em, EM20W *w) {
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
        break;
    case 0x1:
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
            em_char_set(em, 0x52, 6, 0);
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
            em20_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_move00_005F5F20(EMW *em, EM20W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_act00_005EC740(em, w);
        break;
    case 1:
        em_act01_005EC820(em, w);
        break;
    case 2:
        em_act02_005EC910(em, w);
        break;
    case 3:
        em_act03_005EC9A0(em, w);
        break;
    case 4:
        em_act04_005ECA20(em, w);
        break;
    case 5:
        em_act05_005ECAC0(em, w);
        break;
    case 6:
        em_act06_005ECBB0(em, w);
        break;
    case 7:
        em_act07_005ECC80(em, w);
        break;
    case 8:
        em_act08_005ECD20(em, w);
        break;
    case 9:
        em_act09_005ECD30(em, w);
        break;
    case 10:
        em_act10_005ECDE0(em, w);
        break;
    case 11:
        em_act11_005ECEF0(em, w);
        break;
    case 12:
        em_act12_005ECFC0(em, w);
        break;
    case 13:
        em_act13_005ED040(em, w);
        break;
    case 14:
        em_act14_005ED110(em, w);
        break;
    case 15:
        em_act15_005ED4A0(em, w);
        break;
    case 16:
        em_act16_005ED5D0(em, w);
        break;
    case 17:
        em_act17_005ED700(em, w);
        break;
    case 18:
        em_act18_005ED780(em, w);
        break;
    case 19:
        em_act19_005ED940(em, w);
        break;
    case 20:
        em_act20_005EDB80(em, w);
        break;
    case 21:
        em_act21_005EDC90(em, w);
        break;
    case 22:
        em_act22_005EDE50(em, w);
        break;
    case 23:
        em_act23_005EDED0(em, w);
        break;
    case 24:
        em_act24_005EDFA0(em, w);
        break;
    case 25:
        em_act25_005EE070(em, w);
        break;
    case 26:
        em_act26_005EE0F0(em, w);
        break;
    case 27:
        em_act27_005EE230(em, w);
        break;
    case 28:
        em_act28_005EE340(em, w);
        break;
    case 29:
        em_act29_005EE410(em, w);
        break;
    case 31:
        em_act31_005EE550(em, w);
        break;
    case 33:
        em_act33_005EE690(em, w);
        break;
    case 34:
        em_act34(em, w);
        break;
    case 40:
        em_act40_005EDAC0(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move01_005F6180(EMW *em, EM20W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_mv00_005EE7A0(em, w);
        break;
    case 1:
        em_mv01_005EE900(em, w);
        break;
    case 2:
        em_mv02_005EE920(em, w);
        break;
    case 3:
        em_mv03_005EEA80(em, w);
        break;
    case 4:
        em_mv04_005EED50(em, w);
        break;
    case 5:
        em_mv05_005EEEB0(em, w);
        break;
    case 6:
        em_mv06_005EF180(em, w);
        break;
    case 7:
        em_mv07_005EF2E0(em, w);
        break;
    case 8:
        em_mv04_005EED50(em, w);
        break;
    case 9:
        em_mv09_005EF420(em, w);
        break;
    case 10:
        em_mv10_005EF430(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move02_005F6270(EMW *em, EM20W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_fly00_005EF5C0(em, w);
        break;
    case 1:
        em_fly01_005EF6C0(em, w);
        break;
    case 2:
        em_fly02_005EF900(em, w);
        break;
    case 3:
        em_fly03_005EFA10(em, w);
        break;
    case 4:
        em_fly04_005EFCA0(em, w);
        break;
    case 5:
        em_fly05_005EFE10(em, w);
        break;
    case 6:
        em_fly06_005EFF80(em, w);
        break;
    case 7:
        em_fly07_005F01A0(em, w);
        break;
    case 8:
        em_fly08_005F02E0(em, w);
        break;
    case 9:
        em_fly09_005F0580(em, w);
        break;
    case 10:
        em_fly10_005F0730(em, w);
        break;
    case 11:
        em_fly11_005F0940(em, w);
        break;
    case 12:
        em_fly12_005F0B90(em, w);
        break;
    case 13:
        em_fly13_005F0CF0(em, w);
        break;
    case 14:
        em_fly14_005F1050(em, w);
        break;
    case 15:
        em_fly15_005F1120(em, w);
        break;
    case 16:
        em_fly16_005F11E0(em, w);
        break;
    case 17:
        em_fly17_005F12F0(em, w);
        break;
    case 18:
        em_fly18_005F1530(em, w);
        break;
    case 19:
        em_fly19_005F1700(em, w);
        break;
    case 20:
        em_fly20_005F18F0(em, w);
        break;
    case 21:
        em_fly21_005F1A40(em, w);
        break;
    case 22:
        em_fly22_005F1B80(em, w);
        break;
    case 23:
        em_fly23_005F1CE0(em, w);
        break;
    case 24:
        em_fly24_005F1F20(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move03_005F6440(EMW *em, EM20W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_atk00_005F2040(em, w);
        break;
    case 2:
        em_atk02_005F20E0(em, w);
        break;
    case 3:
        em_atk03_005F2350(em, w);
        break;
    case 4:
        em_atk04_005F23D0(em, w);
        break;
    case 6:
        em_atk06_005F24F0(em, w);
        break;
    case 7:
        em_atk07_005F25E0(em, w);
        break;
    case 8:
        em_atk08_005F2680(em, w);
        break;
    case 9:
        em_atk09_005F2FE0(em, w);
        break;
    case 10:
        em_atk10_005F30D0(em, w);
        break;
    case 11:
        em_atk11_005F3170(em, w);
        break;
    case 18:
        em_atk18_005F32A0(em, w);
        break;
    case 21:
        em_atk21_005F3440(em, w);
        break;
    case 23:
        em_atk26_005F3CE0(em, w, 1);
        break;
    case 24:
        em_atk26_005F3CE0(em, w, 2);
        break;
    case 25:
        em_atk26_005F3CE0(em, w, 3);
        break;
    case 26:
        em_atk26_005F3CE0(em, w, 0);
        break;
    case 27:
        em_atk27_005F3E70(em, w);
        break;
    case 28:
        em_atk28_005F3F30(em, w);
        break;
    case 29:
        em_atk29_005F3FF0(em, w);
        break;
    case 30:
        em_atk30_005F4090(em, w);
        break;
    case 31:
        em_atk31_005F41D0(em, w);
        break;
    }
}

static void em_move04_005F6680(EMW *em, EM20W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_dmg00_005F4260(em, w);
        break;
    case 1:
        em_dmg01_005F42F0(em, w);
        break;
    case 2:
        em_dmg02_005F4380(em, w);
        break;
    case 3:
        em_dmg03_005F4410(em, w);
        break;
    case 4:
        em_dmg04_005F44A0(em, w);
        break;
    case 5:
        em_dmg05_005F44B0(em, w);
        break;
    case 6:
        em_dmg06_005F45D0(em, w);
        break;
    case 7:
        em_dmg07_005F46F0(em, w);
        break;
    case 8:
        em_dmg08_005F47E0(em, w);
        break;
    case 9:
        em_dmg09_005F4940(em, w);
        break;
    case 10:
        em_dmg10_005F4AD0(em, w);
        break;
    case 11:
        em_dmg11_005F4B60(em, w);
        break;
    case 12:
        em_dmg12_005F4C40(em, w);
        break;
    case 13:
        em_dmg13_005F4D70(em, w);
        break;
    case 14:
        em_dmg14_005F4EA0(em, w);
        break;
    case 15:
        em_dmg15_005F4FC0(em, w);
        break;
    case 16:
        em_dmg16_005F5090(em, w);
        break;
    case 17:
        em_dmg17_005F5160(em, w);
        break;
    case 18:
        em_dmg18_005F5290(em, w);
        break;
    case 19:
        em_dmg19_005F5390(em, w);
        break;
    case 20:
        em_dmg20_005F5490(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move05_005F6810(EMW *em, EM20W *w) {
    u8 temp_a2;

    em->x40E = 5;
    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_die00_005F5990(em, w);
        break;
    case 1:
        em_die01_005F5B30(em, w);
        break;
    case 2:
        em_die02_005F5CB0(em, w);
        break;
    }
}

static void em_move06_005F6880(EMW *em, EM20W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_demo00_005F55D0(em, w);
        break;
    case 1:
        em_demo00_005F55D0(em, w);
        break;
    case 2:
        em_demo00_005F55D0(em, w);
        break;
    case 3:
        em_demo00_005F55D0(em, w);
        break;
    case 4:
        em_demo04_005F5870(em, w);
        break;
    }
}

void em20_main(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
    u8 sp3C;
    s16 temp_a2;
    s16 temp_t0;
    s16 temp_v0;
    u32 temp_v0_2;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_v1;

    em_mode_timer_sub(em);
    em_no_floor_ck(em);
    if (em->mode != 5) {
        if (em->x8B6 != 0) {
            w->x08 ^= 1;
        } else {
            w->x08 = 0;
        }
    } else {
        w->x08 = 0;
    }
    if ((em->x8C2 != 1) && (em->x8B6 == 0)) {
        if (game_w.x2E == 4) {
            if (em->kind == 0x14) {
                em_egg_ck(em);
            }
            em_thirst_ck(em);
        }
        if (game_w.x2E == 2) {
            em_hinshi_ck(em, 0.200000003f);
            em_thirst_ck(em);
            em_hungry_ck(em);
        }
    }
    temp_v0 = w->x06;
    if (temp_v0 != 0) {
        w->x06 = (s16) (temp_v0 - 1);
    }
    temp_v0_2 = Em_Dmg_Sys(em, &sp3C) & 0xFF;
    switch (temp_v0_2) {                            /* switch 1 */
    case 1:                                         /* switch 1 */
    case 2:                                         /* switch 1 */
        if (em->x388 == 2) {
            em20_act_set(em, 5, 2, 2);
        } else if (em->x9EA != 0) {
            em20_act_set(em, 5, 1, 2);
        } else {
            em20_act_set(em, 5, 0, 2);
        }
        break;
    case 3:                                         /* switch 1 */
    case 4:                                         /* switch 1 */
        if (em->x9EA == 0) {
            if (sp3C == 0) {
                em->x95A = 0x10;
            } else if (em->x8B6 == 0) {
                em->x95A = 0xA;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em20_act_set(em, 4, 0xC, 2);
        }
        break;
    case 16:                                        /* switch 1 */
        em20_act_set(em, 4, 0, 2);
        break;
    case 15:                                        /* switch 1 */
        if (((em->mode != 4) || (em->x15 != 0xF)) && (em->x9EA == 0)) {
            if (em->x388 == 2) {
                em_ikari_add(em, em->x8B0);
                em20_act_set(em, 4, 8, 2);
            } else if ((em->x8B6 == 0) && (em->kind != 0x14)) {
                em_ikari_add(em, em->x8B0);
                em20_act_set(em, 4, 0xF, 2);
            }
        }
        break;
    case 5:                                         /* switch 1 */
        if ((em->mode != 4) || (em->x15 != 8)) {
            em20_act_set(em, 4, 8, 2);
        }
        break;
    case 6:                                         /* switch 1 */
        if (em->x9EA != 0) {
            em_mahi_dmg_timer_set(em);
            em20_act_set(em, 4, 0xE, 2);
        } else {
            temp_a0 = em->mode;
            if ((temp_a0 != 4) || (em->x15 != 0xB)) {
                if (temp_a0 == 4) {
                    if (em->x15 != 8) {
                        goto block_56;
                    }
                } else {
block_56:
                    em_mahi_dmg_timer_set(em);
                    em20_act_set(em, 4, 0xB, 2);
                }
            }
        }
        break;
    case 7:                                         /* switch 1 */
        if (em->x9EA != 0) {
            em_sleep2_dmg_timer_set(em);
            if ((em_hokaku_ck(em, 0.300000012f) & 0xFF) == 1) {
                em20_act_set(em, 6, 4, 4);
            } else {
                em20_act_set(em, 0, 0x1F, 2);
            }
        } else {
            temp_a0_2 = em->mode;
            if (temp_a0_2 == 0) {
                if (em->x15 != 0x1B) {
                    goto block_66;
                }
            } else {
block_66:
                if (temp_a0_2 == 4) {
                    if (em->x15 != 8) {
                        goto block_69;
                    }
                } else {
block_69:
                    em_sleep2_dmg_timer_set(em);
                    em20_act_set(em, 0, 0x1B, 2);
                }
            }
        }
        break;
    case 8:                                         /* switch 1 */
        if (em->x9EA != 0) {
            em_sleep_dmg_timer_set(em);
            em20_act_set(em, 0, 0x1D, 2);
        } else {
            temp_a0_3 = em->mode;
            if (temp_a0_3 == 0) {
                if (em->x15 != 0x14) {
                    goto block_76;
                }
            } else {
block_76:
                if (temp_a0_3 == 4) {
                    if (em->x15 != 8) {
                        goto block_79;
                    }
                } else {
block_79:
                    em_sleep_dmg_timer_set(em);
                    em20_act_set(em, 0, 0x14, 2);
                }
            }
        }
        break;
    case 10:                                        /* switch 1 */
        temp_v1 = em->x15;
        switch (temp_v1) {                          /* switch 2; irregular */
        case 18:                                    /* switch 2 */
            em20_act_set(em, 0, 0x17, 2);
            em->x839 = 0;
            em_ikari_add(em, em->x8B0);
            break;
        case 21:                                    /* switch 2 */
        case 20:                                    /* switch 2 */
            em20_act_set(em, 0, 0x18, 2);
block_132:
            em->x839 = 0;
            break;
        case 27:                                    /* switch 2 */
            em20_act_set(em, 0, 0x1C, 2);
            goto block_132;
        case 29:                                    /* switch 2 */
            em20_act_set(em, 4, 0x12, 2);
            goto block_132;
        case 31:                                    /* switch 2 */
            em20_act_set(em, 4, 0x13, 2);
            goto block_132;
        }
        break;
    case 11:                                        /* switch 1 */
        em20_act_set(em, 4, 4, 2);
        break;
    case 12:                                        /* switch 1 */
        pl_flag_clr((PLW *) em, 0x20000);
        if (em->x388 == 2) {
            em20_act_set(em, 4, 8, 2);
        } else if (em->kind == 0x14) {
            temp_t0 = em->x792;
            temp_a2 = em->x302;
            if ((((temp_t0 * 0x1E) / 100) + ((u32) (temp_t0 * 0x1E) >> 0x1F)) >= temp_a2) {
                if (((s32) em->x39A % 100) < 0x1E) {
                    em20_act_set(em, 4, 0x10, 2);
                } else {
                    goto block_111;
                }
            } else if (((((temp_t0 * 0x32) / 100) + ((u32) (temp_t0 * 0x32) >> 0x1F)) >= temp_a2) && (((s32) em->x39A % 100) < 0x14)) {
                em20_act_set(em, 4, 0x10, 2);
            } else {
                goto block_111;
            }
        } else {
block_111:
            temp_a0_4 = (u8) em->x38E;
            switch (temp_a0_4) {                    /* switch 3 */
            case 0:                                 /* switch 3 */
            case 7:                                 /* switch 3 */
                em20_act_set(em, 4, 0, 2);
                break;
            case 6:                                 /* switch 3 */
                if ((w->x1B != 0) && ((s32) em->hagi[6].cnt >= 2)) {
                    em20_act_set(em, 4, 0x14, 2);
                } else {
                case 5:                             /* switch 3 */
                    em20_act_set(em, 4, 2, 2);
                }
                break;
            case 1:                                 /* switch 3 */
            case 2:                                 /* switch 3 */
                em20_act_set(em, 4, 3, 2);
                break;
            default:                                /* switch 3 */
                if ((s32) M2C_FIELD((((temp_a0_4 & 0xFF) * 8) + em), u8 *, 0x30A) >= 2) {
                    if (temp_a0_4 != 3) {
                        em20_act_set(em, 4, 5, 2);
                    } else {
                        em20_act_set(em, 4, 6, 2);
                    }
                } else {
                    em20_act_set(em, 4, 1, 2);
                }
                break;
            }
        }
        break;
    case 13:                                        /* switch 1 */
        if (em->x388 != 2) {
            em20_act_set(em, 4, 0, 2);
            goto block_132;
        }
        break;
    }
    if ((quest_w.no == 0x94) && (em->stg == 0x2E) && (Event_flag_ck(0x10) == 0)) {
        if ((*(u8 *)0x3F360F == 1) && (em->mode != 6)) {
            em20_act_set(em, 6, 0, 1);
        }
    } else if (em->x734 != 3) {

    } else if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em20_main_sub(em, w);
    if (em->x6FF != 0) {
        em20_main_sub(em, w);
        em->x6FF = 0;
    }
    if ((em->mode == 2) && ((s8) w->x18 == 1)) {
        em->x8BB = 2;
    }
}

void em20_main_sub(EMW *em, EM20W *w) {
    u8 temp_v1;

    em->mode_old = em->mode;
    em->x15_old = em->x15;
    temp_v1 = em->mode;
    switch (temp_v1) {
    case 0:
        em_move00_005F5F20(em, w);
        break;
    case 1:
        em_move01_005F6180(em, w);
        break;
    case 2:
        em_move02_005F6270(em, w);
        break;
    case 3:
        em_move03_005F6440(em, w);
        break;
    case 4:
        em_move04_005F6680(em, w);
        break;
    case 5:
        em_move05_005F6810(em, w);
        break;
    case 6:
        em_move06_005F6880(em, w);
        break;
    case 7:
        em_move06_005F6880(em, w);
        break;
    }
    if ((em->pos[0] <= 0.0f) || (em->pos[2] <= 0.0f)) {
        em_dur_set(em, 0);
    }
}

void em20_uvmove(EMW *em) {
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

static void sound_call_sub_005F75F0(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_005F7660(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_005F75F0(em, se, joint);
    }
}

static void sound_call_parts_005F76C0(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

static void quake_call_005F7760(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

static void move_default_005F77B0(EMW *em) {
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

static void ef_move_sub_005F7800(EMW *em, EM20W *w) {
    f32 v[3];
    f32 v2[3];
    f32 sp30[3];
    s16 temp_v1;
    u16 temp_a0;

    temp_a0 = em->char0;
    if (temp_a0 != w->anim) {
        w->anim = (s16) temp_a0;
    }
    temp_v1 = w->anim;
    switch (temp_v1) {
    case 0x3E9:
        break;
    case 0x3EB:
        sound_call_005F7660(em, 0x34, 1, 0x14);
        sound_call_005F7660(em, 0x74, 1, 0x1A);
        quake_call_005F7760(em, 0x34, 1);
        quake_call_005F7760(em, 0x74, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 2);
        }
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell17_set(em, 3);
        }
        if (em_frame_check(em, 0, 120.0f) != 0) {
            shell17_set(em, 4);
        }
        break;
    case 0x3EC:
        sound_call_005F7660(em, 0x28, 6, 0x14);
        sound_call_005F7660(em, 0x4C, 6, 0x1A);
        if (em_frame_check(em, 0, 38.0f) != 0) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            Eft20_set(1.0f, em, 2, 0);
            return;
        }
        break;
    case 0x3ED:
        sound_call_005F7660(em, 0x19, 1, 0x14);
        sound_call_005F7660(em, 0x38, 1, 0x1A);
        quake_call_005F7760(em, 0x3C, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 5);
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell17_set(em, 6);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(*(u16 *)0x3F340E & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x3EE:
        sound_call_005F7660(em, 0x19, 1, 0x1A);
        sound_call_005F7660(em, 0x38, 1, 0x14);
        quake_call_005F7760(em, 0x3C, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell17_set(em, 7);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 8);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 0);
            return;
        }
        break;
    case 0x3EF:
        sound_call_005F7660(em, 6, 0x20, 0x23);
        sound_call_005F7660(em, 0xE, 1, 0x1A);
        sound_call_005F7660(em, 0x26, 1, 0x14);
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell17_set(em, 9);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 0xA);
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
        sound_call_005F7660(em, 6, 0x20, 0x23);
        sound_call_005F7660(em, 0xE, 1, 0x14);
        sound_call_005F7660(em, 0x26, 1, 0x1A);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 0xB);
        }
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell17_set(em, 0xC);
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
        sound_call_005F7660(em, 6, 0x31, 0x23);
        sound_call_005F7660(em, 6, 4, 0x1A);
        sound_call_005F7660(em, 0x4C, 0, 0x14);
        sound_call_005F7660(em, 0x1C, 0x12, 0x14);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            shell17_set(em, 0x19);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 0x1A);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            shell17_set(em, 0x1A);
            return;
        }
        break;
    case 0x3F3:
        sound_call_005F7660(em, 0xE, 0xB, 6);
        sound_call_005F7660(em, 0x12, 0xB, 0xC);
        sound_call_005F7660(em, 0x46, 0xB, 6);
        sound_call_005F7660(em, 0x42, 0xB, 0xC);
        sound_call_005F7660(em, 0x7A, 0xB, 6);
        sound_call_005F7660(em, 0x7E, 0xB, 0xC);
        if ((em_frame_check(em, 0, 52.0f) == 0) && (em_frame_check(em, 0, 104.0f) == 0)) {
            if (em_frame_check(em, 0, 162.0f) != 0) {
                goto block_70;
            }
        } else {
block_70:
            Eft13_set_em_scl(em, 2, 5.0f, 7);
            return;
        }
        break;
    case 0x3F4:
        sound_call_005F7660(em, 6, 0xC, 6);
        sound_call_005F7660(em, 4, 0xC, 0xC);
        sound_call_005F7660(em, 0x40, 0xC, 6);
        sound_call_005F7660(em, 0x3C, 0xC, 0xC);
        sound_call_005F7660(em, 4, 0x1A, 0x22);
        sound_call_005F7660(em, 6, 0x19, 0xC);
        sound_call_005F7660(em, 0xA, 0x19, 6);
        sound_call_005F7660(em, 0x24, 0x1A, 0x22);
        sound_call_005F7660(em, 0x26, 0x19, 0xC);
        sound_call_005F7660(em, 0x28, 0x19, 6);
        sound_call_005F7660(em, 0x46, 0x1A, 0x22);
        sound_call_005F7660(em, 0x48, 0x19, 0xC);
        sound_call_005F7660(em, 0x4C, 0x19, 6);
        break;
    case 0x3F5:
        sound_call_005F7660(em, 0xE, 0xF, 6);
        sound_call_005F7660(em, 0x12, 0xF, 0xC);
        sound_call_005F7660(em, 0x2C, 0xF, 6);
        sound_call_005F7660(em, 0x28, 0xF, 0xC);
        sound_call_005F7660(em, 0x42, 0xF, 6);
        sound_call_005F7660(em, 0x46, 0xF, 0xC);
        if ((em_frame_check(em, 0, 26.0f) != 0) || (em_frame_check(em, 0, 52.0f) != 0)) {
            Eft13_set_em_scl(em, 1, 5.0f, 7);
            return;
        }
        break;
    case 0x3F6:
    case 0x43D:
    case 0x43E:
    case 0x440:
    case 0x441:
        sound_call_005F7660(em, 0xA, 0x1A, 0x22);
        sound_call_005F7660(em, 0xE, 0x19, 0xC);
        sound_call_005F7660(em, 0x12, 0x19, 6);
        sound_call_005F7660(em, 0x2A, 0x19, 0x22);
        sound_call_005F7660(em, 0x2E, 0x19, 0xC);
        sound_call_005F7660(em, 0x32, 0x19, 6);
        if ((em->x8B6 != 0) && (((s32) GAME_X1E16 % 10) == 0)) {
            Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 1) + 2));
            return;
        }
        break;
    case 0x3F7:
        sound_call_005F7660(em, 4, 0xC, 6);
        sound_call_005F7660(em, 8, 0xC, 0xC);
        sound_call_005F7660(em, 0x3A, 0xB, 6);
        sound_call_005F7660(em, 0x3E, 0xB, 0xC);
        sound_call_005F7660(em, 0x8E, 0xB, 6);
        sound_call_005F7660(em, 0x8A, 0xB, 0xC);
        sound_call_005F7660(em, 0xD4, 0xB, 6);
        sound_call_005F7660(em, 0xD8, 0xB, 0xC);
        if ((em_frame_check(em, 0, 8.0f) == 0) && (em_frame_check(em, 0, 86.0f) == 0)) {
            if (em_frame_check(em, 0, 160.0f) != 0) {
                goto block_84;
            }
        } else {
block_84:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 70.0f, 84.0f) == 0) && (em_frame_check3(em, 0, 146.0f, 156.0f) == 0)) {
                if (em_frame_check3(em, 0, 220.0f, 234.0f) != 0) {
                    goto block_91;
                }
            } else {
block_91:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3F9:
        sound_call_005F7660(em, 0x1C, 6, 0x14);
        sound_call_005F7660(em, 0x3E, 6, 0x1A);
        sound_call_005F7660(em, 0x5E, 6, 0x14);
        sound_call_005F7660(em, 0x2E, 0x29, 0x23);
        sound_call_005F7660(em, 0x52, 0x29, 0x23);
        sound_call_005F7660(em, 0x2E, 0x18, 0x23);
        sound_call_005F7660(em, 0x52, 0x18, 0x23);
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell17_set(em, 0x13);
        }
        if (em_frame_check(em, 0, 48.0f) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, 546, 11469);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, 546, 11469);
            }
        }
        if (em_frame_check(em, 0, 86.0f) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, 1092, 0xD112);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, 1092, 0xD112);
            }
        }
        if ((em_frame_check(em, 0, 12.0f) != 0) || (em_frame_check(em, 0, 72.0f) != 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if (em_frame_check(em, 0, 40.0f) != 0) {
            Eft20_set(1.0f, em, 3, 0);
            return;
        }
        break;
    case 0x3FA:
        sound_call_005F7660(em, 0x1A, 0xD, 6);
        sound_call_005F7660(em, 0x1E, 0xD, 0xC);
        sound_call_005F7660(em, 0x52, 0xB, 6);
        sound_call_005F7660(em, 0x56, 0xB, 0xC);
        sound_call_005F7660(em, 0x88, 0xB, 6);
        sound_call_005F7660(em, 0x8C, 0xB, 0xC);
        sound_call_005F7660(em, 0xB6, 0xB, 6);
        sound_call_005F7660(em, 0xBA, 0xB, 0xC);
        sound_call_005F7660(em, 0x2A, 7, 0x1A);
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
        sound_call_005F7660(em, 2, 0xB, 6);
        sound_call_005F7660(em, 6, 0xB, 0xC);
        sound_call_005F7660(em, 0xC, 4, 0x1A);
        sound_call_005F7660(em, 4, 1, 0x14);
        sound_call_005F7660(em, 0xE, 9, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 0xD);
        }
        if (em_frame_check(em, 0, 16.0f) != 0) {
            ground_land_eff_set_005FC860(em);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 18.0f, 44.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x405:
        sound_call_005F7660(em, 0x1E, 0x13, 0);
        sound_call_005F7660(em, 2, 0x20, 0x23);
        sound_call_005F7660(em, 4, 0xE, 0xC);
        sound_call_005F7660(em, 8, 0xE, 6);
        break;
    case 0x407:
        sound_call_005F7660(em, 2, 0x2D, 0x23);
        sound_call_005F7660(em, 0xB6, 0x2D, 0x23);
        sound_call_005F7660(em, 0x16C, 0x2D, 0x23);
        v[1] = 10.0f;
        v[2] = 140.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v, 1.60000002f);
        break;
    case 0x408:
        sound_call_005F7660(em, 4, 0x2F, 0x23);
        sound_call_005F7660(em, 0x46, 0, 0x1A);
        sound_call_005F7660(em, 0x8E, 3, 0x14);
        sound_call_005F7660(em, 4, 0x17, 0);
        sound_call_005F7660(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005F7660(em, 4, 0x2F, 0x23);
        sound_call_005F7660(em, 0x46, 0x2D, 0x23);
        sound_call_005F7660(em, 0x7E, 3, 0x1A);
        sound_call_005F7660(em, 0xB6, 3, 0x14);
        sound_call_005F7660(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_005F7660(em, 0x24, 0x2F, 0x23);
        v2[1] = 10.0f;
        v2[2] = 140.0f;
        v2[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.60000002f);
        break;
    case 0x40B:
        sound_call_005F7660(em, 0x32, 2, 0x14);
        sound_call_005F7660(em, 0x3A, 6, 0x1A);
        sound_call_005F7660(em, 0x54, 1, 0x1A);
        sound_call_005F7660(em, 0x68, 4, 0x14);
        sound_call_005F7660(em, 0x7C, 0x11, 0x1A);
        sound_call_005F7660(em, 0xCC, 6, 0x1A);
        sound_call_005F7660(em, 0xD2, 2, 0x1A);
        sound_call_005F7660(em, 0xFE, 0x12, 0x14);
        sound_call_005F7660(em, 0x120, 2, 0x1A);
        sound_call_005F7660(em, 0x20, 0xF, 0xC);
        sound_call_005F7660(em, 0x24, 0xF, 6);
        sound_call_005F7660(em, 0x80, 0xF, 0xC);
        sound_call_005F7660(em, 0x84, 0xF, 6);
        sound_call_005F7660(em, 0x122, 0xE, 0xC);
        sound_call_005F7660(em, 0x126, 0xE, 6);
        sound_call_005F7660(em, 2, 0x15, 0);
        sound_call_005F7660(em, 2, 0x23, 0x23);
        sound_call_005F7660(em, 0x30, 0x21, 0x23);
        sound_call_005F7660(em, 0x80, 0x22, 0x23);
        sound_call_005F7660(em, 0xB8, 0x24, 0x23);
        sound_call_005F7660(em, 0x120, 0x22, 0x23);
        break;
    case 0x40C:
        sound_call_005F7660(em, 0x24, 0x24, 0x23);
        sound_call_005F7660(em, 0x14, 0, 0x1A);
        sound_call_005F7660(em, 0x28, 0, 0x14);
        sound_call_005F7660(em, 0x3C, 0x15, 0x22);
        sound_call_005F7660(em, 0x3C, 0xC, 0xC);
        sound_call_005F7660(em, 0xC, 0xC, 6);
        sound_call_005F7660(em, 0xA6, 0, 0x1A);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell17_set(em, 1);
        }
        if (em_frame_check(em, 0, 46.0f) != 0) {
            shell17_set(em, 0x21);
            return;
        }
        break;
    case 0x40D:
        sound_call_005F7660(em, 0x1E, 0x20, 0x23);
        sound_call_005F7660(em, 0xC, 0xC, 0x1A);
        sound_call_005F7660(em, 0x1C, 2, 0x1A);
        sound_call_005F7660(em, 0x2E, 3, 0x14);
        sound_call_005F7660(em, 0x48, 1, 0x1A);
        break;
    case 0x40E:
        sound_call_005F7660(em, 4, 0x2F, 0x23);
        sound_call_005F7660(em, 0x48, 0x16, 0);
        sound_call_005F7660(em, 0xA4, 9, 0);
        sound_call_005F7660(em, 0x90, 3, 0xC);
        sound_call_005F7660(em, 0x88, 0xE, 6);
        sound_call_005F7660(em, 0xB2, 4, 0x22);
        if (em_frame_check(em, 0, 148.0f) != 0) {
            shell17_set(em, 0xF);
        }
        if (em_frame_check(em, 0, 156.0f) != 0) {
            Eft20_set(1.0f, em, 0, 0);
            return;
        }
        break;
    case 0x40F:
        sound_call_005F7660(em, 2, 0x2F, 0x23);
        sound_call_005F7660(em, 2, 0x17, 0);
        sound_call_005F7660(em, 0x40, 3, 0x1A);
        sound_call_005F7660(em, 0x66, 0x10, 0x1A);
        break;
    case 0x410:
        sound_call_005F7660(em, 0x2A, 0xD, 6);
        sound_call_005F7660(em, 0x2E, 0xD, 0xC);
        sound_call_005F7660(em, 0x66, 0xD, 6);
        sound_call_005F7660(em, 0x6A, 0xD, 0xC);
        sound_call_005F7660(em, 0x88, 0xA, 6);
        sound_call_005F7660(em, 0x8C, 0xA, 0xC);
        sound_call_005F7660(em, 0x8C, 0x27, 0x23);
        sound_call_005F7660(em, 2, 0x13, 0);
        sound_call_005F7660(em, 0x34, 4, 0x1A);
        sound_call_005F7660(em, 0x3A, 5, 0x1A);
        sound_call_005F7660(em, 0x70, 0x1A, 0x22);
        sound_call_005F7660(em, 0x74, 0x1B, 0xC);
        sound_call_005F7660(em, 0x78, 0x1B, 6);
        sound_call_005F7660(em, 0x8E, 0x1A, 0x22);
        sound_call_005F7660(em, 0x94, 0x1B, 0xC);
        sound_call_005F7660(em, 0x96, 0x1B, 6);
        sound_call_005F7660(em, 0xB0, 0x1A, 0x22);
        sound_call_005F7660(em, 0xB4, 0x1B, 0xC);
        sound_call_005F7660(em, 0xB8, 0x1B, 6);
        sound_call_005F7660(em, 0xB0, 0x1A, 0x22);
        sound_call_005F7660(em, 0xD6, 0x1B, 0xC);
        sound_call_005F7660(em, 0xDA, 0x1B, 6);
        if (em_frame_check(em, 0, 60.0f) != 0) {
            shell17_set(em, 0x1C);
            return;
        }
        break;
    case 0x411:
        sound_call_005F7660(em, 0xC, 0x27, 0x23);
        sound_call_005F7660(em, 2, 0xF, 6);
        sound_call_005F7660(em, 6, 0xF, 0xC);
        sound_call_005F7660(em, 0x16, 0xE, 6);
        sound_call_005F7660(em, 0x1A, 0xE, 0xC);
        sound_call_005F7660(em, 0x34, 0xA, 6);
        sound_call_005F7660(em, 0x38, 0xA, 0xC);
        sound_call_005F7660(em, 0x5C, 0xA, 6);
        sound_call_005F7660(em, 0x60, 0xA, 0xC);
        sound_call_005F7660(em, 0x12, 0x11, 0x1A);
        sound_call_005F7660(em, 0x16, 0x10, 0x14);
        sound_call_005F7660(em, 0x30, 6, 0x1A);
        sound_call_005F7660(em, 0x34, 8, 0x14);
        sound_call_005F7660(em, 0x5C, 6, 0x1A);
        sound_call_005F7660(em, 0x70, 6, 0x14);
        sound_call_005F7660(em, 0x8E, 6, 0x1A);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 0x1D);
        }
        if ((em_frame_check2(em, 0, 20.0f) != 0) && (em_frame_check2(em, 0, 50.0f) == 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if (em_frame_check(em, 0, 102.0f) != 0) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if ((em_frame_check2(em, 0, 22.0f) != 0) && (em_frame_check2(em, 0, 38.0f) == 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em_frame_check(em, 0, 62.0f) != 0) {
            Eft20_set(1.0f, em, 3, 0);
            return;
        }
        break;
    case 0x413:
        sound_call_005F7660(em, 8, 0x23, 0x23);
        sound_call_005F7660(em, 0xE, 0xF, 6);
        sound_call_005F7660(em, 0x1E, 0x14, 0x2A);
        sound_call_005F7660(em, 0x2A, 0, 0x1A);
        sound_call_005F7660(em, 0x48, 1, 0x14);
        quake_call_005F7760(em, 0x2C, 1);
        quake_call_005F7760(em, 0x49, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell17_set(em, 0x11);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x414:
        sound_call_005F7660(em, 0x1C, 0xB, 6);
        sound_call_005F7660(em, 0x20, 0xB, 0xC);
        sound_call_005F7660(em, 6, 0x1A, 0x22);
        sound_call_005F7660(em, 0xA, 0x1B, 0xC);
        sound_call_005F7660(em, 0xE, 0x1B, 6);
        sound_call_005F7660(em, 0x26, 0x1A, 0x22);
        sound_call_005F7660(em, 0x2A, 0x1B, 0xC);
        sound_call_005F7660(em, 0x32, 0x1B, 6);
        break;
    case 0x415:
        sound_call_005F7660(em, 0x16, 0x27, 0x23);
        sound_call_005F7660(em, 4, 0xD, 6);
        sound_call_005F7660(em, 8, 0xD, 0xC);
        sound_call_005F7660(em, 0x32, 0xD, 6);
        sound_call_005F7660(em, 0x36, 0xD, 0xC);
        sound_call_005F7660(em, 0x62, 0xB, 6);
        sound_call_005F7660(em, 0x66, 0xB, 0xC);
        sound_call_005F7660(em, 0x98, 0xB, 6);
        sound_call_005F7660(em, 0x9C, 0xB, 0xC);
        sound_call_005F7660(em, 4, 0x1C, 0x22);
        sound_call_005F7660(em, 4, 0x1D, 0xC);
        sound_call_005F7660(em, 8, 0x1D, 6);
        sound_call_005F7660(em, 0x22, 0x1E, 0x22);
        sound_call_005F7660(em, 0x22, 0x1D, 0xC);
        sound_call_005F7660(em, 0x26, 0x1D, 6);
        sound_call_005F7660(em, 0x40, 0x1E, 0x22);
        sound_call_005F7660(em, 0x40, 0x1D, 0xC);
        sound_call_005F7660(em, 0x44, 0x1D, 6);
        sound_call_005F7660(em, 0x5E, 0x1C, 0x22);
        sound_call_005F7660(em, 0x5E, 0x1B, 0xC);
        sound_call_005F7660(em, 0x62, 0x1B, 6);
        sound_call_005F7660(em, 0x7C, 0x1A, 0x22);
        sound_call_005F7660(em, 0x7C, 0x1D, 0xC);
        sound_call_005F7660(em, 0x80, 0x1D, 6);
        sound_call_005F7660(em, 0x9A, 0x1A, 0x22);
        sound_call_005F7660(em, 0x9A, 0x1D, 0xC);
        sound_call_005F7660(em, 0x9E, 0x1D, 6);
        if (em_frame_check(em, 0, 22.0f) != 0) {
            shell17_set(em, 0x14);
            shell17_set(em, 0x15);
        }
        if ((em_frame_check(em, 0, 20.0f) == 0) && (em_frame_check(em, 0, 68.0f) == 0)) {
            if (em_frame_check(em, 0, 114.0f) != 0) {
                goto block_183;
            }
        } else {
block_183:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 9, 0);
                Eft20_set(1.0f, em, 0xE, 0);
                return;
            }
        }
        break;
    case 0x417:
        sound_call_005F7660(em, 4, 0x16, 0);
        sound_call_005F7660(em, 0x10, 0x12, 0x14);
        sound_call_005F7660(em, 0x1A, 3, 0x14);
        sound_call_005F7660(em, 0x16, 0x27, 0x23);
        sound_call_005F7660(em, 0x4C, 0x12, 0x14);
        sound_call_005F7660(em, 0x16, 0x18, 0x23);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell17_set(em, 0x22);
            return;
        }
        break;
    case 0x418:
        sound_call_005F7660(em, 2, 0x22, 0x23);
        sound_call_005F7660(em, 0x22, 0x22, 0x23);
        sound_call_005F7660(em, 0x40, 0x22, 0x23);
        sound_call_005F7660(em, 0x72, 0x20, 0x23);
        sound_call_005F7660(em, 0x10, 1, 0x14);
        break;
    case 0x41B:
        if (em->kind == 0x14) {
            sound_call_005F7660(em, 4, 0x16, 0);
            sound_call_005F7660(em, 0x20, 0x20, 0x23);
            sound_call_005F7660(em, 0x58, 0x38, 0x22);
            sound_call_005F7660(em, 0x92, 0x38, 0x22);
            sound_call_005F7660(em, 0xC6, 0x38, 0x22);
            sound_call_005F7660(em, 0x44, Code_Make(0x32, 4, 0x33, 4), 0x23);
            sound_call_005F7660(em, 0xB6, Code_Make(0x34, 4, 0x33, 4), 0x23);
            sound_call_005F7660(em, 0x88, Code_Make(0x34, 4, 0x32, 4), 0x23);
            if (em->x8B6 != 0) {
                if ((em_frame_check3(em, 0, 86.0f, 92.0f) == 0) && (em_frame_check3(em, 0, 146.0f, 152.0f) == 0)) {
                    if (em_frame_check3(em, 0, 196.0f, 202.0f) != 0) {
                        goto block_196;
                    }
                } else {
block_196:
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                }
            }
            if ((M2C_FIELD(w, u8 *, 0x1B) != 0) && ((em_frame_check(em, 0, 88.0f) != 0) || (em_frame_check(em, 0, 146.0f) != 0) || (em_frame_check(em, 0, 198.0f) != 0))) {
                M2C_FIELD(w, s8 *, 8) = 1;
                return;
            }
        } else {
            sound_call_005F7660(em, 0x38, 0x12, 0x1A);
            sound_call_005F7660(em, 0x58, 6, 0x1A);
            sound_call_005F7660(em, 0x96, 0x28, 0x23);
            sound_call_005F7660(em, 0xFC, 0x27, 0x23);
            sound_call_005F7660(em, 0xEE, 0x16, 0);
            if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 72.0f, 80.0f) != 0) || (em_frame_check3(em, 0, 132.0f, 140.0f) != 0) || (em_frame_check3(em, 0, 184.0f, 192.0f) != 0))) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x41C:
        sound_call_005F7660(em, 0x16, 0x28, 0x23);
        sound_call_005F7660(em, 0x48, 0x27, 0x23);
        sound_call_005F7660(em, 6, 0x14, 0x22);
        sound_call_005F7660(em, 0x16, 0, 0x1A);
        sound_call_005F7660(em, 0x3A, 0xF, 6);
        sound_call_005F7660(em, 0x3E, 0xF, 0xC);
        sound_call_005F7660(em, 0x6A, 0xB, 6);
        sound_call_005F7660(em, 0x6E, 0xB, 0xC);
        sound_call_005F7660(em, 0xAC, 0xB, 6);
        sound_call_005F7660(em, 0xB0, 0xB, 0xC);
        if (em_frame_check(em, 0, 72.0f) != 0) {
            Eft20_set(1.0f, em, 4, 0);
        }
        if (em_frame_check(em, 0, 80.0f) != 0) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if (em_frame_check(em, 0, 84.0f) != 0) {
            Eft20_set(1.0f, em, 4, 1);
            return;
        }
        break;
    case 0x41E:
        sound_call_005F7660(em, 2, 0x12, 0);
        sound_call_005F7660(em, 0x1C, 4, 0x1A);
        sound_call_005F7660(em, 0x38, 4, 0x14);
        sound_call_005F7660(em, 0x1C, 5, 0x1A);
        sound_call_005F7660(em, 0x78, 4, 0x14);
        sound_call_005F7660(em, 0x108, 4, 0x1A);
        sound_call_005F7660(em, 4, 0x14, 0x23);
        sound_call_005F7660(em, 0x40, 0x28, 0x23);
        sound_call_005F7660(em, 0x84, 0x27, 0x23);
        sound_call_005F7660(em, 0x84, 0x29, 0x23);
        sound_call_005F7660(em, 0xA, 0xC, 0xC);
        sound_call_005F7660(em, 0x10, 0xC, 6);
        break;
    case 0x421:
        sound_call_005F7660(em, 0x4C, 0x20, 0x23);
        sound_call_005F7660(em, 0xC, 0x12, 0x14);
        sound_call_005F7660(em, 0x2E, 8, 0x14);
        sound_call_005F7660(em, 2, 0xD, 6);
        sound_call_005F7660(em, 4, 0xD, 0xC);
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft20_set(1.0f, em, 4, 0);
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            Eft20_set(1.0f, em, 5, 0);
        }
        if (em_frame_check(em, 0, 28.0f) != 0) {
            shell17_set(em, 0x1B);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell17_set(em, 0x1F);
        }
        if (em_frame_check(em, 0, 36.0f) != 0) {
            shell17_set(em, 0x20);
            return;
        }
        break;
    case 0x423:
        sound_call_005F7660(em, 0xC, 0x16, 0);
        sound_call_005F7660(em, 6, 0x22, 0x23);
        sound_call_005F7660(em, 0x68, 3, 0x1A);
        break;
    case 0x424:
        sound_call_005F7660(em, 6, 0x2B, 0x23);
        sound_call_005F7660(em, 0x48, 0x13, 0x2A);
        sound_call_005F7660(em, 0x30, 6, 0x1A);
        sound_call_005F7660(em, 0x30, 0x11, 0x14);
        sound_call_005F7660(em, 0x30, 3, 0x14);
        sound_call_005F7660(em, 0x74, 3, 0x14);
        sound_call_005F7660(em, 0x94, 3, 0x1A);
        sound_call_005F7660(em, 0xC4, 3, 0x1A);
        sound_call_005F7660(em, 0x34, 0xC, 6);
        sound_call_005F7660(em, 0x38, 0xD, 0xC);
        if (em_frame_check(em, 0, 44.0f) != 0) {
            Eft13_set_em_scl(em, 0x1B, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 62.0f) != 0) {
            Eft13_set_em_scl(em, 0x16, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 80.0f) != 0) {
            Eft20_set(0.5f, em, 1, 0x80);
            return;
        }
        break;
    case 0x425:
        sound_call_005F7660(em, 6, 0x2B, 0x23);
        sound_call_005F7660(em, 0xA, 0x12, 0x1A);
        sound_call_005F7660(em, 0x26, 6, 0x1A);
        sound_call_005F7660(em, 0x4E, 4, 0x14);
        sound_call_005F7660(em, 0xAC, 3, 0x1A);
        sound_call_005F7660(em, 0x6E, 3, 0x14);
        sound_call_005F7660(em, 0x5A, 0x15, 0);
        if (em_frame_check(em, 0, 24.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
            return;
        }
        break;
    case 0x426:
        sound_call_005F7660(em, 4, 0x27, 0x23);
        sound_call_005F7660(em, 0x56, 0x1F, 0x23);
        sound_call_005F7660(em, 0x10, 3, 0x14);
        sound_call_005F7660(em, 4, 0xD, 6);
        sound_call_005F7660(em, 8, 0xD, 0xC);
        sound_call_005F7660(em, 0x62, 0x13, 0x2A);
        sound_call_005F7660(em, 0x7E, 3, 0x1A);
        break;
    case 0x427:
        sound_call_005F7660(em, 4, 0x2A, 0x23);
        sound_call_005F7660(em, 4, 0x13, 0);
        sound_call_005F7660(em, 0x4C, 0x1F, 0x23);
        sound_call_005F7660(em, 0xE, 0, 0x1A);
        break;
    case 0x428:
        sound_call_005F7660(em, 4, 0x2A, 0x23);
        sound_call_005F7660(em, 4, 0x13, 0);
        sound_call_005F7660(em, 0x76, 0x20, 0x23);
        sound_call_005F7660(em, 0x7A, 0, 0x1A);
        break;
    case 0x429:
        sound_call_005F7660(em, 4, 0x2C, 0x23);
        sound_call_005F7660(em, 0xA8, 0x2A, 0x23);
        sound_call_005F7660(em, 4, 0x27, 0x2C);
        sound_call_005F7660(em, 0xFC, 0x20, 0x23);
        sound_call_005F7660(em, 0x130, 0x27, 0x23);
        sound_call_005F7660(em, 0x3E, 4, 0x2C);
        sound_call_005F7660(em, 0x12, 0x12, 0x14);
        sound_call_005F7660(em, 0x2C, 6, 0x1A);
        sound_call_005F7660(em, 0x3A, 0x10, 0x1A);
        sound_call_005F7660(em, 0xD2, 3, 0x1A);
        sound_call_005F7660(em, 0x142, 3, 0x1A);
        sound_call_005F7660(em, 0x3E, 9, 0);
        sound_call_005F7660(em, 0x4A, 9, 0);
        sound_call_005F7660(em, 0x64, 0x12, 0);
        sound_call_005F7660(em, 0xA2, 0x12, 0);
        sound_call_005F7660(em, 0xE, 0xC, 6);
        sound_call_005F7660(em, 0x12, 0xC, 0xC);
        sound_call_005F7660(em, 0x11E, 0xD, 6);
        sound_call_005F7660(em, 0x122, 0xD, 0xC);
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
        sound_call_005F7660(em, 4, 0x2A, 0x23);
        sound_call_005F7660(em, 4, 0x13, 0);
        break;
    case 0x42B:
        sound_call_005F7660(em, 0x46, 0x2D, 0x23);
        sound_call_005F7660(em, 0x46, 0x17, 0);
        break;
    case 0x42C:
        sound_call_005F7660(em, 0x1C, 2, 0x1A);
        sound_call_005F7660(em, 0x30, 4, 0x14);
        sound_call_005F7660(em, 0xDC, 0x10, 0x1A);
        sound_call_005F7660(em, 0x1E, 0x16, 0);
        sound_call_005F7660(em, 0x108, 9, 0);
        sound_call_005F7660(em, 0x108, 7, 0);
        sound_call_005F7660(em, 0x114, 0x17, 0);
        sound_call_005F7660(em, 0x180, 0x16, 0);
        sound_call_005F7660(em, 0x18, 0x2A, 0x23);
        sound_call_005F7660(em, 0x8C, 0x28, 0x23);
        sound_call_005F7660(em, 0xB4, 0x2C, 0x23);
        sound_call_005F7660(em, 0x13A, 0x2F, 0x23);
        sound_call_005F7660(em, 0x192, 0x2B, 0x23);
        sound_call_005F7660(em, 0x174, 0xF, 6);
        sound_call_005F7660(em, 0x176, 0xE, 0xC);
        if (em_frame_check(em, 0, 270.0f) != 0) {
            Eft20_set(0.899999976f, em, 0x12, 0x80);
            return;
        }
        break;
    case 0x42D:
        sound_call_005F7660(em, 4, 0x2A, 0);
        sound_call_005F7660(em, 4, 0x13, 0x2A);
        sound_call_005F7660(em, 0x18, 9, 0);
        sound_call_005F7660(em, 0x18, 0x11, 0);
        quake_call_005F7760(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x80);
            return;
        }
        break;
    case 0x42E:
    case 0x433:
        sound_call_005F7660(em, 8, 0x16, 0);
        sound_call_005F7660(em, 0x3A, 0x2A, 0x23);
        break;
    case 0x42F:
        sound_call_005F7660(em, 4, 0x16, 0x23);
        sound_call_005F7660(em, 0x1E, 0x20, 0x23);
        sound_call_005F7660(em, 0x30, 1, 0x14);
        sound_call_005F7660(em, 0x46, 1, 0x1A);
        break;
    case 0x430:
        sound_call_005F7660(em, 4, 0x16, 0x23);
        sound_call_005F7660(em, 0x1E, 0x20, 0x23);
        sound_call_005F7660(em, 0x30, 1, 0x1A);
        sound_call_005F7660(em, 0x46, 1, 0x14);
        break;
    case 0x432:
        sound_call_005F7660(em, 4, 0x2A, 0);
        sound_call_005F7660(em, 4, 0x13, 0x2A);
        sound_call_005F7660(em, 0x18, 9, 0);
        sound_call_005F7660(em, 0x18, 0x11, 0);
        quake_call_005F7760(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x81);
            return;
        }
        break;
    case 0x434:
        sound_call_005F7660(em, 0x10, 0x37, 0x23);
        sound_call_005F7660(em, 0x10, 1, 0x1A);
        em_mahi_eff_set(em, 2);
        break;
    case 0x435:
        sound_call_005F7660(em, 4, 0x2B, 0x23);
        sound_call_005F7660(em, 0x14, 0xB, 0xC);
        sound_call_005F7660(em, 0x3A, 0x14, 0);
        sound_call_005F7660(em, 0x58, 0x1C, 0x22);
        sound_call_005F7660(em, 0x58, 0x1B, 6);
        sound_call_005F7660(em, 0x5C, 0x1B, 0xC);
        break;
    case 0x436:
        sound_call_005F7660(em, 0xA, 9, 0);
        sound_call_005F7660(em, 0xA, 7, 0x2A);
        sound_call_005F7660(em, 0xC, 0x11, 0);
        sound_call_005F7660(em, 4, 0x2C, 0x23);
        if (em_frame_check(em, 0, 8.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0);
            return;
        }
        break;
    case 0x437:
        sound_call_005F7660(em, 0xC, 0x24, 0x23);
        sound_call_005F7660(em, 0x24, 0x22, 0x23);
        sound_call_005F7660(em, 0x66, 0x22, 0x23);
        sound_call_005F7660(em, 0xA4, 0x22, 0x23);
        if ((em_frame_check(em, 0, 48.0f) == 0) && (em_frame_check(em, 0, 106.0f) == 0)) {
            if (em_frame_check(em, 0, 174.0f) != 0) {
                goto block_275;
            }
        } else {
block_275:
            shell17_set(em, 0x10);
            return;
        }
        break;
    case 0x438:
        sound_call_005F7660(em, 2, 0x16, 0x22);
        sound_call_005F7660(em, 0x64, 0x17, 0x22);
        sound_call_005F7660(em, 0x26, 0x2D, 0x23);
        sound_call_005F7660(em, 0xC8, 0x20, 0x23);
        sound_call_005F7660(em, 0x1C, 3, 0x1A);
        sound_call_005F7660(em, 0x140, 3, 0x1A);
        break;
    case 0x439:
        sound_call_005F7660(em, 2, 0x16, 0x22);
        sound_call_005F7660(em, 0x64, 0x17, 0x22);
        sound_call_005F7660(em, 0x26, 0x2D, 0x23);
        sound_call_005F7660(em, 0xC8, 0x20, 0x23);
        sound_call_005F7660(em, 0x1C, 3, 0x14);
        sound_call_005F7660(em, 0x140, 3, 0x14);
        break;
    case 0x43A:
        sound_call_005F7660(em, 4, 0x16, 0);
        break;
    case 0x43C:
        sound_call_005F7660(em, 0x16, 0xD, 6);
        sound_call_005F7660(em, 0x1A, 0xD, 0xC);
        sound_call_005F7660(em, 0x34, 0xA, 6);
        sound_call_005F7660(em, 0x38, 0xE, 0xC);
        sound_call_005F7660(em, 0x4E, 0xE, 6);
        sound_call_005F7660(em, 0x4A, 0xA, 0xC);
        sound_call_005F7660(em, 0x58, 0xC, 6);
        sound_call_005F7660(em, 0x5C, 0xC, 0xC);
        sound_call_005F7660(em, 0x20, 5, 0x1A);
        sound_call_005F7660(em, 0x1C, 0, 0x14);
        if ((em_frame_check(em, 0, 42.0f) == 0) && (em_frame_check(em, 0, 60.0f) == 0)) {
            if (em_frame_check(em, 0, 80.0f) != 0) {
                goto block_285;
            }
        } else {
block_285:
            Eft20_set(1.0f, em, 1, 0);
            return;
        }
        break;
    case 0x43F:
        sound_call_005F7660(em, 4, 0x1A, 0x22);
        sound_call_005F7660(em, 8, 0x1B, 0xC);
        sound_call_005F7660(em, 4, 0x1B, 6);
        sound_call_005F7660(em, 0x22, 0x1C, 0x22);
        sound_call_005F7660(em, 0x40, 0x1C, 0x22);
        break;
    case 0x442:
        sound_call_005F7660(em, 0x20, 0xD, 6);
        sound_call_005F7660(em, 0x24, 0xD, 0xC);
        sound_call_005F7660(em, 0x4A, 0xD, 6);
        sound_call_005F7660(em, 0x4E, 0xD, 0xC);
        sound_call_005F7660(em, 0x7C, 0xB, 6);
        sound_call_005F7660(em, 0x80, 0xB, 0xC);
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
    case 0x443:
        sound_call_005F7660(em, 0x10, 6, 0x14);
        sound_call_005F7660(em, 0x38, 6, 0x1A);
        sound_call_005F7660(em, 0xB0, 3, 0x14);
        sound_call_005F7660(em, 0xD6, 3, 0x1A);
        sound_call_005F7660(em, 0x108, 1, 0x14);
        sound_call_005F7660(em, 0x130, 1, 0x1A);
        sound_call_005F7660(em, 4, 0x11, 0x23);
        sound_call_005F7660(em, 0x64, 9, 0);
        sound_call_005F7660(em, 0x5C, 0x10, 0);
        sound_call_005F7660(em, 0x8C, 0x11, 0);
        sound_call_005F7660(em, 0x10, 0xF, 6);
        sound_call_005F7660(em, 0x14, 0xF, 0xC);
        sound_call_005F7660(em, 0x114, 0xC, 6);
        sound_call_005F7660(em, 0x118, 0xC, 0xC);
        if (em_frame_check(em, 0, 34.0f) != 0) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em_frame_check(em, 0, 70.0f) != 0) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if ((em_frame_check2(em, 0, 96.0f) != 0) && (em_frame_check2(em, 0, 120.0f) == 0) && (((s32) GAME_X1E16 % 6) == 0)) {
            Eft20_set(1.0f, em, 0x12, 0);
        }
        if ((em->x8B6 != 0) && (((s32) GAME_X1E16 % 19) == 0)) {
            Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 1) + 2));
            return;
        }
        break;
    case 0x447:
        sound_call_005F7660(em, 4, 0x10, 0);
        sound_call_005F7660(em, 0xA, 0x12, 0);
        sound_call_005F7660(em, 0x14, 9, 0);
        sound_call_005F7660(em, 4, 0x27, 0x23);
        sound_call_005F7660(em, 4, 0x2A, 0x23);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0x80);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 4.0f, 60.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x448:
        sound_call_005F7660(em, 0x16, 0x12, 0);
        sound_call_005F7660(em, 0x48, 0x12, 0);
        sound_call_005F7660(em, 4, 0xC, 6);
        sound_call_005F7660(em, 8, 0xC, 0xC);
        sound_call_005F7660(em, 0x30, 0x13, 0);
        sound_call_005F7660(em, 4, 0x28, 0x23);
        sound_call_005F7660(em, 4, 0x16, 0);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 12.0f, 24.0f) != 0) || (em_frame_check3(em, 0, 46.0f, 90.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x449:
        sound_call_005F7660(em, 0x22, 0x16, 6);
        sound_call_005F7660(em, 0x26, 0x16, 0xC);
        sound_call_005F7660(em, 0x62, 4, 0x22);
        sound_call_005F7660(em, 0x6C, 3, 0x22);
        sound_call_005F7660(em, 4, 0x2A, 0x23);
        sound_call_005F7660(em, 4, 0x2B, 0x23);
        break;
    case 0x44A:
        sound_call_005F7660(em, 4, 0x16, 6);
        sound_call_005F7660(em, 8, 0x16, 0xC);
        sound_call_005F7660(em, 0x6C, 3, 0x22);
        sound_call_005F7660(em, 4, 0x28, 0x23);
        sound_call_005F7660(em, 0x46, 0x20, 0x23);
        break;
    case 0x44C:
        sound_call_005F7660(em, 4, 0x16, 0x23);
        sound_call_005F7660(em, 0x2E, 0x27, 0x23);
        sound_call_005F7660(em, 0x24, 0xD, 6);
        sound_call_005F7660(em, 0x28, 0xD, 0xC);
        sound_call_005F7660(em, 0x92, 0x34, 0x23);
        sound_call_005F7660(em, 0xB2, 0x33, 0x23);
        sound_call_005F7660(em, 0xD2, 0x34, 0x23);
        sound_call_005F7660(em, 0x26, 0x12, 0x14);
        sound_call_005F7660(em, 0x6C, 0, 0x14);
        sound_call_005F7660(em, 0x84, 0, 0x14);
        sound_call_005F7660(em, 0x8E, 6, 0x14);
        sound_call_005F7660(em, 0xBA, 6, 0x1A);
        sound_call_005F7660(em, 0xDC, 6, 0x14);
        sound_call_005F7660(em, 0xE0, 0, 0x1A);
        sound_call_005F7660(em, 0xFC, 0, 0x14);
        sound_call_005F7660(em, 0x78, 0x38, 0x22);
        sound_call_005F7660(em, 0x98, 0x38, 0x22);
        sound_call_005F7660(em, 0xBA, 0x38, 0x22);
        sound_call_005F7660(em, 0xDE, 0x38, 0x22);
        sound_call_005F7660(em, 0x120, 0x38, 0x22);
        if ((em_frame_check(em, 0, 118.0f) == 0) && (em_frame_check(em, 0, 164.0f) == 0)) {
            if (em_frame_check(em, 0, 204.0f) != 0) {
                goto block_325;
            }
        } else {
block_325:
            shell17_set(em, 0x23);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 54.0f, 250.0f) == 0) {
                if (em_frame_check3(em, 0, 282.0f, 296.0f) != 0) {
                    goto block_330;
                }
            } else {
block_330:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x44D:
        sound_call_005F7660(em, 4, 0x27, 0x23);
        sound_call_005F7660(em, 4, 0xD, 6);
        sound_call_005F7660(em, 8, 0xD, 0xC);
        sound_call_005F7660(em, 0x144, 0xD, 6);
        sound_call_005F7660(em, 0x24, 0xD, 0xC);
        sound_call_005F7660(em, 0x36, 6, 0x1A);
        sound_call_005F7660(em, 0x36, 7, 0x14);
        if (em_frame_check(em, 0, 52.0f) != 0) {
            Eft20_set(1.0f, em, 1, 0x80);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 10.0f, 68.0f) == 0) {
                if (em_frame_check3(em, 0, 84.0f, 96.0f) != 0) {
                    goto block_338;
                }
            } else {
block_338:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x44E:
        sound_call_005F7660(em, 4, 0x34, 0x23);
        sound_call_005F7660(em, 0x1C, 0x32, 0x23);
        sound_call_005F7660(em, 0x32, 0x33, 0x23);
        sound_call_005F7660(em, 0x46, 0x32, 0x23);
        sound_call_005F7660(em, 0x5A, 0x33, 0x23);
        sound_call_005F7660(em, 0x1C, 5, 0x1A);
        sound_call_005F7660(em, 0x1C, 0x12, 0x14);
        sound_call_005F7660(em, 0x40, 1, 0x1A);
        if ((em_frame_check(em, 0, 32.0f) != 0) || (em_frame_check(em, 0, 54.0f) != 0) || (em_frame_check(em, 0, 74.0f) != 0) || (em_frame_check(em, 0, 92.0f) != 0)) {
            shell17_set(em, 0x18);
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 10.0f, 20.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
            if ((em_frame_check(em, 0, 30.0f) != 0) || (em_frame_check(em, 0, 54.0f) != 0) || (em_frame_check(em, 0, 72.0f) != 0) || (em_frame_check(em, 0, 92.0f) != 0)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x450:
        sound_call_005F7660(em, 4, 0x36, 0x23);
        sound_call_005F7660(em, 0x4E, 0x32, 0x23);
        sound_call_005F7660(em, 0x68, 0x33, 0x23);
        sound_call_005F7660(em, 0x1C, 2, 0x14);
        sound_call_005F7660(em, 0x34, 6, 0x1A);
        sound_call_005F7660(em, 0x4C, 6, 0x14);
        sound_call_005F7660(em, 0x64, 6, 0x1A);
        sound_call_005F7660(em, 0x24, 0xD, 6);
        sound_call_005F7660(em, 0x28, 0xD, 0xC);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell17_set(em, 0x17);
        }
        if ((em_frame_check(em, 0, 18.0f) != 0) || (em_frame_check(em, 0, 60.0f) != 0)) {
            Eft13_set_em_scl(em, 0x1A, 4.0f, 3);
        }
        if ((em_frame_check(em, 0, 38.0f) != 0) || (em_frame_check(em, 0, 80.0f) != 0)) {
            Eft13_set_em_scl(em, 0x14, 4.0f, 3);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 54.0f, 112.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x451:
        sound_call_005F7660(em, 4, 0x16, 0);
        sound_call_005F7660(em, 0x1C, 6, 0x1A);
        sound_call_005F7660(em, 0x34, 6, 0x14);
        sound_call_005F7660(em, 0x48, 0x29, 0x23);
        sound_call_005F7660(em, 0x4A, 0x18, 0x23);
        sound_call_005F7660(em, 0x8C, 0x29, 0x23);
        sound_call_005F7660(em, 0x94, 0x18, 0x23);
        sound_call_005F7660(em, 0x102, 4, 0x14);
        sound_call_005F7660(em, 0x11A, 6, 0x14);
        sound_call_005F7660(em, 0xE8, 0x29, 0x23);
        sound_call_005F7660(em, 0xEE, 0x18, 0x23);
        sound_call_005F7660(em, 0x120, 0x29, 0x23);
        sound_call_005F7660(em, 0x126, 0x18, 0x23);
        sound_call_005F7660(em, 0x186, 1, 0x14);
        sound_call_005F7660(em, 0x178, 0x20, 0x23);
        sound_call_005F7660(em, 0x190, 0, 0x14);
        sound_call_005F7660(em, 0x1A4, 0, 0x1A);
        if (em_frame_check(em, 0, 78.0f) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, 0xF556, 0xEF4B);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, 0xF556, 0xEF4B);
            }
        }
        if (em_frame_check(em, 0, 150.0f) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, 0xF556, 6554);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, 0xF556, 6554);
            }
        }
        if (em_frame_check(em, 0, 240.0f) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, 0xF1C8, 0xDC73);
            } else {
                Shell08_set_ang(em, 0x22, 2, 0, 0xF1C8, 0xDC73);
            }
        }
        if (em_frame_check(em, 0, 296.0f) != 0) {
            if (em->kind == 6) {
                Shell08_set_ang(em, 0x22, 3, 0, 0xF940, 7282);
                return;
            }
            Shell08_set_ang(em, 0x22, 2, 0, 0xF940, 7282);
            return;
        }
        break;
    case 0x452:
        sound_call_005F7660(em, 4, 0x12, 0x14);
        sound_call_005F7660(em, 0x12, 0x24, 0x23);
        sound_call_005F7660(em, 0x32, 0x32, 0x23);
        sound_call_005F7660(em, 0x4A, 0x33, 0x23);
        sound_call_005F7660(em, 0x60, 0x32, 0x23);
        sound_call_005F7660(em, 0x74, 0x33, 0x23);
        sound_call_005F7660(em, 8, 6, 0x14);
        sound_call_005F7660(em, 0x30, 6, 0x1A);
        sound_call_005F7660(em, 0x5A, 1, 0x1A);
        sound_call_005F7660(em, 0x10, 0xD, 6);
        sound_call_005F7660(em, 0x14, 0xC, 0xC);
        if (em_frame_check(em, 0, 20.0f) != 0) {
            shell17_set(em, 0x12);
        }
        if ((em_frame_check(em, 0, 50.0f) != 0) || (em_frame_check(em, 0, 76.0f) != 0) || (em_frame_check(em, 0, 96.0f) != 0) || (em_frame_check(em, 0, 116.0f) != 0)) {
            shell17_set(em, 0x18);
        }
        if (em_frame_check(em, 0, 20.0f) != 0) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 24.0f, 42.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
            if ((em_frame_check(em, 0, 54.0f) != 0) || (em_frame_check(em, 0, 80.0f) != 0) || (em_frame_check(em, 0, 100.0f) != 0) || (em_frame_check(em, 0, 122.0f) != 0)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x453:
        sound_call_005F7660(em, 0x12, 0x36, 0x23);
        sound_call_005F7660(em, 0x8C, 0x24, 0x23);
        sound_call_005F7660(em, 0x26, 0x39, 0x22);
        sound_call_005F7660(em, 0x26, 0x3A, 0x22);
        sound_call_005F7660(em, 0, 0x16, 0);
        sound_call_005F7660(em, 0x16, 0xD, 6);
        sound_call_005F7660(em, 0x1A, 0xD, 0xC);
        sound_call_005F7660(em, 0x8C, 0xC, 6);
        sound_call_005F7660(em, 0x90, 0xC, 0xC);
        sound_call_005F7660(em, 0x22, 5, 0x1A);
        sound_call_005F7660(em, 0x22, 5, 0x14);
        sound_call_005F7660(em, 0x86, 1, 0x1A);
        sound_call_005F7660(em, 0xA6, 1, 0x14);
        sound_call_005F7660(em, 0xC2, 0, 0x1A);
        if (M2C_FIELD(w, u8 *, 0x1B) != 0) {
            M2C_FIELD(w, s8 *, 8) = 1;
            if (em_frame_check(em, 0, 40.0f) != 0) {
                flmatGetTrans(sp30, em->mdl->bone + 0x36B0);
                Eft14_set3(sp30, 5, 1.0f, (PLW *)em);
                shell17_set3(em, 0x16);
                return;
            }
        }
        break;
    case 0x454:
        sound_call_005F7660(em, 0, 0x16, 0);
        sound_call_005F7660(em, 0xE, 0x12, 0x1A);
        sound_call_005F7660(em, 0x12, 0x2C, 0x23);
        sound_call_005F7660(em, 0x19C, 0x20, 0x23);
        sound_call_005F7660(em, 0xC8, 0x37, 0x23);
        sound_call_005F7660(em, 0x130, 0x16, 0x22);
        break;
    case 0x455:
        sound_call_005F7660(em, 0x10, 0x36, 0x23);
        sound_call_005F7660(em, 0x36, 0x27, 0x23);
        sound_call_005F7660(em, 0xB6, 0x23, 0x23);
        sound_call_005F7660(em, 4, 0x17, 0);
        sound_call_005F7660(em, 4, 0x16, 0);
        sound_call_005F7660(em, 0x88, 0x16, 0);
        sound_call_005F7660(em, 0x20, 0x14, 0x23);
        sound_call_005F7660(em, 0xA2, 3, 0x14);
        sound_call_005F7660(em, 0xCC, 6, 0x1A);
        sound_call_005F7660(em, 0xDA, 6, 0x14);
        sound_call_005F7660(em, 0x44, 0xC, 0xC);
        sound_call_005F7660(em, 0x54, 0xD, 6);
        sound_call_005F7660(em, 0xAC, 0xD, 0xC);
        sound_call_005F7660(em, 0xB0, 0xD, 6);
        if (em_frame_check(em, 0, 20.0f) != 0) {
            shell17_set(em, 0xE);
        }
        if ((em_frame_check(em, 0, 20.0f) == 0) && (em_frame_check(em, 0, 92.0f) == 0)) {
            if (em_frame_check(em, 0, 152.0f) != 0) {
                goto block_416;
            }
        } else {
block_416:
            Eft13_set_em_scl(em, 2, 3.0f, 7);
            return;
        }
        break;
    default:
        move_default_005F77B0(em);
        break;
    }
}

void em20_effect_move(EMW *em) {
    EM20W *w = (EM20W *)em->ex;
    u8 temp_a2;

    temp_a2 = w->eff;
    switch (temp_a2) {                              /* irregular */
    case 0:
        w->eff = temp_a2 + 1;
        break;
    case 1:
        ef_move_sub_005F7800(em, w);
        break;
    }
    em20_uvmove(em);
}

static void ground_land_eff_set_005FC860(EMW *em) {
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

static void takeoff_eff_set_005FC910(EMW *em) {
    f32 sp20[3];
    f32 temp_f1;

    if (game_w.stage == 0) {
        get_joint_pos_em(em, 0, sp20);
        temp_f1 = em->x5AC;
        sp20[1] = temp_f1;
        if (temp_f1 <= 46.0f) {
            eft11_set(em, sp20, 1);
        }
    }
}

static void takeon_eff_set_005FC980(EMW *em) {
    f32 pos[3];
    f32 temp_f2;

    if (game_w.stage == 0 && GAME_X1E16 % 10 == 0) {
        get_joint_pos_em(em, 0, pos);
        temp_f2 = em->x5AC;
        if (pos[1] - temp_f2 < 400.0f && temp_f2 <= 46.0f) {
            eft11_set(em, pos, 1);
        }
    }
}

static void hover_eff_set2_005FCA20(EMW *em) {
    if (GAME_X1E16 % 5 == 0) {
        Eft20_set(1.0f, em, 0xA, 0);
    }
}

static s32 kyusyu_char_set_005FCA70(EMW *em) {
    f32 sp20[3];
    f32 temp_f0;
    s8 temp_v1;

    temp_v1 = em->x617;
    if (temp_v1 == -1) {
        return 0;
    }
    em_pl_pos_set(em, temp_v1 & 0xFF, sp20);
    temp_f0 = flvecCalcDistance(em->pos, sp20);
    if (em->char0 == 0x415) {
        return 1;
    }
    if (temp_f0 <= (4.5f + (30.0f * em->adj_z))) {
        em_char_set(em, 0x2D, 0xA, 0);
        return 1;
    }
    return 0;
}

void em20_atk_end_sel(EMW *em, EM20W *w) {
    if (em->x734 == 3) {
        em20_to_normal(em, 0, 0);
        return;
    }
    if (((s32) em->x39A % 10) == 0) {
        em20_act_set(em, 0, 1, 1);
        return;
    }
    em20_to_normal(em, 0, 0);
}

static void kyusyu_senkai_ret_005FCBA0(EMW *em) {
    em20_to_fly(em, 1);
}

void em20_material_sub(EMW *em, int type, u8 *tbl) {
    u8 *base = *(u8 **)((u8 *)em->mdl + 0x10);
    int i = 0;
    EM20W *w = (EM20W *)em->ex;
    s32 *p = (s32 *)(tbl + type * 0x8C);

    if (p[1] > 0) {
        s32 *num = &p[1];

        do {
            u8 *m = base + p[2] * 0x4C;

            *(f32 *)(m + 0x10) = em->x798;
            switch (type) {
            case 0:
                switch (i) {
                case 1:
                    *(s32 *)(m + 0x10) = 0;
                    break;
                case 3:
                    if (w->x1B == 0) {
                        *(s32 *)(m + 0x10) = 0;
                    } else {
                        *(f32 *)(m + 0x10) = 1.0f;
                    }
                    break;
                }
                break;
            case 2:
                switch (i) {
                case 5:
                    if (w->x1B != 0) {
                        *(s32 *)(m + 0x10) = 0;
                    }
                    break;
                case 6:
                    *(s32 *)(m + 0x10) = 0;
                    break;
                }
                break;
            case 3:
                switch (i) {
                case 0:
                    if (w->x1B == 0 || w->x08 == 0) {
                        *(s32 *)(m + 0x10) = 0;
                    } else {
                        *(f32 *)(m + 0x10) = 1.0f;
                    }
                    break;
                case 5:
                    if (em->x8B6 != 0 && em->mode != 5) {
                        *(f32 *)(m + 0x10) = 1.0f;
                    } else {
                        *(s32 *)(m + 0x10) = 0;
                    }
                    break;
                }
                break;
            }
            flSetRenderState((i + 0x3A) & 0xFF, (u32)m);
            i++;
            p++;
        } while (i < *num);
    }
}

void dummy_em_prog_005FCDB0(void) {

}


