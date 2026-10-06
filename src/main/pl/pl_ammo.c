/* pl_ammo - gun attachment checks (SLPM_654.95 0x00154D00-0x00154D90): Pl_scope_ck, Pl_silencer_ck, Pl_barrel_ck. Whole file in pl_nm.c. */
/* Player code (f_pl.s, 0x134950..): working file; matched functions are moved
 * to plX.c, what is left here is near-match (not built). */
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];
u16 calc_vec_ang(f32, f32, f32, f32);
/* pl_demo000 matches since `q = (u8 *)game_w.x28 + (int)i` (linked as pl_demo.c) */
typedef struct { u8 _pad00[0x14]; s32 x14; } PL_QUEST_W_UNUSED;
EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);
#include "f_game.h"
extern u8 ot0[], ot1[];
void add_prim(void *, void *, int, int);
/* pl_move_sub (0x14C500, 2144 bytes): near-match, 469/536 insns differ only through delay slots.
 * The original never hoists the `daddu a0,s0,zero` argument copy into a branch delay slot (we do:
 * `bne; daddu a0,s0` vs original `bne; nop; jal; daddu a0,s0`), same effect as timer_calc_sub_pl.
 * Stack layout also differs (original: slide at sp+32, pos[3] at sp+48, gy at sp+60). Tried K&R
 * prototypes, switch form of the work8C2 test: no change. Prototypes below are K&R-style guesses. */
void em_ninshiki_ck();
void pl_timer_calc();
void pl_item_sel();
void pl_shell_sel();
void func_63A260();
u8 pl_status_ck();
void Pl_atck_adj_calc();
void Pl_def_adj_calc();
void pl_dm_value_sub();
void pl_move_sub_sub();
void Pl_pos_adj();
void Pl_status_set();
void hit_stop_calc();
void pl_chr_sub();
void hit_timer_calc();
void HitWallPlayer();
void GetFloorSlide();
void St_unique_adr_set();
int GetGroundHitStatusAreaPl();
void GetPlayerMaterialData();
void pl_light_ck();
f32 Get_dist_to_view();
void Pl_act_set();
void Pl_view_reset();
#include "flow.h"
extern u8 Equip_Bonus[0x630];
s16 Get_equip_value(u8 kind);
s16 Pl_item_num_ck(PLW *, int);
void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];
f32 *Stage_data_get(int stg);
void flmatGetTrans(f32 *, u8 *);
#include "hit.h"
u8 Get_hit_id(void);
typedef struct W24 { s32 a, b, c, d, e, f; } W24;
ST_ITEM *Stage_item_data_get(u8);
ST_UNIQ *Stage_unique_data_get(u8);
u16 *Stage_item_probability_get(int);
int Share_item_stack();
#include "flow.h"
#define ANG2RAD(a) (2.0f * (3.1415927f * (((360.0f * (f32)(a)) / 65536.0f) / 360.0f)))
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];
extern s16 *Pl_slash_tbl[6];
extern char lit_1805_0035B1B0[];
extern char lit_1806_0035B1D0[];
extern char lit_1807_0035B1F0[];
extern u8 chat_act_tbl_002F1860[0xD];
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
#include "flow.h"
typedef struct TUTO_REC {
    u16 id;     /* 0x00 */
    u16 _02;
    f32 x;      /* 0x04 */
    f32 y;      /* 0x08 */
    f32 z;      /* 0x0C */
    f32 r;      /* 0x10 */
} TUTO_REC;
s16 Pl_item_num_ck(PLW *, int);
void adx_se_set(PLW *, int);
void init_set_work();
void init_eft_work();
void init_shell_work();
void init_item_work();
void clr_set_work();
void clr_eft_work();
void clr_shell_work();
void clr_item_work();
void clr_used_heap(int, int);
s32 Pl_scope_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x40) != 0;
}
s32 Pl_silencer_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x10) != 0;
}
s32 Pl_barrel_ck(PLW *pl) {
    if (pl->work35F != 7) {
        if (pl) { /* permuter no-op: gives the original's extra branch stub */
            return 0;
        } else {
            return 0;
        }
    }
    return (pl->wpn_ammo & 0x20) != 0;
}
#include "flow.h"
void Pl_vital_calc_item(PLW *, int);
void Pl_max_vital_calc(PLW *, int);
void func_639DF0(PLW *, int);
void set01_set(int, int, int);
#include "flow.h"
void flmatGetTrans(f32 *, u8 *);
void RotMatVec(f32 *, f32 *, int);
f32 flSqrt(f32);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);
#include "flow.h"
void pl_body_make(PLW *pl, f32 *out, f32 radius);
void hit_cap_pk(void *, void *);
int hit_cap_cap3_m(void *, void *, f32 *);
extern f32 *D_63FC50[];
extern f32 *D_6103A0[];
void flvecNormalize(f32 *);
int hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb);
extern s16 *D_63FA10[];
extern s16 *D_610370[];
void body_ptr_ck2(PLW *, s16 **);
int hit_data_expand(PLW *, s16 *, f32 *, f32 *);
int hit_data_expand2(f32 *, s16 *, f32 *, f32 *);
int hit_cap_sphr_m(void *, f32 *, f32 *, f32);
s32 Pl_stg_ck_tw(PLW *, PLW *);
#include "flow.h"
extern u16 Psw[];
void Item_box_get_efct();
void Item_box_get_item(u16, u8);
void net_send_host(int, u8);
s16 Pl_item_num_ck(PLW *, int);
