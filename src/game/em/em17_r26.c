/* em17_r26 - monster 17 AI 0x005E1F80-0x005E638C: sound_call_sub_005E1F80, sound_call_005E1FF0, sound_call_parts_005E2090, quake_call_005E2130, move_default_005E2180, ef_move_sub_005E21D0 (file-static helpers, aliased in config/game_aliases.txt). Whole file in em17_nm.c. */
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
#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

static void em17_uvmove(EMW *em) {
    int i;

    for (i = 0; i < 4; i++) {
        if (em->uvtm[i] != 0xFFFF) {
            em->uvtm[i]++;
        }
        switch (em->uvty[i]) {
        case 0:
            UVR(i);
            break;
        case 1:
            if (em->uvtm[i] >= 0x3E) {
                UVR(i);
            } else {
                int k = ((u32)em->uvtm[i] >> 1) + 1;

                em->uv[i][0] = 0.125f * (f32)(k % 8);
                em->uv[i][1] = 0.25f * (f32)(k / 8 % 4);
            }
            break;
        case 2:
            em->uv[i][0] = 0.125f;
            em->uv[i][1] = 0.0f;
            em->uvtm[i] = 0xFFFF;
            em->uvty[i] = 0xFF;
            break;
        case 3:
            if (em->uvtm[i] >= 0xC) {
                UVR(i);
            } else {
                int k = ((u32)em->uvtm[i] >> 1) + 2;

                em->uv[i][0] = 0.125f * (f32)(k % 4);
                em->uv[i][1] = 0.25f * (f32)(k / 4 % 4);
            }
            break;
        case 0xFF:
            break;
        }
    }
}

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





extern u8 *em17_act_add[3];
extern u16 *em17_rail_add[2];
extern u16 *em17_rail_half_add[1];






extern u8 *em17_act_add[3];

















































































































static void sound_call_sub_005E1F80(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

static void sound_call_005E1FF0(EMW *em, int frame, int se, int joint) {
    int add;

    add = 0;
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        if (em->ex[0x1B] == 1) {
            if (joint == 0x14 || joint == 0x1A) {
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
        break;
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
    case 0x3F8:
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
        sound_call_005E1FF0(em, 0x1E, 0x13, 0);
        sound_call_005E1FF0(em, 2, 0x20, 0x23);
        sound_call_005E1FF0(em, 4, 0xE, 0xC);
        sound_call_005E1FF0(em, 8, 0xE, 6);
        break;
    case 0x406:
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

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_005E21D0(em, w);
        break;
    }
    em17_uvmove(em);
}
