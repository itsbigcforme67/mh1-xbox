/* em15_r12 - em15 0x005CBA30-0x005CEFC8: sound_call_sub_005CBA30, sound_call_005CBAA0, sound_call_parts_005CBB00, quake_call_005CBBA0, move_default_005CBBF0, ef_move_sub_005CBC40 (all file-static, aliased in config/game_aliases.txt). Whole file in em15_nm.c. */
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
static void sound_call_sub_005CBA30(EMW *em, int se, int joint);
static void sound_call_005CBAA0(EMW *em, int frame, int se, int joint);
static void sound_call_parts_005CBB00(EMW *em, int frame, int se, int joint, u8 layer);
static void quake_call_005CBBA0(EMW *em, int frame, int arg);
void Em_set_quake_sub(EMW *, int);
static void move_default_005CBBF0(EMW *em);
static void ef_move_sub_005CBC40(EMW *em, EM15W *w);
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
