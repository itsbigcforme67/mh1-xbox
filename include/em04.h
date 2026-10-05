#ifndef EM04_H
#define EM04_H
/* Shared declarations for the monster 4 files (em04.c, em04b.c, em04c.c,
 * em04_nm.c): game.bin 0x0058BA40-0x0058F498. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM04W {
    u8 eff;             /* 0x00 em04_effect_move step */
    u8 _pad01[5];
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    u8 _pad08[4];
    u16 x0C;            /* 0x0C counted down each frame */
    u8 _pad0E[6];
    f32 home[3];        /* 0x14 position it returns to (mov05) */
    u8 _pad20[4];
    u16 tgt_ang;        /* 0x24 facing to turn to (mov00) */
} EM04W;

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
u16 em_act_search(void *);
void target_kind_set(EMW *, f32 *);
void em_action_timer_calc(EMW *, int);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
u16 Em_Calc_angY(f32 *, f32 *);
void em09_dir_calc(s32 *, s32 *, int);
void cpRotMatrix(s32 *, f32 (*)[4]);
u32 ran_suu(int);
f32 flvecCalcDistance(f32 *, f32 *);
void pl_flag_set(EMW *, u32);
void pl_flag_clr(EMW *, u32);
void em_cmd_reset(EMW *);
void shell02_set(EMW *, int);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
int em_frame_check(EMW *, f32, int);
void em_rate_clear_g(EMW *);
int rate_add_g2(EMW *);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void em_rate_clear(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
int Event_flag_ck();
void em_dur_set(EMW *, int);
void em_cmd_ck(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
int act_ck(EMW *, int, int);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void Quest_enemy_escape(EMW *);
void em04_init(EMW *);
void Eft13_set_em_scl(EMW *, int, f32, int);
int Code_Make(int, int, int, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);

/* Called with a varying number of arguments in the original (the setter
 * has three parameters, callers pass a fourth), so it is declared without
 * a prototype. */
void em04_act_set();
void em04_next_act_set(EMW *em);
void em04_main_sub(EMW *em);
extern u8 em04_act_tbl[];
void em_move00_0058C350(EMW *em);
void em_move01_0058CBD0(EMW *em);
void em_move03_0058CF10(EMW *em);
void em_dm00_0058CFB0(EMW *em);
void em_dm01_0058D0A0(EMW *em);
void em_dm02_0058D2E0(EMW *em);
void em_dm03_0058D4E0(EMW *em);
void move_default_0058E4F0(EMW *em);
void ef_move_sub_0058E500(EMW *em, EM04W *w);
void sound_call_0058F430(EMW *em, int frame, int se);

#endif
