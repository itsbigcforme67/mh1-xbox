/* em14_r12 - monster 14 AI 0x005BCB30-0x005C10C0: sound_call_sub_005BCB30, sound_call_005BCBA0, sound_call_parts_005BCC00, quake_call_005BCCA0, move_default_005BCCF0, ef_move_sub_005BCD40. Whole file in em14_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em14.h"

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
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
void em_cmd_ck(EMW *);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, int, f32);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
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
int Event_flag_ck();
void em_dur_set(EMW *, int);
void Eft20_set(f32, EMW *, int, int);
void Eft15_set3(EMW *, int, f32, int);
FLMAT *get_joint_wmat_em(EMW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
int em_frame_check3(EMW *, int, f32, f32);
void em14_act_set(EMW *em, int kind, u16 no, u16 arg);
int Em_stg_ck(EMW *);
s16 em_hp_vital_set2(EMW *, s16, s16);
void em_no_battle_area_ck(EMW *, int, int);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void Eft13_set_pos2(f32, EMW *, f32 *, int);
void get_joint_pos_em(EMW *, int, f32 *);
void em17_act_set(EMW *em, int kind, u16 no, u16 arg);
void em_range_set(EMW *em, s8 no);
void xang_calc_target(EMW *em, int *ang, f32 a, f32 b);
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
void Eft13_set_pos(f32, f32 *, int);
void AddVector(f32 *, f32 *, f32 *);
void Eft17_set_ex(f32 *, int, int, f32);
void vib_set(int, int);
void Quest_enemy_hagi_set();
extern s16 em14_stay_timer_tbl[];
extern s16 em14_runaway_timer_tbl[];

void em14_local_init(EMW *em);
void em14_init(EMW *em);
void act_dist_select_005B5650(EMW *em);
void em14_to_normal(EMW *em, s16 a, s16 b);
void em14_to_swim(EMW *em);
void em14_to_fly(EMW *em, int flag);
void em14_frame_reset(EMW *em, int i);
void em_act00_005B58E0(EMW *em, EM14W *w);
void em_act01_005B59C0(EMW *em, EM14W *w);
void em_act02_005B5AB0(EMW *em, EM14W *w);
void em_act03_005B5B80(EMW *em, EM14W *w);
void em_act04_005B5C10(EMW *em, EM14W *w);
void em_act05_005B5CB0(EMW *em, EM14W *w);
void em_act06_005B5DA0(EMW *em, EM14W *w);
void em_act07_005B5E70(EMW *em, EM14W *w);
void em_act08_005B5F10(EMW *em, EM14W *w);
void em_act09_005B5F20(EMW *em, EM14W *w);
void em_act10_005B5FC0(EMW *em, EM14W *w);
void em_act11_005B60D0(EMW *em, EM14W *w);
void em_act12_005B61A0(EMW *em, EM14W *w);
void em_act13_005B61B0(EMW *em, EM14W *w);
void em_act14_005B6280(EMW *em, EM14W *w);
void em_act15_005B6290(EMW *em, EM14W *w);
void em_act16_005B63C0(EMW *em, EM14W *w);
void em_act17_005B64F0(EMW *em, EM14W *w);
void em_act18_005B6570(EMW *em, EM14W *w);
void em_act19_005B66D0(EMW *em, EM14W *w);
void em_act20_005B66E0(EMW *em, EM14W *w);
void em_act21_005B67F0(EMW *em, EM14W *w);
void em_act22_005B6950(EMW *em, EM14W *w);
void em_act23_005B69D0(EMW *em, EM14W *w);
void em_act24_005B6AA0(EMW *em, EM14W *w);
void em_act25_005B6B70(EMW *em, EM14W *w);
void em_act26_005B6BF0(EMW *em, EM14W *w);
void em_act27_005B6C00(EMW *em, EM14W *w);
void em_act28_005B6D10(EMW *em, EM14W *w);
void em_act29_005B6DE0(EMW *em, EM14W *w);
void em_act33_005B6FD0(EMW *em, EM14W *w);
void em_mv00_005B7060(EMW *em, EM14W *w);
void em_mv01_005B71C0(EMW *em, EM14W *w);
void em_mv02_005B71D0(EMW *em, EM14W *w);
void em_mv03_005B72E0(EMW *em, EM14W *w);
void em_mv04_005B75B0(EMW *em, EM14W *w);
void em_mv05_005B76F0(EMW *em, EM14W *w);
void em_mv06_005B79C0(EMW *em, EM14W *w);
void em_mv07_005B7B20(EMW *em, EM14W *w);
void em_fly00_005B7C60(EMW *em, EM14W *w);
void em_fly01_005B7D30(EMW *em, EM14W *w);
void em_fly02_005B7DC0(EMW *em, EM14W *w);
void em_fly03_005B7EE0(EMW *em, EM14W *w);
void em_fly04_005B8090(EMW *em, EM14W *w);
void em_fly05_005B8260(EMW *em, EM14W *w);
void em_fly06_005B83C0(EMW *em, EM14W *w);
void em_fly07_005B84C0(EMW *em, EM14W *w);
void em_fly08_005B85D0(EMW *em, EM14W *w);
void em_fly09_005B86E0(EMW *em, EM14W *w);
void em_fly10_005B8800(EMW *em, EM14W *w);
void em_fly11_005B8890(EMW *em, EM14W *w);
void em_fly12_005B89D0(EMW *em, EM14W *w);
void em_fly14_005B8C10(EMW *em, EM14W *w);
void em_fly23_005B8CD0(EMW *em, EM14W *w);
void em_atk00_005B8F00(EMW *em, EM14W *w);
void em_atk03_005B8FA0(EMW *em, EM14W *w);
void em_atk06_005B9020(EMW *em, EM14W *w);
void em_atk15_005B9110(EMW *em, EM14W *w);
void em_atk16_005B9250(EMW *em, EM14W *w);
void em_atk17_005B9390(EMW *em, EM14W *w);
void em_atk26_005B94D0(EMW *em, EM14W *w);
void em_atk27_005B9570(EMW *em, EM14W *w);
void em_atk28_005B9610(EMW *em, EM14W *w);
void em_atk29_005B9690(EMW *em, EM14W *w);
void em_atk30_005B9730(EMW *em, EM14W *w);
void em_atk31_005B97B0(EMW *em, EM14W *w);
void em_dmg00_005B9830(EMW *em, EM14W *w);
void em_dmg01_005B98C0(EMW *em, EM14W *w);
void em_dmg02_005B9950(EMW *em, EM14W *w);
void em_dmg03_005B99E0(EMW *em, EM14W *w);
void em_dmg04_005B9A70(EMW *em, EM14W *w);
void em_dmg05_005B9C40(EMW *em, EM14W *w);
void em_dmg06_005B9D60(EMW *em, EM14W *w);
void em_dmg07_005B9ED0(EMW *em, EM14W *w);
static void em_dmg08_005BA140(EMW *em, EM14W *w);
static void em_dmg09_005BA2A0(EMW *em, EM14W *w);
static void em_dmg10_005BA400(EMW *em, EM14W *w);
static void em_dmg11_005BA490(EMW *em, EM14W *w);
static void em_dmg13_005BA560(EMW *em, EM14W *w);
static void em_dmg14_005BA660(EMW *em, EM14W *w);
static void em_dmg15_005BA800(EMW *em, EM14W *w);
static void em_dmg16_005BA880(EMW *em, EM14W *w);
static void em_dmg17_005BA9A0(EMW *em, EM14W *w);
static void em_dmg18_005BAA90(EMW *em, EM14W *w);
static void em_demo00_005BAB80(EMW *em, EM14W *w);
static void em_die00_005BAEF0(EMW *em, EM14W *w);
static void em_die01_005BB090(EMW *em, EM14W *w);
static void em_die02_005BB290(EMW *em, EM14W *w);
void em_move00_005BB460(EMW *em, EM14W *w);
void em_move01_005BB690(EMW *em, EM14W *w);
void em_move02_005BB760(EMW *em, EM14W *w);
void em_move03_005BB900(EMW *em, EM14W *w);
void em_move04_005BBA60(EMW *em, EM14W *w);
void em_move05_005BBBC0(EMW *em, EM14W *w);
void em_move06_005BBC30(EMW *em, EM14W *w);
void em14_main(EMW *em);
void em14_main_sub(EMW *em, EM14W *w);
void em14_uvmove(EMW *em);
void sound_call_sub_005BCB30(EMW *em, int se, int joint);
void sound_call_005BCBA0(EMW *em, int frame, int se, int joint);
void sound_call_parts_005BCC00(EMW *em, int frame, int se, int joint, u8 layer);
void quake_call_005BCCA0(EMW *em, int frame, int arg);
void Em_set_quake_sub(EMW *, int);
void move_default_005BCCF0(EMW *em);
void ef_move_sub_005BCD40(EMW *em, EM14W *w);
void em14_effect_move(EMW *em);
void ground_land_eff_set_005C1120(EMW *em);
void em14_atk_end_sel(EMW *em, EM14W *w);
void dummy_em_prog_005C1250(void);










extern u8 *em14_act_add[3];














































































































void sound_call_sub_005BCB30(EMW *em, int se, int joint) {
    f32 pos[3];

    flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
    Em_se_req2(em, se, 0, pos, 3, 0);
}

void sound_call_005BCBA0(EMW *em, int frame, int se, int joint) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        sound_call_sub_005BCB30(em, se, joint);
    }
}

void sound_call_parts_005BCC00(EMW *em, int frame, int se, int joint, u8 layer) {
    f32 pos[3];

    if (em_frame_check(em, layer, (f32)frame) != 0) {
        flmatGetTrans(pos, em->mdl->bone + joint * 0x190);
        Em_se_req2(em, se, 0, pos, 3, 0);
    }
}

void quake_call_005BCCA0(EMW *em, int frame, int arg) {
    if (em_frame_check(em, 0, (f32)frame) != 0) {
        Em_set_quake_sub(em, arg);
    }
}

void move_default_005BCCF0(EMW *em) {
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

void ef_move_sub_005BCD40(EMW *em, EM14W *w) {
    f32 sp50[3];
    FLMAT m50;
    f32 v3[3];
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
        break;
    case 0x3EB:
        if (em->x388 != 4) {
            sound_call_005BCBA0(em, 0x34, 1, 0x14);
            sound_call_005BCBA0(em, 0x74, 1, 0x1A);
            quake_call_005BCCA0(em, 0x34, 1);
            quake_call_005BCCA0(em, 0x74, 1);
            if (em_frame_check(em, 0, 2.0f) != 0) {
                shell18_set(em, 2);
            }
            if (em_frame_check(em, 0, 50.0f) != 0) {
                shell18_set(em, 3);
            }
            if (em_frame_check(em, 0, 120.0f) != 0) {
                shell18_set(em, 4);
            }
        }
        break;
    case 0x3ED:
        if (em->x388 != 4) {
            sound_call_005BCBA0(em, 0x19, 1, 0x14);
            sound_call_005BCBA0(em, 0x38, 1, 0x1A);
            quake_call_005BCCA0(em, 0x3C, 1);
            if (em_frame_check(em, 0, 2.0f) != 0) {
                shell18_set(em, 5);
            }
            if (em_frame_check(em, 0, 30.0f) != 0) {
                shell18_set(em, 6);
            }
            if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(*(u16 *)0x3F340E & 3)) {
                Eft20_set(1.0f, em, 0x1A, 1);
                return;
            }
        }
        break;
    case 0x3EE:
        if (em->x388 != 4) {
            sound_call_005BCBA0(em, 0x19, 1, 0x1A);
            sound_call_005BCBA0(em, 0x38, 1, 0x14);
            quake_call_005BCCA0(em, 0x3C, 1);
            if (em_frame_check(em, 0, 30.0f) != 0) {
                shell18_set(em, 7);
            }
            if (em_frame_check(em, 0, 2.0f) != 0) {
                shell18_set(em, 8);
            }
            if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 2.0f, 78.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, 0);
                return;
            }
        }
        break;
    case 0x3EF:
        sound_call_005BCBA0(em, 6, 0x20, 0x23);
        sound_call_005BCBA0(em, 0xE, 1, 0x1A);
        sound_call_005BCBA0(em, 0x26, 1, 0x14);
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell18_set(em, 9);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0xA);
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
        sound_call_005BCBA0(em, 6, 0x20, 0x23);
        sound_call_005BCBA0(em, 0xE, 1, 0x14);
        sound_call_005BCBA0(em, 0x26, 1, 0x1A);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0xB);
        }
        if (em_frame_check(em, 0, 18.0f) != 0) {
            shell18_set(em, 0xC);
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
        sound_call_005BCBA0(em, 6, 0x2B, 0x23);
        sound_call_005BCBA0(em, 6, 4, 0x1A);
        sound_call_005BCBA0(em, 0x4C, 0, 0x14);
        sound_call_005BCBA0(em, 0x1C, 0x12, 0x14);
        if (em_frame_check(em, 0, 42.0f) != 0) {
            shell18_set(em, 0x19);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0x1A);
        }
        if (em_frame_check(em, 0, 74.0f) != 0) {
            shell18_set(em, 0x1A);
            return;
        }
        break;
    case 0x3F3:
        sound_call_005BCBA0(em, 0xE, 0xB, 6);
        sound_call_005BCBA0(em, 0x12, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x46, 0xB, 6);
        sound_call_005BCBA0(em, 0x42, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x7A, 0xB, 6);
        sound_call_005BCBA0(em, 0x7E, 0xB, 0xC);
        if ((em_frame_check(em, 0, 52.0f) == 0) && (em_frame_check(em, 0, 104.0f) == 0)) {
            if (em_frame_check(em, 0, 162.0f) != 0) {
                goto block_132;
            }
        } else {
block_132:
            Eft13_set_em_scl(em, 2, 5.0f, 7);
            return;
        }
        break;
    case 0x3F4:
        sound_call_005BCBA0(em, 6, 0xC, 6);
        sound_call_005BCBA0(em, 4, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x40, 0xC, 6);
        sound_call_005BCBA0(em, 0x3C, 0xC, 0xC);
        break;
    case 0x3F5:
        sound_call_005BCBA0(em, 0xE, 0xF, 6);
        sound_call_005BCBA0(em, 0x12, 0xF, 0xC);
        sound_call_005BCBA0(em, 0x2C, 0xF, 6);
        sound_call_005BCBA0(em, 0x28, 0xF, 0xC);
        sound_call_005BCBA0(em, 0x42, 0xF, 6);
        sound_call_005BCBA0(em, 0x46, 0xF, 0xC);
        if ((em_frame_check(em, 0, 26.0f) != 0) || (em_frame_check(em, 0, 52.0f) != 0)) {
            Eft13_set_em_scl(em, 1, 5.0f, 7);
            return;
        }
        break;
    case 0x3F7:
        sound_call_005BCBA0(em, 4, 0xC, 6);
        sound_call_005BCBA0(em, 8, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x3A, 0xB, 6);
        sound_call_005BCBA0(em, 0x3E, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x8E, 0xB, 6);
        sound_call_005BCBA0(em, 0x8A, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xD4, 0xB, 6);
        sound_call_005BCBA0(em, 0xD8, 0xB, 0xC);
        if ((em_frame_check(em, 0, 8.0f) == 0) && (em_frame_check(em, 0, 86.0f) == 0)) {
            if (em_frame_check(em, 0, 160.0f) != 0) {
                goto block_141;
            }
        } else {
block_141:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 70.0f, 84.0f) == 0) && (em_frame_check3(em, 0, 146.0f, 156.0f) == 0)) {
                if (em_frame_check3(em, 0, 220.0f, 234.0f) != 0) {
                    goto block_148;
                }
            } else {
block_148:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3F9:
        if (em->x388 == 4) {
            if ((game_w.stage == em->stg) && !(GAME_X1E16 & 3)) {
                SetVector(sp50, em->pos[0], em->x7E4, em->pos[2]);
                Eft13_set_pos(20.0f, sp50, 0x14);
                return;
            }
        } else {
            sound_call_005BCBA0(em, 4, 0x27, 0x23);
            sound_call_005BCBA0(em, 4, 0x16, 0);
            sound_call_005BCBA0(em, 4, 0xB, 6);
            sound_call_005BCBA0(em, 8, 0xB, 0xC);
            sound_call_005BCBA0(em, 0x1A, 1, 0x14);
            sound_call_005BCBA0(em, 0x62, 0x1E, 0x14);
            sound_call_005BCBA0(em, 0x78, 6, 0x14);
            sound_call_005BCBA0(em, 0x80, 0x38, 0x14);
            sound_call_005BCBA0(em, 0x90, 6, 0x1A);
            sound_call_005BCBA0(em, 0x96, 0x38, 0x14);
            if (em_frame_check(em, 0, 106.0f) != 0) {
                shell18_set(em, 0xE);
            }
            if ((em_frame_check(em, 0, 100.0f) != 0) || (em_frame_check(em, 0, 154.0f) != 0)) {
                Eft20_set(1.0f, em, 2, 0);
            }
            if ((em_frame_check(em, 0, 88.0f) != 0) || (em_frame_check(em, 0, 132.0f) != 0)) {
                Eft20_set(1.0f, em, 3, 0);
            }
            if (em->x8B6 != 0) {
                if ((em_frame_check(em, 0, 106.0f) != 0) || (em_frame_check(em, 0, 132.0f) != 0) || (em_frame_check(em, 0, 158.0f) != 0)) {
                    Eft20_set(1.0f, em, 0x19, (s16)(((u16)ran_suu(1) & 1) + 2));
                }
                if ((em_frame_check3(em, 0, 4.0f, 50.0f) != 0) && !(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x3FA:
        sound_call_005BCBA0(em, 4, 0x23, 0x23);
        sound_call_005BCBA0(em, 0xC, 4, 0x14);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x38, 0xF, 6);
        sound_call_005BCBA0(em, 0x3C, 0xF, 0xC);
        sound_call_005BCBA0(em, 0x8A, 0xB, 6);
        sound_call_005BCBA0(em, 0x8E, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xBE, 0xB, 6);
        sound_call_005BCBA0(em, 0xC2, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xEE, 0xB, 6);
        sound_call_005BCBA0(em, 0xF2, 0xB, 0xC);
        if ((em_frame_check(em, 0, 74.0f) == 0) && (em_frame_check(em, 0, 140.0f) == 0) && (em_frame_check(em, 0, 194.0f) == 0)) {
            if (em_frame_check(em, 0, 240.0f) != 0) {
                goto block_176;
            }
        } else {
block_176:
            if (em->pos[1] <= (1000.0f + em->x5AC)) {
                Eft20_set(1.0f, em, 1, 0);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 10.0f, 64.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            }
            if ((em_frame_check3(em, 0, 136.0f, 140.0f) == 0) && (em_frame_check3(em, 0, 188.0f, 192.0f) == 0)) {
                if (em_frame_check3(em, 0, 136.0f, 234.0f) != 0) {
                    goto block_186;
                }
            } else {
block_186:
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x3FB:
        sound_call_005BCBA0(em, 2, 0xB, 6);
        sound_call_005BCBA0(em, 6, 0xB, 0xC);
        sound_call_005BCBA0(em, 0xC, 4, 0x1A);
        sound_call_005BCBA0(em, 4, 1, 0x14);
        sound_call_005BCBA0(em, 0xE, 9, 0);
        sound_call_005BCBA0(em, 0x3A, 0x16, 0);
        sound_call_005BCBA0(em, 0x74, 0, 0x14);
        sound_call_005BCBA0(em, 0x96, 1, 0x14);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0xD);
        }
        if (em_frame_check(em, 0, 16.0f) != 0) {
            ground_land_eff_set_005C1120(em);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check(em, 0, 14.0f) != 0) {
                Eft20_set(1.0f, em, 0x19, 2);
                Eft20_set(1.0f, em, 0x19, 3);
            }
            if ((em_frame_check3(em, 0, 18.0f, 44.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x405:
        sound_call_005BCBA0(em, 0x38, 0x1F, 0x23);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 4, 0xF, 0xC);
        sound_call_005BCBA0(em, 8, 0xF, 6);
        sound_call_005BCBA0(em, 0x78, 0xE, 0xC);
        sound_call_005BCBA0(em, 0x7C, 0xE, 6);
        break;
    case 0x407:
        sound_call_005BCBA0(em, 2, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0xB6, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0x16C, 0x2D, 0x23);
        v[1] = 10.0f;
        v[2] = 140.0f;
        v[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v, 1.60000002f);
        break;
    case 0x408:
        sound_call_005BCBA0(em, 4, 0x2F, 0x23);
        sound_call_005BCBA0(em, 0x6C, 0x1F, 0x23);
        sound_call_005BCBA0(em, 0x46, 0, 0x1A);
        sound_call_005BCBA0(em, 0x8E, 3, 0x14);
        sound_call_005BCBA0(em, 4, 0x17, 0);
        sound_call_005BCBA0(em, 4, 0x13, 0);
        break;
    case 0x409:
        sound_call_005BCBA0(em, 4, 0x2F, 0x23);
        sound_call_005BCBA0(em, 0x46, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0x7E, 3, 0x1A);
        sound_call_005BCBA0(em, 0xB6, 3, 0x14);
        sound_call_005BCBA0(em, 0x38, 0x16, 0);
        break;
    case 0x40A:
        sound_call_005BCBA0(em, 0x24, 0x2F, 0x23);
        v2[1] = 10.0f;
        v2[2] = 140.0f;
        v2[0] = 0.0f;
        em_sleep_eff_set(em, 0x22, v2, 1.60000002f);
        break;
    case 0x40C:
        sound_call_005BCBA0(em, 0x24, 0x24, 0x23);
        sound_call_005BCBA0(em, 0x14, 0, 0x1A);
        sound_call_005BCBA0(em, 0x28, 0, 0x14);
        sound_call_005BCBA0(em, 0x3C, 0x15, 0x22);
        sound_call_005BCBA0(em, 0x3C, 0xC, 0xC);
        sound_call_005BCBA0(em, 0xC, 0xC, 6);
        sound_call_005BCBA0(em, 0xA6, 0, 0x1A);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell18_set(em, 1);
        }
        if (em_frame_check(em, 0, 46.0f) != 0) {
            shell18_set(em, 0x1F);
            return;
        }
        break;
    case 0x40D:
        sound_call_005BCBA0(em, 0x1E, 0x20, 0x23);
        sound_call_005BCBA0(em, 0xC, 0xC, 0x1A);
        sound_call_005BCBA0(em, 0x1C, 2, 0x1A);
        sound_call_005BCBA0(em, 0x2E, 3, 0x14);
        sound_call_005BCBA0(em, 0x48, 1, 0x1A);
        break;
    case 0x40E:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x57, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x75, 0x23);
        }
        sound_call_005BCBA0(em, 0x48, 0x16, 0);
        sound_call_005BCBA0(em, 0xA4, 9, 0);
        sound_call_005BCBA0(em, 0x90, 3, 0xC);
        sound_call_005BCBA0(em, 0x88, 0xE, 6);
        sound_call_005BCBA0(em, 0xB2, 4, 0x22);
        if (em_frame_check(em, 0, 148.0f) != 0) {
            shell18_set(em, 0xF);
        }
        if (em_frame_check(em, 0, 156.0f) != 0) {
            Eft20_set(1.0f, em, 0, 0);
            return;
        }
        break;
    case 0x40F:
        sound_call_005BCBA0(em, 2, 0x2F, 0x23);
        sound_call_005BCBA0(em, 2, 0x17, 0);
        sound_call_005BCBA0(em, 0x40, 3, 0x1A);
        sound_call_005BCBA0(em, 0x66, 0x10, 0x1A);
        break;
    case 0x413:
        sound_call_005BCBA0(em, 8, 0x23, 0x23);
        sound_call_005BCBA0(em, 0xE, 0xF, 6);
        sound_call_005BCBA0(em, 0x1E, 0x14, 0x2A);
        sound_call_005BCBA0(em, 0x2A, 0, 0x1A);
        sound_call_005BCBA0(em, 0x48, 1, 0x14);
        quake_call_005BCCA0(em, 0x2C, 1);
        quake_call_005BCCA0(em, 0x49, 1);
        if (em_frame_check(em, 0, 30.0f) != 0) {
            shell18_set(em, 0x11);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 6.0f, 38.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, 1);
            return;
        }
        break;
    case 0x41B:
        sound_call_005BCBA0(em, 4, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x26, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0x38, 0x12, 0x1A);
        sound_call_005BCBA0(em, 0x58, 6, 0x1A);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x96, 0x56, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x96, 0x74, 0x23);
        }
        sound_call_005BCBA0(em, 0xEE, 0x16, 0);
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft20_set(0.800000012f, em, 2, 2);
        }
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0x1B);
        }
        if (em_frame_check(em, 0, 168.0f) != 0) {
            shell18_set(em, 0x1B);
        }
        if (em_frame_check(em, 0, 56.0f) != 0) {
            shell18_set(em, 0x1C);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 156.0f, 248.0f) == 0) {
                if (em_frame_check3(em, 0, 276.0f, 346.0f) != 0) {
                    goto block_246;
                }
            } else {
block_246:
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x41E:
        sound_call_005BCBA0(em, 2, 0x12, 0);
        sound_call_005BCBA0(em, 0x1C, 4, 0x1A);
        sound_call_005BCBA0(em, 0x38, 4, 0x14);
        sound_call_005BCBA0(em, 0x1C, 5, 0x1A);
        sound_call_005BCBA0(em, 0x78, 4, 0x14);
        sound_call_005BCBA0(em, 0x108, 4, 0x1A);
        sound_call_005BCBA0(em, 4, 0x14, 0x23);
        sound_call_005BCBA0(em, 6, 0x23, 0x23);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x4E, 0x56, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x4E, 0x74, 0x23);
        }
        sound_call_005BCBA0(em, 0xA, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x10, 0xC, 6);
        if (em_frame_check(em, 0, 102.0f) != 0) {
            shell18_set(em, 0x12);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 16.0f, 26.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x424:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 6, 0x53, 0x23);
        } else {
            sound_call_005BCBA0(em, 6, 0x71, 0x23);
        }
        sound_call_005BCBA0(em, 0x48, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x30, 6, 0x1A);
        sound_call_005BCBA0(em, 0x30, 0x11, 0x14);
        sound_call_005BCBA0(em, 0x30, 3, 0x14);
        sound_call_005BCBA0(em, 0x74, 3, 0x14);
        sound_call_005BCBA0(em, 0x94, 3, 0x1A);
        sound_call_005BCBA0(em, 0x34, 0xC, 6);
        sound_call_005BCBA0(em, 0x38, 0xD, 0xC);
        sound_call_005BCBA0(em, 0xC2, 3, 0x1A);
        if (em_frame_check(em, 0, 44.0f) != 0) {
            Eft13_set_em_scl(em, 0x1B, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 62.0f) != 0) {
            Eft13_set_em_scl(em, 0x16, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 80.0f) != 0) {
            Eft20_set(0.600000024f, em, 1, 0x80);
            return;
        }
        break;
    case 0x425:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 6, 0x53, 0x23);
        } else {
            sound_call_005BCBA0(em, 6, 0x71, 0x23);
        }
        sound_call_005BCBA0(em, 0xA, 0x12, 0x1A);
        sound_call_005BCBA0(em, 0x26, 6, 0x1A);
        sound_call_005BCBA0(em, 0x4E, 4, 0x14);
        sound_call_005BCBA0(em, 0x6E, 3, 0x14);
        sound_call_005BCBA0(em, 0x5A, 0x15, 0);
        sound_call_005BCBA0(em, 0xAA, 3, 0x1A);
        if (em_frame_check(em, 0, 24.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
            return;
        }
        break;
    case 0x426:
        sound_call_005BCBA0(em, 4, 0x27, 0x23);
        sound_call_005BCBA0(em, 0x56, 0x1F, 0x23);
        sound_call_005BCBA0(em, 0x10, 3, 0x14);
        sound_call_005BCBA0(em, 4, 0xD, 6);
        sound_call_005BCBA0(em, 8, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x62, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x7E, 3, 0x1A);
        break;
    case 0x427:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0);
        sound_call_005BCBA0(em, 0x4C, 0x1F, 0x23);
        break;
    case 0x428:
        sound_call_005BCBA0(em, 4, 0x12, 0x1A);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0);
        sound_call_005BCBA0(em, 0x44, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x8E, 3, 0x14);
        break;
    case 0x429:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x54, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x72, 0x23);
        }
        sound_call_005BCBA0(em, 0xA8, 0x2A, 0x23);
        sound_call_005BCBA0(em, 4, 0x27, 0x2C);
        sound_call_005BCBA0(em, 0xFC, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x130, 0x27, 0x23);
        sound_call_005BCBA0(em, 0x3E, 4, 0x2C);
        sound_call_005BCBA0(em, 0x12, 0x12, 0x14);
        sound_call_005BCBA0(em, 0x2C, 6, 0x1A);
        sound_call_005BCBA0(em, 0x3A, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0xD2, 3, 0x1A);
        sound_call_005BCBA0(em, 0x142, 3, 0x1A);
        sound_call_005BCBA0(em, 0x3E, 9, 0);
        sound_call_005BCBA0(em, 0x4A, 9, 0);
        sound_call_005BCBA0(em, 0x64, 0x12, 0);
        sound_call_005BCBA0(em, 0xA2, 0x12, 0);
        sound_call_005BCBA0(em, 0xE, 0xC, 6);
        sound_call_005BCBA0(em, 0x12, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x11E, 0xD, 6);
        sound_call_005BCBA0(em, 0x122, 0xD, 0xC);
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
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0);
        break;
    case 0x42B:
        sound_call_005BCBA0(em, 0x46, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0x46, 0x17, 0);
        break;
    case 0x42C:
        sound_call_005BCBA0(em, 0x1C, 2, 0x1A);
        sound_call_005BCBA0(em, 0x30, 4, 0x14);
        sound_call_005BCBA0(em, 0xDC, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0x1E, 0x16, 0);
        sound_call_005BCBA0(em, 0x108, 9, 0);
        sound_call_005BCBA0(em, 0x108, 7, 0);
        sound_call_005BCBA0(em, 0x114, 0x17, 0);
        sound_call_005BCBA0(em, 0x180, 0x16, 0);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x18, 0x52, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x18, 0x70, 0x23);
        }
        sound_call_005BCBA0(em, 0x8C, 0x2C, 0x23);
        sound_call_005BCBA0(em, 0x192, 0x2B, 0x23);
        sound_call_005BCBA0(em, 0x174, 0xF, 6);
        sound_call_005BCBA0(em, 0x176, 0xE, 0xC);
        if (em_frame_check(em, 0, 268.0f) != 0) {
            Eft20_set(0.899999976f, em, 0, 0);
            return;
        }
        break;
    case 0x42D:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x18, 9, 0);
        sound_call_005BCBA0(em, 0x18, 0x11, 0);
        quake_call_005BCCA0(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x80);
            return;
        }
        break;
    case 0x42E:
    case 0x433:
        sound_call_005BCBA0(em, 8, 0x16, 0);
        sound_call_005BCBA0(em, 0x32, 0x1F, 0x23);
        break;
    case 0x42F:
        sound_call_005BCBA0(em, 4, 0x16, 0x23);
        sound_call_005BCBA0(em, 0x1E, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x30, 1, 0x14);
        sound_call_005BCBA0(em, 0x46, 1, 0x1A);
        break;
    case 0x430:
        sound_call_005BCBA0(em, 4, 0x16, 0x23);
        sound_call_005BCBA0(em, 0x1E, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x30, 1, 0x1A);
        sound_call_005BCBA0(em, 0x46, 1, 0x14);
        break;
    case 0x431:
        sound_call_005BCBA0(em, 4, 0x1E, 0x22);
        sound_call_005BCBA0(em, 4, 0x1D, 6);
        sound_call_005BCBA0(em, 8, 0x1D, 0xC);
        sound_call_005BCBA0(em, 0xA, 0x2A, 0x23);
        sound_call_005BCBA0(em, 0x4C, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x18, 0x12, 0);
        sound_call_005BCBA0(em, 0x1E, 0x10, 0);
        sound_call_005BCBA0(em, 0x1E, 9, 0);
        sound_call_005BCBA0(em, 0x22, 7, 0);
        quake_call_005BCCA0(em, 0x1A, 2);
        if ((em_frame_check2(em, 0, 28.0f) != 0) && (em_frame_check2(em, 0, 52.0f) == 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0xA, 0x80);
            return;
        }
        break;
    case 0x432:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x52, 0);
        } else {
            sound_call_005BCBA0(em, 4, 0x70, 0);
        }
        sound_call_005BCBA0(em, 4, 0x13, 0x2A);
        sound_call_005BCBA0(em, 0x18, 9, 0);
        sound_call_005BCBA0(em, 0x18, 0x11, 0);
        quake_call_005BCCA0(em, 0x1A, 2);
        if (em_frame_check(em, 0, 26.0f) != 0) {
            Eft20_set(1.0f, em, 0x12, 0x81);
            return;
        }
        break;
    case 0x434:
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x10, 0x5A, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x10, 0x78, 0x23);
        }
        sound_call_005BCBA0(em, 0x6E, 0x37, 0x23);
        em_mahi_eff_set(em, 2);
        break;
    case 0x435:
        sound_call_005BCBA0(em, 4, 0x2B, 0x23);
        sound_call_005BCBA0(em, 0x14, 0xB, 0xC);
        sound_call_005BCBA0(em, 0x3A, 0x14, 0);
        break;
    case 0x436:
        sound_call_005BCBA0(em, 0xA, 9, 0);
        sound_call_005BCBA0(em, 0xA, 7, 0x2A);
        sound_call_005BCBA0(em, 0xC, 0x11, 0);
        sound_call_005BCBA0(em, 0x10, 0x2B, 0x23);
        if (em_frame_check(em, 0, 8.0f) != 0) {
            Eft20_set(1.0f, em, 0xB, 0);
            return;
        }
        break;
    case 0x438:
        sound_call_005BCBA0(em, 2, 0x16, 0x22);
        sound_call_005BCBA0(em, 0x64, 0x17, 0x22);
        sound_call_005BCBA0(em, 0x26, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0xC8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x1C, 3, 0x1A);
        sound_call_005BCBA0(em, 0x140, 3, 0x1A);
        break;
    case 0x439:
        sound_call_005BCBA0(em, 2, 0x16, 0x22);
        sound_call_005BCBA0(em, 0x64, 0x17, 0x22);
        sound_call_005BCBA0(em, 0x26, 0x2D, 0x23);
        sound_call_005BCBA0(em, 0xC8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x1C, 3, 0x14);
        sound_call_005BCBA0(em, 0x140, 3, 0x14);
        break;
    case 0x43A:
        sound_call_005BCBA0(em, 4, 0x37, 0x23);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        break;
    case 0x43B:
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 4, 0x17, 0);
        sound_call_005BCBA0(em, 0xA, 0x10, 0x1A);
        sound_call_005BCBA0(em, 0xA, 0, 0x1A);
        sound_call_005BCBA0(em, 0x1C, 0, 0x14);
        sound_call_005BCBA0(em, 0x34, 0, 0x1A);
        sound_call_005BCBA0(em, 0x58, 0, 0x14);
        sound_call_005BCBA0(em, 0x80, 0, 0x1A);
        sound_call_005BCBA0(em, 0xC6, 0, 0x14);
        sound_call_005BCBA0(em, 4, 0x2B, 0x23);
        sound_call_005BCBA0(em, 0xA8, 0x27, 0x23);
        sound_call_005BCBA0(em, 0xA2, 0xD, 6);
        sound_call_005BCBA0(em, 0xA6, 0xD, 0xC);
        break;
    case 0x442:
        sound_call_005BCBA0(em, 0x20, 0xD, 6);
        sound_call_005BCBA0(em, 0x24, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x4A, 0xD, 6);
        sound_call_005BCBA0(em, 0x4E, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x7C, 0xB, 6);
        sound_call_005BCBA0(em, 0x80, 0xB, 0xC);
        break;
    case 0x448:
        sound_call_005BCBA0(em, 0x16, 0x12, 0);
        sound_call_005BCBA0(em, 0x48, 0x12, 0);
        sound_call_005BCBA0(em, 4, 0xC, 6);
        sound_call_005BCBA0(em, 8, 0xC, 0xC);
        sound_call_005BCBA0(em, 0x30, 0x13, 0);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x50, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x6E, 0x23);
        }
        sound_call_005BCBA0(em, 4, 0x16, 0);
        if ((em->x8B6 != 0) && ((em_frame_check3(em, 0, 12.0f, 24.0f) != 0) || (em_frame_check3(em, 0, 46.0f, 90.0f) != 0)) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x449:
        sound_call_005BCBA0(em, 0x22, 0x16, 6);
        sound_call_005BCBA0(em, 0x26, 0x16, 0xC);
        sound_call_005BCBA0(em, 0x62, 4, 0x22);
        sound_call_005BCBA0(em, 0x6C, 3, 0x22);
        sound_call_005BCBA0(em, 4, 0x2A, 0x23);
        sound_call_005BCBA0(em, 4, 0x2B, 0x23);
        break;
    case 0x44A:
        sound_call_005BCBA0(em, 4, 0x16, 6);
        sound_call_005BCBA0(em, 8, 0x16, 0xC);
        sound_call_005BCBA0(em, 0x6C, 3, 0x22);
        sound_call_005BCBA0(em, 4, 0x28, 0x23);
        sound_call_005BCBA0(em, 0x46, 0x20, 0x23);
        break;
    case 0x44C:
        sound_call_005BCBA0(em, 0x14, 6, 0x1A);
        sound_call_005BCBA0(em, 0x14, 0x10, 0x14);
        sound_call_005BCBA0(em, 0x1A, 0x11, 0x14);
        sound_call_005BCBA0(em, 0x8E, 0, 0x14);
        sound_call_005BCBA0(em, 0x9A, 0, 0x1A);
        sound_call_005BCBA0(em, 0x50, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x68, 0x16, 0);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            shell18_set(em, 0x16);
        }
        if (em_frame_check(em, 0, 18.0f) != 0) {
            Eft20_set(1.79999995f, em, 2, 6);
        }
        if ((em_frame_check3(em, 0, 40.0f, 80.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft13_set_em_scl(em, 2, 7.0f, 3);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check(em, 0, 22.0f) != 0) {
                Eft20_set(1.0f, em, 0x19, 2);
                Eft20_set(1.0f, em, 0x19, 3);
            }
            if ((em_frame_check3(em, 0, 72.0f, 116.0f) != 0) && !(GAME_X1E16 & 3)) {
                Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                return;
            }
        }
        break;
    case 0x44F:
        sound_call_005BCBA0(em, 0x10, 0x11, 0x1A);
        sound_call_005BCBA0(em, 0x4E, 0x10, 0x14);
        sound_call_005BCBA0(em, 0xE, 0x1D, 6);
        sound_call_005BCBA0(em, 0x2C, 0x1D, 0xC);
        sound_call_005BCBA0(em, 0x3E, 0x1D, 6);
        sound_call_005BCBA0(em, 0x68, 0x1E, 0);
        sound_call_005BCBA0(em, 0x82, 0x16, 0);
        quake_call_005BCCA0(em, 0x68, 4);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            shell18_set(em, 0x20);
        }
        if (em_frame_check(em, 0, 20.0f) != 0) {
            Eft20_set(1.20000005f, em, 2, 0);
        }
        if (em_frame_check(em, 0, 82.0f) != 0) {
            Eft20_set(1.20000005f, em, 3, 0);
        }
        if (em_frame_check(em, 0, 104.0f) != 0) {
            Eft20_set(1.0f, em, 1, 0);
        }
        if ((em_frame_check3(em, 0, 40.0f, 62.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft13_set_em_scl(em, 0xF, 5.0f, 3);
        }
        if ((em_frame_check(em, 0, 18.0f) != 0) || (em_frame_check(em, 0, 22.0f) != 0)) {
            Eft13_set_em_scl(em, 9, 5.0f, 3);
        }
        if ((em_frame_check(em, 0, 148.0f) != 0) || (em_frame_check(em, 0, 160.0f) != 0) || (em_frame_check(em, 0, 172.0f) != 0) || (em_frame_check(em, 0, 184.0f) != 0)) {
            Eft13_set_em_scl(em, 0x29, 6.0f, 3);
        }
        if (!(em->x948 & 1) && (em_frame_check(em, 0, 180.0f) != 0)) {
            Eft13_set_em_scl(em, 0x2A, 0.699999988f, 0x13);
            return;
        }
        break;
    case 0x450:
        sound_call_005BCBA0(em, 0xC, 0x1D, 0x22);
        sound_call_005BCBA0(em, 0xE, 0x1E, 0x22);
        sound_call_005BCBA0(em, 0x14, 0x2E, 0x23);
        sound_call_005BCBA0(em, 0xA8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x3C, 8, 0x14);
        sound_call_005BCBA0(em, 0x3C, 6, 0x14);
        sound_call_005BCBA0(em, 0x40, 2, 0x1A);
        sound_call_005BCBA0(em, 0xB2, 1, 0x14);
        sound_call_005BCBA0(em, 0xE8, 0, 0x1A);
        sound_call_005BCBA0(em, 0xAE, 0xC, 6);
        sound_call_005BCBA0(em, 0xB2, 0xC, 0xC);
        sound_call_005BCBA0(em, 0xC, 0x15, 0x22);
        quake_call_005BCCA0(em, 2, 5);
        if (em_frame_check(em, 0, 10.0f) != 0) {
            shell18_set(em, 0x15);
        }
        if (em_frame_check(em, 0, 40.0f) != 0) {
            shell18_set(em, 0x21);
        }
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft13_set_em_scl(em, 0x22, 1.39999998f, 0x13);
            if (Em_stg_ck(em) != 0) {
                get_joint_pos_em(em, 0x22, sp50);
                sp50[1] = em->x5AC;
                Eft17_set_ex(sp50, (u16)em->ang[1], 8, 1.0f);
            }
        }
        if (em->x8B6 != 0) {
            if ((em_frame_check3(em, 0, 12.0f, 36.0f) == 0) && (em_frame_check3(em, 0, 66.0f, 130.0f) == 0)) {
                if (em_frame_check3(em, 0, 168.0f, 204.0f) != 0) {
                    goto block_406;
                }
            } else {
block_406:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x451:
        sound_call_005BCBA0(em, 6, 7, 0x22);
        sound_call_005BCBA0(em, 6, 0x1D, 0x22);
        sound_call_005BCBA0(em, 0x44, 0xC, 6);
        sound_call_005BCBA0(em, 0x48, 0xC, 0xC);
        sound_call_005BCBA0(em, 4, 0x36, 0x23);
        sound_call_005BCBA0(em, 0xC, 0x15, 0x22);
        quake_call_005BCCA0(em, 0xC, 3);
        if (em_frame_check(em, 0, 12.0f) != 0) {
            Eft13_set_em_scl(em, 2, 1.39999998f, 0x13);
            if (Em_stg_ck(em) != 0) {
                get_joint_pos_em(em, 2, sp50);
                sp50[1] = em->x5AC;
                Eft17_set_ex(sp50, (u16)em->ang[1], 8, 1.0f);
                return;
            }
        }
        break;
    case 0x452:
        sound_call_005BCBA0(em, 8, 1, 0x1A);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x8A, 0x16, 0);
        sound_call_005BCBA0(em, 0x12, 0x15, 0x22);
        sound_call_005BCBA0(em, 0x2C, 0x30, 0x22);
        sound_call_005BCBA0(em, 0x94, 0, 0x14);
        sound_call_005BCBA0(em, 0xB4, 0, 0x1A);
        sound_call_005BCBA0(em, 0x30, 0x1F, 0x23);
        if (em_frame_check(em, 0, 48.0f) != 0) {
            shell18_set(em, 0x18);
        }
        if (em_frame_check(em, 0, 50.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 5.0f, 3);
        }
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 5.0f, 3);
        }
        if ((em->x8B6 != 0) && (em_frame_check3(em, 0, 86.0f, 152.0f) != 0) && !(GAME_X1E16 & 3)) {
            Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
            return;
        }
        break;
    case 0x453:
        sound_call_005BCBA0(em, 4, 0x34, 0x23);
        sound_call_005BCBA0(em, 0x32, 0x35, 0x23);
        sound_call_005BCBA0(em, 0x32, 0x15, 0x22);
        sound_call_005BCBA0(em, 0xA, 6, 0x1A);
        sound_call_005BCBA0(em, 0xA8, 0, 0x14);
        sound_call_005BCBA0(em, 0xD8, 0, 0x1A);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x74, 0xC, 6);
        sound_call_005BCBA0(em, 0x78, 0xC, 0xC);
        if (em_frame_check(em, 0, 50.0f) != 0) {
            shell18_set(em, 0x10);
        }
        if (em_frame_check(em, 0, 10.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 1.0f, 7);
        }
        if (em_frame_check(em, 0, 44.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 5.0f, 3);
        }
        if (em->x8B6 != 0) {
            if (em_frame_check3(em, 0, 8.0f, 24.0f) == 0) {
                if (em_frame_check3(em, 0, 136.0f, 172.0f) != 0) {
                    goto block_432;
                }
            } else {
block_432:
                if (!(GAME_X1E16 & 3)) {
                    Eft20_set(1.0f, em, 0x1A, (s16)((u16)ran_suu(1) & 1));
                    return;
                }
            }
        }
        break;
    case 0x454:
        sound_call_005BCBA0(em, 8, 0x20, 0x23);
        sound_call_005BCBA0(em, 0x10, 3, 0x14);
        sound_call_005BCBA0(em, 0x4A, 0, 0x1A);
        sound_call_005BCBA0(em, 0xC2, 0, 0x1A);
        sound_call_005BCBA0(em, 0x102, 0, 0x14);
        sound_call_005BCBA0(em, 0x3A, 0x14, 0x2A);
        sound_call_005BCBA0(em, 0x46, 0x15, 0x2C);
        sound_call_005BCBA0(em, 0x56, 8, 0x2C);
        sound_call_005BCBA0(em, 0x5A, 0x12, 0x2C);
        if (em_frame_check(em, 0, 70.0f) != 0) {
            shell18_set(em, 0x1D);
        }
        if (!(em->x948 & 1) && (em_frame_check(em, 0, 84.0f) != 0)) {
            Eft20_set(1.0f, em, 2, 7);
            return;
        }
        break;
    case 0x455:
        sound_call_005BCBA0(em, 4, 0x12, 0x1A);
        sound_call_005BCBA0(em, 4, 0x1F, 0x23);
        sound_call_005BCBA0(em, 0xA, 3, 0x1A);
        sound_call_005BCBA0(em, 0x70, 4, 0x14);
        sound_call_005BCBA0(em, 0xD8, 1, 0x1A);
        sound_call_005BCBA0(em, 0x108, 3, 0x14);
        sound_call_005BCBA0(em, 0x38, 0x1D, 0);
        sound_call_005BCBA0(em, 0x38, 0x14, 0);
        sound_call_005BCBA0(em, 0x54, 0x16, 0);
        if (em_frame_check(em, 0, 54.0f) != 0) {
            shell18_set(em, 0x1E);
        }
        if (em_frame_check(em, 0, 54.0f) != 0) {
            Eft13_set_em_scl(em, 0x1A, 2.0f, 3);
        }
        if (em_frame_check(em, 0, 52.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 2.0f, 3);
        }
        if (em_frame_check(em, 0, 56.0f) != 0) {
            Eft13_set_em_scl(em, 0x14, 2.0f, 3);
            return;
        }
        break;
    case 0x457:
        sound_call_005BCBA0(em, 0xE, 2, 0x14);
        sound_call_005BCBA0(em, 0x13E, 4, 0x14);
        sound_call_005BCBA0(em, 0x170, 1, 0x1A);
        sound_call_005BCBA0(em, 4, 0x16, 0x23);
        sound_call_005BCBA0(em, 4, 0x1F, 0x23);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 0x64, 0x5B, 0x23);
        } else {
            sound_call_005BCBA0(em, 0x64, 0x79, 0x23);
        }
        sound_call_005BCBA0(em, 0x130, 0x16, 0);
        if (em_frame_check(em, 0, 112.0f) != 0) {
            shell18_set(em, 0x13);
            Eft15_set3(em, 5, 1.0f, 6);
            return;
        }
        break;
    case 0x458:
        sound_call_005BCBA0(em, 0x32, 0x28, 0x23);
        sound_call_005BCBA0(em, 0x68, 0x27, 0x23);
        sound_call_005BCBA0(em, 0x22, 4, 0x14);
        sound_call_005BCBA0(em, 0x86, 0, 0x14);
        sound_call_005BCBA0(em, 0xB6, 1, 0x1A);
        sound_call_005BCBA0(em, 0x26, 0x15, 0x2A);
        sound_call_005BCBA0(em, 0x52, 0x15, 0x2A);
        sound_call_005BCBA0(em, 0x72, 0x15, 0x2A);
        sound_call_005BCBA0(em, 0x9C, 0x14, 0x2A);
        sound_call_005BCBA0(em, 0xC4, 0x13, 0x2A);
        if (em_frame_check(em, 0, 40.0f) != 0) {
            shell18_set(em, 0x14);
            return;
        }
        break;
    case 0x459:
        sound_call_005BCBA0(em, 4, 0x16, 0);
        if (em->type == 0) {
            sound_call_005BCBA0(em, 4, 0x5E, 0x23);
        } else {
            sound_call_005BCBA0(em, 4, 0x7C, 0x23);
        }
        sound_call_005BCBA0(em, 0x10E, 0x2B, 0x23);
        sound_call_005BCBA0(em, 4, 0x11, 0x14);
        sound_call_005BCBA0(em, 0x36, 4, 0x14);
        sound_call_005BCBA0(em, 0x86, 0, 0x14);
        sound_call_005BCBA0(em, 0xDC, 3, 0x14);
        sound_call_005BCBA0(em, 0x128, 3, 0x1A);
        break;
    case 0x45A:
        sound_call_005BCBA0(em, 4, 0x37, 0x23);
        sound_call_005BCBA0(em, 0x66, 0x34, 0x23);
        sound_call_005BCBA0(em, 0x22, 6, 0x1A);
        sound_call_005BCBA0(em, 0x1E, 0xF, 6);
        sound_call_005BCBA0(em, 0x22, 0xF, 0xC);
        quake_call_005BCCA0(em, 4, 3);
        quake_call_005BCCA0(em, 0x68, 0);
        if (em_frame_check(em, 0, 2.0f) != 0) {
            v3[1] = 30.0f;
            v3[0] = 0.0f;
            v3[2] = 100.0f;
            flvecApplyMat33_2(v3, get_joint_wmat_em(em, 0x27));
            get_joint_pos_em(em, 0x27, sp50);
            AddVector(sp50, sp50, v3);
            Eft02_set3(em, 0, 9, 0, sp50, 1.0f);
            Eft02_set3(em, 0, 9, 1, sp50, 1.0f);
            Eft13_set_pos2(10.0f, em, sp50, 0x22);
        }
        if (!(GAME_X1E16 & 0x1F)) {
            Eft21_set(em, 2);
            return;
        }
        break;
    case 0x45B:
        sound_call_005BCBA0(em, 4, 0x37, 0x23);
        sound_call_005BCBA0(em, 0x5C, 0x28, 0x23);
        sound_call_005BCBA0(em, 4, 0x16, 0);
        sound_call_005BCBA0(em, 0x52, 0xD, 6);
        sound_call_005BCBA0(em, 0x56, 0xD, 0xC);
        sound_call_005BCBA0(em, 0x48, 0x1D, 0x22);
        sound_call_005BCBA0(em, 0x6C, 6, 0x1A);
        sound_call_005BCBA0(em, 0x8E, 5, 0x14);
        sound_call_005BCBA0(em, 0xA6, 0, 0x1A);
        if (em_frame_check(em, 0, 68.0f) != 0) {
            Eft21_set(em, 2);
            return;
        }
        break;
    default:
        move_default_005BCCF0(em);
        break;
    }
}
