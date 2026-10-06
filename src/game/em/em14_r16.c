/* em14_r16 - monster 14 AI 0x005B7B20-0x005B7C5C: em_mv07_005B7B20. Whole file in em14_nm.c. */
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
void em_dmg08_005BA140(EMW *em, EM14W *w);
void em_dmg09_005BA2A0(EMW *em, EM14W *w);
void em_dmg10_005BA400(EMW *em, EM14W *w);
void em_dmg11_005BA490(EMW *em, EM14W *w);
void em_dmg13_005BA560(EMW *em, EM14W *w);
void em_dmg14_005BA660(EMW *em, EM14W *w);
void em_dmg15_005BA800(EMW *em, EM14W *w);
void em_dmg16_005BA880(EMW *em, EM14W *w);
void em_dmg17_005BA9A0(EMW *em, EM14W *w);
void em_dmg18_005BAA90(EMW *em, EM14W *w);
void em_demo00_005BAB80(EMW *em, EM14W *w);
void em_die00_005BAEF0(EMW *em, EM14W *w);
void em_die01_005BB090(EMW *em, EM14W *w);
void em_die02_005BB290(EMW *em, EM14W *w);
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














































































































void em_mv07_005B7B20(EMW *em, EM14W *w) {
    u16 temp_v1;
    u32 temp_a1_2;
    s32 temp_a0;
    u8 temp_a1;

    temp_a1 = em->x05;
    switch (temp_a1) {                              /* irregular */
    case 0:
        em->x05 = temp_a1 + 1;
        em->x388 = 0;
        em->x3F4 = 0;
        em_char_set(em, 0x36, 0, 0);
        w->tgt_ang = Em_Calc_angY(em->pos, em->tgt_pos);
        break;
    case 1:
        temp_a0 = em->ang[1];
        temp_v1 = w->tgt_ang;
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
            em14_to_normal(em, 0, 0);
        }
        break;
    }
}
