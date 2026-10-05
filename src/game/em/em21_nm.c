/* em21 draft */
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
static void em_act00_00600490(EMW *em, EM21W *w);
static void em_act01_00600570(EMW *em, EM21W *w);
static void em_act02_00600650(EMW *em, EM21W *w);
static void em_act04_00600700(EMW *em, EM21W *w);
static void em_act05_006007F0(EMW *em, EM21W *w);
static void em_act06_00600900(EMW *em, EM21W *w);
static void em_act07_006009D0(EMW *em, EM21W *w);
static void em_act08_00600A50(EMW *em, EM21W *w);
static void em_act10_00600B90(EMW *em, EM21W *w);
static void em_act12_00600CD0(EMW *em, EM21W *w);
static void em_act13_00600DE0(EMW *em, EM21W *w);
static void em_act14_00600EB0(EMW *em, EM21W *w);
static void em_act15_00601000(EMW *em, EM21W *w);
static void em_act16_006010D0(EMW *em, EM21W *w);
static void em_act17_00601220(EMW *em, EM21W *w);
static void em_act18_006012F0(EMW *em, EM21W *w);
static void em_act19_00601440(EMW *em, EM21W *w);
static void em_act20_00601510(EMW *em, EM21W *w);
static void em_mv00_006015A0(EMW *em, EM21W *w);
static void em_mv01_00601700(EMW *em, EM21W *w);
static void em_mv02_00601870(EMW *em, EM21W *w);
static void em_mv03_00601AC0(EMW *em, EM21W *w);
static void em_fly00_00601C20(EMW *em, EM21W *w);
static void em_fly01_00601CB0(EMW *em, EM21W *w);
static void em_fly02_00601D80(EMW *em, EM21W *w);
static void em_fly03_00601D90(EMW *em, EM21W *w);
static void em_fly04_00602030(EMW *em, EM21W *w);
static void em_fly05_00602280(EMW *em, EM21W *w);
static void em_fly06_00602450(EMW *em, EM21W *w);
static void em_fly07_006024F0(EMW *em, EM21W *w);
static void em_fly08_00602640(EMW *em, EM21W *w);
static void em_fly09_00602780(EMW *em, EM21W *w);
static void em_fly10_00602A70(EMW *em, EM21W *w);
static void em_fly11_00602CC0(EMW *em, EM21W *w);
static void em_fly12_00602F10(EMW *em, EM21W *w);
static void em_fly13_00603000(EMW *em, EM21W *w);
static void em_fly14_006030F0(EMW *em, EM21W *w);
static void em_fly15_006033A0(EMW *em, EM21W *w);
static void em_fly16_00603460(EMW *em, EM21W *w);
static void em_fly17_00603720(EMW *em, EM21W *w);
static void em_fly18_006038C0(EMW *em, EM21W *w);
static void em_fly19_00603B90(EMW *em, EM21W *w);
static void em_fly20_00603E40(EMW *em, EM21W *w);
static void em_fly21_00604110(EMW *em, EM21W *w);
static void em_fly22_006041A0(EMW *em, EM21W *w);
static void em_fly23_00604340(EMW *em, EM21W *w);
static void em_fly24_00604440(EMW *em, EM21W *w);
static void em_fly25_006044D0(EMW *em, EM21W *w);
static void em_atk00_00604560(EMW *em, EM21W *w);
static void em_atk01_006045E0(EMW *em, EM21W *w);
static void em_atk02_006046C0(EMW *em, EM21W *w);
static void em_atk03_006047B0(EMW *em, EM21W *w);
static void em_atk04_006048C0(EMW *em, EM21W *w);
static void em_atk05_006049D0(EMW *em, EM21W *w);
static void em_atk06_00604AE0(EMW *em, EM21W *w);
static void em_atk07_00604DC0(EMW *em, EM21W *w);
static void em_dmg00_00604E40(EMW *em, EM21W *w);
static void em_dmg01_00604F20(EMW *em, EM21W *w);
static void em_dmg02_00604FB0(EMW *em, EM21W *w);
static void em_dmg03_00605040(EMW *em, EM21W *w);
static void em_dmg04_006050D0(EMW *em, EM21W *w);
static void em_dmg05_006051D0(EMW *em, EM21W *w);
static void em_dmg06_00605370(EMW *em, EM21W *w);
static void em_dmg07_006054D0(EMW *em, EM21W *w);
static void em_dmg08_00605630(EMW *em, EM21W *w);
static void em_dmg09_00605770(EMW *em, EM21W *w);
static void em_dmg10_006058E0(EMW *em, EM21W *w);
static void em_dmg11_00605970(EMW *em, EM21W *w);
static void em_dmg12_00605A50(EMW *em, EM21W *w);
static void em_dmg13_00605B70(EMW *em, EM21W *w);
static void em_dmg14_00605CA0(EMW *em, EM21W *w);
static void em_dmg15_00605DC0(EMW *em, EM21W *w);
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
static void em_move00_006075D0(EMW *em, EM21W *w);
static void em_move02_006077C0(EMW *em, EM21W *w);
static void em_move03_006079A0(EMW *em, EM21W *w);
static void em_move04_00607A60(EMW *em, EM21W *w);
static void em_move01_00607730(EMW *em, EM21W *w);
static void em_move05_00607C10(EMW *em, EM21W *w);
static void em_move06_00607CA0(EMW *em, EM21W *w);
void em21_uvmove(EMW *em);
static void sound_call_sub_00608D00(EMW *em, int se, int joint);
static void sound_call_00608D70(EMW *em, int frame, int se, int joint);
static void quake_call_00608DD0(EMW *em, int frame, int arg);
static void move_default_00608E20(EMW *em);
static void ef_move_sub_00608E70(EMW *em, EM21W *w);
void Em_set_quake_sub(EMW *, int);
static void hire_move_sub2_0060BAD0(EMW *em, EM21W *w, int i);
static void hire_move_sub1_0060BCA0(EMW *em, EM21W *w, int i);
static void hire_move_0060BFE0(EMW *em, EM21W *w);
void em21_effect_move(EMW *em);
static void hire_req_set_0060C0C0(EMW *em, EM21W *w, u8 mode);
static void ground_land_eff_set_0060C180(EMW *em);
static void swim_eff_set_0060C1A0(f32 scale, EMW *em);
static void swim_eff_set2_0060C260(f32 scale, EMW *em);
void swim_eff_set3(f32 scale, EMW *em);
void dummy_em_prog_0060C390(void);


void em21_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *) em, 0);
}

void em21_init(EMW *em) {
    EM21W *w = (EM21W *)em->ex;
    s16 temp_v0;
    u8 temp_a0;
    u8 temp_a1;

    em_char_set(em, 1, 0, 0);
    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
        switch (game_w.stage) {
        case 0x36:
            em->pos[0] = 6000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 10000.0f;
            em->ang[1] = 0x4000;
            em->x388 = 4;
            em21_act_set(em, 2, 0, 0);
            break;
        default:
            em->pos[0] = 5000.0f + 1500.0f * (f32)(u32)em->x13;
            em->pos[1] = 0.0f;
            em->pos[2] = 9000.0f;
            em->x388 = 4;
            em21_act_set(em, 2, 0, 0);
            break;
        }
    } else {
        switch (game_w.stage) {
        case 0x36:
            em->x388 = 4;
            em21_act_set(em, 2, 0, 0);
            break;
        case 0x2D:
            em->x388 = 4;
            em21_act_set(em, 2, 0, 0);
            break;
        default:
            em->x388 = 4;
            em21_act_set(em, 2, 0, 0);
            break;
        }
    }
    w->x10 = 0;
    temp_v0 = em_hp_vital_set2(em, 0x4B0, 0x514);
    em->x302 = temp_v0;
    em->x792 = temp_v0;
    em->x839 = 1;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em21_stay_timer_tbl[em->stg];
    em->runaway_tm = em21_runaway_timer_tbl[em->stg];
    em->x7E0 = 670.0f;
    w->tgt_ang = 0x4000;
    w->x28 = 0x100;
    w->x2C = 0x200;
    w->x30 = 0x100;
    w->x18 = 0x2000;
    w->x14 = 0;
    w->x38 = 0;
    w->x39 = 0;
    w->x3A = 0;
    temp_a0 = em->x948 & 1;
    em->x948 = temp_a0;
    if (temp_a0 == 0) {
        temp_a1 = em->kind;
        switch (temp_a1) {
        case 1:
        case 6:
        case 0xB:
        case 0xF:
        case 0xE:
        case 0x11:
        case 0x15:
        case 0x16:
            em->ex[0xA3] = 0;
            eft09_set(em, temp_a1, 0x100, 0x4000);
            break;
        case 0x14:
            break;
        }
    }
}

void em21_to_normal(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 0;
    em->x3F4 = 0;
    em->x839 = 1;
    if (em->x302 < ((s16)(0.2f * (f32) em->x792))) {
        em21_act_set(em, 0, 1, 0);
        return;
    }
    em21_act_set(em, 0, 1, 0);
}

void em21_to_swim(EMW *em) {
    em->act_spd = 1.0f;
    em->x388 = 4;
    em->x3F4 = 0;
    em->x839 = 1;
    em21_act_set(em, 2, 0, 0);
}

s32 em21_act_sub(EMW *em, s32 arg1) {
    if (EM_LYR(em, arg1) == 0 || arg1 == 0xFF) {
        em21_to_normal(em);
        return 1;
    }
    return 0;
}

static void em_act00_00600490(EMW *em, EM21W *w) {
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

static void em_act01_00600570(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        if (em->char0 != 0x3E9) {
            em_char_set2(em, 0x3E9, 0, 0, 0);
        }
        if (em->x2DE != 0x4B1) {
            em_char_set2(em, 0x4B1, 0, 0, 1);
        }
        if (em->x2E0 != 0x579) {
            em_char_set2(em, 0x579, 0, 0, 2);
        }
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_act02_00600650(EMW *em, EM21W *w) {
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x19, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0x5C);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_act04_00600700(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x79, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x388 = 1;
            em_char_set(em, 0x7A, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x388 = 0;
            em_char_set(em, 0x7C, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_act05_006007F0(EMW *em, EM21W *w) {
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
            em21_act_set(em, 0, 6, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 6, 4);
        }
        break;
    }
}

static void em_act06_00600900(EMW *em, EM21W *w) {
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
            em21_act_set(em, 0, 7, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act07_006009D0(EMW *em, EM21W *w) {
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
            em21_to_normal(em);
        }
        break;
    }
}

static void em_act08_00600A50(EMW *em, EM21W *w) {
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
            em21_act_set(em, 4, 0x13, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 0x13, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_00608D00(em, 0x57, 0x23);
    }
}

static void em_act10_00600B90(EMW *em, EM21W *w) {
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
            em21_act_set(em, 4, 0x14, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 0x14, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.6f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_00608D00(em, 0x57, 0x23);
    }
}

static void em_act12_00600CD0(EMW *em, EM21W *w) {
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
            em21_act_set(em, 0, 0xD, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 0xD, 4);
        }
        break;
    }
}

static void em_act13_00600DE0(EMW *em, EM21W *w) {
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
            em21_act_set(em, 0, 7, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act14_00600EB0(EMW *em, EM21W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 1;
        em->x3F4 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x80, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x89, 0, 0);
            return;
        }
        break;
    case 3:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em21_act_set(em, 0, 0xF, 4);
            return;
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 0xF, 4);
        }
        break;
    }
}

static void em_act15_00601000(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x7C, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_act_set(em, 0, 7, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act16_006010D0(EMW *em, EM21W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x80, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x89, 0, 0);
            return;
        }
        break;
    case 3:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em21_act_set(em, 0, 0x11, 4);
            return;
        }
        break;
    case 4:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 0x11, 4);
        }
        break;
    }
}

static void em_act17_00601220(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em_char_set(em, 0x7C, 0, 0);
        em_range_set(em, 0);
        em->x88B = 1;
        Em_Sleep2_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_act_set(em, 0, 7, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act18_006012F0(EMW *em, EM21W *w) {
    s16 temp_a0;
    s16 temp_v1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x20, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        em->x762 = 2;
        em->work08 = 0x708;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x1F, 0, 0);
        }
        break;
    case 2:
        temp_a0 = em->x792;
        temp_v1 = em->x302;
        if (temp_v1 < temp_a0) {
            em->x302 = temp_v1 + 1;
        } else {
            em->x302 = temp_a0;
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em->x302 = em->x792;
            em_hinshi_end(em);
            em21_act_set(em, 0, 0x13, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 0x13, 4);
        }
        break;
    }
}

static void em_act19_00601440(EMW *em, EM21W *w) {
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
            em21_act_set(em, 0, 7, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 7, 4);
        }
        break;
    }
}

static void em_act20_00601510(EMW *em, EM21W *w) {
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
            em21_to_normal(em);
        }
        break;
    }
}

static void em_mv00_006015A0(EMW *em, EM21W *w) {
    f32 sp30[3];
    int d;
    f32 temp_f1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
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
            em21_to_normal(em);
        }
        break;
    }
}

static void em_mv01_00601700(EMW *em, EM21W *w) {
    f32 sp30[3];
    int d;
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 4, 0, 0);
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
            em_char_set(em, 0x85, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_mv02_00601870(EMW *em, EM21W *w) {
    s32 temp_t0;
    u16 temp_a2;
    u32 temp_s0;
    u32 temp_v1;
    u8 temp_a3;

    temp_a3 = em->x05;
    switch (temp_a3) {                              /* irregular */
    case 0:
        em->x05 = temp_a3 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
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
            temp_t0 = em->ang[1];
            temp_a2 = w->tgt_ang;
            temp_s0 = (temp_a2 - (temp_t0 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_s0 + 0x1D4) & 0xFFFF) < 0x3A8) {
                    em->x05 = temp_a3 + 1;
                    pl_flag_clr((PLW *) em, 0x20000);
                    em21_to_normal(em);
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
            if ((u32) ((temp_s0 + 0x1D4) & 0xFFFF) < 0x3A8) {
                em->ang[1] = (s32) temp_a2;
                return;
            }
            if (temp_s0 < 0x8000) {
                em->ang[1] = (temp_t0 + 0x1D4) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_t0 - 0x1D4) & 0xFFFF;
        } else {
            return;
        }
        break;
    }
}

static void em_mv03_00601AC0(EMW *em, EM21W *w) {
    f32 sp30[3];
    int d;
    f32 temp_f1;
    s32 temp_v1_2;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
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
            em21_to_normal(em);
        }
        break;
    }
}

static void em_fly00_00601C20(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x64, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly01_00601CB0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x64, 0, 0);
        em->x388 = 4;
        em->stay_tm = em21_stay_timer_tbl[em->stg];
        em->runaway_tm = em21_runaway_timer_tbl[em->stg];
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly02_00601D80(EMW *em, EM21W *w) {

}

static void em_fly03_00601D90(EMW *em, EM21W *w) {
    f32 temp_f1;
    u32 var_t0;
    s32 temp_a3;
    u16 temp_a1;
    u32 temp_a2;
    u32 temp_v1;
    u8 temp_a0;

    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if ((temp_v1 < 0x2AA9) || (temp_v1 >= 0xD558)) {
            em_char_set(em, 0x65, 0, 0);
            return;
        }
        if (temp_v1 >= 0x8000) {
            em_char_set(em, 0x6A, 0, 0);
            return;
        }
        em_char_set(em, 0x6B, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            temp_f1 = (32768.0f / (em->x1A8 / 2.0f)) * em->act_spd;
            var_t0 = (u32)temp_f1;
            temp_a3 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_a2 = (temp_a1 - (temp_a3 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_a2 + var_t0) & 0xFFFF) < (u32) (var_t0 * 2)) {
                    em->x05 += 1;
                    em21_to_swim(em);
                    return;
                }
                if ((temp_a2 < 0x2AA9) || (temp_a2 >= 0xD558)) {
                    em_char_set(em, 0x65, 0, 0);
                    return;
                }
                if (temp_a2 >= 0x8000) {
                    em_char_set(em, 0x6A, 0, 0);
                    return;
                }
                em_char_set(em, 0x6B, 0, 0);
                return;
            }
            if ((u32) ((temp_a2 + var_t0) & 0xFFFF) < (u32) (var_t0 * 2)) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_a2 < 0x8000) {
                em->ang[1] = (temp_a3 + var_t0) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a3 - var_t0) & 0xFFFF;
        } else {
            return;
        }
        break;
    }
}

static void em_fly04_00602030(EMW *em, EM21W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    s32 temp_a0;
    s32 temp_v1;
    s32 var_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x65, 0, 0);
        em_rate_clear(em);
        em->adj_z = 5.0f;
        em->x3C0[2] = 1.0f;
        break;
    case 1:
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (!(em->adj_z < 50.0f)) {
            em->adj_z = 50.0f;
        }
        if (w->has_tgt != 0) {
            temp_a0 = em->ang[1];
            temp_v1 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - temp_a0) & 0xFFFF;
            if (temp_v1 < 0x8001) {
                if (temp_v1 < 0x40) {
                    var_v1 = temp_a0 + temp_v1;
                } else {
                    var_v1 = temp_a0 + 0x40;
                }
            } else if (temp_v1 >= 0xFFC1) {
                var_v1 = temp_a0 + temp_v1;
            } else {
                var_v1 = temp_a0 - 0x40;
            }
            em->ang[1] = var_v1;
        }
        temp_f1 = w->dist - em->adj_z;
        w->dist = temp_f1;
        if (temp_f1 <= 0.0f) {
            em->x05 = 0xA;
            em21_to_swim(em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x66, 0, 0);
        }
        break;
    case 2:
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (!(em->adj_z < 50.0f)) {
            em->adj_z = 50.0f;
        }
        temp_f1_2 = w->dist - em->adj_z;
        w->dist = temp_f1_2;
        if (temp_f1_2 <= 0.0f) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly05_00602280(EMW *em, EM21W *w) {
    int d;
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em_rate_clear(em);
        em->adj_z = 30.0f;
        em->x3C0[2] = 10.0f;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            xang_calc_target(em, w->spd, 0.0f, 0.0f);
            w->spd[1] = (s32) em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            if (!(em->adj_z < 100.0f)) {
                em->adj_z = 100.0f;
            }
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
            temp_f1 = w->dist - em->adj_z;
            w->dist = temp_f1;
            if (temp_f1 <= 0.0f) {
                em->x05 += 1;
                em21_to_swim(em);
            }
            swim_eff_set_0060C1A0(8.0f, em);
            swim_eff_set3(8.0f, em);
        }
        break;
    }
}

static void em_fly06_00602450(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x6E, 0, 0);
        em21_fly_adjy2_init(em, 0);
        break;
    case 1:
        if (em21_fly_adjy2(em)) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly07_006024F0(EMW *em, EM21W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x68, 0, 0);
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->adj_y = 2.0f;
        em->x3C0[1] = 4.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->adj_y <= 50.0f)) {
            em->adj_y = 50.0f;
            em->x3C0[1] = 0.0f;
        }
        temp_f1 = em->x7E4;
        if (!(em->pos[1] <= temp_f1)) {
            em->pos[1] = temp_f1;
            em->x05 += 1;
            em_char_set(em, 0x86, 0, 0);
            swim_eff_set2_0060C260(8.0f, em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly08_00602640(EMW *em, EM21W *w) {
    f32 temp_f1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x69, 0, 0);
        em_rate_clear(em);
        em->adj_z = 20.0f;
        em->adj_y = -2.0f;
        em->x3C0[1] = -4.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (em->adj_y < -50.0f) {
            em->adj_y = -50.0f;
            em->x3C0[1] = 0.0f;
        }
        temp_f1 = em->x5AC;
        if (em->pos[1] < temp_f1) {
            em->pos[1] = temp_f1;
            em->x05 += 1;
            em_char_set(em, 0x87, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly09_00602780(EMW *em, EM21W *w) {
    f32 temp_f1;
    u32 var_t0;
    s32 temp_a3;
    s32 temp_v1_2;
    u16 temp_a1;
    u32 temp_a2_2;
    u32 temp_v1;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        temp_v1 = (w->tgt_ang - em->ang[1]) & 0xFFFF;
        if ((temp_v1 < 0x2AA9) || (temp_v1 >= 0xD558)) {
            em_char_set(em, 0x65, 0, 0);
            return;
        }
        if (temp_v1 >= 0x8000) {
            em_char_set(em, 0x6A, 0, 0);
            return;
        }
        em_char_set(em, 0x6B, 0, 0);
        break;
    case 1:
        if (em->x1C4 == 0) {
            temp_f1 = (32768.0f / (em->x1A8 / 2.0f)) * em->act_spd;
            var_t0 = (u32)temp_f1;
            temp_a3 = em->ang[1];
            temp_a1 = w->tgt_ang;
            temp_a2_2 = (temp_a1 - (temp_a3 & 0xFFFF)) & 0xFFFF;
            if (em->x194 == 0) {
                if ((u32) ((temp_a2_2 + var_t0) & 0xFFFF) < (u32) (var_t0 * 2)) {
                    em->x05 += 1;
                    em->work08 = 0x12C;
                    em21_act_set(em, 2, 0xA, 1);
                    return;
                }
                if ((temp_a2_2 < 0x2AA9) || (temp_a2_2 >= 0xD558)) {
                    em_char_set(em, 0x65, 0, 0);
                    return;
                }
                if (temp_a2_2 >= 0x8000) {
                    em_char_set(em, 0x6A, 0, 0);
                    return;
                }
                em_char_set(em, 0x6B, 0, 0);
                return;
            }
            if ((u32) ((temp_a2_2 + var_t0) & 0xFFFF) < (u32) (var_t0 * 2)) {
                em->ang[1] = (s32) temp_a1;
                return;
            }
            if (temp_a2_2 < 0x8000) {
                em->ang[1] = (temp_a3 + var_t0) & 0xFFFF;
                return;
            }
            em->ang[1] = (temp_a3 - var_t0) & 0xFFFF;
        }
        break;
    case 2:
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->work08 = 0x12C;
            if (em->x8C3 == 0) {
                em21_act_set(em, 2, 0xA, 1);
            }
        }
        break;
    }
}

static void em_fly10_00602A70(EMW *em, EM21W *w) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_a1;
    u8 temp_v0_2;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em->x05 += 1;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
        }
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_9;
            }
        } else {
block_9:
            temp_v0_2 = w->x07 - 1;
            w->x07 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em->x05 += 1;
                em21_to_swim(em);
            } else {
                em->x883 += 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)(((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f));
                em_act_set(em, 2, 0xA);
            }
        }
        temp_v1 = em->ang[1];
        temp_v0_3 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - temp_v1) & 0xFFFF;
        if (temp_v0_3 < 0x8001) {
            if (temp_v0_3 < 0x200) {
                var_v0 = temp_v1 + temp_v0_3;
            } else {
                var_v0 = temp_v1 + 0x200;
            }
        } else if (temp_v0_3 >= 0xFE01) {
            var_v0 = temp_v1 + temp_v0_3;
        } else {
            var_v0 = temp_v1 - 0x200;
        }
        em->ang[1] = var_v0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        swim_eff_set_0060C1A0(8.0f, em);
        swim_eff_set3(8.0f, em);
        break;
    }
}

static void em_fly11_00602CC0(EMW *em, EM21W *w) {
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_a1;
    u8 temp_v0_2;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x388 = 4;
        em_char_set(em, 0x67, 0, 0);
        em->x05 += 1;
        em->x07 = 0;
        em->x3F4 = 0;
        em->rate_x = 0.0f;
        em->adj_y = 0.0f;
        em_rate_clear_g(em);
        if (em->adj_z <= 50.0f) {
            em->adj_z = 50.0f;
        }
        break;
    case 1:
        if (!(CalcDistanceXZ(em->pos, em->tgt_pos) <= 300.0f)) {
            temp_v0 = em->work08 - 1;
            em->work08 = temp_v0;
            if (temp_v0 <= 0) {
                goto block_9;
            }
        } else {
block_9:
            temp_v0_2 = w->x07 - 1;
            w->x07 = temp_v0_2;
            if ((temp_v0_2 & 0xFF) <= 0) {
                em->x05 += 1;
                em21_to_swim(em);
            } else {
                em->x883 -= 1;
                em->x883 &= 3;
                target_kind_set(em, em->tgt_pos);
                em->work08 = (s32)(((1.5f * CalcDistanceXZ(em->pos, em->tgt_pos)) / 50.0f));
                em_act_set(em, 2, 0xB);
            }
        }
        temp_v1 = em->ang[1];
        temp_v0_3 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - temp_v1) & 0xFFFF;
        if (temp_v0_3 < 0x8001) {
            if (temp_v0_3 < 0x200) {
                var_v0 = temp_v1 + temp_v0_3;
            } else {
                var_v0 = temp_v1 + 0x200;
            }
        } else if (temp_v0_3 >= 0xFE01) {
            var_v0 = temp_v1 + temp_v0_3;
        } else {
            var_v0 = temp_v1 - 0x200;
        }
        em->ang[1] = var_v0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        swim_eff_set_0060C1A0(8.0f, em);
        swim_eff_set3(8.0f, em);
        break;
    }
}

static void em_fly12_00602F10(EMW *em, EM21W *w) {
    s32 temp_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x72, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        em->ang[1] += 0x1F0;
        em->ang[1] = (s32) (u16) em->ang[1];
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
}

static void em_fly13_00603000(EMW *em, EM21W *w) {
    s32 temp_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x73, 0, 0);
        em->work08 = 0x84;
        em_rate_clear(em);
        em->adj_z = 40.0f;
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        em->ang[1] -= 0x1F0;
        em->ang[1] = (s32) (u16) em->ang[1];
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    }
}

static void em_fly14_006030F0(EMW *em, EM21W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->adj_y = 130.0f;
        em->adj_z = 70.0f;
        em->x3C0[1] = -4.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 0x1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->pos[1] < em->x7E4)) {
            if (em->x388 == 4) {
                swim_eff_set2_0060C260(8.0f, em);
            }
            em->x388 = 2;
        }
        if (em->adj_y < 0.0f) {
            if (em->x7E8 != 0) {
                if (em->pos[1] < em->x7E4) {
                    em->x05 += 1;
                    em->pos[1] = em->x7E4;
                    em->x388 = 4;
                    em->adj_y = 0.0f;
                    em->x3C0[1] = 0.0f;
                    em_char_set(em, 0x67, 0xA, 0);
                    w->dist = 1000.0f;
                    em->work08 = 0x3C;
                    temp_v0 = em->work08;
                    temp_v1 = temp_v0 * temp_v0;
                    var_v0 = temp_v1 >> 1;
                    if (temp_v1 < 0) {
                        var_v0 = (s32) (temp_v1 + 1) >> 1;
                    }
                    em->x3C0[2] = (w->dist - ((f32) temp_v0 * em->adj_z)) / (f32) var_v0;
                    swim_eff_set2_0060C260(8.0f, em);
                    return;
                }
            } else if (em->pos[1] < em->x5AC) {
                em->x05 = 0x63;
                em->pos[1] = em->x5AC;
                em21_act_set(em, 0, 4, 4);
                return;
            }
        } else {
            return;
        }
        break;
    case 0x2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_f0 = w->dist - em->adj_z;
        w->dist = temp_f0;
        if ((temp_f0 <= 0.0f) || (em->adj_z <= 0.0f)) {
            em->x05 += 1;
            em21_to_swim(em);
            return;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em21_act_set(em, 0, 4, 4);
        }
        break;
    }
}

static void em_fly15_006033A0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x79, 0, 0);
        em_rate_clear(em);
        em->pos[1] = em->x7E4;
        swim_eff_set2_0060C260(8.0f, em);
        break;
    case 1:
        if (em_frame_check(em, 0, 10.0f) != 0) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly16_00603460(EMW *em, EM21W *w) {
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x78, 0, 0);
        em_rate_clear(em);
        break;
    case 0x1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x388 = 2;
            em_char_set(em, 0x77, 0, 0);
            em->adj_y = 50.0f;
            em->adj_z = 100.0f;
            em->x3C0[1] = -4.0f;
        }
        break;
    case 0x2:
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x7E4) {
            em->x388 = 4;
        }
        if (em->adj_y < 0.0f) {
            if (em->x7E8 != 0) {
                if (em->pos[1] < em->x7E4) {
                    em->x05 += 1;
                    em->pos[1] = em->x7E4;
                    em->x388 = 4;
                    em->adj_y = 0.0f;
                    em->x3C0[1] = 0.0f;
                    em_char_set(em, 0x67, 0xA, 0);
                    w->dist = 1000.0f;
                    em->work08 = 0x3C;
                    temp_v0 = em->work08;
                    temp_v1 = temp_v0 * temp_v0;
                    var_v0 = temp_v1 >> 1;
                    if (temp_v1 < 0) {
                        var_v0 = (s32) (temp_v1 + 1) >> 1;
                    }
                    em->x3C0[2] = (w->dist - ((f32) temp_v0 * em->adj_z)) / (f32) var_v0;
                    swim_eff_set2_0060C260(8.0f, em);
                    return;
                }
            } else if (em->pos[1] < em->x5AC) {
                em->x05 = 0x63;
                em->pos[1] = em->x5AC;
                em21_act_set(em, 0, 4, 4);
                return;
            }
        }
        break;
    case 0x3:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_f0 = w->dist - em->adj_z;
        w->dist = temp_f0;
        if ((temp_f0 <= 0.0f) || (em->adj_z <= 0.0f)) {
            em->x05 += 1;
            em21_to_swim(em);
            return;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em21_act_set(em, 0, 4, 4);
        }
        break;
    }
}

static void em_fly17_00603720(EMW *em, EM21W *w) {
    PLW *temp_s0;
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    temp_s0 = &player_work[em->x617];
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x64, 0, 0);
        em->x8B9 = 0;
        em->work08 = 0x78;
        em->x8BD = 1;
        if (Kaeru_ck(temp_s0) == 1) {
            temp_s0->x881 = (s8) em->work08;
            vib_set_pl(temp_s0, 2);
        }
        break;
    case 1:
        if ((em->x8C3 == 0) && ((s16)(act_ck(temp_s0, 0, 0x53)) != 0) && (em->x888 == 0) && (temp_s0->be_flag != 0) && (Pl_stg_ck_tw(em, temp_s0) != 0) && (Kaeru_ck(temp_s0) == 1)) {
            em->x05 += 1;
            em->x8B9 = 1;
            em->x87C = (u8) em->x617;
            em21_act_set(em, 2, 0x12, 1);
            return;
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if ((temp_v1 <= 0) || (em->x888 == 1)) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly18_006038C0(EMW *em, EM21W *w) {
    f32 ofs[3];
    f32 out[3];
    s32 ang[3];
    FLMAT m;
    PLW *temp_s0;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_v1;
    u8 temp_a1;

    temp_s0 = &player_work[em->x617];
    em->x8BB = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x84, 0, 0);
        em->x8B9 = 1;
        em->x87C = (u8) em->x617;
        em->x8BD = 1;
        em->work08 = 0xC3;
        ofs[2] = 700.0f;
        ofs[0] = 0.0f;
        ofs[1] = 0.0f;
        ang[0] = 0;
        ang[1] = temp_s0->ang[1];
        ang[2] = 0;
        cpRotMatrixYXZ2(ang, &m);
        flvecApplyMat33(out, ofs, &m[0][0]);
        em->tgt_pos[0] = temp_s0->pos[0] + out[0];
        em->tgt_pos[1] = temp_s0->pos[1] + out[1];
        em->tgt_pos[2] = temp_s0->pos[2] + out[2];
        em_rate_clear(em);
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->adj_y = (em->tgt_pos[1] - em->pos[1]) / (f32) em->work08;
        em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / (f32) em->work08;
        w->spd[0] = 0;
        w->spd[2] = 0;
        swim_eff_set2_0060C260(8.0f, em);
        vib_set_pl(temp_s0, 2);
        if (Pl_master_ck(temp_s0) == 1) {
            FishWyvernCameraRequest();
        }
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add(em, w->spd);
        temp_a0 = em->work08;
        if ((temp_a0 == 0x3C) || (temp_a0 == 0x78) || (temp_a0 == 0xB4) || (temp_a0 == 0xF0) || (temp_a0 == 0x12C)) {
            swim_eff_set2_0060C260(8.0f, em);
        }
        temp_a0_2 = em->work08;
        if ((temp_a0_2 == 0x1E) || (temp_a0_2 == 0x3C) || (temp_a0_2 == 0x5A) || (temp_a0_2 == 0x78) || (temp_a0_2 == 0x96) || (temp_a0_2 == 0xB4) || (temp_a0_2 == 0xD2) || (temp_a0_2 == 0xF0) || (temp_a0_2 == 0x10E) || (temp_a0_2 == 0x12C)) {
            vib_set_pl(temp_s0, 2);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em21_act_set(em, 4, 0xF, 2);
        }
        break;
    }
}

static void em_fly19_00603B90(EMW *em, EM21W *w) {
    f32 v[3];
    f32 v2[3];
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
        em->x88B = 0;
        Em_Sleep_Start(em);
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
        if (em->pos[1] < em->x7E4) {
            em->x05 = temp_a2 + 1;
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
            em->x88B = 1;
            em21_act_set(em, 2, 0x18, 4);
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
            em21_act_set(em, 2, 0x18, 4);
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

static void em_fly20_00603E40(EMW *em, EM21W *w) {
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

static void em_fly21_00604110(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_char_set(em, 0x83, 0, 0);
        Em_Mahi_End(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly22_006041A0(EMW *em, EM21W *w) {
    f32 temp_f3;
    s32 temp_v1;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 2;
        em_rate_clear(em);
        em_char_set(em, 0x77, 0, 0);
        em->work08 = 0x96;
        em->tgt_pos[0] = 6000.0f;
        em->tgt_pos[1] = -1000.0f;
        em->tgt_pos[2] = 10000.0f;
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->x3C0[1] = -2.0f;
        temp_f3 = (f32) em->work08;
        em->adj_y = ((em->tgt_pos[1] - em->pos[1]) / temp_f3) - ((em->x3C0[1] * temp_f3) / 2.0f);
        em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / (f32) em->work08;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em->pos[1] = -1000.0f;
            em21_act_set(em, 2, 0, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em->pos[1] = -1000.0f;
            em21_act_set(em, 2, 0, 4);
        }
        break;
    }
}

static void em_fly23_00604340(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x762 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em->x388 = 0;
        em->x8BD = 1;
        em->x9EA = 0;
        em->x959 = 0;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x80, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x7C, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_fly24_00604440(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em->x88B = 1;
        Em_Sleep_End(em);
        em_char_set(em, 0x83, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_fly25_006044D0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em->x88B = 1;
        Em_Sleep2_End(em);
        em_char_set(em, 0x83, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_atk00_00604560(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x24, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_atk01_006045E0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x2F, 0, 0);
        break;
    case 1:
        if (em_frame_check(em, 0, 78.0f) != 0) {
            Shell08_set_ang_time(em, 0x22, 1, 0, 0x71C, 0xF1C8, (s32)(11.0f * em->act_spd));
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_atk02_006046C0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
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
            em21_to_normal(em);
        }
        break;
    }
}

static void em_atk03_006047B0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x74, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if (em_frame_check(em, 0, 114.0f) != 0) {
            Shell08_set_ang_time(em, 0x22, 1, 0, 0x71C, 0, (s32)(46.0f * em->act_spd));
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            swim_eff_set2_0060C260(8.0f, em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_atk04_006048C0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x75, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if (em_frame_check(em, 0, 114.0f) != 0) {
            Shell08_set_ang_time(em, 0x22, 1, 0, 0x71C, 0, (s32)(46.0f * em->act_spd));
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            swim_eff_set2_0060C260(8.0f, em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_atk05_006049D0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x70, 0, 0);
        em->x388 = 4;
        break;
    case 1:
        if (em_frame_check(em, 0, 118.0f) != 0) {
            Shell08_set_ang_time(em, 0x22, 1, 1, 0x71C, 0, (s32)(30.0f * em->act_spd));
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            swim_eff_set2_0060C260(8.0f, em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em21_to_swim(em);
        }
        break;
    }
}

static void em_atk06_00604AE0(EMW *em, EM21W *w) {
    f32 temp_f0;
    f32 temp_f1;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0x0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x6C, 0, 0);
        em->x388 = 4;
        em_rate_clear(em);
        em->adj_z = 100.0f;
        em->adj_y = 100.0f;
        em->x3C0[1] = -5.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        em->work08 = 0x12C;
        break;
    case 0x1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->pos[1] < em->x7E4)) {
            em->x05 += 1;
            em->x388 = 2;
        }
        if (em->x194 == 0) {
            em->x05 = 0x63;
            em21_to_swim(em);
        }
        break;
    case 0x2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_f1 = em->x5AC;
        if (em->pos[1] < temp_f1) {
            em->pos[1] = temp_f1;
            em->x3C0[1] = 0.0f;
        }
        if (em->pos[1] <= em->x7E4) {
            em->x05 += 1;
            em->x388 = 4;
            em->pos[1] = em->x7E4;
            em->adj_y = 0.0f;
            em->x3C0[1] = 0.0f;
            em_char_set(em, 0x67, 0xA, 0);
            w->dist = 1000.0f;
            em->work08 = 0x3C;
            temp_v0 = em->work08;
            temp_v1 = temp_v0 * temp_v0;
            var_v0 = temp_v1 >> 1;
            if (temp_v1 < 0) {
                var_v0 = (s32) (temp_v1 + 1) >> 1;
            }
            em->x3C0[2] = (w->dist - ((f32) temp_v0 * em->adj_z)) / (f32) var_v0;
            swim_eff_set2_0060C260(8.0f, em);
            return;
        }
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 = 0x63;
            em->pos[1] = em->x5AC;
            em21_act_set(em, 0, 4, 4);
            return;
        }
        break;
    case 0x3:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        temp_f0 = w->dist - em->adj_z;
        w->dist = temp_f0;
        if ((temp_f0 <= 0.0f) || (em->adj_z <= 0.0f)) {
            em->x05 += 1;
            em21_to_swim(em);
            return;
        }
        break;
    case 0x63:
        if (em->x8C3 == 0) {
            em->pos[1] = em->x5AC;
            em21_act_set(em, 0, 4, 4);
        }
        break;
    }
}

static void em_atk07_00604DC0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_char_set(em, 0x88, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_dmg00_00604E40(EMW *em, EM21W *w) {
    u8 temp_a3;

    temp_a3 = em->x05;
    switch (temp_a3) {                              /* irregular */
    case 0:
        em->x05 = temp_a3 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3C, 0, 0);
        break;
    case 1:
        if (EM_LYR(em, em->x07) == 0) {
            em->x05 = temp_a3 + 1;
            em21_to_normal(em);
            return;
        }
        if (M2C_FIELD(em, s32 *, 0x1E4) == 0) {
            em_char_set(em, 1, 0, 0);
        }
        if (M2C_FIELD(em, s32 *, 0x234) == 0) {
            em_char_set(em, 1, 0, 0);
        }
        break;
    }
}

static void em_dmg01_00604F20(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x42, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_dmg02_00604FB0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3F, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_dmg03_00605040(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x40, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_dmg04_006050D0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x388 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x4B, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x80, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x7C, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_dmg05_006051D0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, (s16)((em->x07 != 0) ? 0x45 : 0x4A), 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0;
            em_char_set(em, (s16)((em->x07 != 0) ? 0x46 : 0x4B), 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, (s16)((em->x07 != 0) ? 0x81 : 0x80), 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0;
            em_char_set(em, (s16)((em->x07 != 0) ? 0x47 : 0x7C), 0, 0);
            return;
        }
        break;
    case 4:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg06_00605370(EMW *em, EM21W *w) {
    u8 temp_a1;
    u8 var_v1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em->x388 = 2;
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = -10.0f;
        em->x3C0[2] = 0.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        } else {
            em->x388 = 4;
        }
        if (em->x7E8 != 0) {
            if (em->pos[1] < em->x7E4) {
                em->x05 += 1;
                em->x388 = 4;
                em->pos[1] = em->x7E4;
                em21_to_swim(em);
            }
        } else {
            if (em->pos[1] < em->x5AC) {
                em->x05 += 1;
                em->x388 = 0;
                em->pos[1] = em->x5AC;
                em21_act_set(em, 4, 0x10, 2);
            }
        }
        break;
    }
}

static void em_dmg07_006054D0(EMW *em, EM21W *w) {
    s32 temp_v0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x4B, 0, 0);
        em->x388 = 0;
        em_cmd_reset(em);
        Em_Mahi_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x80, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x8A, 0, 0);
            return;
        }
        break;
    case 3:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em21_act_set(em, 4, 0x15, 3);
        }
        em_mahi_eff_set(em, 2);
        break;
    case 4:
        em_mahi_eff_set(em, 2);
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 0x15, 3);
        }
        break;
    }
}

static void em_dmg08_00605630(EMW *em, EM21W *w) {
    s8 temp_v0;
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
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
                em21_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em21_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg09_00605770(EMW *em, EM21W *w) {
    u8 temp_a1;
    u8 var_v1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x88B = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x7E, 0, 0);
        em->x388 = 2;
        em->x3C0[0] = 0.0f;
        em->x3C0[1] = -10.0f;
        em->x3C0[2] = 0.0f;
        w->spd[0] = 0;
        w->spd[2] = 0;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->pos[1] < em->x7E4)) {
            em->x388 = 2;
        } else {
            em->x388 = 4;
        }
        if (em->x7E8 != 0) {
            if (em->pos[1] < em->x7E4) {
                em->x05 += 1;
                em->x388 = 4;
                em->pos[1] = em->x7E4;
                em21_to_swim(em);
            }
        } else {
            if (em->pos[1] < em->x5AC) {
                em->x05 += 1;
                em->x388 = 0;
                em->pos[1] = em->x5AC;
                em21_act_set(em, 4, 0x10, 2);
            }
        }
        break;
    }
}

static void em_dmg10_006058E0(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->x88B = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em21_to_normal(em);
        }
        break;
    }
}

static void em_dmg11_00605970(EMW *em, EM21W *w) {
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
        break;
    case 1:
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em21_act_set(em, 0, 0x14, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 0, 0x14, 4);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg12_00605A50(EMW *em, EM21W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
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
            em21_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg13_00605B70(EMW *em, EM21W *w) {
    s8 temp_v0;
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
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
                em21_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em21_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg14_00605CA0(EMW *em, EM21W *w) {
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
            em21_act_set(em, 4, 8, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em21_act_set(em, 4, 8, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}


static void em_dmg15_00605DC0(EMW *em, EM21W *w) {
    f32 temp_f3;
    s32 temp_v1;
    u8 temp_a1;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {
    case 0:
        em->x05 = temp_a2 + 1;
        em->x3F4 = 0;
        em->x388 = 4;
        em_rate_clear(em);
        em_char_set(em, 0x7E, 0, 0);
        em->ang[1] = Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF;
        em->x8B9 = 0;
        em->x88B = 1;
        em->work08 = 0x1E;
        em->x3C0[1] = -10.0f;
        temp_f3 = (f32) em->work08;
        em->adj_y = ((em->tgt_pos[1] - em->pos[1]) / temp_f3) - ((em->x3C0[1] * temp_f3) / 2.0f);
        em->adj_z = CalcDistanceXZ(em->pos, em->tgt_pos) / (f32) em->work08;
        Em_Mode_Chg(em, 1, em_atk_mode_timer_tbl[em->kind]);
        w->spd[0] = 0;
        w->spd[2] = 0;
        em->x762 = 3;
        em_cmd_reset(em);
        em->x88B = 1;
        break;
    case 1:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if (!(em->pos[1] < em->x7E4)) {
            temp_a1 = em->x05;
            em->x05 = temp_a1 + 1;
            em->x388 = 2;
            swim_eff_set2_0060C260(8.0f, em);
        }
        /* fallthrough */
    case 2:
        w->spd[1] = (s32) em->ang[1];
        speed_add_g(em, w->spd);
        if ((em->adj_y < 0.0f) && (em->pos[1] < em->x5AC)) {
            em->x05 += 1;
            em->x388 = 0;
            em->pos[1] = em->x5AC;
            em_char_set(em, 0x79, 0, 0);
            return;
        }
    default:
        break;
    case 3:
        if (em->x194 == 0) {
            em->x302 -= 0x64;
            if (em->x302 <= 0) {
                em->x302 = 0;
                em->x05 = 0x63;
                em21_act_set(em, 5, 3, 2);
                return;
            }
            em->x05 += 1;
            em_char_set(em, 0x4B, 0, 0);
            em->work08 = 0x12C;
            return;
        }
        break;
    case 4:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            return;
        }
        break;
    case 5:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x80, 0, 0);
            return;
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 0x7C, 0, 0);
            return;
        }
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em->x762 = 0;
            em_ikari_add(em, em->x8B0);
            em21_to_normal(em);
        }
        break;
    }
}

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

static void em_move00_006075D0(EMW *em, EM21W *w) {
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

static void em_move01_00607730(EMW *em, EM21W *w) {
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

static void em_move02_006077C0(EMW *em, EM21W *w) {
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

static void em_move03_006079A0(EMW *em, EM21W *w) {
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

static void em_move04_00607A60(EMW *em, EM21W *w) {
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

static void em_move05_00607C10(EMW *em, EM21W *w) {
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

static void em_move06_00607CA0(EMW *em, EM21W *w) {
    switch (em->x15) {
    case 0:
        em_demo00_00606B70(em, w);
        break;
    }
}

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

void em21_main(EMW *em) {
    EM21W *w = (EM21W *)em->ex;
    u8 dmg[4];
    u8 var_s0;
    int i;
    int n;
    u8 r;
    PLW *pl;

    var_s0 = 0;
    if (em->x8C3 == 0 && em->x04 == 1) {
        if (game_w.x2E == 3) {
            n = game_w.pl_num;
            pl = player_work;
            for (i = 0; i < n; i++, pl++) {
                if (em->stg == pl->stg) {
                    break;
                }
            }
            if (i >= n) {
                if (em->mode < 5) {
                    if (em->x388 != 4) {
                        if (!M2(0x16)) {
                            em_cmd_reset(em);
                            em->x839 = 0;
                            em21_act_set(em, 2, 0x16, 2);
                        }
                    } else if (!M2(0)) {
                        em_cmd_reset(em);
                        em21_act_set(em, 2, 0, 2);
                    }
                }
            }
        } else if (game_w.x2E == 6) {
            em_no_floor_ck(em);
            if (em->x388 == 0) {
                em_no_battle_area_ck(em, 0, 1);
            } else if (em->x388 == 4) {
                em_no_battle_area_ck(em, 0, 0);
            }
        }
    } else if (em->stg == game_w.stage) {
        switch (em->x388) {
        case 0:
            if (em->x7E8 != 0) {
                em21_act_set(em, 2, 0, 0);
            }
            break;
        case 4:
            if (em->x7E8 == 0) {
                em21_act_set(em, 0, 1, 0);
            }
            break;
        }
    }
    em_mode_timer_sub(em);
    r = Em_Dmg_Sys(em, dmg);
    switch (r) {
    case 1:
    case 2:
        if (em->x388 == 4) {
            em21_act_set(em, 5, 2, 2);
        } else if (em->x9EA != 0) {
            em21_act_set(em, 5, 1, 2);
        } else {
            em21_act_set(em, 5, 0, 2);
        }
        break;
    case 3:
    case 4:
        if (em->x9EA == 0) {
            if (dmg[0] == 0) {
                em->x95A = 0x10;
            } else if (em->x8B6 == 0) {
                em->x95A = 0xA;
            } else {
                em->x95A = 6;
            }
            em_ana_loop_cnt_set(em);
            em21_act_set(em, 4, 0xC, 2);
        }
        break;
    case 15:
        if (em->x388 == 4) {
            em21_act_set(em, 4, 0x11, 2);
            em_ikari_add(em, em->x8B0);
        }
        break;
    case 5:
        if (!M4(6)) {
            em21_act_set(em, 4, 6, 2);
        }
        break;
    case 6:
        var_s0 = 1;
        if (em->x9EA != 0) {
            em_mahi_dmg_timer_set(em);
            em21_act_set(em, 4, 0xE, 2);
        } else if (!M4(0xB) && !M4(6) && !M4(7) && em->x388 == 1) {
            em_mahi_dmg_timer_set(em);
            em21_act_set(em, 4, 7, 2);
        } else if (!M4(0xB) && !M4(6) && !M4(7) && em->x388 == 0) {
            em_mahi_dmg_timer_set(em);
            em21_act_set(em, 4, 0xB, 2);
        } else if (!M4(0xF) && !M4(6) && !M4(7) && em->x388 == 4) {
            em_mahi_dmg_timer_set(em);
            em21_act_set(em, 4, 0x12, 2);
        }
        break;
    case 7:
        if (em->x9EA != 0) {
            em_sleep2_dmg_timer_set(em);
            if ((u8)em_hokaku_ck(em, 0.3f) == 1) {
                em21_act_set(em, 6, 0, 4);
            } else {
                em21_act_set(em, 0, 0xA, 2);
            }
        } else if (!M4(0x16) && !M4(6) && em->x388 == 4) {
            em_sleep2_dmg_timer_set(em);
            em21_act_set(em, 4, 0x16, 2);
        } else if (!M0(0x10) && !M4(6) && em->x388 == 1) {
            em_sleep2_dmg_timer_set(em);
            em21_act_set(em, 0, 0x10, 2);
        } else if (!M0(0xC) && !M4(6) && em->x388 == 0) {
            em_sleep2_dmg_timer_set(em);
            em21_act_set(em, 0, 0xC, 2);
        }
        break;
    case 8:
        if (em->x959 == 6) {
            em_sleep_dmg_timer_set(em);
            em21_act_set(em, 0, 8, 2);
        } else if (!M2(0x13) && !M4(6) && em->x388 == 4) {
            em_sleep_dmg_timer_set(em);
            em21_act_set(em, 2, 0x13, 2);
        } else if (!M0(0xE) && !M4(6) && em->x388 == 1) {
            em_sleep_dmg_timer_set(em);
            em21_act_set(em, 0, 0xE, 2);
        } else if (!M0(5) && !M4(6) && em->x388 == 0) {
            em_sleep_dmg_timer_set(em);
            em21_act_set(em, 0, 5, 2);
        }
        break;
    case 10:
        if (em->mode == 0) {
            switch (em->x15) {
            case 5:
                em21_act_set(em, 0, 6, 2);
                em->x839 = 0;
                break;
            case 8:
                em21_act_set(em, 4, 0x13, 2);
                em->x839 = 0;
                break;
            case 10:
                em21_act_set(em, 4, 0x14, 2);
                em->x839 = 0;
                break;
            case 12:
                em21_act_set(em, 0, 0xD, 2);
                em->x839 = 0;
                break;
            case 14:
                em21_act_set(em, 0, 0xF, 2);
                em->x839 = 0;
                break;
            case 18:
                em21_act_set(em, 0, 0x13, 2);
                em->x839 = 0;
                break;
            }
        } else if (em->mode == 2) {
            switch (em->x15) {
            case 19:
                em21_act_set(em, 2, 0x18, 2);
                em->x839 = 0;
                break;
            case 20:
                em21_act_set(em, 2, 0x19, 2);
                em->x839 = 0;
                break;
            }
        } else if (em->mode == 4) {
            if (em->x15 == 0x16) {
                em21_act_set(em, 2, 0x19, 2);
                em->x839 = 0;
            }
        }
        break;
    case 12:
        var_s0 = 1;
        switch (em->x388) {
        case 3:
        case 0:
            switch ((u8)em->x38E) {
            case 0:
            case 7:
                em21_act_set(em, 4, 0, 2);
                break;
            case 5:
            case 6:
                em21_act_set(em, 4, 2, 2);
                break;
            case 1:
            case 2:
                em21_act_set(em, 4, 3, 2);
                break;
            default:
                if (em->hagi[(u8)em->x38E].cnt >= 2) {
                    em21_act_set(em, 4, 5, 2);
                    if ((u8)em->x38E != 3) {
                        em->x07 = 0;
                    } else {
                        em->x07 = 1;
                    }
                } else {
                    em21_act_set(em, 4, 1, 2);
                }
                break;
            }
            break;
        case 1:
            em21_act_set(em, 4, 4, 2);
            break;
        case 2:
            em21_act_set(em, 4, 6, 2);
            break;
        case 4:
            em21_act_set(em, 4, 0x11, 2);
            break;
        }
        break;
    case 13:
        var_s0 = 1;
        if (em->x388 != 2) {
            em21_act_set(em, 4, 0, 2);
            em->x839 = 0;
        }
        break;
    case 14:
        var_s0 = 1;
        if (M2(0x12)) {
            em->x839 = 0;
        }
        break;
    }
    if (var_s0 != 0 && em->x94E == 0) {
        em->x88B = 1;
    }
    if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em21_main_sub(em, w);
    if (em->x6FF != 0) {
        em21_main_sub(em, w);
        em->x6FF = 0;
    }
    if (em->x388 == 4) {
        f32 temp_f1;
        f32 temp_f1_2;

        temp_f1 = em->x7E4;
        if (!(em->pos[1] <= temp_f1)) {
            em->pos[1] = temp_f1;
        }
        temp_f1_2 = em->x5AC;
        if (em->pos[1] < temp_f1_2) {
            em->pos[1] = temp_f1_2;
        }
    }
}

void em21_main_sub(EMW *em, EM21W *w) {
    em->mode_old = em->mode;
    em->x15_old = em->x15;
    switch (em->mode) {
    case 0:
        em_move00_006075D0(em, w);
        break;
    case 1:
        em_move01_00607730(em, w);
        break;
    case 2:
        em_move02_006077C0(em, w);
        break;
    case 3:
        em_move03_006079A0(em, w);
        break;
    case 4:
        em_move04_00607A60(em, w);
        break;
    case 5:
        em_move05_00607C10(em, w);
        break;
    case 6:
        em_move06_00607CA0(em, w);
        break;
    case 7:
        em_move06_00607CA0(em, w);
        break;
    }
    if (em->pos[0] <= 0.0f || em->pos[2] <= 0.0f) {
        em_dur_set(em, 0);
    }
}

void em21_uvmove(EMW *em) {
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

static void sound_call_sub_00608D00(EMW *em, int se, int joint) {
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

static void ef_move_sub_00608E70(EMW *em, EM21W *w) {
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

/* Second state machine of part i: swings hire_ang[i][0] back to 0 (mode 1)
 * or to hire_down_angx[i] (modes 2, 3) over 20 frames. */
static void hire_move_sub2_0060BAD0(EMW *em, EM21W *w, int i) {
    switch (w->hire_st[i][1]) {
    case 0:
        switch (w->hire_mode) {
        case 1:
            if (w->hire_ang[i][0] != 0) {
                w->hire_st[i][1] = 1;
                w->hire_cnt2[i] = 20;
            }
            break;
        case 3:
        case 2:
            if (w->hire_ang[i][0] != hire_down_angx_003893F0[i]) {
                w->hire_st[i][1] = 2;
                w->hire_cnt2[i] = 20;
            }
            break;
        }
        break;
    case 1:
        w->hire_cnt2[i]--;
        if (w->hire_cnt2[i] <= 0) {
            w->hire_ang[i][0] = 0;
            w->hire_st[i][1] = 0;
            return;
        }
        w->hire_ang[i][0] += (u16)((s32)(0x10000 - w->hire_ang[i][0]) / w->hire_cnt2[i]);
        break;
    case 2:
        w->hire_cnt2[i]--;
        if (w->hire_cnt2[i] <= 0) {
            w->hire_ang[i][0] = hire_down_angx_003893F0[i];
            w->hire_st[i][1] = 0;
            return;
        }
        w->hire_ang[i][0] -= (u16)((s32)((0x10000 - (hire_down_angx_003893F0[i] - w->hire_ang[i][0])) & 0xFFFF) / w->hire_cnt2[i]);
        break;
    }
}

/* First state machine of part i: waits hire_start_timer, then walks the
 * hire_normal_add (or hire_down_add) list adding up hire_ang[i][1]. */
static void hire_move_sub1_0060BCA0(EMW *em, EM21W *w, int i) {
    switch (w->hire_st[i][0]) {
    case 0:
        switch (w->hire_mode) {
        case 1:
            w->hire_st[i][0]++;
            w->hire_tm[i] = hire_start_timer_tbl0_003893D0[i];
            break;
        case 2:
            w->hire_st[i][0]++;
            w->hire_tm[i] = hire_start_timer_tbl1_003893D8[i];
            break;
        }
        break;
    case 1:
        w->hire_tm[i]--;
        if (w->hire_tm[i] <= 0) {
            if (w->hire_mode == 2) {
                w->hire_st[i][0] = 3;
            } else {
                w->hire_st[i][0] = 2;
            }
            w->hire_tm[i] = 0;
            w->hire_cnt[i] = 0;
        }
        break;
    case 2:
    case 3: {
        HIRE_ADD *tbl;
        HIRE_ADD *p;
        u16 cnt;
        u8 k;

        cnt = w->hire_cnt[i];
        w->hire_cnt[i] = cnt + 1;
        if (w->hire_st[i][0] == 2) {
            tbl = hire_normal_add_tbl_0066ED90[i];
        } else {
            tbl = hire_down_add_tbl_0066EE40[i];
        }
        p = tbl;
        k = 0;
        for (;;) {
            if (k != 0 && p->t == 0) {
                k |= 0x80;
                break;
            }
            if (p->t < cnt) {
                p++;
                k++;
                continue;
            }
            break;
        }
        if (k & 0x80) {
            if (w->hire_mode == 2) {
                w->hire_st[i][0] = 1;
                w->hire_tm[i] = hire_remove_timer_tbl1_003893E8[i];
            } else if (w->hire_mode == 1) {
                w->hire_st[i][0] = 1;
                w->hire_tm[i] = hire_remove_timer_tbl0_003893E0[i];
            } else {
                w->hire_st[i][0] = 0;
            }
            w->hire_ang[i][1] = tbl[k & 0x7F].add;
            return;
        }
        w->hire_ang[i][1] += tbl[k].add;
        break;
    }
    }
}

static void hire_move_0060BFE0(EMW *em, EM21W *w) {
    int i;

    for (i = 0; i < 4; i++) {
        hire_move_sub1_0060BCA0(em, w, i);
        hire_move_sub2_0060BAD0(em, w, i);
    }
}

void em21_effect_move(EMW *em) {
    EM21W *w = (EM21W *)em->ex;
    u8 *temp_s0;
    u8 temp_a1;

    temp_a1 = w->eff;
    temp_s0 = em->ex;
    switch (temp_a1) {
    case 0:
        w->eff = temp_a1 + 1;
        break;
    case 1:
        ef_move_sub_00608E70(em, (EM21W *)temp_s0);
        hire_move_0060BFE0(em, (EM21W *)temp_s0);
        break;
    }
    em21_uvmove(em);
}

static void hire_req_set_0060C0C0(EMW *em, EM21W *w, u8 mode) {
    w->hire_mode = mode;
    if (w->hire_mode == 1 && (s16)(0.2f * (f32)em->x792) >= em->x302) {
        w->hire_mode = 2;
    }
    if (w->hire_mode == 3 && (s16)(0.2f * (f32)em->x792) < em->x302) {
        w->hire_mode = 0;
    }
}

static void ground_land_eff_set_0060C180(EMW *em) {
    Eft20_set(1.0f, em, 0xB, 0);
}

static void swim_eff_set_0060C1A0(f32 scale, EMW *em) {
    f32 pos[3];

    if (game_w.stage == em->stg && !(GAME_X1E16 & 3) && !(em->pos[1] < em->x7E4 - 100.0f)) {
        SetVector(pos, em->pos[0], 10.0f + (em->x7E4 + em->x7E0), em->pos[2]);
        Eft08_set(pos, 2, 0, scale);
        sound_call_sub_00608D00(em, 0x1B, 6);
    }
}

static void swim_eff_set2_0060C260(f32 scale, EMW *em) {
    f32 pos[3];

    if (game_w.stage == em->stg) {
        SetVector(pos, em->pos[0], 10.0f + (em->x7E4 + em->x7E0), em->pos[2]);
        Eft08_set(pos, 5, 0, scale);
        sound_call_sub_00608D00(em, 0x1E, 6);
    }
}

void swim_eff_set3(f32 scale, EMW *em) {
    f32 pos[3];

    if (game_w.stage == em->stg && !(GAME_X1E16 & 7) && !(em->pos[1] < em->x7E4 - 100.0f)) {
        SetVector(pos, em->pos[0], 10.0f + (em->x7E4 + em->x7E0), em->pos[2]);
        Eft08_set(pos, 4, 0, scale);
    }
}

void dummy_em_prog_0060C390(void) {
}
