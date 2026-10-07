/* em04d - game.bin 0x0058E500-0x0058F3DC: ef_move_sub_0058E500, em04's per-animation sound script (sound_call at fixed frames). In the original this is the same TU as em04_nm.c; there move_default_0058E4F0 is a file static whose (empty) call is kept; here it is extern so the call stays. Whole file in em04_nm.c. */
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
static void move_default_0058E4F0(EMW *em);
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

/* Sound and effect script per animation: sound_call(em, frame, se) plays
 * the sound code se once when the animation reaches frame. */

/* A file static in the original; data tables point at it. */

static void move_default_0058E4F0(EMW *em) {
}

void ef_move_sub_0058E500(EMW *em, EM04W *w) {   /* not static: em04c.c calls it */
    if (em->char0 != w->anim) {
        w->anim = em->char0;
    }
    switch (w->anim) {
    case 0x3E9:
        sound_call_0058F430(em, 16, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 76, Code_Make(1, 2, 2, 2));
        break;
    case 0x3EA:
        sound_call_0058F430(em, 12, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 42, Code_Make(1, 2, 2, 2));
        sound_call_0058F430(em, 6, 19);
        sound_call_0058F430(em, 16, 18);
        sound_call_0058F430(em, 36, 19);
        break;
    case 0x3EB:
        sound_call_0058F430(em, 12, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 38, Code_Make(1, 2, 2, 2));
        sound_call_0058F430(em, 6, 18);
        sound_call_0058F430(em, 30, 19);
        sound_call_0058F430(em, 44, 19);
        break;
    case 0x3EC:
    case 0x3ED:
        sound_call_0058F430(em, 6, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 70, Code_Make(1, 2, 2, 2));
        sound_call_0058F430(em, 22, 19);
        sound_call_0058F430(em, 38, 19);
        sound_call_0058F430(em, 56, 18);
        sound_call_0058F430(em, 68, 19);
        sound_call_0058F430(em, 100, 19);
        break;
    case 0x3EE:
        sound_call_0058F430(em, 40, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 60, Code_Make(1, 4, 2, 4));
        sound_call_0058F430(em, 80, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 100, Code_Make(1, 4, 2, 4));
        sound_call_0058F430(em, 154, 15);
        sound_call_0058F430(em, 20, 18);
        sound_call_0058F430(em, 40, 18);
        sound_call_0058F430(em, 164, 18);
        break;
    case 0x3EF:
        sound_call_0058F430(em, 38, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 58, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 92, Code_Make(4, 4, 5, 4));
        sound_call_0058F430(em, 114, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 164, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 180, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 210, Code_Make(4, 4, 5, 4));
        sound_call_0058F430(em, 232, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 24, 18);
        sound_call_0058F430(em, 40, 18);
        sound_call_0058F430(em, 130, 18);
        sound_call_0058F430(em, 164, 18);
        sound_call_0058F430(em, 246, 18);
        break;
    case 0x3F0:
        sound_call_0058F430(em, 4, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 36, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 4, 19);
        sound_call_0058F430(em, 42, 19);
        sound_call_0058F430(em, 52, 18);
        break;
    case 0x3F1:
        sound_call_0058F430(em, 6, Code_Make(0, 4, 1, 4));
        sound_call_0058F430(em, 70, Code_Make(0, 2, 1, 2));
        sound_call_0058F430(em, 38, 19);
        sound_call_0058F430(em, 76, 19);
        break;
    case 0x3F2:
        sound_call_0058F430(em, 36, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 58, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 128, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 150, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 22, 18);
        sound_call_0058F430(em, 116, 19);
        break;
    case 0x3F3:
        sound_call_0058F430(em, 22, 15);
        sound_call_0058F430(em, 10, 18);
        sound_call_0058F430(em, 82, 18);
        break;
    case 0x3F4:
        sound_call_0058F430(em, 4, 14);
        sound_call_0058F430(em, 68, 15);
        sound_call_0058F430(em, 12, 18);
        sound_call_0058F430(em, 70, 19);
        sound_call_0058F430(em, 94, 19);
        break;
    case 0x3F5:
        sound_call_0058F430(em, 24, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 40, 18);
        break;
    case 0x3F6:
        sound_call_0058F430(em, 68, 13);
        sound_call_0058F430(em, 36, 20);
        sound_call_0058F430(em, 74, 16);
        if (em_frame_check(em, 0, 66.0f)) {
            Eft13_set_em_scl(em, 6, 0.7f, 3);
        }
        break;
    case 0x3F7:
        sound_call_0058F430(em, 10, 14);
        sound_call_0058F430(em, 24, 19);
        sound_call_0058F430(em, 36, 19);
        if (em_frame_check(em, 0, 16.0f)) {
            Eft13_set_em_scl(em, 4, 2.0f, 3);
        }
        if (em_frame_check(em, 0, 30.0f)) {
            Eft13_set_em_scl(em, 17, 3.5f, 3);
        }
        break;
    case 0x3F8:
        sound_call_0058F430(em, 20, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 8, 19);
        sound_call_0058F430(em, 14, 19);
        sound_call_0058F430(em, 12, 13);
        sound_call_0058F430(em, 98, 18);
        if (em_frame_check(em, 0, 10.0f) || em_frame_check(em, 0, 14.0f)) {
            Eft13_set_em_scl(em, 6, 1.2f, 3);
            Eft13_set_em_scl(em, 9, 1.2f, 3);
        }
        break;
    case 0x3F9:
        sound_call_0058F430(em, 32, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 70, 15);
        sound_call_0058F430(em, 200, Code_Make(4, 4, 5, 4));
        sound_call_0058F430(em, 28, 20);
        sound_call_0058F430(em, 46, 19);
        break;
    case 0x3FB:
        sound_call_0058F430(em, 14, 13);
        sound_call_0058F430(em, 60, Code_Make(1, 4, 2, 4));
        sound_call_0058F430(em, 80, Code_Make(0, 4, 1, 4));
        break;
    case 0x3FC:
        sound_call_0058F430(em, 8, Code_Make(7, 4, 8, 4));
        sound_call_0058F430(em, 26, 21);
        sound_call_0058F430(em, 60, 18);
        break;
    case 0x424:
    case 0x425:
        sound_call_0058F430(em, 12, Code_Make(7, 4, 8, 4));
        sound_call_0058F430(em, 96, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 38, 20);
        sound_call_0058F430(em, 42, 20);
        sound_call_0058F430(em, 84, 18);
        sound_call_0058F430(em, 108, 18);
        break;
    case 0x426:
    case 0x42B:
        sound_call_0058F430(em, 12, Code_Make(9, 4, 10, 4));
        break;
    case 0x427:
        sound_call_0058F430(em, 16, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 14, 17);
        if (em_frame_check(em, 0, 2.0f)) {
            Eft13_set_em_scl(em, 2, 0.8f, 7);
        }
        break;
    case 0x428:
        sound_call_0058F430(em, 8, 11);
        sound_call_0058F430(em, 8, 16);
        sound_call_0058F430(em, 30, 16);
        break;
    case 0x42A:
    case 0x430:
        sound_call_0058F430(em, 18, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 22, 16);
        sound_call_0058F430(em, 30, 18);
        sound_call_0058F430(em, 60, 19);
        sound_call_0058F430(em, 88, 18);
        sound_call_0058F430(em, 104, 18);
        sound_call_0058F430(em, 128, Code_Make(5, 4, 6, 4));
        break;
    case 0x42F:
        sound_call_0058F430(em, 6, Code_Make(9, 4, 10, 4));
        sound_call_0058F430(em, 36, 17);
        sound_call_0058F430(em, 66, Code_Make(3, 4, 4, 4));
        sound_call_0058F430(em, 96, Code_Make(3, 4, 6, 4));
        sound_call_0058F430(em, 124, Code_Make(5, 4, 6, 4));
        sound_call_0058F430(em, 180, Code_Make(0, 4, 1, 4));
        if (em_frame_check(em, 0, 34.0f)) {
            Eft13_set_em_scl(em, 2, 1.0f, 6);
        }
        break;
    case 0x42E:
        sound_call_0058F430(em, 4, 12);
        sound_call_0058F430(em, 20, 18);
        sound_call_0058F430(em, 52, 18);
        sound_call_0058F430(em, 62, 18);
        sound_call_0058F430(em, 120, 16);
        sound_call_0058F430(em, 280, Code_Make(1, 4, 2, 4));
        break;
    default:
        move_default_0058E4F0(em);
        break;
    }
}
