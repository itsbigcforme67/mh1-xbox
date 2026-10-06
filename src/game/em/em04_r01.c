/* em04_r01 - near-match fixes: em_dm03_0058D4E0. Whole file in em04_nm.c. 0x0058D4E0-0x0058D5F8: em_dm03_0058D4E0. Whole file in em04_nm.c. */
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
int em_frame_check(EMW *, int, f32);
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
void em04_act_set();
#define em04_act_set_k em04_act_set
void em04_main_sub(EMW *em);
void move_default_0058E4F0(EMW *em);
void Eft13_set_em_scl(EMW *, int, f32, int);
int Code_Make(int, int, int, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void sound_call_0058F430(EMW *em, int frame, int se);

extern u8 em04_act_tbl[];
extern f32 em05_rev_set_tbl_st69[][6];
extern f32 em05_rev_set_tbl_st18[][6];

void em04_next_act_set(EMW *em);





































typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number */
} QUEST_W;
extern QUEST_W quest_w;
extern GAME_W game_w;








void em_dm03_0058D4E0(EMW *em) {
    if (em->work08 > 0) {
        em->work08--;
    }
    switch (em->x05) {
    case 0:
        em->x05++;
        em->x388 = 0;
        em_char_set(em, 71, 0, 0);
        em->ang[1] = em->dm_ang + 0x8000;
        Em_Mahi_Start(em);
        em->x8BD = 1;
        em_cmd_reset(em);
        break;
    case 1:
        em_mahi_eff_set(em, 2);
        if (em->work08 <= 0) {
            em->x05++;
            em_char_set(em, 72, 0, 0);
            em->ang[1] = em->ang[1] + 0x8000;
            Em_Mahi_End(em);
        }
        break;
    case 2:
        if (em->x194 == 0) {
            em->x8BD = 0;
            em04_next_act_set(em);
        }
        break;
    }
}
