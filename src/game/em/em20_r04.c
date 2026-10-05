/* em20_r04 - monster 20 AI 0x005EC910-0x005EE8FC: em_act02_005EC910, em_act03_005EC9A0, em_act04_005ECA20, em_act05_005ECAC0, em_act06_005ECBB0, em_act07_005ECC80, em_act08_005ECD20, em_act09_005ECD30, em_act10_005ECDE0, em_act11_005ECEF0, em_act12_005ECFC0, em_act13_005ED040, em_act14_005ED110, em_act15_005ED4A0, em_act16_005ED5D0, em_act17_005ED700, em_act18_005ED780, em_act19_005ED940, em_act40_005EDAC0, em_act20_005EDB80, em_act21_005EDC90, em_act22_005EDE50, em_act23_005EDED0, em_act24_005EDFA0, em_act25_005EE070, em_act26_005EE0F0, em_act27_005EE230, em_act28_005EE340, em_act29_005EE410, em_act31_005EE550, em_act33_005EE690, em_act34, em_mv00_005EE7A0. Whole file in em20_ai_nm.c. */
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





























































































void em_act02_005EC910(EMW *em, EM20W *w) {
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

void em_act03_005EC9A0(EMW *em, EM20W *w) {
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

void em_act04_005ECA20(EMW *em, EM20W *w) {
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

void em_act05_005ECAC0(EMW *em, EM20W *w) {
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

void em_act06_005ECBB0(EMW *em, EM20W *w) {
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

void em_act07_005ECC80(EMW *em, EM20W *w) {
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

void em_act08_005ECD20(EMW *em, EM20W *w) {

}

void em_act09_005ECD30(EMW *em, EM20W *w) {
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

void em_act10_005ECDE0(EMW *em, EM20W *w) {
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

void em_act11_005ECEF0(EMW *em, EM20W *w) {
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

void em_act12_005ECFC0(EMW *em, EM20W *w) {
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

void em_act13_005ED040(EMW *em, EM20W *w) {
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

void em_act14_005ED110(EMW *em, EM20W *w) {
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

void em_act15_005ED4A0(EMW *em, EM20W *w) {
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

void em_act16_005ED5D0(EMW *em, EM20W *w) {
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

void em_act17_005ED700(EMW *em, EM20W *w) {
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

void em_act18_005ED780(EMW *em, EM20W *w) {
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

void em_act19_005ED940(EMW *em, EM20W *w) {
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

void em_act40_005EDAC0(EMW *em, EM20W *w) {
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

void em_act20_005EDB80(EMW *em, EM20W *w) {
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

void em_act21_005EDC90(EMW *em, EM20W *w) {
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

void em_act22_005EDE50(EMW *em, EM20W *w) {
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

void em_act23_005EDED0(EMW *em, EM20W *w) {
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

void em_act24_005EDFA0(EMW *em, EM20W *w) {
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

void em_act25_005EE070(EMW *em, EM20W *w) {
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

void em_act26_005EE0F0(EMW *em, EM20W *w) {
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

void em_act27_005EE230(EMW *em, EM20W *w) {
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

void em_act28_005EE340(EMW *em, EM20W *w) {
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

void em_act29_005EE410(EMW *em, EM20W *w) {
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

void em_act31_005EE550(EMW *em, EM20W *w) {
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

void em_act33_005EE690(EMW *em, EM20W *w) {
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

void em_mv00_005EE7A0(EMW *em, EM20W *w) {
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
