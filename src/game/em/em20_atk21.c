/* em20_atk21 - game.bin 0x005F3440-0x005F3CD8: em_atk21_005F3440, a monster 20 attack. Whole file in em20_ai_nm.c. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"
#include "quest.h"
#include "em20.h"
void em20_xang_set_pl(EMW *em, int mode, f32 h);   /* PC: prototype (the draft passed f12 first as an int) */

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
void item_theft_005EC560(EMW *em, PLW *pl);
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
void em_atk31_005F41D0(EMW *em, EM20W *w);
void em_dmg00_005F4260(EMW *em, EM20W *w);
void em_dmg01_005F42F0(EMW *em, EM20W *w);
void em_dmg02_005F4380(EMW *em, EM20W *w);
void em_dmg03_005F4410(EMW *em, EM20W *w);
void em_dmg04_005F44A0(EMW *em, EM20W *w);
void em_dmg05_005F44B0(EMW *em, EM20W *w);
void em_dmg06_005F45D0(EMW *em, EM20W *w);
void em_dmg07_005F46F0(EMW *em, EM20W *w);
void em_dmg08_005F47E0(EMW *em, EM20W *w);
void em_dmg09_005F4940(EMW *em, EM20W *w);
void em_dmg10_005F4AD0(EMW *em, EM20W *w);
void em_dmg11_005F4B60(EMW *em, EM20W *w);
void em_dmg12_005F4C40(EMW *em, EM20W *w);
void em_dmg13_005F4D70(EMW *em, EM20W *w);
void em_dmg14_005F4EA0(EMW *em, EM20W *w);
void em_dmg15_005F4FC0(EMW *em, EM20W *w);
void em_dmg16_005F5090(EMW *em, EM20W *w);
void em_dmg17_005F5160(EMW *em, EM20W *w);
void em_dmg18_005F5290(EMW *em, EM20W *w);
void em_dmg19_005F5390(EMW *em, EM20W *w);
void em_dmg20_005F5490(EMW *em, EM20W *w);
void em_demo00_005F55D0(EMW *em, EM20W *w);
void em_demo04_005F5870(EMW *em, EM20W *w);
void em_die00_005F5990(EMW *em, EM20W *w);
void em_die01_005F5B30(EMW *em, EM20W *w);
void em_die02_005F5CB0(EMW *em, EM20W *w);
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
void em20_atk_end_sel(EMW *em, EM20W *w);
void kyusyu_senkai_ret_005FCBA0(EMW *em, EM20W *w);
void em20_material_sub(EMW *em, int type, u8 *tbl);
void dummy_em_prog_005FCDB0(void);

extern u8 *em20_act_add[3];
extern u16 *em20_rail_add[2];
extern u16 *em20_rail_half_add[1];

extern u8 *em20_act_add[3];

extern FLYNEED *em_hungry_tbl[];

#define UVR(i) \
    do { \
        em->uv[i][0] = 0.0f; \
        em->uv[i][1] = 0.0f; \
        em->uvtm[i] = 0xFFFF; \
        em->uvty[i] = 0xFF; \
    } while (0)

void em_atk21_005F3440(EMW *em, EM20W *w) {
    f32 sp60[3];
    f32 sp50[3];
    f32 sp40[3];
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;
    f32 temp_f1_4;
    f32 temp_f1_5;
    f32 temp_f1_6;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_3;
    s8 temp_s2;
    s8 temp_v1_5;
    u16 temp_v1_2;
    u8 temp_a1;
    u8 temp_v1_4;
    STAGE_DATA *temp_v0;
    PLW *pl;

    temp_v0 = Stage_data_get(em->stg);
    temp_a1 = em->x05;
    switch (temp_a1) {
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 2;
        em->x3F4 = 0;
        break;
    case 1:
        w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
        w->dang = (u16) (w->dang - em->ang[1]);
        em20_senkai_sub(em, 3, 1);
        temp_f1 = em->adj_z;
        if (100.0f < temp_f1) {
            em->adj_z = temp_f1 - 2.0f;
        } else if (CalcDistanceXZ(em->pos, em->tgt_pos) <= 3000.0f) {
            em->x05 += 1;
            em->work08 = 0x258;
        }
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add(em, w->spd);
        temp_v1 = em->work08 - 1;
        em->work08 = temp_v1;
        if (temp_v1 <= 0) {
            kyusyu_senkai_ret_005FCBA0(em, w);
        }
        break;
    case 2:
        if (em->x617 == -1) {
            kyusyu_senkai_ret_005FCBA0(em, w);
        } else {
            temp_f1_2 = em->adj_z;
            if (80.0f < temp_f1_2) {
                em->adj_z = temp_f1_2 - 1.0f;
            }
            temp_s2 = em->x617;
            em_pl_pos_set(em, temp_s2 & 0xFF, sp50);
            pl = &player_work[(s8)temp_s2];
            World_calc2(pl->stg, sp50, sp60);
            w->dang = Em_Calc_angY(em->x754, sp60);
            w->dang = (u16) (w->dang - em->ang[1]);
            em20_senkai_sub(em, 3, 1);
            temp_v1_2 = w->dang;
            if (((s32) temp_v1_2 <= 0x800) || ((s32) temp_v1_2 >= 0xF800)) {
                if (!(4000.0f < CalcDistanceXZ(em->pos, sp50))) {
                    kyusyu_senkai_ret_005FCBA0(em, w);
                } else {
                    em->x05 += 1;
                    w->turn = 0x100;
                    em_char_set(em, 0x2C, 0, 0);
                    em->work08 = 0x258;
                }
            }
            w->spd[0] = (s32) em->ang[0];
            w->spd[1] = (s32) em->ang[1];
            w->spd[2] = 0;
            speed_add(em, w->spd);
            temp_v0_2 = em->work08 - 1;
            em->work08 = temp_v0_2;
            if (temp_v0_2 > 0) {
                if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                    goto block_24;
                }
            } else {
block_24:
                kyusyu_senkai_ret_005FCBA0(em, w);
            }
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
            em20_xang_set_pl(em, 0, -200.0f);
            temp_f1_3 = em->adj_z;
            if (50.0f < temp_f1_3) {
                em->adj_z = temp_f1_3 - 0.1f;
            }
        }
        if (em->x194 == 0) {
            em->x05 += 1;
            em_char_set(em, 0x57, 0, 0);
            em->adj_z = 100.0f;
            em->work08 = 0x258;
        }
        em20_senkai_player(em);
        w->spd[0] = (s32) em->ang[0];
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0xF;
        speed_add(em, w->spd);
        em->pos[1] -= 0.1f;
        temp_v0_3 = em->work08 - 1;
        em->work08 = temp_v0_3;
        if (temp_v0_3 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_38;
            }
        } else {
block_38:
            kyusyu_senkai_ret_005FCBA0(em, w);
        }
        break;
    case 4:
        if (em->pos[1] <= (1000.0f + temp_v0->floor_y)) {
            em->x05 = temp_a1 + 1;
            em->work08 = 0x258;
        }
        if (kyusyu_char_set_005FCA70(em) != 0) {
            temp_v1_4 = em->x05;
            if (temp_v1_4 == 4) {
                em->x05 = temp_v1_4 + 1;
                em->work08 = 0x258;
            }
        }
        if (em->char0 == 0x415) {
            em20_xang_set_pl(em, 2, 0.0f);
        } else {
            em20_xang_set_pl(em, 0, -200.0f);
        }
        em20_senkai_player(em);
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        xang_calc_pl(em, w->spd, -3000.0f, 0.0f);
        if (em->x74C != 0) {
            w->spd[0] = 0;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            temp_f1_4 = em->adj_y;
            if (temp_f1_4 < 0.0f) {
                em->pos[1] -= temp_f1_4;
            }
            em->pos[1] += 30.0f;
        }
        temp_v0_4 = em->work08 - 1;
        em->work08 = temp_v0_4;
        if (temp_v0_4 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_57;
            }
        } else {
block_57:
            kyusyu_senkai_ret_005FCBA0(em, w);
        }
        break;
    case 5:
        kyusyu_char_set_005FCA70(em);
        if ((em->char0 == 0x415) && (em->x194 == 0)) {
            em->x05 = 6;
            em->x3C0[0] = 0.0f;
            em->x3C0[2] = -10.0f;
            em->x3C0[1] = -10.0f;
        }
        em20_senkai_player(em);
        em20_xang_set_pl(em, 2, 100.0f);
        if ((em->char0 == 0x415) && (em_frame_check2(em, 0, 76.0f) != 0)) {
            xang_calc_pl(em, w->spd, 50.0f, 0.0f);
        } else {
            temp_v1_5 = em->x617;
            if (temp_v1_5 != -1) {
                em_pl_pos_set(em, temp_v1_5 & 0xFF, sp40);
                if (!((200.0f + sp40[1]) < em->pos[1])) {
                    xang_calc_pl(em, w->spd, 50.0f, 0.0f);
                } else {
                    xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
                }
            } else {
                xang_calc_pl(em, w->spd, -2000.0f, 0.0f);
            }
        }
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        if (em->x74C != 0) {
            w->spd[0] = 0;
        }
        speed_add(em, w->spd);
        if (em->x74C != 0) {
            w->spd[0] = 0;
            temp_f1_5 = em->adj_y;
            if (temp_f1_5 < 0.0f) {
                em->pos[1] -= temp_f1_5;
            }
            em->pos[1] += 30.0f;
        }
        temp_v0_5 = em->work08 - 1;
        em->work08 = temp_v0_5;
        if (temp_v0_5 > 0) {
            if (!(em_target_pl_samestage_ck(em) & 0xFF)) {
                goto block_80;
            }
        } else {
block_80:
            kyusyu_senkai_ret_005FCBA0(em, w);
        }
        break;
    case 6:
        if (em->adj_z < 0.0f) {
            em->adj_z = 0.0f;
            em->x3C0[2] = 0.0f;
        }
        w->spd[0] = 0;
        w->spd[1] = (s32) em->ang[1];
        w->spd[2] = 0;
        speed_add_g(em, w->spd);
        if (em->pos[1] <= em->x5AC) {
            em->x05 += 1;
            em->ang[0] = 0;
            em->ang[2] = 0;
            em->pos[1] = em->x5AC;
            em->x388 = 0;
            em_char_set(em, 0x29, 0, 0);
        }
        break;
    case 7:
        if (em->x194 == 0) {
            em->x05 = temp_a1 + 1;
            em20_atk_end_sel(em, w);
        }
        break;
    }
    temp_f1_6 = em->x5AC;
    if (em->pos[1] < temp_f1_6) {
        em->pos[1] = temp_f1_6;
    }
}
