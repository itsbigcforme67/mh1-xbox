/* em21_r12 - monster 21 AI 0x00608D00-0x0060BAC4: sound_call_sub_00608D00, sound_call_00608D70, quake_call_00608DD0, move_default_00608E20, ef_move_sub_00608E70. Whole file in em21_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em21.h"

typedef struct EML {
    s32 v;              /* 0x194 + i * 0x50: layer i is busy while non-zero (as EMW.x194) */
    u8 _pad[0x4C];
} EML;
#define EM_LYR(em, i) (((EML *)&(em)->x194)[i].v)
#define GAME_X1E16 (*(u16 *)((u8 *)&game_w + 0x1E))
#define M2C_FIELD(ptr, type, off) (*(type)((u8 *)(ptr) + (off)))



f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em_rate_clear(EMW *);
void target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
void SetVector(f32 *, f32, f32, f32);
void speed_add(EMW *, s32 *);
void speed_add_g(EMW *, s32 *);
int em_frame_check2(EMW *, int, f32);
void em_act_set(EMW *, int, u16);
void Shell08_set_ang_time(EMW *, s16, u8, u8, u16, u16, int);
void em21_fly_adjy2_init(EMW *, u8);
u8 em21_fly_adjy2(EMW *);
u8 em21_senkai_pos_no(EMW *em, f32 *out);
void Eft19_set(EMW *, int, int);
void em_cmd_reset(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
void Em_Sleep_End(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
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
void em_action_timer_calc(EMW *, int);
void em_rate_clear_g(EMW *);
void em_ikari_add(EMW *, s16);
void Eft20_set(f32, EMW *, int, int);
int em_frame_check3(EMW *, int, f32, f32);
void em21_act_set(EMW *em, int kind, u16 no, u16 arg);
s16 em_hp_vital_set2(EMW *, s16, s16);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void em_range_set(EMW *em, s8 no);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void Em_Sleep2_End(EMW *em);
void Em_Sleep2_Start(EMW *em);
void Em_Sleep_Flag_Ck(EMW *em);
void em_hinshi_end(EMW *em);
void em_suimin_end(EMW *em);
void em_ana_loop_cnt_set(EMW *em);
void Eft08_set(f32 *, int, int, f32);
void Quest_enemy_capture();
extern s16 em_atk_mode_timer_tbl[35];
extern s16 em21_stay_timer_tbl[];
extern s16 em21_runaway_timer_tbl[];

void em21_local_init(EMW *em);
void em21_init(EMW *em);
void em21_to_normal(EMW *em);
void em21_to_swim(EMW *em);
s32 em21_act_sub(EMW *em, s32 arg1);
void em_act00_00600490(EMW *em, EM21W *w);
void em_act01_00600570(EMW *em, EM21W *w);
void em_act02_00600650(EMW *em, EM21W *w);
void em_act04_00600700(EMW *em, EM21W *w);
void em_act05_006007F0(EMW *em, EM21W *w);
void em_act06_00600900(EMW *em, EM21W *w);
void em_act07_006009D0(EMW *em, EM21W *w);
void em_act08_00600A50(EMW *em, EM21W *w);
void em_act10_00600B90(EMW *em, EM21W *w);
void em_act12_00600CD0(EMW *em, EM21W *w);
void em_act13_00600DE0(EMW *em, EM21W *w);
void em_act14_00600EB0(EMW *em, EM21W *w);
void em_act15_00601000(EMW *em, EM21W *w);
void em_act16_006010D0(EMW *em, EM21W *w);
void em_act17_00601220(EMW *em, EM21W *w);
void em_act18_006012F0(EMW *em, EM21W *w);
void em_act19_00601440(EMW *em, EM21W *w);
void em_act20_00601510(EMW *em, EM21W *w);
void em_mv00_006015A0(EMW *em, EM21W *w);
void em_mv01_00601700(EMW *em, EM21W *w);
void em_mv02_00601870(EMW *em, EM21W *w);
void em_mv03_00601AC0(EMW *em, EM21W *w);
void em_fly00_00601C20(EMW *em, EM21W *w);
void em_fly01_00601CB0(EMW *em, EM21W *w);
void em_fly02_00601D80(EMW *em, EM21W *w);
void em_fly03_00601D90(EMW *em, EM21W *w);
void em_fly04_00602030(EMW *em, EM21W *w);
void em_fly05_00602280(EMW *em, EM21W *w);
void em_fly06_00602450(EMW *em, EM21W *w);
void em_fly07_006024F0(EMW *em, EM21W *w);
void em_fly08_00602640(EMW *em, EM21W *w);
void em_fly09_00602780(EMW *em, EM21W *w);
void em_fly10_00602A70(EMW *em, EM21W *w);
void em_fly11_00602CC0(EMW *em, EM21W *w);
void em_fly12_00602F10(EMW *em, EM21W *w);
void em_fly13_00603000(EMW *em, EM21W *w);
void em_fly14_006030F0(EMW *em, EM21W *w);
void em_fly15_006033A0(EMW *em, EM21W *w);
void em_fly16_00603460(EMW *em, EM21W *w);
void em_fly17_00603720(EMW *em, EM21W *w);
void em_fly18_006038C0(EMW *em, EM21W *w);
void em_fly19_00603B90(EMW *em, EM21W *w);
void em_fly20_00603E40(EMW *em, EM21W *w);
void em_fly21_00604110(EMW *em, EM21W *w);
void em_fly22_006041A0(EMW *em, EM21W *w);
void em_fly23_00604340(EMW *em, EM21W *w);
void em_fly24_00604440(EMW *em, EM21W *w);
void em_fly25_006044D0(EMW *em, EM21W *w);
void em_atk00_00604560(EMW *em, EM21W *w);
void em_atk01_006045E0(EMW *em, EM21W *w);
void em_atk02_006046C0(EMW *em, EM21W *w);
void em_atk03_006047B0(EMW *em, EM21W *w);
void em_atk04_006048C0(EMW *em, EM21W *w);
void em_atk05_006049D0(EMW *em, EM21W *w);
void em_atk06_00604AE0(EMW *em, EM21W *w);
void em_atk07_00604DC0(EMW *em, EM21W *w);
void em_dmg00_00604E40(EMW *em, EM21W *w);
void em_dmg01_00604F20(EMW *em, EM21W *w);
void em_dmg02_00604FB0(EMW *em, EM21W *w);
void em_dmg03_00605040(EMW *em, EM21W *w);
void em_dmg04_006050D0(EMW *em, EM21W *w);
void em_dmg05_006051D0(EMW *em, EM21W *w);
void em_dmg06_00605370(EMW *em, EM21W *w);
void em_dmg07_006054D0(EMW *em, EM21W *w);
void em_dmg08_00605630(EMW *em, EM21W *w);
void em_dmg09_00605770(EMW *em, EM21W *w);
void em_dmg10_006058E0(EMW *em, EM21W *w);
void em_dmg11_00605970(EMW *em, EM21W *w);
void em_dmg12_00605A50(EMW *em, EM21W *w);
void em_dmg13_00605B70(EMW *em, EM21W *w);
void em_dmg14_00605CA0(EMW *em, EM21W *w);
void em_dmg15_00605DC0(EMW *em, EM21W *w);
static void em_dmg16_006060F0(EMW *em, EM21W *w);
static void em_dmg17_006061D0(EMW *em, EM21W *w);
static void em_dmg18_006063E0(EMW *em, EM21W *w);
static void em_dmg19_00606600(EMW *em, EM21W *w);
static void em_dmg20_00606700(EMW *em, EM21W *w);
void em_dmg21(EMW *em, EM21W *w);
void em_dmg22(EMW *em, EM21W *w);
static void em_demo00_00606B70(EMW *em, EM21W *w);
static void em_die00_00606C90(EMW *em, EM21W *w);
static void em_die01_00606E20(EMW *em, EM21W *w);
static void em_die02_00606F90(EMW *em, EM21W *w);
static void em_die03_006073F0(EMW *em, EM21W *w);
void em_move00_006075D0(EMW *em, EM21W *w);
void em_move02_006077C0(EMW *em, EM21W *w);
void em_move03_006079A0(EMW *em, EM21W *w);
void em_move04_00607A60(EMW *em, EM21W *w);
void em_move01_00607730(EMW *em, EM21W *w);
void em_move05_00607C10(EMW *em, EM21W *w);
void em_move06_00607CA0(EMW *em, EM21W *w);
void em21_uvmove(EMW *em);
void sound_call_sub_00608D00(EMW *em, int se, int joint);
static void sound_call_00608D70(EMW *em, int frame, int se, int joint);
static void quake_call_00608DD0(EMW *em, int frame, int arg);
static void move_default_00608E20(EMW *em);
void ef_move_sub_00608E70(EMW *em, EM21W *w);
void Em_set_quake_sub(EMW *, int);
void hire_move_sub2_0060BAD0(EMW *em, EM21W *w, int i);
void hire_move_sub1_0060BCA0(EMW *em, EM21W *w, int i);
void hire_move_0060BFE0(EMW *em, EM21W *w);
void em21_effect_move(EMW *em);
void hire_req_set_0060C0C0(EMW *em, EM21W *w, u8 mode);
void ground_land_eff_set_0060C180(EMW *em);
void swim_eff_set_0060C1A0(f32 scale, EMW *em);
void swim_eff_set2_0060C260(f32 scale, EMW *em);
void swim_eff_set3(f32 scale, EMW *em);
void dummy_em_prog_0060C390(void);



































































































#define M4(n) (em->mode == 4 && em->x15 == (n))
#define M0(n) (em->mode == 0 && em->x15 == (n))
#define M2(n) (em->mode == 2 && em->x15 == (n))

void em21_main_sub(EMW *em, EM21W *w);
void em_no_floor_ck(EMW *em);
void em_no_battle_area_ck(EMW *, int, int);
void em_sleep2_dmg_timer_set(EMW *em);
void em_dur_set(EMW *, int);
int em_hokaku_ck(EMW *em, f32 rate);
void em_cmd_ck(EMW *);










typedef struct HIRE_ADD {
    u16 t;              /* frame limit, 0 ends the list */
    u16 add;            /* angle added per frame up to it */
} HIRE_ADD;

extern s16 hire_start_timer_tbl0_003893D0[4];
extern s16 hire_start_timer_tbl1_003893D8[4];
extern s16 hire_remove_timer_tbl0_003893E0[4];
extern s16 hire_remove_timer_tbl1_003893E8[4];
extern u16 hire_down_angx_003893F0[4];
extern HIRE_ADD *hire_normal_add_tbl_0066ED90[4];
extern HIRE_ADD *hire_down_add_tbl_0066EE40[4];











void sound_call_sub_00608D00(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_00608D70(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_00608D00(em, se, joint);
    }
}

static void quake_call_00608DD0(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

static void move_default_00608E20(EMW *em) {
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

void ef_move_sub_00608E70(EMW *em, EM21W *w) {
    f32 v[3];
    f32 v2[3];
    f32 v3[3];
    f32 v4[3];
    s16 temp_v1;
    u16 temp_a0;

    temp_a0 = em->char0;
    if (temp_a0 != w->anim) {
        w->anim = (s16) temp_a0;
    }
    temp_v1 = w->anim;
    switch (temp_v1) {                              /* irregular */
    case 0x3E9:
        sound_call_00608D70(em, 0x40, 0x20, 0x23);
        sound_call_00608D70(em, 0xC8, Code_Make(0x21, 2, 0x21, 2), 0x23);
        sound_call_00608D70(em, 0x19A, Code_Make(0x21, 2, 0x21, 2), 0x23);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x3EB:
        sound_call_00608D70(em, 0x34, 1, 0x14);
        sound_call_00608D70(em, 0x74, 1, 0x1A);
        quake_call_00608DD0(em, 0x34, 1);
        quake_call_00608DD0(em, 0x74, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell20_set(em, 2);
        }
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell20_set(em, 3);
        }
        if (em_frame_check(em, 0, 120.0f) != 0) {
            shell20_set(em, 4);
        }
        hire_req_set_0060C0C0(em, w, 1);
        break;
    case 0x3EC:
        sound_call_00608D70(em, 0xA, 0x56, 0x23);
        sound_call_00608D70(em, 0x1C, 6, 0x14);
        sound_call_00608D70(em, 0x34, 6, 0x1A);
        sound_call_00608D70(em, 0x4C, 6, 0x14);
        sound_call_00608D70(em, 0x64, 6, 0x1A);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 22.0f) != 0) {
            shell20_set(em, 0x13);
        }
        if ((em_frame_check(em, 0, 16.0f) == 0) && (em_frame_check(em, 0, 56.0f) == 0)) {
            if (em_frame_check(em, 0, 102.0f) != 0) {
                goto block_91;
            }
        } else {
block_91:
            shell20_set(em, 0x14);
        }
        if ((em_frame_check(em, 0, 30.0f) != 0) || (em_frame_check(em, 0, 78.0f) != 0)) {
            shell20_set(em, 0x15);
        }
        if ((em_frame_check(em, 0, 18.0f) != 0) || (em_frame_check(em, 0, 66.0f) != 0)) {
            Eft13_set_em_scl(em, 0x1A, 4.0f, 3);
        }
        if ((em_frame_check(em, 0, 42.0f) != 0) || (em_frame_check(em, 0, 90.0f) != 0)) {
            Eft13_set_em_scl(em, 0x14, 4.0f, 3);
        }
        break;
    case 0x3ED:
        sound_call_00608D70(em, 0x1A, 1, 0x14);
        sound_call_00608D70(em, 0x30, 1, 0x1A);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell20_set(em, 5);
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell20_set(em, 6);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(*(u16 *)0x3F340E & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x3EE:
        sound_call_00608D70(em, 0x1A, 1, 0x1A);
        sound_call_00608D70(em, 0x30, 1, 0x14);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell20_set(em, 7);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell20_set(em, 8);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 0);
            return;
        }
        break;
    case 0x3F2:
        sound_call_00608D70(em, 6, 0x31, 0x23);
        sound_call_00608D70(em, 6, 4, 0x1A);
        sound_call_00608D70(em, 0x4C, 0, 0x14);
        sound_call_00608D70(em, 0x1C, 0x12, 0x14);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            shell20_set(em, 0x11);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell20_set(em, 0x12);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            shell20_set(em, 0x12);
            return;
        }
        break;
    case 0x3F3:
        sound_call_00608D70(em, 0xE, 0xB, 6);
        sound_call_00608D70(em, 0x12, 0xB, 0xC);
        sound_call_00608D70(em, 0x46, 0xB, 6);
        sound_call_00608D70(em, 0x42, 0xB, 0xC);
        sound_call_00608D70(em, 0x7A, 0xB, 6);
        sound_call_00608D70(em, 0x7E, 0xB, 0xC);
        hire_req_set_0060C0C0(em, w, 1);
        break;
    case 0x3FA:
        sound_call_00608D70(em, 0x28, 8, 0x1A);
        sound_call_00608D70(em, 0x1A, 0xD, 6);
        sound_call_00608D70(em, 0x1E, 0xD, 0xC);
        sound_call_00608D70(em, 0x52, 0xB, 6);
        sound_call_00608D70(em, 0x56, 0xB, 0xC);
        sound_call_00608D70(em, 0x88, 0xB, 6);
        sound_call_00608D70(em, 0x8C, 0xB, 0xC);
        sound_call_00608D70(em, 0xB6, 0xB, 6);
        sound_call_00608D70(em, 0xBA, 0xB, 0xC);
        hire_req_set_0060C0C0(em, w, 1);
        if (((em_frame_check(em, 0, 42.0f) != 0) || (em_frame_check(em, 0, 102.0f) != 0) || (em_frame_check(em, 0, 156.0f) != 0)) && (em->pos[1] <= (1000.0f + em->x5AC))) {
            Eft20_set(1.0f, em, 1, 0);
            return;
        }
        break;
    case 0x3FB:
        sound_call_00608D70(em, 2, 0xB, 6);
        sound_call_00608D70(em, 6, 0xB, 0xC);
        sound_call_00608D70(em, 0xC, 4, 0x1A);
        sound_call_00608D70(em, 4, 1, 0x14);
        sound_call_00608D70(em, 0xE, 9, 0);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell20_set(em, 0xD);
        }
        if (em_frame_check(em, 0, 16.0f) != 0) {
            ground_land_eff_set_0060C180(em);
            return;
        }
        break;
    case 0x401:
        sound_call_00608D70(em, 4, 0x1F, 0x23);
        sound_call_00608D70(em, 0x6C, 0x22, 0x23);
        sound_call_00608D70(em, 0xAA, 0x20, 0x23);
        sound_call_00608D70(em, 0xD6, 0x22, 0x23);
        sound_call_00608D70(em, 0x114, 0x1F, 0x23);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x407:
        sound_call_00608D70(em, 2, 0x2D, 0x23);
        sound_call_00608D70(em, 0xB6, 0x2D, 0x23);
        sound_call_00608D70(em, 0x16C, 0x2D, 0x23);
        v[1] = 10.0f;
        v[2] = 140.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v, 1.6f);
        break;
    case 0x408:
        sound_call_00608D70(em, 4, 0x2F, 0x23);
        sound_call_00608D70(em, 0x46, 0, 0x1A);
        sound_call_00608D70(em, 0x8E, 3, 0x14);
        sound_call_00608D70(em, 4, 0x17, 0);
        sound_call_00608D70(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_00608D70(em, 4, 0x1F, 0x23);
        sound_call_00608D70(em, 0x46, 0x2D, 0x23);
        sound_call_00608D70(em, 0x7E, 3, 0x1A);
        sound_call_00608D70(em, 0xB6, 3, 0x14);
        sound_call_00608D70(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_00608D70(em, 0x24, 0x57, 0x23);
        hire_req_set_0060C0C0(em, w, 3);
        v2[1] = 10.0f;
        v2[2] = 140.0f;
        v2[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.6f);
        break;
    case 0x40C:
        sound_call_00608D70(em, 0x24, 0x24, 0x23);
        sound_call_00608D70(em, 0x38, 0x22, 0x23);
        sound_call_00608D70(em, 0x14, 0, 0x1A);
        sound_call_00608D70(em, 0x28, 0, 0x14);
        sound_call_00608D70(em, 0x3C, 0x15, 0x22);
        sound_call_00608D70(em, 0x3C, 0xC, 0xC);
        sound_call_00608D70(em, 0xC, 0xC, 6);
        sound_call_00608D70(em, 0xA6, 0, 0x1A);
        hire_req_set_0060C0C0(em, w, 3);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell20_set(em, 1);
            return;
        }
        break;
    case 0x40D:
        sound_call_00608D70(em, 0x1E, 0x20, 0x23);
        sound_call_00608D70(em, 0xC, 0xC, 0x1A);
        sound_call_00608D70(em, 0x1C, 2, 0x1A);
        sound_call_00608D70(em, 0x2E, 3, 0x14);
        sound_call_00608D70(em, 0x48, 1, 0x1A);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x40E:
        sound_call_00608D70(em, 4, 0x57, 0x23);
        sound_call_00608D70(em, 0x48, 0x16, 0);
        sound_call_00608D70(em, 0xA4, 9, 0);
        sound_call_00608D70(em, 0x90, 3, 0xC);
        sound_call_00608D70(em, 0x88, 0xE, 6);
        sound_call_00608D70(em, 0xB2, 4, 0x22);
        hire_req_set_0060C0C0(em, w, 3);
        if (em_frame_check(em, 0, 148.0f) != 0) {
            shell20_set(em, 0xF);
        }
        if (em_frame_check(em, 0, 170.0f) != 0) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
        }
        v3[1] = 10.0f;
        v3[2] = 140.0f;
        v3[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v3, 1.6f);
        if (em_frame_check(em, 0, 170.0f) != 0) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
            return;
        }
        break;
    case 0x413:
        sound_call_00608D70(em, 8, 0x23, 0x23);
        sound_call_00608D70(em, 0xE, 0xF, 6);
        sound_call_00608D70(em, 0x1E, 0x14, 0x2A);
        sound_call_00608D70(em, 0x2A, 0, 0x1A);
        sound_call_00608D70(em, 0x48, 1, 0x14);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell20_set(em, 0x10);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x417:
        sound_call_00608D70(em, 2, 0x12, 0);
        sound_call_00608D70(em, 0x50, 0x18, 0x23);
        sound_call_00608D70(em, 0x14, 0x28, 0x23);
        sound_call_00608D70(em, 0x4A, 0x29, 0x23);
        sound_call_00608D70(em, 0x4C, 0xD, 6);
        sound_call_00608D70(em, 0x48, 0xD, 0xC);
        sound_call_00608D70(em, 0xB4, 0x13, 6);
        sound_call_00608D70(em, 0xBB, 0x13, 0xC);
        sound_call_00608D70(em, 0xF0, 0x12, 0);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 66.0f) != 0) {
            shell20_set(em, 0x19);
            return;
        }
        break;
    case 0x418:
        sound_call_00608D70(em, 2, 0x22, 0x23);
        sound_call_00608D70(em, 0x22, 0x22, 0x23);
        sound_call_00608D70(em, 0x40, 0x22, 0x23);
        sound_call_00608D70(em, 0x72, 0x20, 0x23);
        sound_call_00608D70(em, 0x10, 1, 0x14);
        break;
    case 0x424:
        sound_call_00608D70(em, 6, 0x53, 0x23);
        sound_call_00608D70(em, 0x48, 0x13, 0x2A);
        sound_call_00608D70(em, 0x30, 6, 0x1A);
        sound_call_00608D70(em, 0x30, 0x11, 0x14);
        sound_call_00608D70(em, 0x30, 3, 0x14);
        sound_call_00608D70(em, 0x74, 3, 0x14);
        sound_call_00608D70(em, 0x94, 3, 0x1A);
        sound_call_00608D70(em, 0x34, 0xC, 6);
        sound_call_00608D70(em, 0x38, 0xD, 0xC);
        sound_call_00608D70(em, 0xC2, 3, 0x1A);
        hire_req_set_0060C0C0(em, w, 1);
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
        sound_call_00608D70(em, 6, 0x53, 0x23);
        sound_call_00608D70(em, 0xA, 0x12, 0x1A);
        sound_call_00608D70(em, 0x1E, 6, 0x1A);
        sound_call_00608D70(em, 0x2E, 4, 0x14);
        sound_call_00608D70(em, 0x48, 3, 0x14);
        sound_call_00608D70(em, 0x6A, 3, 0x1A);
        sound_call_00608D70(em, 0xAC, 3, 0x1A);
        sound_call_00608D70(em, 0x5A, 0x15, 0);
        if (em_frame_check(em, 0, 24.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
            return;
        }
        break;
    case 0x426:
        sound_call_00608D70(em, 4, 0x4F, 0x23);
        sound_call_00608D70(em, 0x56, 0x1F, 0x23);
        sound_call_00608D70(em, 0x10, 3, 0x14);
        sound_call_00608D70(em, 4, 0xD, 6);
        sound_call_00608D70(em, 8, 0xD, 0xC);
        sound_call_00608D70(em, 0x62, 0x13, 0x2A);
        sound_call_00608D70(em, 0x7E, 3, 0x1A);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x427:
        sound_call_00608D70(em, 4, 0x52, 0x23);
        sound_call_00608D70(em, 4, 0x13, 0);
        sound_call_00608D70(em, 0x4C, 0x1F, 0x23);
        hire_req_set_0060C0C0(em, w, 1);
        break;
    case 0x428:
        sound_call_00608D70(em, 4, 0x52, 0x23);
        sound_call_00608D70(em, 4, 0x13, 0);
        sound_call_00608D70(em, 0x44, 0x20, 0x23);
        sound_call_00608D70(em, 0x78, 3, 0x1A);
        hire_req_set_0060C0C0(em, w, 1);
        break;
    case 0x42A:
        sound_call_00608D70(em, 4, 0x52, 0x23);
        sound_call_00608D70(em, 4, 0x13, 0);
        hire_req_set_0060C0C0(em, w, 1);
        break;
    case 0x42B:
        sound_call_00608D70(em, 0x46, 0x2D, 0x23);
        sound_call_00608D70(em, 0x46, 0x17, 0);
        hire_req_set_0060C0C0(em, w, 1);
        break;
    case 0x42C:
        sound_call_00608D70(em, 0x1A, 0x2C, 0x23);
        sound_call_00608D70(em, 0x2E, 8, 0);
        sound_call_00608D70(em, 4, 0x14, 0);
        sound_call_00608D70(em, 0x1A, 0x30, 0x23);
        sound_call_00608D70(em, 0x1A, 0x2E, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            Eft20_set(0.800000012f, em, 0, 0);
            return;
        }
        break;
    case 0x42D:
        sound_call_00608D70(em, 4, 0x52, 0);
        sound_call_00608D70(em, 4, 0x13, 0x2A);
        sound_call_00608D70(em, 0x18, 9, 0);
        sound_call_00608D70(em, 0x18, 0x11, 0);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x80);
            return;
        }
        break;
    case 0x42E:
        sound_call_00608D70(em, 0x20, 0x1D, 0x22);
        sound_call_00608D70(em, 0x44, 0x1C, 0);
        hire_req_set_0060C0C0(em, w, 3);
        if ((em_frame_check(em, 0, 16.0f) != 0) || (em_frame_check(em, 0, 66.0f) != 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
            return;
        }
        break;
    case 0x42F:
        sound_call_00608D70(em, 4, 0x20, 0x23);
        sound_call_00608D70(em, 0x38, 0, 0x14);
        sound_call_00608D70(em, 0x3E, 1, 0x1A);
        sound_call_00608D70(em, 4, 0x16, 0);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x432:
        sound_call_00608D70(em, 4, 0x52, 0);
        sound_call_00608D70(em, 4, 0x13, 0x2A);
        sound_call_00608D70(em, 0x18, 9, 0);
        sound_call_00608D70(em, 0x18, 0x11, 0);
        hire_req_set_0060C0C0(em, w, 1);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x81);
            return;
        }
        break;
    case 0x433:
        sound_call_00608D70(em, 0x20, 0x1D, 0x22);
        sound_call_00608D70(em, 0x44, 0x1C, 0);
        hire_req_set_0060C0C0(em, w, 3);
        if ((em_frame_check(em, 0, 16.0f) != 0) || (em_frame_check(em, 0, 66.0f) != 0)) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
            return;
        }
        break;
    case 0x434:
        sound_call_00608D70(em, 0x10, 0x5A, 0x23);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x447:
        sound_call_00608D70(em, 4, 0x10, 0);
        sound_call_00608D70(em, 0xA, 0x12, 0);
        sound_call_00608D70(em, 0x14, 9, 0);
        sound_call_00608D70(em, 4, 0x27, 0x23);
        sound_call_00608D70(em, 4, 0x2A, 0x23);
        hire_req_set_0060C0C0(em, w, 1);
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 4.0f, 60.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x448:
        sound_call_00608D70(em, 0x16, 0x12, 0);
        sound_call_00608D70(em, 0x48, 0x12, 0);
        sound_call_00608D70(em, 4, 0xC, 6);
        sound_call_00608D70(em, 8, 0xC, 0xC);
        sound_call_00608D70(em, 0x30, 0x13, 0);
        sound_call_00608D70(em, 4, 0x28, 0x23);
        sound_call_00608D70(em, 4, 0x16, 0);
        hire_req_set_0060C0C0(em, w, 1);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 12.0f, 24.0f) != 0) || (em_frame_check3(em, 0, 46.0f, 90.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x449:
        sound_call_00608D70(em, 0x22, 0x16, 6);
        sound_call_00608D70(em, 0x26, 0x16, 0xC);
        sound_call_00608D70(em, 0x62, 4, 0x22);
        sound_call_00608D70(em, 0x6C, 3, 0x22);
        sound_call_00608D70(em, 4, 0x2A, 0x23);
        sound_call_00608D70(em, 4, 0x2B, 0x23);
        break;
    case 0x44A:
        sound_call_00608D70(em, 4, 0x16, 6);
        sound_call_00608D70(em, 8, 0x16, 0xC);
        sound_call_00608D70(em, 0x6C, 3, 0x22);
        sound_call_00608D70(em, 4, 0x28, 0x23);
        sound_call_00608D70(em, 0x46, 0x20, 0x23);
        break;
    case 0x44D:
        sound_call_00608D70(em, 4, 0x65, 0x23);
        sound_call_00608D70(em, 4, 0x16, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x44E:
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x44F:
        sound_call_00608D70(em, 4, 0x24, 0x23);
        sound_call_00608D70(em, 4, 0x16, 0);
        sound_call_00608D70(em, 0x20, 0x15, 0x14);
        sound_call_00608D70(em, 0x24, 0x15, 0x1A);
        sound_call_00608D70(em, 0x46, 0x17, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x450:
        sound_call_00608D70(em, 0x14, 0x56, 0x23);
        sound_call_00608D70(em, 4, 0x15, 0x23);
        sound_call_00608D70(em, 0x22, 0x30, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x451:
        sound_call_00608D70(em, 4, 0x15, 0x23);
        sound_call_00608D70(em, 0x1E, 0x30, 0x2A);
        sound_call_00608D70(em, 0x30, 0x30, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x452:
        sound_call_00608D70(em, 4, 0x16, 0);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x453:
        sound_call_00608D70(em, 4, 0x16, 0);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x454:
        sound_call_00608D70(em, 4, 0x56, 0x23);
        sound_call_00608D70(em, 4, 0x16, 0);
        sound_call_00608D70(em, 0x30, 0x30, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            shell20_set(em, 0xE);
            return;
        }
        break;
    case 0x457:
        sound_call_00608D70(em, 4, 0x17, 0x23);
        sound_call_00608D70(em, 4, 0x2B, 0);
        sound_call_00608D70(em, 0x7A, 0x16, 0);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x458:
        sound_call_00608D70(em, 0x74, 0x1A, 0x23);
        sound_call_00608D70(em, 4, 0x28, 0x23);
        sound_call_00608D70(em, 0x58, 0x29, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x459:
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x45A:
        sound_call_00608D70(em, 4, 0x16, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x45B:
        sound_call_00608D70(em, 4, 0x16, 0x2A);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x45C:
        sound_call_00608D70(em, 0x74, 0x18, 0x23);
        sound_call_00608D70(em, 4, 0x28, 0x23);
        sound_call_00608D70(em, 0x58, 0x29, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x45D:
        sound_call_00608D70(em, 0x74, 0x18, 0x23);
        sound_call_00608D70(em, 4, 0x28, 0x23);
        sound_call_00608D70(em, 0x58, 0x29, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x45E:
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x45F:
        sound_call_00608D70(em, 0x14, 0x27, 0x23);
        sound_call_00608D70(em, 4, 0xD, 6);
        sound_call_00608D70(em, 8, 0xD, 0xC);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell20_set(em, 0x18);
            return;
        }
        break;
    case 0x460:
        sound_call_00608D70(em, 4, 0x16, 0x23);
        sound_call_00608D70(em, 0x1E, 0x12, 0x14);
        sound_call_00608D70(em, 0x1E, 0x15, 0);
        sound_call_00608D70(em, 0x1E, 0x14, 0);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x461:
        sound_call_00608D70(em, 0xE, 0x10, 0);
        sound_call_00608D70(em, 0xA, 9, 0);
        sound_call_00608D70(em, 0x38, 8, 0x23);
        sound_call_00608D70(em, 0x38, 0x12, 0x14);
        sound_call_00608D70(em, 0x1E, 0x20, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            Eft20_set(0.699999988f, em, 0, 0x89);
            return;
        }
        break;
    case 0x462:
        sound_call_00608D70(em, 4, 0x35, 0);
        sound_call_00608D70(em, 0x32, 0x35, 0);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell20_set(em, 0x17);
        }
        if ((em_frame_check(em, 0, 24.0f) != 0) || (em_frame_check(em, 0, 42.0f) != 0) || (em_frame_check(em, 0, 62.0f) != 0) || (em_frame_check(em, 0, 82.0f) != 0)) {
            Eft13_set_em_scl(em, 6, 8.0f + 0.0005f * (f32)((u16)ran_suu(1) & 0x3FF), 3);
        }
        if ((em_frame_check(em, 0, 10.0f) != 0) || (em_frame_check(em, 0, 32.0f) != 0) || (em_frame_check(em, 0, 52.0f) != 0) || (em_frame_check(em, 0, 74.0f) != 0)) {
            Eft13_set_em_scl(em, 0xC, 8.0f + 0.0005f * (f32)((u16)ran_suu(1) & 0x3FF), 3);
            return;
        }
        break;
    case 0x464:
        sound_call_00608D70(em, 0x18, 0x12, 0x14);
        sound_call_00608D70(em, 0x18, 0x15, 0);
        sound_call_00608D70(em, 0x36, 0, 0x14);
        sound_call_00608D70(em, 0x3C, 4, 0x1A);
        sound_call_00608D70(em, 4, 0x16, 1);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x466:
        sound_call_00608D70(em, 4, 0x53, 0x23);
        sound_call_00608D70(em, 0x5A, 0x30, 0x2A);
        hire_req_set_0060C0C0(em, w, 3);
        break;
    case 0x467:
        sound_call_00608D70(em, 0x1A, 0x2C, 0x23);
        sound_call_00608D70(em, 0x2E, 8, 0);
        sound_call_00608D70(em, 4, 0x14, 0);
        sound_call_00608D70(em, 0x1A, 0x2E, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            Eft20_set(0.800000012f, em, 0, 0);
            return;
        }
        break;
    case 0x468:
        sound_call_00608D70(em, 4, 0x2C, 0x23);
        sound_call_00608D70(em, 4, 0x10, 0);
        sound_call_00608D70(em, 0x32, 9, 0x23);
        sound_call_00608D70(em, 4, 0x16, 0x23);
        hire_req_set_0060C0C0(em, w, 3);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
            return;
        }
        break;
    case 0x469:
        sound_call_00608D70(em, 4, 0x2A, 0x23);
        sound_call_00608D70(em, 4, 0x10, 0);
        sound_call_00608D70(em, 0x32, 9, 0x23);
        sound_call_00608D70(em, 4, 0x16, 0x23);
        hire_req_set_0060C0C0(em, w, 3);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            Eft13_set_em_scl(em, 2, 4.0f, 6);
            return;
        }
        break;
    case 0x46A:
        sound_call_00608D70(em, 4, 0x16, 0);
        sound_call_00608D70(em, 0x86, 0x17, 0);
        sound_call_00608D70(em, 0x86, 0x16, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x46B:
        sound_call_00608D70(em, 4, 0x17, 0);
        sound_call_00608D70(em, 0x20, 0x16, 0);
        sound_call_00608D70(em, 0x6E, 0x15, 0x23);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x46C:
        sound_call_00608D70(em, 0x24, 0x17, 0);
        sound_call_00608D70(em, 0x12, 0x16, 0x22);
        sound_call_00608D70(em, 0x46, 0x16, 0x22);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x46D:
        sound_call_00608D70(em, 0xA, 6, 0x14);
        sound_call_00608D70(em, 4, 0x1F, 0x23);
        sound_call_00608D70(em, 0x1E, 0x12, 0x14);
        sound_call_00608D70(em, 0x1E, 0x15, 0);
        sound_call_00608D70(em, 0x1E, 0x14, 0);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 6.0f) == 0) {
            if (em_frame_check(em, 0, 10.0f) != 0) {
                goto block_284;
            }
        } else {
block_284:
            Eft13_set_em_scl(em, 2, 7.0f, 3);
            return;
        }
        break;
    case 0x46E:
        sound_call_00608D70(em, 4, 0x34, 0x23);
        sound_call_00608D70(em, 0x1E, 0x30, 0);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x46F:
        sound_call_00608D70(em, 4, 0x34, 0x23);
        sound_call_00608D70(em, 0x1E, 0x30, 0);
        hire_req_set_0060C0C0(em, w, 0);
        break;
    case 0x470:
        sound_call_00608D70(em, 4, 0x12, 0x1A);
        sound_call_00608D70(em, 0xA, 3, 0x1A);
        sound_call_00608D70(em, 0x70, 4, 0x14);
        sound_call_00608D70(em, 0xD8, 1, 0x1A);
        sound_call_00608D70(em, 0x108, 3, 0x14);
        sound_call_00608D70(em, 0x38, 0x15, 0);
        sound_call_00608D70(em, 0x38, 8, 0);
        sound_call_00608D70(em, 4, 0x1F, 0x23);
        sound_call_00608D70(em, 0x54, 0x16, 0);
        hire_req_set_0060C0C0(em, w, 0);
        if (em_frame_check(em, 0, 54.0f) != 0) {
            shell20_set(em, 0x16);
            return;
        }
        break;
    case 0x471:
        sound_call_00608D70(em, 4, 0x57, 0x23);
        sound_call_00608D70(em, 0x122, 0x2D, 0x23);
        v4[1] = 10.0f;
        v4[2] = 140.0f;
        v4[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v4, 1.6f);
        break;
    case 0x472:
        sound_call_00608D70(em, 0x1A, 0x2B, 0x23);
        sound_call_00608D70(em, 0x78, 0x2A, 0x23);
        break;
    default:
        move_default_00608E20(em);
        break;
    }
}
