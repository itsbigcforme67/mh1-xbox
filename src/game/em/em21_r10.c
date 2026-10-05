/* em21_r10 - monster 21 AI 0x006060F0-0x00607CD4: em_dmg16_006060F0, em_dmg17_006061D0, em_dmg18_006063E0, em_dmg19_00606600, em_dmg20_00606700, em_dmg21, em_dmg22, em_demo00_00606B70, em_die00_00606C90, em_die01_00606E20, em_die02_00606F90, em_die03_006073F0, em_move00_006075D0, em_move01_00607730, em_move02_006077C0, em_move03_006079A0, em_move04_00607A60, em_move05_00607C10, em_move06_00607CA0. Whole file in em21_nm.c. */
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











static void em_dmg16_006060F0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x79, 0xC, 0);
        em_cmd_reset(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x7C, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg17_006061D0(EMW *em, EM21W *w) {
    u8 temp_a1;
    u8 temp_a1_2;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 60.0f;
        em->x3C0[1] = -2.75f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        if (!(em->pos[1] <= em->x7E4)) {
            em->x388 = 2;
            em->x05 += 1;
            swim_eff_set2_0060C260(8.0f, em);
        }
        break;
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x7E4) {
            temp_a1_2 = em->x05;
            em->x05 = temp_a1_2 + 1;
            em->x388 = 4;
            swim_eff_set2_0060C260(8.0f, em);
            return;
        }
        break;
    case 3:
        if (em->x7E8 != 0) {
            if (em->pos[1] <= em->x7E4) {
                em->x05 = temp_a1 + 1;
                em->x388 = 4;
                em->pos[1] = em->x7E4;
                em21_to_swim(em);
                return;
            }
            w->spd[1] = (s32) em->ang[1];
            speed_add_g(em, w->spd);
            if (em->x388 == 2) {
                if (!(em->pos[1] <= em->x7E4)) {
                    em->x388 = 2;
                    return;
                }
                em->x388 = 4;
                return;
            }
        } else if (em->pos[1] < em->x5AC) {
            em->x05 = temp_a1 + 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em21_act_set(em, 4, 0x10, 2);
        }
        break;
    }
}

static void em_dmg18_006063E0(EMW *em, EM21W *w) {
    s32 temp_v0;
    u8 temp_a1;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 60.0f;
        em->x3C0[1] = -2.75f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        Em_Mahi_Start(em);
        em->x8BD = 1;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        if (!(em->pos[1] <= em->x7E4)) {
            em->x388 = 2;
            em->x05 += 1;
            swim_eff_set2_0060C260(8.0f, em);
            return;
        }
    default:
        break;
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x7E4) {
            temp_a1 = em->x05;
            em->x05 = temp_a1 + 1;
            em->x388 = 4;
            swim_eff_set2_0060C260(8.0f, em);
            return;
        }
        break;
    case 3:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] < em->x7E4) {
            em->x05 += 1;
            em->pos[1] = em->x7E4;
            em->x3F4 = 0;
            em->x388 = 4;
            em_char_set(em, 0x82, 0, 0);
            return;
        }
        break;
    case 4:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em21_act_set(em, 2, 0x15, 4);
        }
        em_mahi_eff_set(em, 2);
        break;
    case 5:
        if (em->x8C3 == 0) {
            em21_act_set(em, 2, 0x15, 4);
        }
        em_mahi_eff_set(em, 2);
        break;
    }
}

static void em_dmg19_00606600(EMW *em, EM21W *w) {
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
            em21_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg20_00606700(EMW *em, EM21W *w) {
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
            em21_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

void em_dmg21(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x7C, 0, 0);
        em->x388 = 1;
        em_cmd_reset(em);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

void em_dmg22(EMW *em, EM21W *w) {
    f32 v[3];
    f32 v2[3];
    s32 temp_v0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 60.0f;
        em->x3C0[1] = -2.75f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        swim_eff_set2_0060C260(8.0f, em);
        Em_Sleep2_Start(em);
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        if (!(em->pos[1] <= em->x7E4)) {
            em->x388 = 2;
            em->x05 += 1;
            swim_eff_set2_0060C260(8.0f, em);
            return;
        }
    default:
        break;
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x7E4) {
            em->x05 += 1;
            em->x388 = 4;
            return;
        }
        break;
    case 3:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] < em->x7E4) {
            em->x05 += 1;
            em->x3F4 = 0;
            em->x388 = 4;
            em_char_set(em, 0x82, 0, 0);
            em->x88B = 0;
            return;
        }
        break;
    case 4:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em->x88B = 1;
            em21_act_set(em, 2, 0x19, 4);
        }
        v[0] = -150.0f;
        v[1] = 50.0f;
        v[2] = 140.0f;
        em_sleep_eff_set(em, 0x22, v, 1.60000002f);
        if (((s32) em->work08 % 135) == 0) {
            sound_call_sub_00608D00(em, 0x57, 0x23);
            return;
        }
        break;
    case 5:
        if (em->x8C3 == 0) {
            em->x88B = 1;
            em21_act_set(em, 2, 0x19, 4);
        }
        v2[0] = -150.0f;
        v2[1] = 50.0f;
        v2[2] = 140.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.60000002f);
        if (((s32) em->work08 % 135) == 0) {
            sound_call_sub_00608D00(em, 0x57, 0x23);
        }
        break;
    }
}

static void em_demo00_00606B70(EMW *em, EM21W *w) {
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
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    temp_a0 = em->work08 + 1;
    em->work08 = temp_a0;
    if ((temp_a0 % 135) == 0) {
        sound_call_sub_00608D00(em, 0x57, 0x23);
    }
}

static void em_die00_00606C90(EMW *em, EM21W *w) {
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
            em21_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die01_00606E20(EMW *em, EM21W *w) {
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
            em21_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die02_00606F90(EMW *em, EM21W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    s32 temp_v0;
    s32 temp_v1_2;
    u8 temp_a1;
    u8 temp_a1_2;
    u8 temp_a3;
    u8 temp_v1;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a3 = em->x05;
    switch (temp_a3) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        em->x05 = temp_a3 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x7E, 0, 0);
        em_rate_clear(em);
        em->adj_y = 60.0f;
        em->x3C0[1] = -2.75f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 0x1:                                       /* switch 1 */
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        if (!(em->pos[1] <= em->x7E4)) {
            temp_a1 = em->x05;
            em->x05 = temp_a1 + 1;
            em->x388 = 2;
            swim_eff_set2_0060C260(8.0f, em);
        }
        break;
    case 0x2:                                       /* switch 1 */
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x7E4) {
            temp_a1_2 = em->x05;
            em->x05 = temp_a1_2 + 1;
            em->x388 = 4;
            swim_eff_set2_0060C260(8.0f, em);
            return;
        }
        break;
    case 0x3:                                       /* switch 1 */
        if (em->x7E8 != 0) {
            if (em->pos[1] < em->x7E4) {
                em->x05 = temp_a3 + 1;
                em->x388 = 4;
                em->x3C0[1] = 1.0f;
                em_char_set(em, 0x6F, 0, 0);
                Quest_enemy_die(em);
                em->x7E0 = 444.3f;
                return;
            }
            w->spd[1] = (s32) em->ang[1];
            speed_add_g(em, w->spd);
            if (em->x388 == 2) {
                if (!(em->pos[1] < em->x7E4)) {
                    em->x388 = 2;
                    return;
                }
                em->x388 = 4;
                return;
            }
        } else if (em->pos[1] < em->x5AC) {
            em->x05 = 0x63;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em21_act_set(em, 5, 0, 2);
            return;
        }
        break;
    case 0x4:                                       /* switch 1 */
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->adj_y < 5.0f)) {
            em->adj_y = 5.0f;
            em->x3C0[1] = 0.0f;
        }
        temp_f1 = em->x7E4;
        if (!(em->pos[1] < temp_f1)) {
            em->pos[1] = temp_f1;
            em->act_spd = 0.0f;
            em->x05 += 1;
            em->x06 = 0;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
            return;
        }
        break;
    case 0x5:                                       /* switch 1 */
        temp_v1 = em->x06;
        switch (temp_v1) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            em->x06 = temp_v1 + 1;
            em->adj_y = -0.5f;
            em->work08 = ((u16)ran_suu(0) & 3) * 10;
            break;
        case 1:                                     /* switch 2 */
            w->spd[1] = (s32) em->ang[1];
            speed_add(em, w->spd);
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                em->x06 += 1;
                em->adj_y = 1.0f;
            }
            break;
        case 2:                                     /* switch 2 */
            w->spd[1] = (s32) em->ang[1];
            speed_add(em, w->spd);
            temp_f1_2 = em->x7E4;
            if (!(em->pos[1] < temp_f1_2)) {
                em->pos[1] = temp_f1_2;
                em->x06 = 0;
            }
            break;
        }
        Em_hagi_point_cnt_ck(em);
        break;
    case 0x6:                                       /* switch 1 */
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x04 += 1;
            em->x01 = 0;
            return;
        }
        em->x798 = (f32) em->work08 / 150.0f;
        break;
    case 0x63:                                      /* switch 1 */
        if (em->x194 == 0) {
            em->x388 = 4;
            em21_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_die03_006073F0(EMW *em, EM21W *w) {
    s32 temp_v1;
    u8 temp_a0;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a0 + 1;
        em_char_set(em, 0x4B, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05 = temp_a0 + 1;
            em_char_set(em, 0x80, 0, 0);
        }
        break;
    case 0x2:
        if (em->x194 == 0) {
            em->x05 = temp_a0 + 1;
            em_char_set(em, 0x7F, 0, 0);
        }
        /* fallthrough */
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
            em21_act_sub(em, 0xFF);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

void em_move00_006075D0(EMW *em, EM21W *w) {
    switch (em->x15) {
    case 0:
        em_act00_00600490(em, w);
        break;
    case 1:
        em_act01_00600570(em, w);
        break;
    case 2:
        em_act02_00600650(em, w);
        break;
    case 4:
        em_act04_00600700(em, w);
        break;
    case 5:
        em_act05_006007F0(em, w);
        break;
    case 6:
        em_act06_00600900(em, w);
        break;
    case 7:
        em_act07_006009D0(em, w);
        break;
    case 8:
        em_act08_00600A50(em, w);
        break;
    case 10:
        em_act10_00600B90(em, w);
        break;
    case 12:
        em_act12_00600CD0(em, w);
        break;
    case 13:
        em_act13_00600DE0(em, w);
        break;
    case 14:
        em_act14_00600EB0(em, w);
        break;
    case 15:
        em_act15_00601000(em, w);
        break;
    case 16:
        em_act16_006010D0(em, w);
        break;
    case 17:
        em_act17_00601220(em, w);
        break;
    case 18:
        em_act18_006012F0(em, w);
        break;
    case 19:
        em_act19_00601440(em, w);
        break;
    case 20:
        em_act20_00601510(em, w);
        break;
    }
}

void em_move01_00607730(EMW *em, EM21W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_mv00_006015A0(em, w);
        break;
    case 1:
        em_mv01_00601700(em, w);
        break;
    case 2:
        em_mv02_00601870(em, w);
        break;
    case 3:
        em_mv03_00601AC0(em, w);
        break;
    }
}

void em_move02_006077C0(EMW *em, EM21W *w) {
    switch (em->x15) {
    case 0:
        em_fly00_00601C20(em, w);
        break;
    case 1:
        em_fly01_00601CB0(em, w);
        break;
    case 2:
        em_fly02_00601D80(em, w);
        break;
    case 3:
        em_fly03_00601D90(em, w);
        break;
    case 4:
        em_fly04_00602030(em, w);
        break;
    case 5:
        em_fly05_00602280(em, w);
        break;
    case 6:
        em_fly06_00602450(em, w);
        break;
    case 7:
        em_fly07_006024F0(em, w);
        break;
    case 8:
        em_fly08_00602640(em, w);
        break;
    case 9:
        em_fly09_00602780(em, w);
        break;
    case 10:
        em_fly10_00602A70(em, w);
        break;
    case 11:
        em_fly11_00602CC0(em, w);
        break;
    case 12:
        em_fly12_00602F10(em, w);
        break;
    case 13:
        em_fly13_00603000(em, w);
        break;
    case 14:
        em_fly14_006030F0(em, w);
        break;
    case 15:
        em_fly15_006033A0(em, w);
        break;
    case 16:
        em_fly16_00603460(em, w);
        break;
    case 17:
        em_fly17_00603720(em, w);
        break;
    case 18:
        em_fly18_006038C0(em, w);
        break;
    case 19:
        em_fly19_00603B90(em, w);
        break;
    case 20:
        em_fly20_00603E40(em, w);
        break;
    case 21:
        em_fly21_00604110(em, w);
        break;
    case 22:
        em_fly22_006041A0(em, w);
        break;
    case 23:
        em_fly23_00604340(em, w);
        break;
    case 24:
        em_fly24_00604440(em, w);
        break;
    case 25:
        em_fly25_006044D0(em, w);
        break;
    }
}

void em_move03_006079A0(EMW *em, EM21W *w) {
    switch (em->x15) {
    case 0:
        em_atk00_00604560(em, w);
        break;
    case 1:
        em_atk01_006045E0(em, w);
        break;
    case 2:
        em_atk02_006046C0(em, w);
        break;
    case 3:
        em_atk03_006047B0(em, w);
        break;
    case 4:
        em_atk04_006048C0(em, w);
        break;
    case 5:
        em_atk05_006049D0(em, w);
        break;
    case 6:
        em_atk06_00604AE0(em, w);
        break;
    case 7:
        em_atk07_00604DC0(em, w);
        break;
    }
}

void em_move04_00607A60(EMW *em, EM21W *w) {
    switch (em->x15) {
    case 0:
        em_dmg00_00604E40(em, w);
        break;
    case 1:
        em_dmg01_00604F20(em, w);
        break;
    case 2:
        em_dmg02_00604FB0(em, w);
        break;
    case 3:
        em_dmg03_00605040(em, w);
        break;
    case 4:
        em_dmg04_006050D0(em, w);
        break;
    case 5:
        em_dmg05_006051D0(em, w);
        break;
    case 6:
        em_dmg06_00605370(em, w);
        break;
    case 7:
        em_dmg07_006054D0(em, w);
        break;
    case 8:
        em_dmg08_00605630(em, w);
        break;
    case 9:
        em_dmg09_00605770(em, w);
        break;
    case 10:
        em_dmg10_006058E0(em, w);
        break;
    case 11:
        em_dmg11_00605970(em, w);
        break;
    case 12:
        em_dmg12_00605A50(em, w);
        break;
    case 13:
        em_dmg13_00605B70(em, w);
        break;
    case 14:
        em_dmg14_00605CA0(em, w);
        break;
    case 15:
        em_dmg15_00605DC0(em, w);
        break;
    case 16:
        em_dmg16_006060F0(em, w);
        break;
    case 17:
        em_dmg17_006061D0(em, w);
        break;
    case 18:
        em_dmg18_006063E0(em, w);
        break;
    case 19:
        em_dmg19_00606600(em, w);
        break;
    case 20:
        em_dmg20_00606700(em, w);
        break;
    case 21:
        em_dmg21(em, w);
        break;
    case 22:
        em_dmg22(em, w);
        break;
    }
}

void em_move05_00607C10(EMW *em, EM21W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_die00_00606C90(em, w);
        break;
    case 1:
        em_die01_00606E20(em, w);
        break;
    case 2:
        em_die02_00606F90(em, w);
        break;
    case 3:
        em_die03_006073F0(em, w);
        break;
    }
}

void em_move06_00607CA0(EMW *em, EM21W *w) {
    switch (em->x15) {
    case 0:
        em_demo00_00606B70(em, w);
        break;
    }
}
