/* em20_r21 - monster 20 AI 0x005F75F0-0x005FC7F4: sound_call_sub_005F75F0, sound_call_005F7660, sound_call_parts_005F76C0, quake_call_005F7760, move_default_005F77B0, ef_move_sub_005F7800. Whole file in em20_ai_nm.c. */
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
u16 *em_act_search2_005EC0A0(EMW *em, u16 *tbl);
void act_dist_select_005EC0E0(EMW *em);
void em20_to_normal(EMW *em, s16 a, s16 b);
void em20_dmg_to_normal(EMW *em, s16 a, s16 b);
void em20_to_fly(EMW *em, int flag);
void item_theft_005EC560(PLW *arg1);
void em20_frame_reset(EMW *em, int i);
void em_act00_005EC740(EMW *em, EM20W *w);
void em_act01_005EC820(EMW *em, EM20W *w);
void em_act02_005EC910(EMW *em, EM20W *w);
void em_act03_005EC9A0(EMW *em, EM20W *w);
void em_act04_005ECA20(EMW *em, EM20W *w);
void em_act05_005ECAC0(EMW *em, EM20W *w);
void em_act06_005ECBB0(EMW *em, EM20W *w);
void em_act07_005ECC80(EMW *em, EM20W *w);
void em_act08_005ECD20(EMW *em, EM20W *w);
void em_act09_005ECD30(EMW *em, EM20W *w);
void em_act10_005ECDE0(EMW *em, EM20W *w);
void em_act11_005ECEF0(EMW *em, EM20W *w);
void em_act12_005ECFC0(EMW *em, EM20W *w);
void em_act13_005ED040(EMW *em, EM20W *w);
void em_act14_005ED110(EMW *em, EM20W *w);
void em_act15_005ED4A0(EMW *em, EM20W *w);
void em_act16_005ED5D0(EMW *em, EM20W *w);
void em_act17_005ED700(EMW *em, EM20W *w);
void em_act18_005ED780(EMW *em, EM20W *w);
void em_act19_005ED940(EMW *em, EM20W *w);
void em_act40_005EDAC0(EMW *em, EM20W *w);
void em_act20_005EDB80(EMW *em, EM20W *w);
void em_act21_005EDC90(EMW *em, EM20W *w);
void em_act22_005EDE50(EMW *em, EM20W *w);
void em_act23_005EDED0(EMW *em, EM20W *w);
void em_act24_005EDFA0(EMW *em, EM20W *w);
void em_act25_005EE070(EMW *em, EM20W *w);
void em_act26_005EE0F0(EMW *em, EM20W *w);
void em_act27_005EE230(EMW *em, EM20W *w);
void em_act28_005EE340(EMW *em, EM20W *w);
void em_act29_005EE410(EMW *em, EM20W *w);
void em_act31_005EE550(EMW *em, EM20W *w);
void em_act33_005EE690(EMW *em, EM20W *w);
void em_act34(EMW *em, EM20W *w);
void em_mv00_005EE7A0(EMW *em, EM20W *w);
void em_mv01_005EE900(EMW *em, EM20W *w);
void em_mv02_005EE920(EMW *em, EM20W *w);
void em_mv03_005EEA80(EMW *em, EM20W *w);
void em_mv04_005EED50(EMW *em, EM20W *w);
void em_mv05_005EEEB0(EMW *em, EM20W *w);
void em_mv06_005EF180(EMW *em, EM20W *w);
void em_mv07_005EF2E0(EMW *em, EM20W *w);
void em_mv09_005EF420(EMW *em, EM20W *w);
void em_mv10_005EF430(EMW *em, EM20W *w);
void em_fly00_005EF5C0(EMW *em, EM20W *w);
void em_fly01_005EF6C0(EMW *em, EM20W *w);
void em_fly02_005EF900(EMW *em, EM20W *w);
void em_fly03_005EFA10(EMW *em, EM20W *w);
void em_fly04_005EFCA0(EMW *em, EM20W *w);
void em_fly05_005EFE10(EMW *em, EM20W *w);
void em_fly06_005EFF80(EMW *em, EM20W *w);
void em_fly07_005F01A0(EMW *em, EM20W *w);
void em_fly08_005F02E0(EMW *em, EM20W *w);
void em_fly09_005F0580(EMW *em, EM20W *w);
void em_fly10_005F0730(EMW *em, EM20W *w);
void em_fly11_005F0940(EMW *em, EM20W *w);
void em_fly12_005F0B90(EMW *em, EM20W *w);
void em_fly13_005F0CF0(EMW *em, EM20W *w);
void em_fly14_005F1050(EMW *em, EM20W *w);
void em_fly15_005F1120(EMW *em, EM20W *w);
void em_fly16_005F11E0(EMW *em, EM20W *w);
void em_fly17_005F12F0(EMW *em, EM20W *w);
void em_fly18_005F1530(EMW *em, EM20W *w);
void em_fly19_005F1700(EMW *em, EM20W *w);
void em_fly20_005F18F0(EMW *em, EM20W *w);
void em_fly21_005F1A40(EMW *em, EM20W *w);
void em_fly22_005F1B80(EMW *em, EM20W *w);
void em_fly23_005F1CE0(EMW *em, EM20W *w);
void em_fly24_005F1F20(EMW *em, EM20W *w);
void em_atk00_005F2040(EMW *em, EM20W *w);
void em_atk02_005F20E0(EMW *em, EM20W *w);
void em_atk03_005F2350(EMW *em, EM20W *w);
void em_atk04_005F23D0(EMW *em, EM20W *w);
void em_atk06_005F24F0(EMW *em, EM20W *w);
void em_atk07_005F25E0(EMW *em, EM20W *w);
void em_atk08_005F2680(EMW *em, EM20W *w);
void em_atk09_005F2FE0(EMW *em, EM20W *w);
void em_atk10_005F30D0(EMW *em, EM20W *w);
void em_atk11_005F3170(EMW *em, EM20W *w);
void em_atk18_005F32A0(EMW *em, EM20W *w);
void em_atk21_005F3440(EMW *em, EM20W *w);
void em_atk26_005F3CE0(EMW *em, EM20W *w, int idx);
void em_atk27_005F3E70(EMW *em, EM20W *w);
void em_atk28_005F3F30(EMW *em, EM20W *w);
void em_atk29_005F3FF0(EMW *em, EM20W *w);
void em_atk30_005F4090(EMW *em, EM20W *w);
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
void em_move00_005F5F20(EMW *em, EM20W *w);
void em_move01_005F6180(EMW *em, EM20W *w);
void em_move02_005F6270(EMW *em, EM20W *w);
void em_move03_005F6440(EMW *em, EM20W *w);
void em_move04_005F6680(EMW *em, EM20W *w);
void em_move05_005F6810(EMW *em, EM20W *w);
void em_move06_005F6880(EMW *em, EM20W *w);
void em20_main(EMW *em);
void em20_main_sub(EMW *em, EM20W *w);
void em20_uvmove(EMW *em);
void sound_call_sub_005F75F0(EMW *em, int se, int joint);
void sound_call_005F7660(EMW *em, int frame, int se, int joint);
void sound_call_parts_005F76C0(EMW *em, int frame, int se, int joint, u8 layer);
void Em_set_quake_sub(EMW *, int);
void quake_call_005F7760(EMW *em, int frame, int arg);
void move_default_005F77B0(EMW *em);
void ef_move_sub_005F7800(EMW *em, EM20W *w);
void em20_effect_move(EMW *em);
void ground_land_eff_set_005FC860(EMW *em);
void takeoff_eff_set_005FC910(EMW *em);
void takeon_eff_set_005FC980(EMW *em);
void hover_eff_set2_005FCA20(EMW *em);
s32 kyusyu_char_set_005FCA70(EMW *em);
void em20_atk_end_sel(EMW *em);
void kyusyu_senkai_ret_005FCBA0(EMW *em);
void em20_material_sub(EMW *em, int type, u8 *tbl);
void dummy_em_prog_005FCDB0(void);





extern u8 *em20_act_add[3];
extern u16 *em20_rail_add[2];
extern u16 *em20_rail_half_add[1];








extern u8 *em20_act_add[3];




















































extern FLYNEED *em_hungry_tbl[];





























































































void sound_call_sub_005F75F0(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

void sound_call_005F7660(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_005F75F0(em, se, joint);
    }
}

void sound_call_parts_005F76C0(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

void quake_call_005F7760(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

void move_default_005F77B0(EMW *em) {
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

void ef_move_sub_005F7800(EMW *em, EM20W *w) {
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
