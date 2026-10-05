/* em09 (part 4) - game.bin 0x005ACC40-0x005ACC58: em09_local_init and the dummy program. */
#include "em.h"
#include "game.h"
#include "pl.h"
#include "plf.h"
#include "fl.h"

/* Per-monster work at EMW+0x444. */
typedef struct EM09W {
    u8 eff;             /* 0x00 em09_effect_move step */
    u8 _pad01[3];
    s16 x04;            /* 0x04 */
    s16 anim;           /* 0x06 animation the sound/effect script follows */
    u8 _pad08[4];
    u16 x0C;            /* 0x0C counted down by em09_main */
    u16 chase;          /* 0x0E non-zero: chasing a target */
    u8 _pad10[4];
    f32 home[3];        /* 0x14 home position */
    s32 shell;          /* 0x20 shell02_set result */
    PLW *pl;            /* 0x24 player the item was stolen from */
    f32 x28[3];         /* 0x28 yobi position (SetVector) */
    f32 x34;            /* 0x34 yobi range */
    u8 _pad38[2];
    u8 x3A;             /* 0x3A */
    u8 _pad3B;
    u8 x3C;             /* 0x3C stage */
    u8 _pad3D[0x44 - 0x3D];
    u16 x44;            /* 0x44 effect timer */
    s16 item;           /* 0x46 stolen item, -1 none */
    s16 item_num;       /* 0x48 */
    u8 x4A;             /* 0x4A eye/face blink state 0..3 (em09_effect_move) */
    u8 x4B;             /* 0x4B */
} EM09W;

extern GAME_W game_w;
extern EMW em_work[];
extern f32 em09_rev_set_tbl_st53[][6];
extern s32 em09_rev_set_tbl_st53A[];
extern u8 em09_act_tbl[];

void em_act_set(EMW *, int, u16);
u16 em_act_search(void *);
void target_kind_set(EMW *, f32 *);
void em_char_set(EMW *, int, int, int);
int Quest_clear_ck();
s16 Pl_item_num_ck(PLW *, int);
void em_char_set(EMW *, int, int, int);
void em09_next_act_set(EMW *);
void Em_Sleep_Start(EMW *);
void Em_Sleep_End(EMW *);
void em_sleep_eff_set(EMW *, int, f32 *, f32);
void Eft24_set_em(EMW *, int, int, int, f32, f32);
void shell14_set(EMW *, int);
int em_frame_check(EMW *, f32, int);
void Quest_enemy_escape(EMW *);
int shell02_set(EMW *, int);
void em_rate_add_g(EMW *);
void flvecRotY(f32 *, f32);
void em09_act_set();
void em_escape_mind_set(EMW *, u8, u8);
void em_cmd_reset(EMW *);
void em_ikari_add(EMW *, s16);
int Em_Yobi_Ck(EMW *, f32 *);
void push_em_yobi(f32 *);
void SetVector(f32 *, f32, f32, f32);
int em_cancel_act_ck(EMW *, u8);
void em_dur_set(EMW *, int);
void em_mahi_dmg_timer_set(EMW *);
void em_sleep_dmg_timer_set(EMW *);
u8 Em_Dmg_Sys(EMW *, u8 *);
void em_cmd_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
int Code_Make(int, int, int, int);
void Eft20_set(f32, EMW *, int, int);
void Eft13_set_em(EMW *, int, int);
void em_rate_clear_g(EMW *);
int rate_add_g2(EMW *);
void Em_Mahi_Start(EMW *);
void Em_Mahi_End(EMW *);
void em_mahi_eff_set(EMW *, int);
void Em_Mode_Chg(EMW *, int, int);
void Quest_enemy_die(EMW *);
int Quest_enemy_revival_ck(EMW *);
void Quest_enemy_revival_set(EMW *);
void em_status_init(EMW *);
void em09_init(EMW *);
#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))
void pull_em_yobi(f32 *);
void Item_stolen(s32, u16, s16);
u16 Em_Calc_angY(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
f32 CalcDistanceXZ(f32 *, f32 *);
int em09_dir_calc(s32 *, s32 *, int);

void oikake_ck(EMW *em);
void em09_act_set();

void em09_local_init_005ACC40(EMW *em) {
    eft01_set((PLW *)em, 0);
}

void dummy_em_prog_005ACC50(void) {
}
