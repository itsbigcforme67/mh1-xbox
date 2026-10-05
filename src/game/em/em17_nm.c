/* em17 draft */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em17.h"

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
void em_char_set(EMW *, int, int, int);
void SetVector(f32 *, f32, f32, f32);
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
void em_dur_set(EMW *, int);
void em_ikari_add(EMW *, s16);
void Eft20_set(f32, EMW *, int, int);
void Eft15_set3(EMW *, int, f32, int);
void Eft13_set_em(EMW *, int, int);
int em_frame_check3(EMW *, int, f32, f32);
int Em_stg_ck(EMW *);
void em17_horm_init(EMW *);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_no_battle_area_ck(EMW *, int, int);
void get_joint_pos_em(EMW *, int, f32 *);
void cpRotMatrixYXZ2(s32 *, FLMAT *);
void em17_act_set(EMW *em, int kind, u16 no, u16 arg);
void em_range_set(EMW *em, s8 no);
void em_area_move_init(EMW *em);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
void em_search_data_set(EMW *em, u8 no);
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
void Eft17_set_ex(f32 *, int, int, f32);
void Quest_enemy_capture();
void Quest_enemy_hagi_set();
extern s16 em17_stay_timer_tbl[];
extern s16 em17_runaway_timer_tbl[];

void em17_local_init(EMW *em);
void em17_init(EMW *em);
static u16 *em_act_search2_005DA580(EMW *em, u16 *tbl);
static void act_dist_select_005DA5C0(EMW *em);
void em17_to_normal(EMW *em, s16 a, s16 b);
void em17_to_fly(EMW *em, int flag);
void em17_frame_reset(EMW *em, int i);
static void em_act00_005DA9F0(EMW *em, EM17W *w);
static void em_act01_005DAAD0(EMW *em, EM17W *w);
static void em_act02_005DABF0(EMW *em, EM17W *w);
static void em_act03_005DAC80(EMW *em, EM17W *w);
static void em_act04_005DAD40(EMW *em, EM17W *w);
static void em_act05_005DADD0(EMW *em, EM17W *w);
static void em_act06_005DAEC0(EMW *em, EM17W *w);
static void em_act07_005DAF90(EMW *em, EM17W *w);
static void em_act08_005DB050(EMW *em, EM17W *w);
static void em_act09_005DB120(EMW *em, EM17W *w);
static void em_act10_005DB1C0(EMW *em, EM17W *w);
static void em_act11_005DB2D0(EMW *em, EM17W *w);
static void em_act12_005DB3A0(EMW *em, EM17W *w);
static void em_act13_005DB440(EMW *em, EM17W *w);
static void em_act14_005DB510(EMW *em, EM17W *w);
static void em_act15_005DB5D0(EMW *em, EM17W *w);
static void em_act16_005DB720(EMW *em, EM17W *w);
static void em_act17_005DB850(EMW *em, EM17W *w);
static void em_act19_005DB8D0(EMW *em, EM17W *w);
static void em_act18_005DB9A0(EMW *em, EM17W *w);
static void em_act20_005DBB00(EMW *em, EM17W *w);
static void em_act21_005DBC10(EMW *em, EM17W *w);
static void em_act22_005DBD70(EMW *em, EM17W *w);
static void em_act23_005DBDF0(EMW *em, EM17W *w);
static void em_act24_005DBEC0(EMW *em, EM17W *w);
static void em_act25_005DBF90(EMW *em, EM17W *w);
static void em_act26_005DC010(EMW *em, EM17W *w);
static void em_act27_005DC0C0(EMW *em, EM17W *w);
static void em_act28_005DC1D0(EMW *em, EM17W *w);
static void em_act29_005DC2A0(EMW *em, EM17W *w);
static void em_act31_005DC3E0(EMW *em, EM17W *w);
static void em_act33_005DC4C0(EMW *em, EM17W *w);
static void em_mv00_005DC550(EMW *em, EM17W *w);
static void em_mv01_005DC6B0(EMW *em, EM17W *w);
static void em_mv02_005DC6C0(EMW *em, EM17W *w);
static void em_mv03_005DC740(EMW *em, EM17W *w);
static void em_mv04_005DCA10(EMW *em, EM17W *w);
static void em_mv05_005DCBD0(EMW *em, EM17W *w);
static void em_mv06_005DCD30(EMW *em, EM17W *w);
static void em_mv07_005DCE90(EMW *em, EM17W *w);
static void em_fly00_005DCFD0(EMW *em, EM17W *w);
static void em_fly01_005DD0D0(EMW *em, EM17W *w);
static void em_fly02_005DD310(EMW *em, EM17W *w);
static void em_fly03_005DD420(EMW *em, EM17W *w);
static void em_fly04_005DD6D0(EMW *em, EM17W *w);
static void em_fly05_005DD840(EMW *em, EM17W *w);
static void em_fly07_005DD9B0(EMW *em, EM17W *w);
static void em_fly09_005DDAF0(EMW *em, EM17W *w);
static void em_fly14_005DDC60(EMW *em, EM17W *w);
static void em_fly16_005DDD20(EMW *em, EM17W *w);
static void em_fly20_005DDE20(EMW *em, EM17W *w);
static void em_fly21_005DDF40(EMW *em, EM17W *w);
static void em_fly23_005DE080(EMW *em, EM17W *w);
static void em_atk00_005DE2C0(EMW *em, EM17W *w);
static void em_atk02_005DE360(EMW *em, EM17W *w);
static void em_atk03_005DE370(EMW *em, EM17W *w);
static void em_atk04_005DE3F0(EMW *em, EM17W *w);
static void em_atk05_005DE530(EMW *em, EM17W *w);
static void em_atk06_005DE710(EMW *em, EM17W *w);
static void em_atk07_005DE800(EMW *em, EM17W *w);
static void em_atk08_005DE9E0(EMW *em, EM17W *w);
static void em_atk09_005DEAA0(EMW *em, EM17W *w);
static void em_dmg00_005DEB30(EMW *em, EM17W *w);
static void em_dmg01_005DEBD0(EMW *em, EM17W *w);
static void em_dmg02_005DEC60(EMW *em, EM17W *w);
static void em_dmg03_005DECF0(EMW *em, EM17W *w);
static void em_dmg04_005DED80(EMW *em, EM17W *w);
static void em_dmg05_005DEF50(EMW *em, EM17W *w);
static void em_dmg06_005DEFF0(EMW *em, EM17W *w);
static void em_dmg07_005DF090(EMW *em, EM17W *w);
static void em_dmg08_005DF180(EMW *em, EM17W *w);
static void em_dmg09_005DF2A0(EMW *em, EM17W *w);
static void em_dmg10_005DF3D0(EMW *em, EM17W *w);
static void em_dmg11_005DF460(EMW *em, EM17W *w);
static void em_dmg12_005DF540(EMW *em, EM17W *w);
static void em_dmg13_005DF670(EMW *em, EM17W *w);
static void em_dmg14_005DF7A0(EMW *em, EM17W *w);
static void em_dmg15_005DF8C0(EMW *em, EM17W *w);
static void em_dmg16_005DF950(EMW *em, EM17W *w);
static void em_dmg17_005DFA40(EMW *em, EM17W *w);
static void em_dmg18_005DFAC0(EMW *em, EM17W *w);
static void em_dmg19_005DFBF0(EMW *em, EM17W *w);
static void em_dmg20_005DFCF0(EMW *em, EM17W *w);
static void em_demo00_005DFDF0(EMW *em, EM17W *w);
static void em_demo04_005E06F0(EMW *em, EM17W *w);
static void em_die00_005E07A0(EMW *em, EM17W *w);
static void em_die01_005E08A0(EMW *em, EM17W *w);
static void em_die02_005E09C0(EMW *em, EM17W *w);
static void em_die03_005E0B90(EMW *em, EM17W *w);
void em17_soukou_dm_sel_set(EMW *em);
static void em_move00_005E0D20(EMW *em, EM17W *w);
static void em_move01_005E0F60(EMW *em, EM17W *w);
static void em_move02_005E1030(EMW *em, EM17W *w);
static void em_move03_005E11A0(EMW *em, EM17W *w);
static void em_move04_005E1270(EMW *em, EM17W *w);
static void em_move05_005E1400(EMW *em, EM17W *w);
static void em_move06_005E1490(EMW *em, EM17W *w);
void em17_main(EMW *em);
void em17_main_sub(EMW *em, EM17W *w);
void em17_uvmove(EMW *em);
static void sound_call_sub_005E1F80(EMW *em, int se, int joint);
static void sound_call_005E1FF0(EMW *em, int frame, int se, int joint);
static void sound_call_parts_005E2090(EMW *em, int frame, int se, int joint, u8 layer);
void Em_set_quake_sub(EMW *, int);
static void quake_call_005E2130(EMW *em, int frame, int arg);
static void move_default_005E2180(EMW *em);
static void ef_move_sub_005E21D0(EMW *em, EM17W *w);
void em17_effect_move(EMW *em);
static void ground_land_eff_set_005E63F0(EMW *em);
static s32 kyusyu_char_set2_005E64A0(EMW *em);
void em17_atk_end_sel(EMW *em, EM17W *w);
void dummy_em_prog_005E65C0(void);


void em17_local_init(EMW *em) {
    Eft19_set(em, 0x22, 0);
    eft01_set((PLW *) em, 0);
}

void em17_init(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    s16 temp_v0;
    u8 temp_a0;
    u8 temp_a1;

    if (quest_w.no == 0) {
        em->ang[1] = 0x8000;
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
    em->mode = 0;
    em->x15 = 0;
    if (em->kind == 0x11) {
        em_char_set(em, 1, 0, 0);
        em->x388 = 0;
        em17_act_set(em, 0, 1, 0);
        temp_v0 = em_hp_vital_set2(em, 0x4EC, 0x4D8);
        em->x302 = temp_v0;
        em->x792 = temp_v0;
        em->x839 = 1;
    } else {
        em_char_set(em, 0x6B, 0, 0);
        em->x388 = 0;
        em17_act_set(em, 0, 0x16, 0);
        temp_v0 = em_hp_vital_set2(em, 0x2A8, 0x334);
        em->x302 = temp_v0;
        em->x792 = temp_v0;
        em->x839 = 0;
    }
    w->x06 = 0;
    em->x88B = 1;
    em->x765 = 1;
    em->x8C2 = 0;
    em->x56A = 0;
    em->stay_tm = em17_stay_timer_tbl[em->stg];
    em->runaway_tm = em17_runaway_timer_tbl[em->stg];
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

static u16 *em_act_search2_005DA580(EMW *em, u16 *tbl) {
    EM17W *w = (EM17W *)em->ex;
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

extern u8 *em17_act_add[3];
extern u16 *em17_rail_add[2];
extern u16 *em17_rail_half_add[1];

static void act_dist_select_005DA5C0(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    u16 *p;
    int k;
    u8 temp_a1;
    u8 temp_a2;

    temp_a1 = em->x734;
    temp_a2 = M2C_FIELD(em, u8 *, 0x735);
    switch (temp_a1) {
    case 0:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, em_act_search(em17_act_add[temp_a2]) & 0xFFFF, 1);
        }
        break;
    case 1:
        if (em->x8C3 == 0) {
            p = em_act_search2_005DA580(em, em17_rail_add[temp_a2]);
            k = p[0];
            if (k == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em17_act_set(em, k, p[1], 1);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            p = em_act_search2_005DA580(em, em17_rail_half_add[temp_a2]);
            k = p[0];
            if (k == 1 && p[1] == 0) {
                w->has_tgt = 1;
            }
            em17_act_set(em, k, p[1], 1);
        }
        break;
    case 3:
        em->x839 = 1;
        if (em->x388 == 0) {
            em17_act_set(em, 0, 1, 0);
        } else {
            em17_act_set(em, 2, 2, 0);
        }
        break;
    }
}

void em17_to_normal(EMW *em, s16 a, s16 b) {
    if (em->x734 != 0) {
        act_dist_select_005DA5C0(em);
        return;
    }
    if (em->x734 == 3) {
        em->act_spd = 1.0f;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x839 = 1;
        if (em->x302 < (s16)(0.3f * (f32)em->x792)) {
            em17_act_set(em, 0, 1, 0);
        } else if (em->x888 == 0) {
            em17_act_set(em, 0, 1, 0);
        } else {
            em17_act_set(em, 0, 0x11, 0);
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
    em17_act_set(em, 0, 1, 0);
}

void em17_to_fly(EMW *em, int flag) {
    if (em->x734 != 3) {
        act_dist_select_005DA5C0(em);
        return;
    }
    em->x839 = 1;
    em->act_spd = 1.0f;
    switch (flag & 0xFF) {
    case 0:
        em_act_set(em, 2, 0xE);
        break;
    }
}

void em17_frame_reset(EMW *em, int i) {
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

static void em_act00_005DA9F0(EMW *em, EM17W *w) {
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

extern u8 *em17_act_add[3];

static void em_act01_005DAAD0(EMW *em, EM17W *w) {
    u16 temp_a2_2;
    u8 temp_a0;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x1D, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a2 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            temp_a0 = em->x734;
            if (((u32) (temp_a0 - 1) < 2) || (temp_a0 == 3)) {
                if (em->x194 == 0) {
                    em->x05 += 1;
                    act_dist_select_005DA5C0(em);
                    return;
                }
            } else {
                temp_a2_2 = em_act_search(*(&em17_act_add + (em->_pad735[0] * 4))) & 0xFFFF;
                if (temp_a2_2 != 1) {
                    em17_act_set(em, 0, temp_a2_2, 0);
                }
            }
        }
        break;
    }
}

static void em_act02_005DABF0(EMW *em, EM17W *w) {
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
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a1 + 1;
            act_dist_select_005DA5C0(em);
        }
        break;
    }
}

static void em_act03_005DAC80(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x66, 0, 0);
        em->work08 = (em->x39A & 3) * 0x12C;
        break;
    case 1:
        if (em->x8C3 == 0) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 += 1;
                em17_to_normal(em, 0, 0);
            }
        }
        break;
    }
}

static void em_act04_005DAD40(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x67, 0, 0);
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act05_005DADD0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    em17_frame_reset(em, 0);
    em17_frame_reset(em, 2);
    sound_call_parts_005E2090(em, 0x20, 0x57, 0x23, 1);
}

static void em_act06_005DAEC0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    em17_frame_reset(em, 0);
    em17_frame_reset(em, 2);
    sound_call_parts_005E2090(em, 0x20, 0x20, 0x23, 1);
    sound_call_parts_005E2090(em, 0xA0, 0x17, 0x23, 1);
}

static void em_act07_005DAF90(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    em17_frame_reset(em, 0);
    em17_frame_reset(em, 2);
    sound_call_parts_005E2090(em, 4, 0x55, 0x23, 1);
}

static void em_act08_005DB050(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6B, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        em->work08 = (em->x39A & 3) * 0x12C;
        break;
    case 1:
        if (em->x8C3 == 0) {
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x05 += 1;
                em17_to_normal(em, 0, 0);
            }
        }
        break;
    }
}

static void em_act09_005DB120(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act10_005DB1C0(EMW *em, EM17W *w) {
    u16 temp_v1;
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act11_005DB2D0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    em17_frame_reset(em, 0);
    em17_frame_reset(em, 2);
    sound_call_parts_005E2090(em, 0x32, 0x23, 0x23, 1);
    sound_call_parts_005E2090(em, 0x68, 0x1F, 0x23, 1);
}

static void em_act12_005DB3A0(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x8BB = 2;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em->x88B = 1;
        em_char_set(em, 0x6C, 0, 0);
        break;
    case 1:
        if ((em->x8C3 == 0) && (em->x194 == 0)) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act13_005DB440(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    em17_frame_reset(em, 0);
    em17_frame_reset(em, 2);
    sound_call_parts_005E2090(em, 2, 0x20, 0x23, 1);
    sound_call_parts_005E2090(em, 0x4A, 0x1F, 0x23, 1);
}

static void em_act14_005DB510(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        break;
    case 1:
        if (em17_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x6E, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act15_005DB5D0(EMW *em, EM17W *w) {
    u8 temp_v1;

    temp_v1 = em->x05;
    switch (temp_v1) {                              /* irregular */
    case 0:
        em->x05 = temp_v1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x50, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 2:
        if (em->x1C4 == 0) {
            em->x05 = temp_v1 + 1;
            em_char_set(em, 0x51, 0, 0);
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_v1 + 1;
            em_char_set(em, 1, 0, 0);
        }
        break;
    case 4:
        if (em->x1C4 == 0) {
            em->x05 = temp_v1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    sound_call_parts_005E2090(em, 0x20, 0x57, 0x23, 1);
    sound_call_parts_005E2090(em, 0x11A, 0x20, 0x23, 1);
}

static void em_act16_005DB720(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act17_005DB850(EMW *em, EM17W *w) {
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
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act19_005DB8D0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x68, 0, 0);
        em->x762 = 3;
        em_search_data_set(em, 1);
        em_range_set(em, 1);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x762 = 0;
            em_search_data_set(em, 0);
            em_range_set(em, 0);
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act18_005DB9A0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 0, 0x17, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x17, 4);
        }
        break;
    }
}

static void em_act20_005DBB00(EMW *em, EM17W *w) {
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

static void em_act21_005DBC10(EMW *em, EM17W *w) {
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

static void em_act22_005DBD70(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x95C = 2;
    em->x7D6 = 2;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6B, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        em->x762 = 3;
        /* fallthrough */
    case 1:
        break;
    }
}

static void em_act23_005DBDF0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act24_005DBEC0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act25_005DBF90(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_act26_005DC010(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x95C = 2;
    em->x7D6 = 2;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x6B, 0, 0);
        em->x88B = 0;
        em_range_set(em, 1);
        em->x762 = 3;
        em_hp_add(em, (s16)(0.05f * (f32) em->x792));
        /* fallthrough */
    case 1:
        break;
    }
}

static void em_act27_005DC0C0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 0, 0x1C, 4);
            return;
        }
        break;
    case 3:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x1C, 4);
        }
        break;
    }
}

static void em_act28_005DC1D0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x19, 4);
        }
        break;
    }
}

static void em_act29_005DC2A0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 4, 0x13, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0x13, 3);
        }
        break;
    }
    v[1] = 10.0f;
    v[2] = 140.0f;
    v[0] = 0.0f;
    em_sleep_eff_set(em, 0x22, v, 1.60000002f);
    if (((s32) em->work08 % 135) == 0) {
        sound_call_sub_005E1F80(em, 0x57, 0x23);
    }
}

static void em_act31_005DC3E0(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    em->x9EA = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 4, 0x14, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0x14, 3);
        }
        break;
    }
}

static void em_act33_005DC4C0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv00_005DC550(EMW *em, EM17W *w) {
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
        em_char_set(em, 2, 0, 0);
        break;
    case 1:
        if (w->has_tgt != 0) {
            temp_v1 = em->ang[1];
            temp_v0 = ((Em_Calc_angY(em->pos, em->tgt_pos) & 0xFFFF) - temp_v1) & 0xFFFF;
            if (temp_v0 < 0x8001) {
                if (temp_v0 < 0x80) {
                    var_v0 = temp_v1 + temp_v0;
                } else {
                    var_v0 = temp_v1 + 0x80;
                }
            } else if (temp_v0 >= 0xFF81) {
                var_v0 = temp_v1 + temp_v0;
            } else {
                var_v0 = temp_v1 - 0x80;
            }
            em->ang[1] = var_v0;
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv01_005DC6B0(EMW *em, EM17W *w) {
    em17_to_normal(em, 0, 0);
}

static void em_mv02_005DC6C0(EMW *em, EM17W *w) {
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
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv03_005DC740(EMW *em, EM17W *w) {
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
            em_char_set(em, 2, 0, 0);
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
                    em17_to_normal(em, 0, 0);
                    return;
                }
                if ((temp_s0 < 0xE39) || (temp_s0 >= 0xF1C8)) {
                    pl_flag_set((PLW *) em, 0x20000);
                    em_char_set(em, 2, 0, 0);
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

static void em_mv04_005DCA10(EMW *em, EM17W *w) {
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
        if (em_frame_check(em, 0, 150.0f) != 0) {
            em->x05 += 1;
        }
        break;
    case 2:
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
            em_char_set(em, 0x64, 0, 0);
            return;
        }
        break;
    case 3:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv05_005DCBD0(EMW *em, EM17W *w) {
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
        em_char_set(em, 2, 0, 0);
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv06_005DCD30(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_mv07_005DCE90(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_fly00_005DCFD0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em17_fly_adjy2_init(em, 2);
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check(em, 0, 96.0f) != 0) {
            em->x05 += 1;
            em->x388 = 2;
            em17_fly_adjy2(em);
        }
        break;
    case 2:
        if (em17_fly_adjy2(em) != 0) {
            em->x05 += 1;
            em17_to_fly(em, 0);
        }
        break;
    }
}

static void em_fly01_005DD0D0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly02_005DD310(EMW *em, EM17W *w) {
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
        em17_fly_adjy(1, temp_a2);
        temp_v0 = em->work08 - 1;
        em->work08 = temp_v0;
        if (temp_v0 <= 0) {
            em->x05 += 1;
            em17_act_set(em, 2, 1, 1);
        }
        em17_senkai_target(em);
        if (em->work08 == 0x12C) {
            em17_to_fly(em, 0);
        }
        break;
    case 2:
        em17_fly_adjy(1, temp_a2);
        em17_senkai_target(em);
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly03_005DD420(EMW *em, EM17W *w) {
    f32 temp_f1;
    f32 temp_f1_2;
    u8 temp_a2;

    temp_a2 = em->x05;
    switch (temp_a2) {
    case 0:
        em->x05 = temp_a2 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x12, 0, 0);
        em_rate_clear(em);
        /* fallthrough */
    case 1:
        if (em_frame_check(em, 0, 96.0f) != 0) {
            em->x05 += 1;
            em_rate_clear(em);
            em17_fly_adjy2_init(em, 2);
            w->x18 = 0;
        }
        break;
    case 2:
        if ((em_frame_check2(em, 0, 50.0f) != 0) && (em_frame_check2(em, 0, 114.0f) == 0)) {
            em17_senkai_target(em);
        }
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            em17_fly_adjy2(em);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 3:
        if ((em17_fly_adjy2(em) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
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
    case 4:
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
    case 5:
        if (em->x194 == 0) {
            w->x06 = 0x96;
            em->x05 += 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_2 = em->x5AC;
    if (em->pos[1] < temp_f1_2) {
        em->pos[1] = temp_f1_2;
    }
}

static void em_fly04_005DD6D0(EMW *em, EM17W *w) {
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
        em17_fly_adjy2_init(em, 3);
        em->adj_z = 20.0f;
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            var_s2 = em17_fly_adjy2(em) & 0xFF;
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly05_005DD840(EMW *em, EM17W *w) {
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
        em17_fly_adjy2_init(em, 3);
        em->adj_z = -20.0f;
        w->x18 = 0;
        break;
    case 1:
        if (em_frame_check2(em, 0, 30.0f) != 0) {
            em->x388 = 2;
            var_s2 = em17_fly_adjy2(em) & 0xFF;
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly07_005DD9B0(EMW *em, EM17W *w) {
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
        em17_senkai_target(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, 0.0f, 0.0f);
        speed_add(em, w->spd);
        em17_fly_adjy(em, 1);
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
            em17_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly09_005DDAF0(EMW *em, EM17W *w) {
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
        em17_senkai_sub(em, 5, 0);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        em->work08 -= 1;
        if ((CalcDistanceXZ(em->pos, em->tgt_pos) <= 1000.0f) || (em->work08 < 0)) {
            em->x05 += 1;
            em17_act_set(em, 2, 1, 1);
        }
        /* fallthrough */
    case 2:
        break;
    }
}

static void em_fly14_005DDC60(EMW *em, EM17W *w) {
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
            em17_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly16_005DDD20(EMW *em, EM17W *w) {
    f32 temp_f1;
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
        em17_fly_adjy(em, 1);
        em17_senkai_target(em);
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0) {
            em->x05 += 1;
            em17_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly20_005DDE20(EMW *em, EM17W *w) {
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
            em17_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly21_005DDF40(EMW *em, EM17W *w) {
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
            em17_to_fly(em, 0);
        }
        break;
    }
    temp_f1 = em->x5AC;
    if (em->pos[1] < temp_f1) {
        em->pos[1] = temp_f1;
    }
}

static void em_fly23_005DE080(EMW *em, EM17W *w) {
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
        em17_fly_adjy2_init(em, 2);
        em->x388 = 2;
        em->x9EA = 0;
        em->x959 = 0;
        em->x8BD = 1;
        break;
    case 1:
        em17_fly_adjy2(temp_a2);
        if (em->x194 == 0) {
            em->x05 += 1;
        }
        break;
    case 2:
        if ((em17_fly_adjy2(temp_a2) & 0xFF) && (em->pos[1] <= (800.0f + em->x5AC))) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_3 = em->x5AC;
    if (em->pos[1] < temp_f1_3) {
        em->pos[1] = temp_f1_3;
    }
}

static void em_atk00_005DE2C0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        if (em17_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x388 = 0;
            em->x3F4 = 0;
            em_char_set(em, 0x24, 0, 0);
        }
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk02_005DE360(EMW *em, EM17W *w) {

}

static void em_atk03_005DE370(EMW *em, EM17W *w) {
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
            em17_atk_end_sel(em, w);
        }
        break;
    }
}

static void em_atk04_005DE3F0(EMW *em, EM17W *w) {
    s8 temp_v0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        break;
    case 1:
        if (em17_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x6E, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 173.0f) != 0) {
            Shell08_set_ang(em, 0x23, 4, 0, 0, 0);
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            temp_v0 = w->x1A - 1;
            w->x1A = temp_v0;
            if ((s8)temp_v0 <= 0) {
                em17_to_normal(em, 0, 0);
                return;
            }
            em17_horm_init(em);
            em_act_set(em, 3, 4);
        }
        break;
    }
}

static void em_atk05_005DE530(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        break;
    case 1:
        if (em17_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em->work08 = 0x78;
            em_char_set(em, 0x6F, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 196.0f) != 0) {
            shell19_set(em, 0xE);
        }
        if ((em_frame_check2(em, 0, 124.0f) != 0) && (((s32) *(u16 *)0x3F340E % 24) == 0)) {
            Eft20_set(1.39999998f, em, 0x1D, 0);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_char_set(em, 0x71, 0, 0);
            return;
        }
        break;
    case 3:
        if ((em_frame_check2(em, 0, 50.0f) == 0) && (((s32) GAME_X1E16 % 24) == 0)) {
            Eft20_set(1.39999998f, em, 0x1D, 0);
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk06_005DE710(EMW *em, EM17W *w) {
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
            em17_atk_end_sel(em, w);
        }
        break;
    }
}

static void em_atk07_005DE800(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        break;
    case 1:
        if (em17_horm_main(temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em->work08 = 0x78;
            em_char_set(em, 0x6F, 0, 0);
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 196.0f) != 0) {
            shell19_set(em, 0xC);
        }
        if ((em_frame_check2(em, 0, 124.0f) != 0) && (((s32) *(u16 *)0x3F340E % 24) == 0)) {
            Eft20_set(1.39999998f, em, 0x1D, 1);
        }
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_char_set(em, 0x71, 0, 0);
            return;
        }
        break;
    case 3:
        if ((em_frame_check2(em, 0, 50.0f) == 0) && (((s32) GAME_X1E16 % 24) == 0)) {
            Eft20_set(1.39999998f, em, 0x1D, 1);
        }
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk08_005DE9E0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        /* fallthrough */
    case 1:
        if (em17_horm_main(em, temp_a1) != 0) {
            em->x05 += 1;
            em->x3F4 = 0;
            em_char_set(em, 0x6D, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x3F4 = 0;
            em->x05 += 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_atk09_005DEAA0(EMW *em, EM17W *w) {
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
            em->x3F4 = 0;
            em->x05 += 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg00_005DEB30(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x3C, 0, 0);
        em->x762 = 3;
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg01_005DEBD0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg02_005DEC60(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg03_005DECF0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg04_005DED80(EMW *em, EM17W *w) {
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
            em17_act_set(em, 4, 0x11, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0x11, 3);
        }
        break;
    }
}

static void em_dmg05_005DEF50(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg06_005DEFF0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
            em->mode_old = em->mode;
            em->x15_old = em->x15;
        }
        break;
    }
}

static void em_dmg07_005DF090(EMW *em, EM17W *w) {
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
            em17_to_fly(em, 1);
            em->adj_y = 0.0f;
        }
        speed_add(em, w->spd);
        break;
    }
}

static void em_dmg08_005DF180(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg09_005DF2A0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
            em_rate_clear(em);
        }
        break;
    }
}

static void em_dmg10_005DF3D0(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg11_005DF460(EMW *em, EM17W *w) {
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
            Em_Mahi_End(em);
            em17_act_set(em, 0, 0x21, 4);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 0, 0x21, 4);
        }
        break;
    }
}

static void em_dmg12_005DF540(EMW *em, EM17W *w) {
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
            em17_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x9EA < 5) {
            em->x9EA = 5;
        }
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg13_005DF670(EMW *em, EM17W *w) {
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
                em17_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em17_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg14_005DF7A0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 4, 0x12, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0x12, 3);
        }
        break;
    }
    em_mahi_eff_set(em, 2);
}

static void em_dmg15_005DF8C0(EMW *em, EM17W *w) {
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x73, 0, 0);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg16_005DF950(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_cmd_reset(em);
        em_char_set(em, 0x74, 0, 0);
        em->x762 = 3;
        em->work08 = 0x258;
        break;
    case 1:
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            em->x05 += 1;
            em_char_set(em, 0x76, 0, 0);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em->x762 = 0;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg17_005DFA40(EMW *em, EM17W *w) {
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
            em17_to_normal(em, 0, 0);
        }
        break;
    }
}

static void em_dmg18_005DFAC0(EMW *em, EM17W *w) {
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
                em17_act_set(em, 2, 0x17, 4);
                return;
            }
            em_char_set(em, 0x60, 0, 0);
        }
        break;
    case 2:
        if ((em->x194 == 0) && (em->x8C3 == 0)) {
            em_char_set(em, 0x60, 0, 0);
            em17_act_set(em, 2, 0x17, 4);
        }
        break;
    }
}

static void em_dmg19_005DFBF0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_dmg20_005DFCF0(EMW *em, EM17W *w) {
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
            em17_act_set(em, 4, 0xD, 3);
        }
        break;
    case 2:
        if (em->x8C3 == 0) {
            em17_act_set(em, 4, 0xD, 3);
        }
        break;
    }
}

static void em_demo00_005DFDF0(EMW *em, EM17W *w) {
    f32 dst[3];
    f32 a120[3];
    f32 out110[3];
    s32 ang[3];
    f32 vF0[3];
    f32 outE0[3];
    FLMAT m;
    EMW *temp_s1;
    STAGE_DATA *temp_s0;
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    s32 temp_v1_2;
    s32 temp_v1_3;
    u16 temp_v1;
    u8 temp_a1;
    u8 temp_v1_4;

    temp_s1 = em->x944;
    temp_s0 = Stage_data_get(em->stg);
    if (temp_s1 == 0) {
        SetVector(dst, 5000.0f, 0.0f, 5000.0f);
    } else {
        SetVector(dst, temp_s1->pos[0], temp_s1->pos[1], temp_s1->pos[2]);
    }
    temp_a1 = em->x05;
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em->tgt_pos[0] = 3000.0f;
        em->tgt_pos[1] = 2000.0f;
        em->tgt_pos[2] = 9000.0f;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16)(w->dang - em->ang[1]);
        em17_senkai_sub(em, 3, 1);
        temp_f1 = em->adj_z;
        if (temp_f1 > 100.0f) {
            em->adj_z = temp_f1 - 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em->x05 += 1;
            em->work08 = 0x384;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        break;
    case 2:
        temp_f1_2 = em->adj_z;
        if (temp_f1_2 > 80.0f) {
            em->adj_z = temp_f1_2 - 1.0f;
        }
        w->dang = Em_Calc_angY(em->pos, dst);
        w->dang = (u16)(w->dang - em->ang[1]);
        em17_senkai_sub(em, 3, 1);
        temp_v1 = w->dang;
        if (temp_v1 >= 0x801 && temp_v1 < 0xF800) {
        } else if (CalcDistanceXZ(em->pos, dst) > 4000.0f) {
            em->x05 += 1;
            w->turn = 0x100;
            em_char_set(em, 0x2C, 0, 0);
            em->work08 = 0x708;
        }
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1_2 = em->work08 - 1;
        em->work08 = temp_v1_2;
        if (temp_v1_2 <= 0 && em->x05 == 2) {
            em->x05 = 0;
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
            em->work08 = 0x708;
        }
        SetVector(em->tgt_pos, dst[0], dst[1], dst[2]);
        em17_senkai_target(em);
        w->spd[0] = em->ang[0];
        w->spd[1] = em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 1.0f;
        break;
    case 4:
        if (em->pos[1] <= 1000.0f + temp_s0->floor_y) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0x708;
        }
        if (kyusyu_char_set2_005E64A0(em) != 0) {
            temp_v1_4 = em->x05;
            if (temp_v1_4 == 4) {
                em->x05 = temp_v1_4 + 1;
                em->work08 = 0x708;
            }
        }
        SetVector(em->tgt_pos, dst[0], dst[1], dst[2]);
        em17_senkai_target(em);
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        xang_calc_target(em, w->spd, -3000.0f, 0.0f);
        speed_add(em, w->spd);
        break;
    case 5:
        kyusyu_char_set2_005E64A0(em);
        SetVector(em->tgt_pos, dst[0], dst[1], dst[2]);
        em17_senkai_target(em);
        if (!(200.0f + em->tgt_pos[1] < em->pos[1])) {
            xang_calc_target(em, w->spd, 0.0f, 0.0f);
        } else {
            xang_calc_target(em, w->spd, -2000.0f, 0.0f);
        }
        w->spd[1] = em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        if (CalcDistanceXZ(em->pos, dst) <= 600.0f || (em->char0 == 0x415 && (em_frame_check2(em, 0, 52.0f) != 0 || em->x194 == 0))) {
            em->x05 += 1;
            em->pos[1] = em->x5AC;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 4, 0);
            if (temp_s1 != 0) {
                ang[0] = 0;
                ang[1] = em->ang[1];
                ang[2] = 0;
                SetVector(a120, 0.0f, 0.0f, 600.0f);
                cpRotMatrixYXZ2(ang, &m);
                flvecApplyMat33(out110, a120, &m[0][0]);
                temp_s1->pos[0] = em->pos[0] + out110[0];
                temp_s1->pos[2] = em->pos[2] + out110[2];
                if (temp_s1->mode != 6 || temp_s1->x15 != 1) {
                    em_act_set(temp_s1, 6, 1);
                    temp_s1->ang[1] = (em->ang[1] - 0x4000) & 0xFFFF;
                }
            }
        }
        break;
    case 6:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x23, 0, 0);
            if (temp_s1 != 0) {
                em_act_set(temp_s1, 6, 2);
            }
        }
        if (temp_s1 != 0) {
            ang[0] = 0;
            ang[1] = em->ang[1];
            ang[2] = 0;
            SetVector(vF0, 0.0f, 0.0f, 600.0f);
            cpRotMatrixYXZ2(ang, &m);
            flvecApplyMat33(outE0, vF0, &m[0][0]);
            temp_s1->pos[0] = em->pos[0] + outE0[0];
            temp_s1->pos[2] = em->pos[2] + outE0[2];
            temp_s1->x40E = 5;
        }
        break;
    case 7:
        if (em_frame_check(em, 0, 60.0f) != 0 || em_frame_check(em, 0, 202.0f) != 0) {
            Eft13_set_em(em, 0x1C, 6);
        }
        if (em_frame_check(em, 0, 52.0f) != 0 || em_frame_check(em, 0, 208.0f) != 0) {
            Eft13_set_em(em, 0x16, 6);
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x4F, 0, 0);
            if (temp_s1 != 0) {
                em_act_set(temp_s1, 6, 3);
            }
        }
        break;
    case 8:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x3B, 0, 0);
        }
        break;
    case 9:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 1, 0, 0);
            if (temp_s1 != 0) {
                temp_s1->act_spd = 0.0f;
            }
        }
        break;
    case 10:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x14, 0, 0);
        }
        break;
    case 11:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em17_to_normal(em, 0, 0);
        }
        break;
    }
    temp_f1_4 = em->x5AC;
    if (em->pos[1] < temp_f1_4) {
        em->pos[1] = temp_f1_4;
    }
}

static void em_demo04_005E06F0(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x40C = 5;
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x3F4 = 0;
        em_char_set(em, 0x61, 0, 0);
        em_range_set(em, 1);
        em->x88B = 0;
        Em_Sleep2_Start(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            Quest_enemy_capture(temp_a1);
        }
        /* fallthrough */
    case 2:
        break;
    }
}

static void em_die00_005E07A0(EMW *em, EM17W *w) {
    u8 temp_a0;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em_char_set(em, 0x44, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        break;
    case 1:
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
    case 2:
        Em_hagi_point_cnt_ck(em);
        break;
    }
}

static void em_die01_005E08A0(EMW *em, EM17W *w) {
    s32 temp_v1;
    u8 temp_a0;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a0 = em->x05;
    switch (temp_a0) {                              /* irregular */
    case 0:
        em->x05 = temp_a0 + 1;
        em_char_set(em, 0x61, 0, 0);
        em->x3F4 = 0;
        em->x388 = 3;
        Quest_enemy_die(em);
        em->work08 = 0xE10;
        em->x9EA = 5;
        break;
    case 1:
        em->x9EA = 5;
        em->work08 -= 1;
        if (em->x194 == 0) {
            em->x05 += 1;
            Em_hagi_point_set(em, 0);
            em->ex[0x90] = 0;
        }
        break;
    case 2:
        Em_hagi_point_cnt_ck(em);
        if ((em->work08 != 0) && (em->x9EA != 0)) {
            em->x9EA = 5;
            temp_v1 = em->work08 - 1;
            em->work08 = temp_v1;
            if (temp_v1 <= 0) {
                em->x9EA = 0;
            }
        }
        break;
    }
}

static void em_die02_005E09C0(EMW *em, EM17W *w) {
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

static void em_die03_005E0B90(EMW *em, EM17W *w) {
    u8 temp_a1;

    em->x40E = 5;
    Em_Mode_Chg(em, 0, 0);
    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 3;
        em->x3F4 = 0;
        if (em->char0 != 0x6C) {
            em_char_set(em, 0x6C, 0, 0);
        }
        Quest_enemy_die(em);
        break;
    case 1:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em_char_set(em, 0x44, 0, 0);
            em->x3F4 = 0;
        }
        break;
    case 2:
        if (em_frame_check(em, 0, 212.0f) != 0) {
            Em_set_quake_sub(em, 2);
        }
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
    }
}

void em17_soukou_dm_sel_set(EMW *em) {
    if ((s32) em->hagi[6].cnt >= 2) {
        em17_act_set(em, 4, 0, 2);
        return;
    }
    em17_act_set(em, 4, 0xF, 2);
}

static void em_move00_005E0D20(EMW *em, EM17W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_act00_005DA9F0(em, w);
        break;
    case 1:
        em_act01_005DAAD0(em, w);
        break;
    case 2:
        em_act02_005DABF0(em, w);
        break;
    case 3:
        em_act03_005DAC80(em, w);
        break;
    case 4:
        em_act04_005DAD40(em, w);
        break;
    case 5:
        em_act05_005DADD0(em, w);
        break;
    case 6:
        em_act06_005DAEC0(em, w);
        break;
    case 7:
        em_act07_005DAF90(em, w);
        break;
    case 8:
        em_act08_005DB050(em, w);
        break;
    case 9:
        em_act09_005DB120(em, w);
        break;
    case 10:
        em_act10_005DB1C0(em, w);
        break;
    case 11:
        em_act11_005DB2D0(em, w);
        break;
    case 12:
        em_act12_005DB3A0(em, w);
        break;
    case 13:
        em_act13_005DB440(em, w);
        break;
    case 14:
        em_act14_005DB510(em, w);
        break;
    case 15:
        em_act15_005DB5D0(em, w);
        break;
    case 16:
        em_act16_005DB720(em, w);
        break;
    case 17:
        em_act17_005DB850(em, w);
        break;
    case 18:
        em_act18_005DB9A0(em, w);
        break;
    case 19:
        em_act19_005DB8D0(em, w);
        break;
    case 20:
        em_act20_005DBB00(em, w);
        break;
    case 21:
        em_act21_005DBC10(em, w);
        break;
    case 22:
        em_act22_005DBD70(em, w);
        break;
    case 23:
        em_act23_005DBDF0(em, w);
        break;
    case 24:
        em_act24_005DBEC0(em, w);
        break;
    case 25:
        em_act25_005DBF90(em, w);
        break;
    case 26:
        em_act26_005DC010(em, w);
        break;
    case 27:
        em_act27_005DC0C0(em, w);
        break;
    case 28:
        em_act28_005DC1D0(em, w);
        break;
    case 29:
        em_act29_005DC2A0(em, w);
        break;
    case 31:
        em_act31_005DC3E0(em, w);
        break;
    case 33:
        em_act33_005DC4C0(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move01_005E0F60(EMW *em, EM17W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_mv00_005DC550(em, w);
        break;
    case 1:
        em_mv01_005DC6B0(em, w);
        break;
    case 2:
        em_mv02_005DC6C0(em, w);
        break;
    case 3:
        em_mv03_005DC740(em, w);
        break;
    case 4:
        em_mv04_005DCA10(em, w);
        break;
    case 5:
        em_mv05_005DCBD0(em, w);
        break;
    case 6:
        em_mv06_005DCD30(em, w);
        break;
    case 7:
        em_mv07_005DCE90(em, w);
        break;
    case 8:
        em_mv04_005DCA10(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move02_005E1030(EMW *em, EM17W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_fly00_005DCFD0(em, w);
        break;
    case 1:
        em_fly01_005DD0D0(em, w);
        break;
    case 2:
        em_fly02_005DD310(em, w);
        break;
    case 3:
        em_fly03_005DD420(em, w);
        break;
    case 4:
        em_fly04_005DD6D0(em, w);
        break;
    case 5:
        em_fly05_005DD840(em, w);
        break;
    case 7:
        em_fly07_005DD9B0(em, w);
        break;
    case 9:
        em_fly09_005DDAF0(em, w);
        break;
    case 14:
        em_fly14_005DDC60(em, w);
        break;
    case 16:
        em_fly16_005DDD20(em, w);
        break;
    case 20:
        em_fly20_005DDE20(em, w);
        break;
    case 21:
        em_fly21_005DDF40(em, w);
        break;
    case 23:
        em_fly23_005DE080(em, w);
        break;
    }
}

static void em_move03_005E11A0(EMW *em, EM17W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_atk00_005DE2C0(em, w);
        break;
    case 2:
        em_atk02_005DE360(em, w);
        break;
    case 3:
        em_atk03_005DE370(em, w);
        break;
    case 4:
        em_atk04_005DE3F0(em, w);
        break;
    case 5:
        em_atk05_005DE530(em, w);
        break;
    case 6:
        em_atk06_005DE710(em, w);
        break;
    case 7:
        em_atk07_005DE800(em, w);
        break;
    case 8:
        em_atk08_005DE9E0(em, w);
        break;
    case 9:
        em_atk09_005DEAA0(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move04_005E1270(EMW *em, EM17W *w) {
    u8 temp_v1;

    temp_v1 = em->x15;
    switch (temp_v1) {
    case 0:
        em_dmg00_005DEB30(em, w);
        break;
    case 1:
        em_dmg01_005DEBD0(em, w);
        break;
    case 2:
        em_dmg02_005DEC60(em, w);
        break;
    case 3:
        em_dmg03_005DECF0(em, w);
        break;
    case 4:
        em_dmg04_005DED80(em, w);
        break;
    case 5:
        em_dmg05_005DEF50(em, w);
        break;
    case 6:
        em_dmg06_005DEFF0(em, w);
        break;
    case 7:
        em_dmg07_005DF090(em, w);
        break;
    case 8:
        em_dmg08_005DF180(em, w);
        break;
    case 9:
        em_dmg09_005DF2A0(em, w);
        break;
    case 10:
        em_dmg10_005DF3D0(em, w);
        break;
    case 11:
        em_dmg11_005DF460(em, w);
        break;
    case 12:
        em_dmg12_005DF540(em, w);
        break;
    case 13:
        em_dmg13_005DF670(em, w);
        break;
    case 14:
        em_dmg14_005DF7A0(em, w);
        break;
    case 15:
        em_dmg15_005DF8C0(em, w);
        break;
    case 16:
        em_dmg16_005DF950(em, w);
        break;
    case 17:
        em_dmg17_005DFA40(em, w);
        break;
    case 18:
        em_dmg18_005DFAC0(em, w);
        break;
    case 19:
        em_dmg19_005DFBF0(em, w);
        break;
    case 20:
        em_dmg20_005DFCF0(em, w);
        /* fallthrough */
    default:
        break;
    }
}

static void em_move05_005E1400(EMW *em, EM17W *w) {
    u8 temp_a2;

    em->x40E = 5;
    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_die00_005E07A0(em, w);
        break;
    case 1:
        em_die01_005E08A0(em, w);
        break;
    case 2:
        em_die02_005E09C0(em, w);
        break;
    case 3:
        em_die03_005E0B90(em, w);
        break;
    }
}

static void em_move06_005E1490(EMW *em, EM17W *w) {
    u8 temp_a2;

    temp_a2 = em->x15;
    switch (temp_a2) {                              /* irregular */
    case 0:
        em_demo00_005DFDF0(em, w);
        break;
    case 1:
        em_demo00_005DFDF0(em, w);
        break;
    case 2:
        em_demo00_005DFDF0(em, w);
        break;
    case 3:
        em_demo00_005DFDF0(em, w);
        break;
    case 4:
        em_demo04_005E06F0(em, w);
        break;
    }
}

void em17_main(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    u8 sp3C;
    s16 temp_v0;
    u32 temp_v0_2;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a0_3;
    u8 temp_a0_4;
    u8 temp_a0_5;
    u8 temp_a0_6;
    u8 temp_v1;

    em_mode_timer_sub(em);
    em_no_floor_ck(em);
    em_no_battle_area_ck(em, 0, 1);
    temp_v0 = w->x06;
    if (temp_v0 != 0) {
        w->x06 = (s16) (temp_v0 - 1);
    }
    temp_v0_2 = Em_Dmg_Sys(em, &sp3C) & 0xFF;
    switch (temp_v0_2) {                            /* switch 1 */
    case 1:                                         /* switch 1 */
    case 2:                                         /* switch 1 */
        if (em->x388 == 2) {
            em17_act_set(em, 5, 2, 2);
        } else if (em->x9EA != 0) {
            em17_act_set(em, 5, 1, 2);
        } else {
            temp_a0 = em->mode;
            if (temp_a0 == 0) {
                if (em->x15 != 0x16) {
                    goto block_13;
                }
                goto block_19;
            }
block_13:
            if (((temp_a0 == 0) && (em->x15 == 0x1A)) || ((temp_a0 == 0) && (em->x15 == 8)) || ((temp_a0 == 0) && (em->x15 == 0xC))) {
block_19:
                em17_act_set(em, 5, 3, 2);
            } else {
                em17_act_set(em, 5, 0, 2);
            }
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
            em17_act_set(em, 4, 0xC, 2);
        }
        break;
    case 5:                                         /* switch 1 */
        if ((em->mode != 4) || (em->x15 != 8)) {
            em17_act_set(em, 4, 8, 2);
        }
        break;
    case 6:                                         /* switch 1 */
        if (em->x9EA != 0) {
            em_mahi_dmg_timer_set(em);
            em17_act_set(em, 4, 0xE, 2);
        } else {
            temp_a0_2 = em->mode;
            if ((temp_a0_2 != 4) || (em->x15 != 0xB)) {
                if (temp_a0_2 == 4) {
                    if (em->x15 != 8) {
                        goto block_43;
                    }
                } else {
block_43:
                    em_mahi_dmg_timer_set(em);
                    em17_act_set(em, 4, 0xB, 2);
                }
            }
        }
        break;
    case 7:                                         /* switch 1 */
        if (em->x9EA != 0) {
            em_sleep2_dmg_timer_set(em);
            em17_act_set(em, 0, 0x1F, 2);
        } else {
            temp_a0_3 = em->mode;
            if (temp_a0_3 == 0) {
                if (em->x15 != 0x1B) {
                    goto block_50;
                }
            } else {
block_50:
                if (temp_a0_3 == 4) {
                    if (em->x15 != 8) {
                        goto block_53;
                    }
                } else {
block_53:
                    em_sleep2_dmg_timer_set(em);
                    em17_act_set(em, 0, 0x1B, 2);
                }
            }
        }
        break;
    case 8:                                         /* switch 1 */
        if (em->x9EA != 0) {
            em_sleep_dmg_timer_set(em);
            em17_act_set(em, 0, 0x1D, 2);
        } else {
            temp_a0_4 = em->mode;
            if (temp_a0_4 == 0) {
                if (em->x15 != 0x14) {
                    goto block_60;
                }
            } else {
block_60:
                if (temp_a0_4 == 4) {
                    if (em->x15 != 8) {
                        goto block_63;
                    }
                } else {
block_63:
                    em_sleep_dmg_timer_set(em);
                    em17_act_set(em, 0, 0x14, 2);
                }
            }
        }
        break;
    case 10:                                        /* switch 1 */
        temp_v1 = em->x15;
        switch (temp_v1) {                          /* switch 2; irregular */
        case 18:                                    /* switch 2 */
            em17_act_set(em, 0, 0x17, 2);
            em->x839 = 0;
            em_ikari_add(em, em->x8B0);
            break;
        case 20:                                    /* switch 2 */
            em17_act_set(em, 0, 0x18, 2);
block_107:
            em->x839 = 0;
            break;
        case 27:                                    /* switch 2 */
            em17_act_set(em, 0, 0x1C, 2);
            goto block_107;
        case 29:                                    /* switch 2 */
            em17_act_set(em, 4, 0x13, 2);
            goto block_107;
        case 31:                                    /* switch 2 */
            em17_act_set(em, 4, 0x14, 2);
            goto block_107;
        }
        break;
    case 11:                                        /* switch 1 */
        em17_act_set(em, 4, 4, 2);
        break;
    case 12:                                        /* switch 1 */
        pl_flag_clr((PLW *) em, 0x20000);
        temp_a0_5 = em->kind;
        if ((temp_a0_5 == 0x11) && (em->hagi[6].cnt == 2)) {
            Quest_enemy_hagi_set(em, 0x10);
        } else if ((temp_a0_5 == 0x16) && (em->hagi[6].cnt == 1)) {
            Quest_enemy_hagi_set(em, 0x20);
        }
        if (em->x388 == 2) {
            em17_act_set(em, 4, 8, 2);
        } else {
            temp_a0_6 = (u8) em->x38E;
            switch (temp_a0_6) {                    /* switch 3 */
            case 6:                                 /* switch 3 */
                em17_soukou_dm_sel_set(em);
                break;
            case 0:                                 /* switch 3 */
            case 7:                                 /* switch 3 */
                em17_act_set(em, 4, 0xF, 2);
                break;
            case 5:                                 /* switch 3 */
                em17_act_set(em, 4, 2, 2);
                break;
            case 1:                                 /* switch 3 */
            case 2:                                 /* switch 3 */
                em17_act_set(em, 4, 3, 2);
                break;
            default:                                /* switch 3 */
                if ((s32) M2C_FIELD((((temp_a0_6 & 0xFF) * 8) + em), u8 *, 0x30A) >= 2) {
                    if (temp_a0_6 != 3) {
                        em17_act_set(em, 4, 5, 2);
                    } else {
                        em17_act_set(em, 4, 6, 2);
                    }
                } else {
                    em17_act_set(em, 4, 1, 2);
                }
                break;
            }
        }
        break;
    case 13:                                        /* switch 1 */
        if (em->x388 != 2) {
            em17_act_set(em, 4, 0x10, 2);
            goto block_107;
        }
        break;
    }
    if (em->x734 != 3) {

    } else if (em->x839 != 0) {
        em_cmd_ck(em);
        em->x839 = 0;
    }
    em17_main_sub(em, w);
    if (em->x6FF != 0) {
        em17_main_sub(em, w);
        em->x6FF = 0;
    }
    if ((em->mode == 2) && ((s8) w->x18 == 1)) {
        em->x8BB = 2;
    }
}

void em17_main_sub(EMW *em, EM17W *w) {
    u8 temp_v1;

    em->mode_old = em->mode;
    em->x15_old = em->x15;
    temp_v1 = em->mode;
    switch (temp_v1) {
    case 0:
        em_move00_005E0D20(em, w);
        break;
    case 1:
        em_move01_005E0F60(em, w);
        break;
    case 2:
        em_move02_005E1030(em, w);
        break;
    case 3:
        em_move03_005E11A0(em, w);
        break;
    case 4:
        em_move04_005E1270(em, w);
        break;
    case 5:
        em_move05_005E1400(em, w);
        break;
    case 6:
        em_move06_005E1490(em, w);
        break;
    case 7:
        em_move06_005E1490(em, w);
        break;
    }
    if ((em->pos[0] <= 0.0f) || (em->pos[2] <= 0.0f)) {
        em_dur_set(em, 0);
    }
}

void em17_uvmove(EMW *em) {
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

static void sound_call_sub_005E1F80(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_005E1FF0(EMW *em, int frame, int se, int joint) {
    EM17W *w = (EM17W *)em->ex;
    int add;

    add = 0;
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        if (w->x1B == 1) {
            if (joint != 0x14) {
                if (joint == 0x1A) {
                    add = 0x35;
                }
            } else {
                add = 0x35;
            }
        }
        sound_call_sub_005E1F80(em, se + add, joint);
    }
}

static void sound_call_parts_005E2090(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

static void quake_call_005E2130(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

static void move_default_005E2180(EMW *em) {
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

static void ef_move_sub_005E21D0(EMW *em, EM17W *w) {
    f32 sp50[3];
    f32 v[3];
    f32 v2[3];
    s16 temp_v1_2;
    u16 temp_v1;

    temp_v1 = em->char0;
    if (temp_v1 != w->anim) {
        w->anim = (s16) temp_v1;
    }
    w->x1B = GetYouganHit(em->pos);
    temp_v1_2 = w->anim;
    switch (temp_v1_2) {                            /* irregular */
    case 0x3E9:
    case 0x3EA:
        sound_call_005E1FF0(em, 0x2C, 1, 0x14);
        sound_call_005E1FF0(em, 0x74, 1, 0x1A);
        quake_call_005E2130(em, 0x2C, 1);
        quake_call_005E2130(em, 0x74, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell19_set(em, 9);
        }
        if (em_frame_check(em, 0, 70.0f) != 0) {
            shell19_set(em, 0xA);
        }
        break;
    case 0x3ED:
        sound_call_005E1FF0(em, 6, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x19, 1, 0x14);
        sound_call_005E1FF0(em, 0x38, 1, 0x1A);
        quake_call_005E2130(em, 0x3C, 1);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell19_set(em, 5);
        }
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell19_set(em, 6);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(*(u16 *)0x3F340E & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x3EE:
        sound_call_005E1FF0(em, 6, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x19, 1, 0x1A);
        sound_call_005E1FF0(em, 0x38, 1, 0x14);
        quake_call_005E2130(em, 0x3C, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell19_set(em, 7);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell19_set(em, 8);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 0);
            return;
        }
        break;
    case 0x3F2:
        sound_call_005E1FF0(em, 6, 0x2B, 0x23);
        sound_call_005E1FF0(em, 6, 4, 0x1A);
        sound_call_005E1FF0(em, 0x4C, 0, 0x14);
        sound_call_005E1FF0(em, 0x1C, 0x12, 0x14);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            shell19_set(em, 0x13);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell19_set(em, 0x14);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            shell19_set(em, 0x14);
            return;
        }
        break;
    case 0x3F3:
        sound_call_005E1FF0(em, 0xE, 0xB, 6);
        sound_call_005E1FF0(em, 0x12, 0xB, 0xC);
        sound_call_005E1FF0(em, 0x46, 0xB, 6);
        sound_call_005E1FF0(em, 0x42, 0xB, 0xC);
        sound_call_005E1FF0(em, 0x7A, 0xB, 6);
        sound_call_005E1FF0(em, 0x7E, 0xB, 0xC);
        if ((em_frame_check(em, 0, 52.0f) == 0) && (em_frame_check(em, 0, 104.0f) == 0)) {
            if (em_frame_check(em, 0, 162.0f) != 0) {
                goto block_115;
            }
        } else {
block_115:
            Eft13_set_em_scl(em, 2, 5.0f, 7);
            return;
        }
        break;
    case 0x3F5:
        sound_call_005E1FF0(em, 0xE, 0xF, 6);
        sound_call_005E1FF0(em, 0x12, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x2C, 0xF, 6);
        sound_call_005E1FF0(em, 0x28, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x42, 0xF, 6);
        sound_call_005E1FF0(em, 0x46, 0xF, 0xC);
        if ((em_frame_check(em, 0, 26.0f) != 0) || (em_frame_check(em, 0, 52.0f) != 0)) {
            Eft13_set_em_scl(em, 1, 5.0f, 7);
            return;
        }
        break;
    case 0x3F7:
    case 0x3F8:
        sound_call_005E1FF0(em, 4, 0xC, 6);
        sound_call_005E1FF0(em, 8, 0xC, 0xC);
        sound_call_005E1FF0(em, 0x3A, 0xB, 6);
        sound_call_005E1FF0(em, 0x3E, 0xB, 0xC);
        sound_call_005E1FF0(em, 0x8E, 0xB, 6);
        sound_call_005E1FF0(em, 0x8A, 0xB, 0xC);
        sound_call_005E1FF0(em, 0xD4, 0xB, 6);
        sound_call_005E1FF0(em, 0xD8, 0xB, 0xC);
        if ((em_frame_check(em, 0, 8.0f) == 0) && (em_frame_check(em, 0, 86.0f) == 0)) {
            if (em_frame_check(em, 0, 160.0f) != 0) {
                goto block_123;
            }
        } else {
block_123:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 70.0f, 84.0f) == 0) && (em_frame_check3(em, 0, 146.0f, 156.0f) == 0)) {
                if (em_frame_check3(em, 0, 220.0f, 234.0f) != 0) {
                    goto block_130;
                }
            } else {
block_130:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3F9:
        sound_call_005E1FF0(em, 6, 0x27, 0x23);
        sound_call_005E1FF0(em, 0x1C, 1, 0x14);
        sound_call_005E1FF0(em, 0xA0, 6, 0x14);
        sound_call_005E1FF0(em, 0xC0, 6, 0x1A);
        if (em_frame_check(em, 0, 150.0f) != 0) {
            if (em->kind == 0x11) {
                shell19_set(em, 0xB);
            } else {
                shell19_set(em, 0x1B);
            }
        }
        if ((em_frame_check(em, 0, 140.0f) != 0) || (em_frame_check(em, 0, 214.0f) != 0)) {
            Eft20_set(1.0f, em, 2, 0);
        }
        if ((em_frame_check(em, 0, 128.0f) != 0) || (em_frame_check(em, 0, 182.0f) != 0)) {
            Eft20_set(1.0f, em, 3, 0);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 8.0f, 116.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x3FA:
        sound_call_005E1FF0(em, 0x60, 1, 0x14);
        sound_call_005E1FF0(em, 0x16, 1, 0x14);
        sound_call_005E1FF0(em, 4, 0x23, 0x23);
        sound_call_005E1FF0(em, 0x50, 0xF, 6);
        sound_call_005E1FF0(em, 0x54, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x62, 7, 0x1A);
        sound_call_005E1FF0(em, 0xA8, 0xB, 6);
        sound_call_005E1FF0(em, 0xAC, 0xB, 0xC);
        sound_call_005E1FF0(em, 0xDE, 0xB, 6);
        sound_call_005E1FF0(em, 0xE2, 0xB, 0xC);
        sound_call_005E1FF0(em, 0x110, 0xB, 6);
        sound_call_005E1FF0(em, 0x114, 0xB, 0xC);
        if ((em_frame_check(em, 0, 98.0f) != 0) && (em->pos[1] <= (1000.0f + em->x5AC))) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if ((em_frame_check(em, 0, 100.0f) != 0) || (em_frame_check(em, 0, 176.0f) != 0) || (em_frame_check(em, 0, 234.0f) != 0) || (em_frame_check(em, 0, 288.0f) != 0)) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 14.0f, 86.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x3FB:
        sound_call_005E1FF0(em, 2, 0xB, 6);
        sound_call_005E1FF0(em, 6, 0xB, 0xC);
        sound_call_005E1FF0(em, 0xC, 4, 0x1A);
        sound_call_005E1FF0(em, 4, 1, 0x14);
        sound_call_005E1FF0(em, 0xA8, 3, 0x14);
        sound_call_005E1FF0(em, 0xC8, 0, 0x1A);
        sound_call_005E1FF0(em, 0xE, 9, 0);
        sound_call_005E1FF0(em, 0x7C, 0x16, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell19_set(em, 0xD);
        }
        if (em_frame_check(em, 0, 16.0f) != 0) {
            ground_land_eff_set_005E63F0(em);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 18.0f, 44.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x405:
    case 0x406:
        sound_call_005E1FF0(em, 0x1E, 0x13, 0);
        sound_call_005E1FF0(em, 2, 0x20, 0x23);
        sound_call_005E1FF0(em, 4, 0xE, 0xC);
        sound_call_005E1FF0(em, 8, 0xE, 6);
        break;
    case 0x407:
        sound_call_005E1FF0(em, 2, 0x55, 0x23);
        sound_call_005E1FF0(em, 0xB6, 0x55, 0x23);
        sound_call_005E1FF0(em, 0x16C, 0x55, 0x23);
        v[1] = 10.0f;
        v[2] = 140.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v, 1.60000002f);
        break;
    case 0x408:
        sound_call_005E1FF0(em, 4, 0x57, 0x23);
        sound_call_005E1FF0(em, 0x46, 0, 0x1A);
        sound_call_005E1FF0(em, 0x8E, 3, 0x14);
        sound_call_005E1FF0(em, 4, 0x17, 0);
        sound_call_005E1FF0(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005E1FF0(em, 4, 0x57, 0x23);
        sound_call_005E1FF0(em, 0x46, 0x55, 0x23);
        sound_call_005E1FF0(em, 0x7E, 3, 0x1A);
        sound_call_005E1FF0(em, 0xB6, 3, 0x14);
        sound_call_005E1FF0(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_005E1FF0(em, 0x24, 0x57, 0x23);
        v2[1] = 10.0f;
        v2[2] = 140.0f;
        v2[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.60000002f);
        break;
    case 0x40B:
        sound_call_005E1FF0(em, 0x32, 2, 0x14);
        sound_call_005E1FF0(em, 0x3A, 6, 0x1A);
        sound_call_005E1FF0(em, 0x54, 1, 0x1A);
        sound_call_005E1FF0(em, 0x68, 4, 0x14);
        sound_call_005E1FF0(em, 0x7C, 0x11, 0x1A);
        sound_call_005E1FF0(em, 0xCC, 6, 0x1A);
        sound_call_005E1FF0(em, 0xD2, 2, 0x1A);
        sound_call_005E1FF0(em, 0xFE, 0x12, 0x14);
        sound_call_005E1FF0(em, 0x120, 2, 0x1A);
        sound_call_005E1FF0(em, 0x20, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x24, 0xF, 6);
        sound_call_005E1FF0(em, 0x80, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x84, 0xF, 6);
        sound_call_005E1FF0(em, 0x122, 0xE, 0xC);
        sound_call_005E1FF0(em, 0x126, 0xE, 6);
        sound_call_005E1FF0(em, 2, 0x15, 0);
        sound_call_005E1FF0(em, 2, 0x23, 0x23);
        sound_call_005E1FF0(em, 0x30, 0x21, 0x23);
        sound_call_005E1FF0(em, 0x80, 0x22, 0x23);
        sound_call_005E1FF0(em, 0xB8, 0x24, 0x23);
        sound_call_005E1FF0(em, 0x120, 0x22, 0x23);
        break;
    case 0x40C:
        sound_call_005E1FF0(em, 0x24, 0x24, 0x23);
        sound_call_005E1FF0(em, 0x14, 0, 0x1A);
        sound_call_005E1FF0(em, 0x28, 0, 0x14);
        sound_call_005E1FF0(em, 0x3C, 0x15, 0x22);
        sound_call_005E1FF0(em, 0x3C, 0xC, 0xC);
        sound_call_005E1FF0(em, 0xC, 0xC, 6);
        sound_call_005E1FF0(em, 0xA6, 0, 0x1A);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            if (em->kind == 0x11) {
                shell19_set(em, 1);
            } else {
                shell19_set(em, 0x19);
            }
        }
        if (em_frame_check(em, 0, 46.0f) != 0) {
            shell19_set(em, 0x15);
            return;
        }
        break;
    case 0x40D:
        sound_call_005E1FF0(em, 0x1E, 0x20, 0x23);
        sound_call_005E1FF0(em, 0xC, 0xC, 0x1A);
        sound_call_005E1FF0(em, 0x1C, 2, 0x1A);
        sound_call_005E1FF0(em, 0x2E, 3, 0x14);
        sound_call_005E1FF0(em, 0x48, 1, 0x1A);
        break;
    case 0x40E:
        sound_call_005E1FF0(em, 4, 0x57, 0x23);
        sound_call_005E1FF0(em, 0x48, 0x16, 0);
        sound_call_005E1FF0(em, 0xA4, 9, 0);
        sound_call_005E1FF0(em, 0x90, 3, 0xC);
        sound_call_005E1FF0(em, 0x88, 0xE, 6);
        sound_call_005E1FF0(em, 0xB2, 4, 0x22);
        if (em_frame_check(em, 0, 148.0f) != 0) {
            shell19_set(em, 0xF);
        }
        if (em_frame_check(em, 0, 156.0f) != 0) {
            Eft20_set(1.0f, em, 0, 0);
            return;
        }
        break;
    case 0x40F:
        sound_call_005E1FF0(em, 2, 0x57, 0x23);
        sound_call_005E1FF0(em, 2, 0x17, 0);
        sound_call_005E1FF0(em, 0x40, 3, 0x1A);
        sound_call_005E1FF0(em, 0x66, 0x10, 0x1A);
        break;
    case 0x413:
        sound_call_005E1FF0(em, 8, 0x23, 0x23);
        sound_call_005E1FF0(em, 0xE, 0xF, 6);
        sound_call_005E1FF0(em, 0x1E, 0x14, 0x2A);
        sound_call_005E1FF0(em, 0x2A, 0, 0x1A);
        sound_call_005E1FF0(em, 0x48, 1, 0x14);
        quake_call_005E2130(em, 0x2C, 1);
        quake_call_005E2130(em, 0x49, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            if (em->kind == 0x11) {
                shell19_set(em, 0x10);
                return;
            }
            shell19_set(em, 0x1C);
            return;
        }
        break;
    case 0x414:
        sound_call_005E1FF0(em, 0x1C, 0xB, 6);
        sound_call_005E1FF0(em, 0x20, 0xB, 0xC);
        sound_call_005E1FF0(em, 6, 0x1A, 0x22);
        sound_call_005E1FF0(em, 0xA, 0x1B, 0xC);
        sound_call_005E1FF0(em, 0xE, 0x1B, 6);
        sound_call_005E1FF0(em, 0x26, 0x1A, 0x22);
        sound_call_005E1FF0(em, 0x2A, 0x1B, 0xC);
        sound_call_005E1FF0(em, 0x32, 0x1B, 6);
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x417:
        sound_call_005E1FF0(em, 2, 0x12, 0);
        sound_call_005E1FF0(em, 0x50, 0x18, 0x23);
        sound_call_005E1FF0(em, 0x14, 0x28, 0x23);
        sound_call_005E1FF0(em, 0x4A, 0x29, 0x23);
        sound_call_005E1FF0(em, 0x4C, 0xD, 6);
        sound_call_005E1FF0(em, 0x48, 0xD, 0xC);
        sound_call_005E1FF0(em, 0xB4, 0x13, 6);
        sound_call_005E1FF0(em, 0xBB, 0x13, 0xC);
        sound_call_005E1FF0(em, 0xF0, 0x12, 0);
        break;
    case 0x418:
        sound_call_005E1FF0(em, 2, 0x22, 0x23);
        sound_call_005E1FF0(em, 0x22, 0x22, 0x23);
        sound_call_005E1FF0(em, 0x40, 0x22, 0x23);
        sound_call_005E1FF0(em, 0x72, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x10, 1, 0x14);
        break;
    case 0x419:
        sound_call_005E1FF0(em, 0x20, 0xF, 6);
        sound_call_005E1FF0(em, 0x24, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x74, 0xB, 6);
        sound_call_005E1FF0(em, 0x78, 0xB, 0xC);
        sound_call_005E1FF0(em, 0xBE, 0xB, 6);
        sound_call_005E1FF0(em, 0xC2, 0xB, 0xC);
        break;
    case 0x41A:
        sound_call_005E1FF0(em, 0x4E, 0x29, 0x23);
        sound_call_005E1FF0(em, 0x4E, 0x18, 0x23);
        sound_call_005E1FF0(em, 0x78, 0x18, 0x23);
        sound_call_005E1FF0(em, 0xA4, 0x18, 0x23);
        sound_call_005E1FF0(em, 4, 0x12, 0x1A);
        sound_call_005E1FF0(em, 4, 0x13, 0x22);
        sound_call_005E1FF0(em, 0x40, 0xF, 6);
        sound_call_005E1FF0(em, 0x40, 0xF, 0xC);
        sound_call_005E1FF0(em, 0xFA, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x12A, 0x12, 0x14);
        sound_call_005E1FF0(em, 0x11E, 0x13, 0);
        break;
    case 0x41B:
        sound_call_005E1FF0(em, 4, 0x13, 0x2A);
        sound_call_005E1FF0(em, 0x26, 0x10, 0x1A);
        sound_call_005E1FF0(em, 0x38, 0x12, 0x1A);
        sound_call_005E1FF0(em, 0x58, 6, 0x1A);
        sound_call_005E1FF0(em, 0x96, 0x2E, 0x23);
        sound_call_005E1FF0(em, 0x9A, 0x2E, 0x23);
        sound_call_005E1FF0(em, 0xEE, 0x16, 0);
        break;
    case 0x41C:
        sound_call_005E1FF0(em, 0x16, 0x28, 0x23);
        sound_call_005E1FF0(em, 0x48, 0x29, 0x23);
        sound_call_005E1FF0(em, 0x52, 0x18, 0x23);
        sound_call_005E1FF0(em, 6, 0x14, 0x22);
        sound_call_005E1FF0(em, 0x16, 0, 0x1A);
        sound_call_005E1FF0(em, 0x3A, 0xF, 6);
        sound_call_005E1FF0(em, 0x3E, 0xF, 0xC);
        sound_call_005E1FF0(em, 0x6A, 0xB, 6);
        sound_call_005E1FF0(em, 0x6E, 0xB, 0xC);
        sound_call_005E1FF0(em, 0xAC, 0xB, 6);
        sound_call_005E1FF0(em, 0xB0, 0xB, 0xC);
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
        sound_call_005E1FF0(em, 2, 0x12, 0);
        sound_call_005E1FF0(em, 0x1C, 4, 0x1A);
        sound_call_005E1FF0(em, 0x38, 4, 0x14);
        sound_call_005E1FF0(em, 0x1C, 5, 0x1A);
        sound_call_005E1FF0(em, 0x78, 4, 0x14);
        sound_call_005E1FF0(em, 0x108, 4, 0x1A);
        sound_call_005E1FF0(em, 0x16, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x20, 0x28, 0x23);
        sound_call_005E1FF0(em, 0x84, 0x27, 0x23);
        sound_call_005E1FF0(em, 0x84, 0x51, 0x23);
        sound_call_005E1FF0(em, 0xA, 0xC, 0xC);
        sound_call_005E1FF0(em, 0x10, 0xC, 6);
        if (em_frame_check(em, 0, 102.0f) != 0) {
            shell19_set(em, 0x11);
        }
        if (em_frame_check(em, 0, 110.0f) != 0) {
            shell19_set(em, 0x12);
            Eft15_set3(em, 5, 1.0f, 4);
            return;
        }
        break;
    case 0x424:
        sound_call_005E1FF0(em, 6, 0x53, 0x23);
        sound_call_005E1FF0(em, 0xD4, 0x23, 0x23);
        sound_call_005E1FF0(em, 0x48, 0x13, 0x2A);
        sound_call_005E1FF0(em, 0x30, 6, 0x1A);
        sound_call_005E1FF0(em, 0x30, 0x11, 0x14);
        sound_call_005E1FF0(em, 0x30, 3, 0x14);
        sound_call_005E1FF0(em, 0xAE, 1, 0x1A);
        sound_call_005E1FF0(em, 0xDE, 0, 0x14);
        sound_call_005E1FF0(em, 0x34, 0xC, 6);
        sound_call_005E1FF0(em, 0x38, 0xD, 0xC);
        if (em_frame_check(em, 0, 48.0f) != 0) {
            Eft13_set_em(em, 0x1B, 6);
            return;
        }
        break;
    case 0x425:
        sound_call_005E1FF0(em, 6, 0x53, 0x23);
        sound_call_005E1FF0(em, 0xA, 0x12, 0x1A);
        sound_call_005E1FF0(em, 0x26, 6, 0x1A);
        sound_call_005E1FF0(em, 0x4E, 4, 0x14);
        sound_call_005E1FF0(em, 0x6E, 3, 0x14);
        sound_call_005E1FF0(em, 0xAC, 0, 0x1A);
        sound_call_005E1FF0(em, 0x5A, 0x15, 0);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
            return;
        }
        break;
    case 0x426:
        sound_call_005E1FF0(em, 4, 0x4F, 0x23);
        sound_call_005E1FF0(em, 0x56, 0x1F, 0x23);
        sound_call_005E1FF0(em, 0x10, 3, 0x14);
        sound_call_005E1FF0(em, 4, 0xD, 6);
        sound_call_005E1FF0(em, 8, 0xD, 0xC);
        sound_call_005E1FF0(em, 0x7E, 0, 0x1A);
        sound_call_005E1FF0(em, 0x62, 0x13, 0x2A);
        break;
    case 0x427:
        sound_call_005E1FF0(em, 0xE, 1, 0x1A);
        sound_call_005E1FF0(em, 4, 0x52, 0x23);
        sound_call_005E1FF0(em, 4, 0x13, 0);
        sound_call_005E1FF0(em, 0x4C, 0x1F, 0x23);
        break;
    case 0x428:
        sound_call_005E1FF0(em, 4, 0x52, 0x23);
        sound_call_005E1FF0(em, 4, 0x13, 0);
        sound_call_005E1FF0(em, 0x44, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x78, 1, 0x1A);
        break;
    case 0x429:
        sound_call_005E1FF0(em, 4, 0x2C, 0x23);
        sound_call_005E1FF0(em, 0x3E, 4, 0x2C);
        sound_call_005E1FF0(em, 0x12, 0x12, 0x14);
        sound_call_005E1FF0(em, 0x2C, 6, 0x1A);
        sound_call_005E1FF0(em, 0x3A, 0x10, 0x1A);
        sound_call_005E1FF0(em, 0x3E, 9, 0);
        sound_call_005E1FF0(em, 0x4A, 9, 0);
        sound_call_005E1FF0(em, 0x64, 0x12, 0);
        sound_call_005E1FF0(em, 0xE, 0xC, 6);
        sound_call_005E1FF0(em, 0x12, 0xC, 0xC);
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
        sound_call_005E1FF0(em, 4, 0x52, 0x23);
        sound_call_005E1FF0(em, 4, 0x13, 0);
        break;
    case 0x42B:
        sound_call_005E1FF0(em, 0x46, 0x55, 0x23);
        sound_call_005E1FF0(em, 0x46, 0x17, 0);
        break;
    case 0x42C:
        sound_call_005E1FF0(em, 0x1C, 2, 0x1A);
        sound_call_005E1FF0(em, 0x30, 4, 0x14);
        sound_call_005E1FF0(em, 0xDC, 0x10, 0x1A);
        sound_call_005E1FF0(em, 0x1E, 0x16, 0);
        sound_call_005E1FF0(em, 0x108, 9, 0);
        sound_call_005E1FF0(em, 0x108, 7, 0);
        sound_call_005E1FF0(em, 0x114, 0x17, 0);
        sound_call_005E1FF0(em, 0x180, 0x16, 0);
        sound_call_005E1FF0(em, 0x18, 0x52, 0x23);
        sound_call_005E1FF0(em, 0x8C, 0x28, 0x23);
        sound_call_005E1FF0(em, 0xB4, 0x2C, 0x23);
        sound_call_005E1FF0(em, 0x13A, 0x2F, 0x23);
        sound_call_005E1FF0(em, 0x192, 0x2B, 0x23);
        sound_call_005E1FF0(em, 0x174, 0xF, 6);
        sound_call_005E1FF0(em, 0x176, 0xE, 0xC);
        if (em_frame_check(em, 0, 268.0f) != 0) {
            Eft20_set(0.899999976f, em, 0, 0);
            return;
        }
        break;
    case 0x42D:
    case 0x432:
        sound_call_005E1FF0(em, 4, 0x52, 0);
        sound_call_005E1FF0(em, 0x16, 6, 0x2A);
        sound_call_005E1FF0(em, 4, 0x12, 0x1A);
        sound_call_005E1FF0(em, 0x10, 0xC, 6);
        sound_call_005E1FF0(em, 0x1A, 0xD, 0xC);
        sound_call_005E1FF0(em, 0x92, 9, 0);
        sound_call_005E1FF0(em, 0x92, 7, 0);
        sound_call_005E1FF0(em, 0xBA, 7, 0);
        sound_call_005E1FF0(em, 0xEE, 7, 0);
        sound_call_005E1FF0(em, 0x11C, 8, 0);
        sound_call_005E1FF0(em, 0x11C, 7, 0);
        sound_call_005E1FF0(em, 0x6C, 0x16, 0);
        sound_call_005E1FF0(em, 0xF2, 0x16, 0);
        sound_call_005E1FF0(em, 0x19A, 0x16, 0);
        sound_call_005E1FF0(em, 0x1CA, 1, 0x1A);
        sound_call_005E1FF0(em, 0x1EE, 0, 0x14);
        if (em_frame_check(em, 0, 162.0f) != 0) {
            if (em->kind == 0x11) {
                shell19_set(em, 0x17);
            } else {
                shell19_set(em, 0x1D);
            }
        }
        if (w->anim == 0x432) {
            if (em_frame_check(em, 0, 142.0f) != 0) {
                Eft20_set(1.0f, em, 0x12, 0x81);
            }
        } else if (em_frame_check(em, 0, 142.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x80);
        }
        if ((em_frame_check(em, 0, 148.0f) != 0) || (em_frame_check(em, 0, 188.0f) != 0) || (em_frame_check(em, 0, 238.0f) != 0) || (em_frame_check(em, 0, 282.0f) != 0)) {
            Eft20_set(1.0f, em, 1, 0x80);
            return;
        }
        break;
    case 0x42E:
    case 0x433:
        sound_call_005E1FF0(em, 8, 0x16, 0);
        sound_call_005E1FF0(em, 4, 0x1F, 0x23);
        break;
    case 0x42F:
        sound_call_005E1FF0(em, 4, 0x16, 0x23);
        sound_call_005E1FF0(em, 0x1E, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x30, 1, 0x14);
        sound_call_005E1FF0(em, 0x46, 1, 0x1A);
        break;
    case 0x430:
        sound_call_005E1FF0(em, 4, 0x16, 0x23);
        sound_call_005E1FF0(em, 0x1E, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x30, 1, 0x1A);
        sound_call_005E1FF0(em, 0x46, 1, 0x14);
        break;
    case 0x431:
        sound_call_005E1FF0(em, 4, 0x1E, 0x22);
        sound_call_005E1FF0(em, 4, 0x1D, 6);
        sound_call_005E1FF0(em, 8, 0x1D, 0xC);
        sound_call_005E1FF0(em, 0xA, 0x2A, 0x23);
        sound_call_005E1FF0(em, 0x4C, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x18, 0x12, 0);
        sound_call_005E1FF0(em, 0x1E, 0x10, 0);
        sound_call_005E1FF0(em, 0x1E, 9, 0);
        sound_call_005E1FF0(em, 0x22, 7, 0);
        quake_call_005E2130(em, 0x1A, 2);
        if ((em_frame_check2(em, 0, 28.0f) != 0) && (em_frame_check2(em, 0, 52.0f) == 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0xA, 0x80);
            return;
        }
        break;
    case 0x434:
        sound_call_005E1FF0(em, 0x10, 0x5A, 0x23);
        em_mahi_eff_set(em, 2);
        break;
    case 0x435:
        sound_call_005E1FF0(em, 4, 0x2B, 0x23);
        sound_call_005E1FF0(em, 0x14, 0xB, 0xC);
        sound_call_005E1FF0(em, 0x3A, 0x14, 0);
        break;
    case 0x436:
        sound_call_005E1FF0(em, 0xA, 9, 0);
        sound_call_005E1FF0(em, 0xA, 7, 0x2A);
        sound_call_005E1FF0(em, 0xC, 0x11, 0);
        sound_call_005E1FF0(em, 0x10, 0x2C, 0x23);
        if (em_frame_check(em, 0, 8.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0);
            return;
        }
        break;
    case 0x437:
        sound_call_005E1FF0(em, 0xC, 0x24, 0x23);
        sound_call_005E1FF0(em, 0x24, 0x22, 0x23);
        sound_call_005E1FF0(em, 0x66, 0x22, 0x23);
        sound_call_005E1FF0(em, 0xA4, 0x22, 0x23);
        break;
    case 0x438:
        sound_call_005E1FF0(em, 2, 0x16, 0x22);
        sound_call_005E1FF0(em, 0x64, 0x17, 0x22);
        sound_call_005E1FF0(em, 0x26, 0x55, 0x23);
        sound_call_005E1FF0(em, 0xC8, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x1C, 3, 0x1A);
        sound_call_005E1FF0(em, 0x140, 3, 0x1A);
        break;
    case 0x439:
        sound_call_005E1FF0(em, 2, 0x16, 0x22);
        sound_call_005E1FF0(em, 0x64, 0x17, 0x22);
        sound_call_005E1FF0(em, 0x26, 0x55, 0x23);
        sound_call_005E1FF0(em, 0xC8, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x1C, 3, 0x14);
        sound_call_005E1FF0(em, 0x140, 3, 0x14);
        break;
    case 0x43A:
        sound_call_005E1FF0(em, 4, 0x16, 0);
        sound_call_005E1FF0(em, 0x32, 0x2C, 0x23);
        break;
    case 0x43B:
        sound_call_005E1FF0(em, 4, 0x16, 0);
        sound_call_005E1FF0(em, 4, 0x17, 0);
        sound_call_005E1FF0(em, 0xA, 0x10, 0x1A);
        sound_call_005E1FF0(em, 0xA, 0, 0x1A);
        sound_call_005E1FF0(em, 0x1C, 0, 0x14);
        sound_call_005E1FF0(em, 0x34, 0, 0x1A);
        sound_call_005E1FF0(em, 0x58, 0, 0x14);
        sound_call_005E1FF0(em, 0x80, 0, 0x1A);
        sound_call_005E1FF0(em, 0xC6, 0, 0x14);
        sound_call_005E1FF0(em, 4, 0x2B, 0x23);
        sound_call_005E1FF0(em, 0xA8, 0x27, 0x23);
        sound_call_005E1FF0(em, 0xA2, 0xD, 6);
        sound_call_005E1FF0(em, 0xA6, 0xD, 0xC);
        break;
    case 0x43C:
        sound_call_005E1FF0(em, 0x16, 0xD, 6);
        sound_call_005E1FF0(em, 0x1A, 0xD, 0xC);
        sound_call_005E1FF0(em, 0x34, 0xA, 6);
        sound_call_005E1FF0(em, 0x38, 0xE, 0xC);
        sound_call_005E1FF0(em, 0x4E, 0xE, 6);
        sound_call_005E1FF0(em, 0x4A, 0xA, 0xC);
        sound_call_005E1FF0(em, 0x58, 0xC, 6);
        sound_call_005E1FF0(em, 0x5C, 0xC, 0xC);
        sound_call_005E1FF0(em, 0x20, 5, 0x1A);
        sound_call_005E1FF0(em, 0x1C, 0, 0x14);
        if ((em_frame_check(em, 0, 42.0f) == 0) && (em_frame_check(em, 0, 60.0f) == 0)) {
            if (em_frame_check(em, 0, 80.0f) != 0) {
                goto block_284;
            }
        } else {
block_284:
            Eft20_set(1.0f, em, 1, 0);
            return;
        }
        break;
    case 0x43F:
        sound_call_005E1FF0(em, 4, 0x1A, 0x22);
        sound_call_005E1FF0(em, 8, 0x1B, 0xC);
        sound_call_005E1FF0(em, 4, 0x1B, 6);
        sound_call_005E1FF0(em, 0x22, 0x1C, 0x22);
        sound_call_005E1FF0(em, 0x40, 0x1C, 0x22);
        break;
    case 0x442:
        sound_call_005E1FF0(em, 0x20, 0xD, 6);
        sound_call_005E1FF0(em, 0x24, 0xD, 0xC);
        sound_call_005E1FF0(em, 0x4A, 0xD, 6);
        sound_call_005E1FF0(em, 0x4E, 0xD, 0xC);
        sound_call_005E1FF0(em, 0x7C, 0xB, 6);
        sound_call_005E1FF0(em, 0x80, 0xB, 0xC);
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
        sound_call_005E1FF0(em, 4, 0x10, 0);
        sound_call_005E1FF0(em, 0xA, 0x12, 0);
        sound_call_005E1FF0(em, 0x14, 9, 0);
        sound_call_005E1FF0(em, 4, 0x27, 0x23);
        sound_call_005E1FF0(em, 4, 0x52, 0x23);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0x80);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 4.0f, 60.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x448:
        sound_call_005E1FF0(em, 0x16, 0x12, 0);
        sound_call_005E1FF0(em, 0x48, 0x12, 0);
        sound_call_005E1FF0(em, 4, 0xC, 6);
        sound_call_005E1FF0(em, 8, 0xC, 0xC);
        sound_call_005E1FF0(em, 0x30, 0x13, 0);
        sound_call_005E1FF0(em, 4, 0x28, 0x23);
        sound_call_005E1FF0(em, 0x28, 0x2B, 0x23);
        sound_call_005E1FF0(em, 4, 0x16, 0);
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 12.0f, 24.0f) == 0) {
                if (em_frame_check3(em, 0, 46.0f, 90.0f) != 0) {
                    goto block_304;
                }
            } else {
block_304:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x449:
        sound_call_005E1FF0(em, 0x22, 0x16, 6);
        sound_call_005E1FF0(em, 0x26, 0x16, 0xC);
        sound_call_005E1FF0(em, 0x62, 4, 0x22);
        sound_call_005E1FF0(em, 0x6C, 3, 0x22);
        sound_call_005E1FF0(em, 4, 0x52, 0x23);
        sound_call_005E1FF0(em, 4, 0x2C, 0x23);
        break;
    case 0x44A:
        sound_call_005E1FF0(em, 4, 0x16, 6);
        sound_call_005E1FF0(em, 8, 0x16, 0xC);
        sound_call_005E1FF0(em, 0x6C, 3, 0x22);
        sound_call_005E1FF0(em, 4, 0x50, 0x23);
        sound_call_005E1FF0(em, 0x46, 0x20, 0x23);
        break;
    case 0x44C:
        sound_call_005E1FF0(em, 0x16, 4, 0x1A);
        sound_call_005E1FF0(em, 0x18, 8, 0x14);
        sound_call_005E1FF0(em, 0x1A, 0x10, 0x1A);
        sound_call_005E1FF0(em, 0xC8, 1, 0x1A);
        sound_call_005E1FF0(em, 0xE0, 0, 0x14);
        sound_call_005E1FF0(em, 0xA6, 0x16, 0);
        sound_call_005E1FF0(em, 0x64, 0x20, 0x23);
        if (em_frame_check(em, 0, 28.0f) != 0) {
            Eft20_set(1.79999995f, em, 2, 6);
        }
        if ((em_frame_check3(em, 0, 54.0f, 120.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft13_set_em_scl(em, 2, 7.0f, 3);
            return;
        }
        break;
    case 0x44D:
        sound_call_005E1FF0(em, 4, 0x17, 0);
        sound_call_005E1FF0(em, 0x18C, 0x16, 0);
        sound_call_005E1FF0(em, 0x32, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x7E, 0x57, 0x23);
        sound_call_005E1FF0(em, 0x178, 0x1F, 0x23);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 58.0f, 270.0f) != 0) || (em_frame_check3(em, 0, 410.0f, 440.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x44F:
        sound_call_005E1FF0(em, 4, 0x16, 0);
        sound_call_005E1FF0(em, 0x68, 0x16, 0);
        sound_call_005E1FF0(em, 4, 0x23, 0x23);
        sound_call_005E1FF0(em, 0x92, 0, 0x1A);
        sound_call_005E1FF0(em, 0xCE, 0, 0x14);
        break;
    case 0x450:
        sound_call_005E1FF0(em, 0x38, 0x22, 0x23);
        sound_call_005E1FF0(em, 0xAE, 0x22, 0x23);
        sound_call_005E1FF0(em, 0x118, 0x22, 0x23);
        sound_call_005E1FF0(em, 0x15E, 0x1F, 0x23);
        sound_call_005E1FF0(em, 0x1C, 1, 0x1A);
        sound_call_005E1FF0(em, 0x16E, 3, 0x14);
        sound_call_005E1FF0(em, 0x196, 0, 0x1A);
        if (((em_frame_check3(em, 0, 94.0f, 136.0f) != 0) || (em_frame_check3(em, 0, 184.0f, 228.0f) != 0)) && !(GAME_X1E16 & 0x1F)) {
            Eft21_set(em, 1);
            return;
        }
        break;
    case 0x452:
        sound_call_005E1FF0(em, 0x30, 0x20, 0x23);
        sound_call_005E1FF0(em, 0x6C, 0x56, 0x23);
        sound_call_005E1FF0(em, 4, 0x12, 0x14);
        sound_call_005E1FF0(em, 0x196, 0, 0x1A);
        if (em_frame_check(em, 0, 110.0f) != 0) {
            shell19_set(em, 0x12);
            Eft15_set3(em, 5, 1.0f, 0xE);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 64.0f, 468.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x454:
        sound_call_005E1FF0(em, 0x68, 0x51, 0x23);
        sound_call_005E1FF0(em, 0x68, 0x15, 0);
        sound_call_005E1FF0(em, 4, 0x16, 0);
        sound_call_005E1FF0(em, 0x26, 0x16, 0);
        sound_call_005E1FF0(em, 0x48, 9, 0xC);
        sound_call_005E1FF0(em, 0x4E, 9, 0xC);
        sound_call_005E1FF0(em, 0x4E, 0x10, 0x22);
        sound_call_005E1FF0(em, 0x76, 4, 0x14);
        sound_call_005E1FF0(em, 0x7E, 3, 0x1A);
        sound_call_005E1FF0(em, 0x58, 0xF, 0x14);
        sound_call_005E1FF0(em, 0x5C, 0xF, 0xC);
        sound_call_005E1FF0(em, 0xF8, 0, 0x1A);
        if (em_frame_check(em, 0, 70.0f) != 0) {
            shell19_set(em, 3);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            Eft13_set_em_scl(em, 2, 1.39999998f, 0x13);
            if (Em_stg_ck(em) != 0) {
                get_joint_pos_em(em, 2, sp50);
                sp50[1] = em->x5AC;
                Eft17_set_ex(sp50, (u16)em->ang[1], 8, 1.0f);
            }
        }
        if (em_frame_check(em, 0, 14.0f) != 0) {
            Eft13_set_em_scl(em, 0xC, 8.0f, 0x14);
        }
        if (em_frame_check(em, 0, 32.0f) != 0) {
            Eft13_set_em_scl(em, 6, 8.0f, 0x14);
            return;
        }
        break;
    case 0x455:
        sound_call_005E1FF0(em, 4, 0x1F, 0x23);
        sound_call_005E1FF0(em, 0x16, 0x12, 0x1A);
        sound_call_005E1FF0(em, 0x1A, 3, 0x1A);
        sound_call_005E1FF0(em, 0x8E, 4, 0x14);
        sound_call_005E1FF0(em, 0x124, 1, 0x1A);
        sound_call_005E1FF0(em, 0x162, 3, 0x14);
        sound_call_005E1FF0(em, 0x5A, 0x13, 0x2A);
        sound_call_005E1FF0(em, 0x4A, 0x15, 0);
        sound_call_005E1FF0(em, 0x4A, 8, 0);
        sound_call_005E1FF0(em, 0x98, 0x16, 0);
        if (em_frame_check(em, 0, 68.0f) != 0) {
            if (em->kind == 0x11) {
                shell19_set(em, 2);
            } else {
                shell19_set(em, 0x1A);
            }
        }
        if (em_frame_check(em, 0, 76.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 2.5f, 3);
        }
        if ((em_frame_check(em, 0, 72.0f) != 0) || (em_frame_check(em, 0, 74.0f) != 0)) {
            Eft13_set_em_scl(em, 0x14, 2.5f, 3);
            return;
        }
        break;
    case 0x456:
        if (em->mode != 0) {
            sound_call_005E1FF0(em, 0x3E, 0x27, 0x23);
            sound_call_005E1FF0(em, 0x96, 0x51, 0x23);
            sound_call_005E1FF0(em, 0xA2, 0x1B, 0x23);
            sound_call_005E1FF0(em, 0xA2, 0x19, 0x23);
            sound_call_005E1FF0(em, 0xC6, 0x1A, 0x23);
            sound_call_005E1FF0(em, 0xC6, 0x18, 0x23);
            sound_call_005E1FF0(em, 0xE, 0xD, 6);
            sound_call_005E1FF0(em, 0x12, 0xD, 0xC);
            sound_call_005E1FF0(em, 0x14, 6, 0x14);
            sound_call_005E1FF0(em, 0x142, 1, 0x14);
            sound_call_005E1FF0(em, 0x166, 0, 0x14);
        } else {
            sound_call_005E1FF0(em, 0x3E, 0x27, 0x23);
            sound_call_005E1FF0(em, 0x88, 0x54, 0x23);
            sound_call_005E1FF0(em, 0xE, 0xD, 6);
            sound_call_005E1FF0(em, 0x12, 0xD, 0xC);
            sound_call_005E1FF0(em, 0x14, 6, 0x14);
            sound_call_005E1FF0(em, 0x142, 1, 0x14);
            sound_call_005E1FF0(em, 0x166, 0, 0x14);
        }
        if (em_frame_check(em, 0, 150.0f) != 0) {
            shell19_set(em, 0x18);
        }
        if ((em_frame_check3(em, 0, 82.0f, 142.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 0);
        }
        if ((em_frame_check3(em, 0, 256.0f, 314.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x457:
        sound_call_005E1FF0(em, 0x14, 0x23, 0x23);
        sound_call_005E1FF0(em, 0x58, 0x24, 0x23);
        sound_call_005E1FF0(em, 0x7E, 0x56, 0x23);
        sound_call_005E1FF0(em, 0x14, 1, 0x1A);
        sound_call_005E1FF0(em, 0x44, 2, 0x14);
        if ((em->kind == 0x11) && !(em->x948 & 1) && (em_frame_check(em, 0, 56.0f) != 0)) {
            Eft13_set_em_scl(em, 0x2C, 2.0f, 7);
            return;
        }
        break;
    case 0x458:
        sound_call_005E1FF0(em, 0x32, 0x28, 0x23);
        sound_call_005E1FF0(em, 0x68, 0x27, 0x23);
        sound_call_005E1FF0(em, 0x38, 4, 0x14);
        sound_call_005E1FF0(em, 0xF8, 1, 0x14);
        sound_call_005E1FF0(em, 0x2E, 0x14, 0x2A);
        sound_call_005E1FF0(em, 0x68, 0x14, 0x2A);
        sound_call_005E1FF0(em, 0x9C, 0x14, 0x2A);
        sound_call_005E1FF0(em, 0xD2, 0x14, 0x2A);
        sound_call_005E1FF0(em, 0x112, 0x13, 0x2A);
        if (em_frame_check(em, 0, 46.0f) != 0) {
            shell19_set(em, 4);
            return;
        }
        break;
    case 0x459:
        sound_call_005E1FF0(em, 0x40, 0x16, 0);
        sound_call_005E1FF0(em, 0x48, 0x4F, 0x23);
        sound_call_005E1FF0(em, 0x70, 1, 0x14);
        sound_call_005E1FF0(em, 0x98, 0, 0x1A);
        break;
    case 0x45B:
        sound_call_005E1FF0(em, 4, 0x16, 0);
        sound_call_005E1FF0(em, 0x46, 0x1F, 0x23);
        break;
    case 0x45C:
        sound_call_005E1FF0(em, 4, 0x2B, 0x23);
        sound_call_005E1FF0(em, 0x34, 0x29, 0x23);
        sound_call_005E1FF0(em, 0x15C, 0x2A, 0x23);
        sound_call_005E1FF0(em, 0x1BA, 0x2A, 0x23);
        sound_call_005E1FF0(em, 0x20, 0x16, 0);
        sound_call_005E1FF0(em, 0x98, 0x16, 0);
        sound_call_005E1FF0(em, 0xC0, 0x10, 0x14);
        sound_call_005E1FF0(em, 0xD2, 0x16, 0);
        sound_call_005E1FF0(em, 0xD6, 8, 0);
        sound_call_005E1FF0(em, 0x12C, 7, 0);
        sound_call_005E1FF0(em, 0x14E, 0xD, 6);
        sound_call_005E1FF0(em, 0x15C, 0x16, 0);
        sound_call_005E1FF0(em, 0x1C8, 0x16, 0);
        if ((em_frame_check(em, 0, 198.0f) == 0) && (em_frame_check(em, 0, 284.0f) == 0) && (em_frame_check(em, 0, 328.0f) == 0) && (em_frame_check(em, 0, 368.0f) == 0) && (em_frame_check(em, 0, 408.0f) == 0)) {
            if (em_frame_check(em, 0, 468.0f) != 0) {
                goto block_381;
            }
        } else {
block_381:
            Eft20_set(1.0f, em, 1, 0);
            return;
        }
        break;
    case 0x45E:
        sound_call_005E1FF0(em, 0x18, 0x11, 0x1A);
        sound_call_005E1FF0(em, 0x70, 1, 0x1A);
        sound_call_005E1FF0(em, 0xA6, 0, 0x14);
        sound_call_005E1FF0(em, 0x28, 0x16, 0);
        break;
    default:
        move_default_005E2180(em);
        break;
    }
}

void em17_effect_move(EMW *em) {
    EM17W *w = (EM17W *)em->ex;
    u8 temp_a2;

    temp_a2 = w->eff;
    switch (temp_a2) {                              /* irregular */
    case 0:
        w->eff = temp_a2 + 1;
        break;
    case 1:
        ef_move_sub_005E21D0(em, w);
        break;
    }
    em17_uvmove(em);
}

static void ground_land_eff_set_005E63F0(EMW *em) {
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

static s32 kyusyu_char_set2_005E64A0(EMW *em) {
    f32 temp_f0;

    temp_f0 = flvecCalcDistance(em->pos, em->tgt_pos);
    if (em->char0 == 0x415) {
        return 1;
    }
    if (temp_f0 <= (2.4199998f + (22.0f * em->adj_z))) {
        em_char_set(em, 0x2D, 0xA, 0);
        return 1;
    }
    return 0;
}

void em17_atk_end_sel(EMW *em, EM17W *w) {
    if (em->x734 == 3) {
        em17_to_normal(em, 0, 0);
        return;
    }
    if (((s32) em->x39A % 10) == 0) {
        em17_act_set(em, 0, 1, 1);
        return;
    }
    em17_to_normal(em, 0, 0);
}

void dummy_em_prog_005E65C0(void) {

}


