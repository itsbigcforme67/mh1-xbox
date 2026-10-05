#ifndef EM03_H
#define EM03_H
/* Shared declarations for the monster 3 files (em03.c, em03b.c, em03c.c,
 * em03_nm.c): game.bin 0x005873D0-0x0058B84C. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM03W {
    u8 eff;             /* 0x00 em03_effect_move step */
    u8 _pad01[5];
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    s32 spd[3];         /* 0x08 speed handed to speed_add_g ([1] = angle) */
    f32 x14;            /* 0x14 time left (counted down by the motion step) */
    u8 x18;             /* 0x18 non-zero: turn toward the target */
} EM03W;
extern EMW em_work[];
extern u8 em_boss_tbl[];

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);
u32 ran_suu(int);
int em_mode_timer_sub(EMW *);
void em_cmd_reset(EMW *);
int Pl_stg_ck_tw(EMW *, PLW *);
void em_escape_mind_set(EMW *, u8, u8);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void Em_Sleep_End(EMW *);
u8 Em_Smoke_Ck(EMW *);
void em_cmd_ck(EMW *);
void em03_move_sub(EMW *em);
void em03_act_set(EMW *em, int kind, u16 no, u16 arg);
void em03_to_normal(EMW *em);
void em03_char_set(EMW *em, int no, int a, int b);
void Em_Sleep_Start(EMW *);
int em_frame_check(EMW *, f32, int);
void Em_Mahi_End(EMW *);
u16 Em_Calc_angY(f32 *, f32 *);
int em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
void mot_miration_ret(EMW *, f32 *);
int em_frame_check2(EMW *, f32, int);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
void GetGroundHitArea(EMW *, f32 *, f32 *);
void Eft04_set_time(EMW *, int, int, f32);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
void Em_Mahi_Start(EMW *);
void em_cdm_act_flag_ck(EMW *);
f32 CalcDistanceXZ(f32 *, f32 *);
void em_act_set2(EMW *, int, u16, u16);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
int Code_Make(int, int, int, int);
void shell02_set(EMW *, int);
void Eft13_set_em_scl(EMW *, int, f32, int);
void em_char_set2(EMW *, int, int, int, int);
void flmatGetTrans(f32 *, void *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void Quest_enemy_escape(EMW *);

void em_move00_00588180(EMW *em, EM03W *w);
void em_mv00_00588290(EMW *em);
void em_mv01_00588350(EMW *em, EM03W *w);
void em_mv02_00588480(EMW *em, EM03W *w, int mode);
void em_mv03_005885F0(EMW *em, EM03W *w, int mode);
void em_mv04_00588760(EMW *em);
void em_mv05_00588810(EMW *em);
void em_mv06_005888C0(EMW *em);
void em_mv07_00588990(EMW *em);
void em_mv08_00588A60(EMW *em);
void em_mv11_00588AD0(EMW *em);
void em_mv12_00588BF0(EMW *em);

#endif
