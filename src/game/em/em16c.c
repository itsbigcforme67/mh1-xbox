/* em16 (part 3) - game.bin 0x005D8190-0x005D8198: dummy program. */
#include "em.h"
#include "game.h"
#include "fl.h"
#include "pl.h"
#include "plf.h"

/* Per-monster work at EMW+0x444 (em16.c has the same start). */
typedef struct EM16W {
    u8 eff;             /* 0x00 em16_effect_move step */
    u8 _pad01[3];
    s16 anim;           /* 0x04 animation the sound/effect script follows */
    u16 tgt_ang;        /* 0x06 angle toward the target (mv00) */
    s32 spd[3];         /* 0x08 passed to speed_add_g (angle in [1]) */
    u8 adj_x;           /* 0x14 fly_adjy2 channels on/off */
    u8 adj_y;           /* 0x15 */
    u8 adj_z;           /* 0x16 */
    u8 adj_type;        /* 0x17 table row */
    s16 adj_tm;         /* 0x18 time into the table */
    u8 yobi_st;         /* 0x1A yobi state */
    u8 has_tgt;         /* 0x1B */
    f32 dist;           /* 0x1C distance to the target (1000 with none) */
    f32 yobi[3];        /* 0x20 yobi position */
    f32 yobi_r;         /* 0x2C yobi range */
    u8 _pad30[2];
    s8 x32;             /* 0x32 */
    u8 _pad33;
    u8 yobi_stg;        /* 0x34 stage */
    u8 _pad35[3];
    s32 x38;            /* 0x38 */
    s32 x3C;            /* 0x3C */
    f32 x40;            /* 0x40 saved 0x3BC */
    s32 x44;            /* 0x44 */
    s32 x48;            /* 0x48 */
    s32 x4C;            /* 0x4C */
    f32 home[3];        /* 0x50 position of the start */
    s32 home_ang;       /* 0x5C */
    u8 x60;             /* 0x60 */
} EM16W;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 quest number (0: free play) */
} QUEST_W;

extern QUEST_W quest_w;
extern GAME_W game_w;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

void em_char_set(EMW *, int, int, int);
void em_act_set(EMW *, int, u16);
s16 em_hp_vital_set(EMW *, s16);
void em_dur_init(EMW *);
void em_boss_work_set(EMW *);
void em_status_init(EMW *);
void em16_act_set(EMW *em, int kind, u16 no, u16 arg);
void em16_to_normal();
u16 Em_Calc_angY(f32 *, f32 *);
int em_frame_check(EMW *, f32, int);
int em_frame_check2(EMW *, int, f32);
void SetVector(f32 *, f32, f32, f32);
void pull_em_yobi(f32 *);
void push_em_yobi(f32 *);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void Em_Mahi_End(EMW *);
void mot_miration_ret(EMW *, f32 *);
u8 em16_fly_adjy2(EMW *);
void em16_fly_adjy2_init(EMW *, u8);
void shell15_set(EMW *, int);
void em_rate_clear(EMW *);
void speed_add_g(EMW *, s32 *);
void em_cmd_reset(EMW *);
void Em_Mahi_Start(EMW *);
void em_mahi_eff_set(EMW *, int);
f32 CalcDistanceXZ(f32 *, f32 *);
void Shell08_set_ang(EMW *, s16, u8, u8, u16, u16);
extern f32 st02_pos_tbl[][6];
extern f32 st09_pos_tbl[][6];
extern f32 st18_pos_tbl[][6];
extern f32 st31_pos_tbl[][6];
extern f32 st38_pos_tbl_00663520[][6];
extern f32 st39_pos_tbl_006635A0[][6];
extern f32 st46_pos_tbl[][6];
extern f32 st49_pos_tbl_00663690[][6];
extern u16 st51_ang_tbl[3];
extern f32 st51_pos_tbl_00663720[][3];
extern u16 st53_ang_tbl[4];
extern f32 st53_pos_tbl_00663750[][3];
extern f32 st58_pos_tbl[][6];
extern f32 st63_pos_tbl[][6];
extern f32 st66_pos_tbl[][6];
extern f32 st69_pos_tbl[][6];
extern f32 st71_pos_tbl[][6];
void shell13_set(EMW *, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void flmatGetTrans(f32 *, void *);
int Code_Make(int, int, int, int);
void Eft13_set_em(EMW *, int, int);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
int em_mode_timer_sub(EMW *);
int Em_Yobi_Ck(EMW *, f32 *);
int em_cancel_act_ck(EMW *, u8);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
void em_cmd_ck(EMW *);
void em16_init2(EMW *);
int Event_flag_ck(int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
void Em_hagi_point_set(EMW *, int);
int Em_hagi_point_cnt_ck(EMW *);
void Em_hagi_point_clr(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em16_init(EMW *);
void Quest_enemy_escape(EMW *);

void dummy_em_prog_005D8190(void) {
}
