/* em15_r07 - monster 15 AI 0x005C7440-0x005C77C4: em_fly24_005C7440, em_fly25_005C75D0, em_fly26. Whole file in em15_nm.c. */
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
void em_act00_005C3090(EMW *em, EM15W *w);
void em_act01_005C3140(EMW *em, EM15W *w);
void em_act02_005C31D0(EMW *em, EM15W *w);
void em_act03_005C3280(EMW *em, EM15W *w);
void em_act04_005C3330(EMW *em, EM15W *w);
void em_act05_005C33B0(EMW *em, EM15W *w);
void em_act06_005C3480(EMW *em, EM15W *w);
void em_act07_005C3500(EMW *em, EM15W *w);
void em_act08_005C35A0(EMW *em, EM15W *w);
void em_act10_005C3630(EMW *em, EM15W *w);
void em_act13_005C3730(EMW *em, EM15W *w);
void em_act14_005C3840(EMW *em, EM15W *w);
void em_act17_005C3BB0(EMW *em, EM15W *w);
void em_act19_005C3C30(EMW *em, EM15W *w);
void em_act40_005C3DB0(EMW *em, EM15W *w);
void em_act18_005C3E70(EMW *em, EM15W *w);
void em_act20_005C3FC0(EMW *em, EM15W *w);
void em_act21_005C40D0(EMW *em, EM15W *w);
void em_act22_005C4220(EMW *em, EM15W *w);
void em_act23_005C42B0(EMW *em, EM15W *w);
void em_act24_005C4380(EMW *em, EM15W *w);
void em_act25_005C4450(EMW *em, EM15W *w);
void em_act26_005C44D0(EMW *em, EM15W *w);
void em_act27_005C4610(EMW *em, EM15W *w);
void em_act28_005C4720(EMW *em, EM15W *w);
void em_act29_005C47F0(EMW *em, EM15W *w);
void em_act31_005C4930(EMW *em, EM15W *w);
void em_mv00_005C4A70(EMW *em, EM15W *w);
void em_mv03_005C4BD0(EMW *em, EM15W *w);
void em_mv05_005C4ED0(EMW *em, EM15W *w);
void em_mv06_005C51D0(EMW *em, EM15W *w);
void em_mv07_005C5330(EMW *em, EM15W *w);
void em_fly00_005C5470(EMW *em, EM15W *w);
void em_fly01_005C5560(EMW *em, EM15W *w);
void em_fly02_005C5790(EMW *em, EM15W *w);
void em_fly03_005C5890(EMW *em, EM15W *w);
void em_fly04_005C5B00(EMW *em, EM15W *w);
void em_fly05_005C5C50(EMW *em, EM15W *w);
void em_fly06_005C5DA0(EMW *em, EM15W *w);
void em_fly07_005C5EC0(EMW *em, EM15W *w);
void em_fly08_005C6000(EMW *em, EM15W *w);
void em_fly09_005C6290(EMW *em, EM15W *w);
void em_fly10_005C6430(EMW *em, EM15W *w);
void em_fly11_005C6630(EMW *em, EM15W *w);
void em_fly12_005C6860(EMW *em, EM15W *w);
void em_fly13_005C6970(EMW *em, EM15W *w);
void em_fly14_005C6C80(EMW *em, EM15W *w);
void em_fly15_005C6D30(EMW *em, EM15W *w);
void em_fly16_005C6E50(EMW *em, EM15W *w);
void em_fly17_005C6F60(EMW *em, EM15W *w);
void em_fly18_005C6F70(EMW *em, EM15W *w);
void em_fly19_005C6F80(EMW *em, EM15W *w);
void em_fly20_005C6F90(EMW *em, EM15W *w);
void em_fly21_005C70D0(EMW *em, EM15W *w);
void em_fly22_005C7210(EMW *em, EM15W *w);
void em_fly23_005C7220(EMW *em, EM15W *w);
void em_fly24_005C7440(EMW *em, EM15W *w);
void em_fly25_005C75D0(EMW *em, EM15W *w);
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
void em_move00_005CA5B0(EMW *em, EM15W *w);
void em_move01_005CA890(EMW *em, EM15W *w);
void em_move02_005CA930(EMW *em, EM15W *w);
void em_move03_005CABA0(EMW *em, EM15W *w);
void em_move04_005CAC70(EMW *em, EM15W *w);
void em_move05_005CAD90(EMW *em, EM15W *w);
void em_move06_005CAE00(EMW *em, EM15W *w);
void em15_uvmove(EMW *em);
void sound_call_sub_005CBA30(EMW *em, int se, int joint);
void sound_call_005CBAA0(EMW *em, int frame, int se, int joint);
void sound_call_parts_005CBB00(EMW *em, int frame, int se, int joint, u8 layer);
void quake_call_005CBBA0(EMW *em, int frame, int arg);
void Em_set_quake_sub(EMW *, int);
void move_default_005CBBF0(EMW *em);
void ef_move_sub_005CBC40(EMW *em, EM15W *w);
void em15_effect_move(EMW *em);
void ground_land_eff_set_005CF030(EMW *em);
void dummy_em_prog_005CF0E0(void);















































































































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















void em_fly24_005C7440(EMW *em, EM15W *w) {
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

void em_fly25_005C75D0(EMW *em, EM15W *w) {
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
