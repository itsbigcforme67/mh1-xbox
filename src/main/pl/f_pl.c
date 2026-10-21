/* f_pl - the player file (SLPM_654.95 0x00134950-0x0014D1C8), one translation unit in the original address order:
   player work set-up (clr_pl_work, pl_init, pl_init_sub), the per-frame player loop and its timers, the pad command
   checks (basic_com_ck, job_special_com_ck, search_act_set, ...), the action handlers pl_mvNNN (moves), pl_atNNN
   (attacks), pl_dmNNN (damage), pl_demoNNN, pl_eggNN, pl_chatNN and their dispatchers (pl_normal, pl_attack, ...),
   and the move helpers (pl_move_sub, pl_turn_sub, pl_horm_sub).
   Built from the earlier per-run files pl01..pl48, plx03/04/09/12, pl_wall, pldmv, plegg, plegg2, pl_demo (agent F)
   by tools/mktu.py (agent B, 12 Oct 2026). As one file pl_at009 and pl_at012 match too: MWCC knows which
   registers the earlier static blend_set uses and keeps pl in a0 across the call.
   Differences to the run files: LOCAL functions are static again, the action handlers that the dispatchers call
   with (pl, 0) take the unused second argument, and pl_init passes (u8)i to pl_work_clr (the run had plf.h's u8
   prototype, here the K&R definition is the only declaration).
   Written or fixed in the TU: gun_adj_sub, sougun_adj_sub (blend_calc's s8 parameter, opt_propagation off),
   pl_turn_sub, pl_horm_sub, pl_egg05, em_ninshiki_ck, pl_at008 (blend_set's u16 parameters).
   6 functions are still kept as the original bytes (config/c_rawfuncs.txt; near-match C in pl_nm.c, which the PC
   build links weak): timer_calc_sub_pl, basic_com_ck, pl_mv021, pl_dm003, pl_dm008, pl_move_sub. */
/* plf.h declares these two with other types than their definitions here: keep its prototypes out */
#define pl_work_clr pl_work_clr_proto_unused
#define stick_pow_get stick_pow_get_proto_unused
#include "g_alias.h"
#ifdef MHG_PS2 /* G calls another function here (tools/check.py VERSION=mhg) */
#define ItemStockRequest func_0011CD20
#endif
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "plst.h"
#include "f_game.h"
#include "flow.h"
#include "hit.h"
#ifdef VERSION_MHG      /* G renumbered the items: pickaxes 112-114, bug nets 115-117 (G search_act_set 0x54BE90) */
#define PICKAXE0 112
#define BUGNET0 115
#else
#define PICKAXE0 0x83
#define BUGNET0 0x86
#endif
#ifdef VERSION_MHG
s32 sub_main_func_004CE8E0(PLW *pl, u8 ammo);   /* pl77.c */
#endif
#undef pl_work_clr
#undef stick_pow_get

f32 flSqrt(f32);
extern u8 Gun_data[26][0x14];
u16 calc_vec_ang(f32, f32, f32, f32);
typedef struct { u8 _pad00[0x14]; s32 x14; } PL_QUEST_W_UNUSED;
EMW *pull_enemy_work(void);
void enemy_mv(EMW *);
void get_joint_pos_em(EMW *, int, f32 *);
void PlComebackCameraRequest(void);
extern u8 ot0[], ot1[];
void add_prim(void *, void *, int, int);
void func_63A260();
void Pl_atck_adj_calc();
void Pl_def_adj_calc();
void Pl_pos_adj();
void Pl_status_set();
void pl_chr_sub();
void hit_timer_calc();
void HitWallPlayer();
void GetFloorSlide();
void St_unique_adr_set();
int GetGroundHitStatusAreaPl(void *, f32 *, f32 *, f32 *, f32 *);
void GetPlayerMaterialData();
void pl_light_ck();
f32 Get_dist_to_view();
void Pl_act_set();
void Pl_view_reset();
extern u8 Equip_Bonus[0x630];
s16 Get_equip_value(u8 kind);
s16 Pl_item_num_ck(PLW *, int);
void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];
f32 *Stage_data_get(int stg);
void flmatGetTrans(f32 *, u8 *);
u8 Get_hit_id(void);
typedef struct W24 { s32 a, b, c, d, e, f; } W24;
ST_ITEM *Stage_item_data_get(u8);
ST_UNIQ *Stage_unique_data_get(u8);
u16 *Stage_item_probability_get(int);
int Share_item_stack();
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
typedef struct TUTO_REC {
    u16 id;     /* 0x00 */
    u16 _02;
    f32 x;      /* 0x04 */
    f32 y;      /* 0x08 */
    f32 z;      /* 0x0C */
    f32 r;      /* 0x10 */
} TUTO_REC;
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
void Pl_vital_calc_item(PLW *, int);
void Pl_max_vital_calc(PLW *, int);
void func_639DF0(PLW *, int);
void set01_set(int, int, int);
void RotMatVec(f32 *, f32 *, int);
void pl_body_make(PLW *pl, f32 *out, f32 radius);
void hit_cap_pk(void *, void *);
#ifdef __MWERKS__
int hit_cap_cap3_m(void *, void *, f32 *);
#else
unsigned char hit_cap_cap3_m(void *, void *, f32 *);   /* the real return type (tools/check_kr_returns.py) */
#endif
extern f32 *D_63FC50[];
extern f32 *D_6103A0[];
void flvecNormalize(f32 *);
#ifdef __MWERKS__
int hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb);
#else
unsigned char hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb);   /* the real return type (tools/check_kr_returns.py) */
#endif
extern s16 *D_63FA10[];
extern s16 *D_610370[];
void body_ptr_ck2(PLW *, s16 **);
int hit_data_expand(PLW *, s16 *, f32 *, f32 *);
int hit_data_expand2(f32 *, s16 *, f32 *, f32 *);
#ifdef __MWERKS__
int hit_cap_sphr_m(void *, f32 *, f32 *, f32);
#else
unsigned char hit_cap_sphr_m(void *, f32 *, f32 *, f32);   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Pl_stg_ck_tw(PLW *, PLW *);
extern u16 Psw[];
void Item_box_get_efct();
void Item_box_get_item(u16, u8);
void net_send_host(int, u8);
s32 Ext_pick_point_ck(PLW *);
void SetVector(f32 *, f32, f32, f32);
typedef struct { u8 _pad00[0x14]; s32 x14; } PL_QUEST_W;
extern PL_QUEST_W quest_w;
extern u8 lit_6243[], lit_6244[], lit_6245[];
s8 Quest_remuneration_calc(void);
void Quest_forfeit_message(void);
void Quest_restart(void);
void die_bgm_set(void);
void PlayerDieCameraRequest(void);
int GetGroundHitStatusAreaPl();

/* prototypes of this file */
static void Pl_item_charge(PLW *pl);
void player_init0(PLW *pl);
void clr_pl_work(void);
void player_mv(void);
void pl_init(int mode);
static void pl_work_clr();
void Pl_ofs_set(PLW *pl, f32 *pos, int ang);
static int skill_hp_calc_00134FF0(PLW *pl);
void pl_init_sub(PLW *pl);
static void blend_set(PLW *pl, u16 a, u16 b);
static void blend_calc(PLW *pl, s8 d);
static void timer_calc_sub_em(PLW *pl);
void pl_timer_calc(PLW *pl);
static void pl_dm_value_sub(PLW *pl);
u16 item_sel_sub(PLW *pl, u16 sel, int dir);
static int item_blank_ck(PLW *pl);
static void pl_item_sel(PLW *pl);
static int shell_chg_ck(PLW *pl);
static int pl_shell_sel(PLW *pl);
static int pl_status_ck(PLW *pl);
void hit_stop_calc(PLW *pl);
static int stick_dir_set(PLW *pl, int no);
static u8 stick_pow_get(PLW *pl, int arg1);
static int em_ninshiki_ck2(PLW *pl);
static int pl_ride_ck(PLW *pl);
static int ex_kabe_ck(PLW *pl);
static void unique_act_set(PLW *pl);
static void job_special_com_ck(PLW *pl, u8 mode);
static void search_act_set(PLW *pl, u8 mode);
static s32 sit_com_ck(PLW *pl);
static void ex_atk_ck(PLW *pl, s32 arg1);
static void basic_atack_ck(PLW *pl);
static void guard_atk_ck(PLW *pl);
static s32 wall_act_ck(PLW *pl, s16 mode);
static s32 wall_vec_set(PLW *pl, s16 mode);
static u8 basic_kabe_ck(PLW *pl);
static void item_action_set(PLW *pl, u8 mode);
static s32 trade_get_ck_00139680(PLW *pl);
static void scope_add(PLW *pl, int arg1);
static void pl_mv000(PLW *pl, s32 unused_arg);
static void pl_mv001(PLW *pl, u32 arg1);
static void pl_mv004(PLW *pl, u32 arg1);
static void pl_mv006(PLW *pl, s32 arg1);
static void pl_mv008(PLW *pl, int arg1);
static void pl_mv013(PLW *pl, s32 unused_arg);
static void pl_mv014(PLW *pl, s32 arg1);
static void pl_mv017(PLW *pl, s32 arg1);
static void pl_mv022(PLW *pl, s32 unused_arg);
static void pl_mv024(PLW *pl, s32 arg1);
static void pl_mv025(PLW *pl, s32 unused_arg);
static void pl_mv028(PLW *pl, s32 arg1);
static void pl_mv030(PLW *pl, s32 unused_arg);
static void pl_mv031(PLW *pl, s32 arg1);
static void pl_mv032(PLW *pl, s32 arg1);
static void pl_mv033(PLW *pl, s32 arg1);
static void pl_mv037(PLW *pl, s32 unused_arg);
static void pl_mv038(PLW *pl, s32 unused_arg);
static void pl_mv039(PLW *pl, s32 unused_arg);
static void pl_mv040(PLW *pl, s32 unused_arg);
static void pl_mv041(PLW *pl, s32 unused_arg);
static void pl_mv042(PLW *pl, s32 unused_arg);
static void pl_mv043(PLW *pl, s32 unused_arg);
static void pl_mv044(PLW *pl, s32 unused_arg);
static void pl_mv046(PLW *pl, s32 unused_arg);
static void pl_mv047(PLW *pl, s32 unused_arg);
static void pl_mv049(PLW *pl, s32 arg1);
static void pl_mv051(PLW *pl, s32 unused_arg);
static void pl_mv052(PLW *pl, s32 unused_arg);
static void pl_mv053(PLW *pl, s32 unused_arg);
static void pl_mv054(PLW *pl, s32 unused_arg);
static void pl_mv072(PLW *pl, s32 unused_arg);
static void pl_mv055(PLW *pl, s32 unused_arg);
static void pl_mv057(PLW *pl, s32 unused_arg);
static void pl_mv058(PLW *pl, s32 unused_arg);
static void pl_mv059(PLW *pl, s32 unused_arg);
static void pl_mv060(PLW *pl, s32 unused_arg);
static void pl_mv061(PLW *pl, s32 unused_arg);
static void pl_mv062(PLW *pl, s32 unused_arg);
static void pl_mv063(PLW *pl, s32 unused_arg);
static void kabe_hosei(PLW *pl, int arg1);
static void kabe_com_ck(PLW *pl);
static void pl_mv064(PLW *pl, s32 arg1);
static void pl_mv065(PLW *pl, s32 arg1);
static void pl_mv068(PLW *pl, s32 unused_arg);
static void pick_set_sub(PLW *pl, u16 arg1);
static void pl_mv071(PLW *pl, u32 arg1);
static void pl_mv093(PLW *pl, s32 arg1);
static void pl_mv076(PLW *pl, s32 unused_arg);
static void pl_mv077(PLW *pl, s32 arg1);
static void fish_com_ck(PLW *pl);
static void pl_mv078(PLW *pl, s32 arg1);
static void pl_mv079(PLW *pl, s32 unused_arg);
static void pl_mv083(PLW *pl, s32 unused_arg);
#ifdef MHG_PS2
void game_func_0054B4B0(u16);
#endif
static void pl_mv084(PLW *pl, s32 arg1);
static void pl_mv085(PLW *pl, s32 unused_arg);
static void pl_mv086(PLW *pl, s32 unused_arg);
static void pl_mv087(PLW *pl, s32 unused_arg);
static void pl_mv088(PLW *pl, s32 arg1);
static void pl_mv113(PLW *pl, s32 unused_arg);
static void pl_mv089(PLW *pl, s32 unused_arg);
static void pl_mv091(PLW *pl, s32 unused_arg);
static void pl_mv094(PLW *pl, s32 arg1);
static void pl_mv095(PLW *pl, s32 arg1);
static void pl_mv097(PLW *pl, s32 unused_arg);
static void pl_mv101(PLW *pl, s32 arg1);
static void pl_mv103(PLW *pl, s32 unused_arg);
static void pl_mv104(PLW *pl, s32 unused_arg);
static void pl_mv105(PLW *pl, s32 unused_arg);
static void pl_mv106(PLW *pl, s32 unused_arg);
static void pl_mv107(PLW *pl, s32 arg1);
static void pl_mv111(PLW *pl, s32 unused_arg);
static void pl_mv112(PLW *pl, s32 unused_arg);
static void pl_normal(PLW *pl);
static void pl_at000(PLW *pl, s32 arg1);
static void pl_at001(PLW *pl, s32 arg1);
static void pl_at004(PLW *pl, u32 arg1);
static void pl_at006(PLW *pl, s32 unused_arg);
static void pl_at009(PLW *pl, s32 unused_arg);
static void pl_at012(PLW *pl, s32 unused_arg);
static void pl_at014(PLW *pl, s32 unused_arg);
static void pl_at020(PLW *pl, s32 unused_arg);
static void pl_at021(PLW *pl, s32 unused_arg);
static s32 we02_hit_sub(PLW *pl);
static void pl_at022(PLW *pl, s32 unused_arg);
static void pl_at023(PLW *pl, s32 arg1);
static s32 tame_pow_ck(PLW *pl);
static void tame_release(PLW *pl);
static void tame_com_ck(PLW *pl);
static void pl_at024(PLW *pl, s32 arg1);
static void pl_at025(PLW *pl, s32 unused_arg);
static void pl_at026(PLW *pl, s32 unused_arg);
static void pl_at028(PLW *pl, s32 unused_arg);
static void pl_at029(PLW *pl, s32 arg1);
static void pl_at030(PLW *pl, s32 unused_arg);
static void pl_at031(PLW *pl, s32 unused_arg);
static void pl_at032(PLW *pl, s32 unused_arg);
static void pl_at034(PLW *pl, s32 arg1);
static void pl_at035(PLW *pl, s32 arg1);
static s32 lance_kan_ck(PLW *pl);
static void pl_at036(PLW *pl, s32 arg1);
static void pl_at037(PLW *pl, s32 arg1);
static void pl_at040(PLW *pl, s32 unused_arg);
static void pl_at041(PLW *pl, s32 arg1);
static void pl_at042(PLW *pl, s32 unused_arg);
static void pl_at043(PLW *pl, s32 unused_arg);
static void pl_at045(PLW *pl, s32 arg1);
static s32 we04_hit_sub(PLW *pl);
static void pl_at048(PLW *pl, s32 arg1);
static void pl_at049(PLW *pl, s32 arg1);
static void pl_at050(PLW *pl, s32 unused_arg);
static void pl_at051(PLW *pl, s32 unused_arg);
static void pl_at052(PLW *pl, s32 unused_arg);
static void pl_at061(PLW *pl, s32 unused_arg);
static void pl_at065(PLW *pl, s32 unused_arg);
static void pl_at070(PLW *pl, s32 arg1);
static void pl_at074(PLW *pl, s32 unused_arg);
static void pl_at077(PLW *pl, s32 unused_arg);
static void pl_at078(PLW *pl, s32 arg1);
static void pl_attack(PLW *pl);
#ifdef VERSION_MHG     /* G only: the Dual Swords (job 6) */
static void ds_atk_set(PLW *pl, int dir, int combo);
static void pl_at083(PLW *pl, s32 arg1);
static void pl_at084(PLW *pl, s32 arg1);
static void pl_at085(PLW *pl, s32 arg1);
static void pl_at089(PLW *pl, s32 unused_arg);
static void pl_at091(PLW *pl, s32 arg1);
static void pl_at093(PLW *pl, s32 arg1);
static void pl_at094(PLW *pl, s32 arg1);
static void pl_at102(PLW *pl, s32 unused_arg);
void shell00_set2(PLW *pl, int arg, int blade);         /* G game 0x6690D0 (shell00.c) */
void Eft05_set(PLW *pl, int timer, int arg);
extern s16 ds_chr_tbl[8];                               /* G game 0x6784F0: {motion, blend} x 4 (pl_at091) */
void eft_ds_aura_set(PLW *pl, int arg);                 /* G game 0x5688E0 (eft_ds_g.c) */
#endif
static void pl_dm000(PLW *pl);
static void pl_dm001(PLW *pl, s32 arg1);
static void pl_dm002(PLW *pl);
static void pl_dm005(PLW *pl);
static void pl_dm006(PLW *pl);
static void pl_dm007(PLW *pl, s32 arg1);
static void pl_dm009(PLW *pl, s32 arg1);
static void pl_dm011(PLW *pl);
static void pl_dm015(PLW *pl, s32 unused_arg);
static void pl_dm016(PLW *pl, s32 arg1);
static void pl_dm017(PLW *pl, s32 unused_arg);
static void piyo_reset(PLW *pl);
static void pl_dm019(PLW *pl, s32 unused_arg);
static void pl_dm020(PLW *pl, s32 unused_arg);
static void pl_dm021(PLW *pl, s32 unused_arg);
static void pl_dm022(PLW *pl, s32 arg1);
static void pl_dm023(PLW *pl, s32 unused_arg);
static void pl_damage(PLW *pl);
static void pl_die000(PLW *pl, s32 arg1);
static void pl_die(PLW *pl);
static void pl_demo000(PLW *pl, s32 unused_arg);
static void pl_demo001(PLW *pl, s32 unused_arg);
static void pl_demo002(PLW *pl, s32 unused_arg);
static void pl_demo003(PLW *pl, s32 unused_arg);
static void pl_demo004(PLW *pl, s32 unused_arg);
static void pl_demo005(PLW *pl, s32 unused_arg);
static void pl_demo(PLW *pl);
static void egg_set(PLW *pl);
static void egg_com_ck(PLW *pl, int arg1);
static void pl_egg00(PLW *pl, s32 arg1);
static void pl_egg01(PLW *pl, s32 unused_arg);
static void pl_egg02(PLW *pl, s32 unused_arg);
static void pl_egg03(PLW *pl, s32 arg1);
static void pl_egg07(PLW *pl, s32 arg1);
static void pl_egg(PLW *pl);
static void pl_chat00(PLW *pl, s32 unused_arg);
static void pl_chat01(PLW *pl, s32 unused_arg);
static void pl_chat02(PLW *pl, s32 unused_arg);
static void pl_chat03(PLW *pl, s32 unused_arg);
static void pl_chat04(PLW *pl, s32 unused_arg);
static void pl_chat05(PLW *pl, s32 unused_arg);
static void pl_chat06(PLW *pl, s32 unused_arg);
static void pl_chat07(PLW *pl, s32 unused_arg);
static void pl_chat08(PLW *pl, s32 unused_arg);
static void pl_chat09(PLW *pl, s32 arg1);
static void pl_chat10(PLW *pl, s32 unused_arg);
static void pl_chat11(PLW *pl, s32 unused_arg);
static void pl_chat12(PLW *pl, s32 unused_arg);
static void pl_chat16(PLW *pl, s32 unused_arg);
static void pl_chat(PLW *pl);
void pl_move(void);
static void pl_move_sub_sub(PLW *pl);
void timer_calc_sub_pl(PLW *pl);
static void em_ninshiki_ck(PLW *pl);
static void basic_com_ck(PLW *pl);
static void gun_adj_sub(PLW *pl);
static void sougun_adj_sub(PLW *pl, u16 id);
static void pl_mv021(PLW *pl, s32 arg1);
static void pl_at008(PLW *pl, s32 arg1);
static void pl_dm003(PLW *pl, s32 arg1);
static void pl_dm008(PLW *pl);
static void pl_egg05(PLW *pl, s32 arg1);
void pl_move_sub(PLW *pl);
static void pl_turn_sub(PLW *pl);
static void pl_horm_sub(PLW *pl);

/* Player code (SLPM_654.95 0x00134950-0x00134A34): Pl_item_charge. */




#if !defined(G_PART)
static void Pl_item_charge(PLW *pl) {
    s16 i;
    s16 *p;
    s16 n;
    u8 c = pl->work8BF;
    if (c == 0) return;
    pl->work8BF = c - 1;
    for (i = 0; i < 20; i++) {
        pl->item[i].id = 0;
        pl->item[i].num = 0;
    }
    p = pl_supp_tbl[pl->work8BF + pl->kind * 6];
    n = *p;
    while (n != -1) {
        u16 a = p[1];
        p += 2;
        Pl_item_supply(pl, 0, a, n);
        n = *p;
    }
}
#endif

/* plx09 - SLPM_654.95 0x00134A40-0x00134C1C: player_init0 (start of a hunter: resets the action state and, for a preset hunter look
 * (work616 != 0), fills the six equipment part ids from equip_set, a random part where the table says -1, and the hair colour; then
 * creates the weapon and armour models and clears the hair sway). */





#if !defined(G_PART)
void player_init0(PLW *pl) {
    u32 i;
    s8 *eq;
    u8 *mx;
    if (game_w.pl_state[pl->id] == 0xFF && Pl_master_ck(pl) == 0) {
        pl->be_flag = 0;
        pl->x01 = 0;
        return;
    }
    pl->x10 = 0;
    pl->work01E = 0;
    pl->work300 = 2;
    pl->work350 = 0;
    pl->work351 = -1;
    if (pl->work616 != 0) {
        eq = &equip_set[pl->work616 * 6 + pl->work011 * 0x24];
        for (i = 0, mx = parts_max_tbl; i < 6; i++) {
            if (*eq < 0) {
                pl->work352[i] = (ran_suu(1) & 0xFFFF) % mx[pl->work011 * 6] + 1;
            } else if (i == 2 && pl->work011 != 0 && softdip_ck(0x50) != 0) {
                pl->work352[i] = 10;
            } else {
                pl->work352[i] = *eq;
            }
            mx++;
            eq++;
        }
        pl->work5FC = test_hair_col[pl->work616 + pl->work011 * 6] | 0xFF000000;
    }
    weapon_create_model(pl->work34C, pl->id, 0);
    armor_create_model(pl);
    yure_init(pl);
}
#endif

/* Player code (SLPM_654.95 0x00134C20-0x00134E68): clr_pl_work, player_mv, pl_init. */




#if !defined(G_PART) || G_PART == 37
void clr_pl_work(void) {
    int i;
    PLW *p = player_work;
    for (i = 0; i < GV(8, 10); i++) {
        memset(p, 0, 0xA00);
        p++;
    }
    memset(pl_cnt_w, 0, 0x10);
}
#endif

#if !defined(G_PART) || G_PART == 37
void player_mv(void) {
    switch (pl_cnt_w[0]) {
    case 0:
        pl_init(0);
    case 1:
        pl_move();
    case 2:
    case 3:
        break;
    }
}
#endif

#if !defined(G_PART)
void pl_init(int mode) {
    int i;
    PLW *pl;
    s16 m = mode;
    pl_cnt_w[1] = 0;
    pl_cnt_w[0] = 1;
    pl = player_work;
    for (i = 0; i < 8; i++) {
        if (pl->be_flag != 0) {
            pl->work568 = get_prim();
            if (pl->work568 != -1) {
                pl->work564 = get_prim_ptr(pl->work568);
                PS32(pl->work564, 0x18) = pl->id;
                PPTR(pl->work564, 0x14) = trans_pl_sub;
            }
            if (m != 0) {
                if (m == 1 && pl->x738 != 0) goto clr;
            } else {
clr:
                pl_work_clr(pl, (u8)i, pl_prog_tbl[0]);
            }
            if (m == 0) {
                if (Pl_master_ck(pl) == 1) {
                    pl->x73A = pl->stg;
                    pl->work73C = pl->pos[0];
                    pl->work740 = pl->pos[1];
                    pl->work744 = pl->pos[2];
                    pl->work570 = pl->ang[1];
                    net_send_pl(pl, 3, 0);
                }
                eft01_set(pl, 1);
                Eft06_set(1.0f, pl, 7, 0, 1);
#ifdef VERSION_MHG
                if (pl->kind == 6) {    /* G (sub_main 0x4F1048): the Dual Swords' demon-mode trails and aura, for the quest */
                    Eft05_set(pl, -1, 3);
                    eft_ds_aura_set(pl, 0);
                }
#endif
            }
        }
        pl++;
    }
    hit_chk_init();
}
#endif

#if !defined(G_PART) || G_PART == 38
static void pl_work_clr(pl, no, prog)
PLW *pl;
int no;
PLPROG *prog;
{
    pl->id = (u8)no;
    if (game_w.pl_state[pl->id] == 1 || Pl_master_ck(pl) == 1) {
        pl->x01 = 1;
    } else {
        pl->x01 = 0;
    }
    pl->prog = prog;
    pl_init_sub(pl);
    pl->chr_no0 = (u8)no + 1;
    pl->prog->init(pl);
    pl->prog->init2(pl);
}
#endif

/* Player code (SLPM_654.95 0x00134F30-0x00135078): Pl_ofs_set, skill_hp_calc_00134FF0. */




#if !defined(G_PART) || G_PART == 38
void Pl_ofs_set(PLW *pl, f32 *pos, int ang) {
    s32 rot[3];
    f32 ofs[4];
    f32 out[4];
    f32 mat[16];
    rot[1] = ang & 0xFFFF;
    rot[0] = 0;
    rot[2] = 0;
    cpRotMatrix(rot, mat);
    ofs[0] = start_ofs[pl->id * 2];
    ofs[1] = 0;
    ofs[2] = start_ofs[pl->id * 2 + 1];
    flvecApplyMat33(out, ofs, mat);
    pos[0] += out[0];
    pos[2] += out[2];
}
#endif

#if !defined(G_PART)
static int skill_hp_calc_00134FF0(PLW *pl) {
    if (Pl_Skill_ck(pl, 0x22) == 1) return 10;
    if (Pl_Skill_ck(pl, 0x23) == 1) return 20;
    if (Pl_Skill_ck(pl, 0x24) == 1) return 30;
    return 0;
}
#endif

/* Player code (SLPM_654.95 0x00135080-0x001359C0): pl_init_sub. */




#if !defined(G_PART)
void pl_init_sub(PLW *pl) {
    s32 temp_a2;
    u16 temp_a1;
    u16 temp_a2_2;
    u16 var_v0;
    u8 temp_a1_2;
    u8 temp_s1;
    u8 temp_v0;
    u8 temp_v1;

    PS32(pl, 4) = 0;
    pl->work2F4 = 0;
    pl->work2FC = 0;
    pl->scl[0] = 1.0f;
    pl->scl[1] = 1.0f;
    pl->scl[2] = 1.0f;
    temp_v1 = pl->x738;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        pl->pos[0] = stage_start_pos[game_w.stage][0];
        pl->pos[1] = stage_start_pos[game_w.stage][1];
        pl->pos[2] = stage_start_pos[game_w.stage][2];
        pl->ang[1] = stage_start_ang[game_w.stage];
        pl->ang[0] = 0;
        pl->ang[2] = 0;
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        Pl_ofs_set(pl, pl->pos, (u16)pl->ang[1]);
        pl->work792 = 0x64;
        pl->work882 = 0x12C;
        pl->work6A4 = 0;
        pl->work6A8 = 0;
        pl->work6A5 = 0;
        pl->work6A9 = 0;
        pl->work6A6 = 0;
        pl->work6AA = 0;
        if (Pl_master_ck(pl) == 1) {
            pl->work792 += game_w.x215;
            pl->work792 = pl->work792 + skill_hp_calc_00134FF0(pl);
            if (pl->work792 > 0x96) {
                pl->work792 = 0x96;
            }
            Pl_max_stamina_calc(pl, game_w.x216);
            pl->work6A4 = game_w.x213;
            pl->work6A8 = game_w.x214;
        }
        pl->vital = (s16) pl->work792;
        pl->vital_red = (s16) pl->work792;
        pl->work8C0 = 0x2A30;
        pl->stamina = (s16) pl->work882;
        temp_a1 = pl->wpn_kind;
        pl->work884 = (s16) ((Ken_data[temp_a1][3] * 0x32) + 0x96);
        pl->work87E = (s16) pl->work884;
        pl->work887 = Pl_slash_lv_ck(pl, temp_a1);
        pl->work8CC = 0;
        pl->work918 = 0;
        pl->work91A = 0;
        pl->work930 = 0;
        pl->work91C = 0;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->work934 = 0;
        pl->work88D = 0;
        pl->atk_rate = 1.0f;
        pl->work7DC = 1.0f;
        Pl_item_idx_calc(pl);
        pl->work91E = 0;
        pl->work7ED = 0;
        pl->work8C7 = 0;
        pl->flag12 = 0;
        pl->work01C = 0;
        pl->work01D = 0;
        pl->x56F = 0;
        parts_init(pl);
        if (game_w.pl_state[pl->id] == 1 || Pl_master_ck(pl) == 1) {
            pl->flag14 = 0;
            pl->flag15 = 0;
        } else {
            Pl_act_set(pl, 4, 5, 2);
        }
        pl->work90E = 0;
        pl->work88A = 0;
        pl->work4D4 = 1;
        pl->work91F = 0;
        temp_v0 = PU8(pl, 2);
        switch (temp_v0) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
        case 4:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0;
            break;
        case 2:                                     /* switch 2 */
            pl->work7D4 = 0;
            pl->work7D5 = 0x64;
            break;
        case 3:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0x64;
            break;
        case 1:                                     /* switch 2 */
        case 5:                                     /* switch 2 */
            pl->work7D4 = 0x64;
            pl->work7D5 = 0x64;
            break;
        default:                                    /* switch 2 */
            pl->work7D4 = 0x32;
            pl->work7D5 = 0x32;
            break;
        }
        Pl_item_charge(pl);
        pl->work88E = Pl_shell_set(pl, 0, 2);
        pl->work8BC = pl->item[pl->work88E].num;
        Shell_type_set(pl, 0);
        normal_char_set(pl, 0, 0);
        break;
    case 1:                                         /* switch 1 */
        pl->pos[0] = pl->work73C;
        pl->pos[1] = pl->work740;
        pl->pos[2] = pl->work744;
        pl->ang[1] = pl->work570;
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        pl->x738 = 0U;
        pl->x56F = 0;
        if ((game_w.pl_state[pl->id] == 1) || (Pl_master_ck(pl) == 1)) {
            pl->flag14 = 0;
            pl->flag15 = 0;
        } else {
            Pl_act_set(pl, 4, 5, 2);
        }
        pl->work90E = 0;
        temp_s1 = pl->work56B;
        if (temp_s1 & 0xF) {
            pl_to_normal_clr(pl);
            pl->work56B = (u8)(temp_s1 & 0xF0) | 5;
            Pl_act_set(pl, 5, 0, 0);
        } else {
            pl_to_normal_clr(pl);
            normal_char_set(pl, 0, 0);
        }
        break;
    case 2:                                         /* switch 1 */
        switch (game_w.stage) {   /* switch 3; irregular */
        case 0x4:                                   /* switch 3 */
            pl->pos[0] = 9150.0f;
            pl->pos[1] = 7.0f;
            pl->pos[2] = 6200.0f;
            pl->ang[1] = 0x2AAB;
            break;
        case 0xA:                                   /* switch 3 */
            pl->pos[0] = 6070.0f;
            pl->pos[1] = 17.0f;
            pl->pos[2] = 6990.0f;
            pl->ang[1] = 0xB778;
            break;
        case 0x14:                                  /* switch 3 */
            pl->pos[0] = 5910.0f;
            pl->pos[1] = 11.0f;
            pl->pos[2] = 5330.0f;
            pl->ang[1] = 0xF778;
            break;
        case 0x15:                                  /* switch 3 */
            pl->pos[0] = 11900.0f;
            pl->pos[1] = 40.0f;
            pl->pos[2] = 10100.0f;
            pl->ang[1] = 0xC001;
            break;
        case 0x32:                                  /* switch 3 */
            pl->pos[0] = 10650.0f;
            pl->pos[1] = 500.0f;
            pl->pos[2] = 5220.0f;
            pl->ang[1] = 0xF778;
            break;
        case 0x3D:                                  /* switch 3 */
            pl->pos[0] = 5180.0f;
            pl->pos[1] = 0;
            pl->pos[2] = 4900.0f;
            pl->ang[1] = 0xE38F;
            break;
        case 0x43:                                  /* switch 3 */
            pl->pos[0] = 8137.0f;
            pl->pos[1] = 0;
            pl->pos[2] = 11785.0f;
            pl->ang[1] = 0x6000;
            break;
        default:                                    /* switch 3 */
            pl->pos[0] = pl->work73C;
            pl->pos[1] = pl->work740;
            pl->pos[2] = pl->work744;
            pl->ang[1] = pl->work570;
            break;
        }
        Pl_ofs_set(pl, pl->pos, (u16)pl->ang[1]);
        cpRotMatrix(pl->ang, (f32 *)((u8 *)pl + 0x20));
        pl->x5AC = pl->pos[1];
        pl->ang_y = (s16) pl->ang[1];
        temp_a2_2 = pl->wpn_kind;
        temp_a1_2 = Ken_data[temp_a2_2][3];
        pl->work884 = (s16) ((temp_a1_2 * 0x32) + 0x96);
        pl->work792 = 0x64;
        if (Pl_master_ck(pl) == 1) {
            pl->work792 = pl->work792 + skill_hp_calc_00134FF0(pl);
            if (pl->work792 > 0x96) {
                pl->work792 = 0x96;
            }
        }
        pl->vital = (s16) pl->work792;
        pl->vital_red = (s16) pl->work792;
        pl->work882 = 0x12C;
        pl->work8C0 = 0x2A30;
        pl->stamina = (s16) pl->work882;
        pl->work8CC = 0;
        pl->work918 = 0;
        pl->work91A = 0;
        pl->work91C = 0;
        pl->work930 = 0;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->x7BA = 0;
        pl->x7B2 = 0;
        pl->x7C4 = 0;
        pl->work934 = 0;
        pl->x738 = 0U;
        pl->x56F = 0x14;
        PS8(pl, 1) = 0;
        pl->work6A4 = 0;
        pl->work6A8 = 0;
        pl->work6A5 = 0;
        pl->work6A9 = 0;
        pl->work6A6 = 0;
        pl->work6AA = 0;
        pl->work7ED = 0;
        Pl_act_set(pl, 4, 0, 0xC);
        break;
    }
    pl->work81C = 0;
    pl->ang_y = (s16) pl->ang[1];
    pl->work750 = 0;
    pl->work81A = 0;
    pl->work74A = 0;
    (*(void * *)((u8 *)((u8 *)pl + 0x41C))) = (void *) (((u8 *)pl + 0x3D0));
    pl->st = 0;
    pl->x56E = 0;
    pl->work3A0 = 1.0f;
    pl->work4E3 = 0;
    pl->flag604 = 0;
    pl->work608 = -1;
    pl->work60A = -1;
    pl->work3F4 = 0;
    pl->work56A = 0xFF;
    pl->work798 = 1.0f;
    pl->work8C5 = 1;
    pl->work8C2 = 0;
    pl->work8F3 = 0;
    pl->work8C3 = 0;
    pl->work8C4 = 0;
    pl->work908 = 0;
    pl->work917 = 0;
    pl->x8C6 = 0;
    pl->work936 = 0;
    pl->x43E = 0;
    pl->work39C = 0;
    pl->work40C = 0x1E;
    pl->chr_spd0 = 2.0f;
    pl->chr_spd1 = 2.0f;
    pl->x8C8 = 0;
    pl->work8ED = 0;
    pl->work8F0 = 0;
    pl->work8F2 = 0;
    pl->work88C = 0;
    pl->work612 = 0;
    pl->work615 = 0;
    pl->work760 = 0;
    pl->work886 = 0;
    pl->work8BE = 0;
    pl->work7D6 = 0;
    Pl_view_reset(pl, 1, 0xFF, -1);
    pl->work800 = 0;
    pl->work804 = 0;
    pl->work808 = 0;
    flvecCopy((f32 *)((u8 *)pl + 0x80C), (f32 *)((u8 *)pl + 0x800));
    pl->work818 = 0;
    pl->work720[0] = 0;
    pl->work724[0] = 0;
    (*(s16 *)((u8 *)pl + 0x72C)) = 0;
    pl->work720[1] = 0;
    pl->work724[1] = 0;
    (*(s16 *)((u8 *)pl + 0x72E)) = 0;
    pl->work720[2] = 0;
    pl->work724[2] = 0;
    (*(s16 *)((u8 *)pl + 0x730)) = 0;
    pl->work720[3] = 0;
    pl->work724[3] = 0;
    (*(s16 *)((u8 *)pl + 0x732)) = 0;
    pl->work8C9 = 0;
    frame_init(pl, pl->act_tm0, pl->blend0, 0);
    frame_init(pl, (*(u16 *)&pl->act_tm1), pl->blend1, 1);
    Pl_reg_calc(pl);
    Pl_light_init(pl);
    World_calc(pl);
}
#endif

/* Player code (SLPM_654.95 0x001359C0-0x00135B00): blend_set, blend_calc. */




#if !defined(G_PART) || G_PART == 1
static void blend_set(PLW *pl, u16 a, u16 b) {
    if (pl->work8C9 == 0) {
        pl->work720[0] = 0;
        pl->work720[1] = 0;
        return;
    }
    pl->work720[0] = 1;
    pl->work720[1] = 1;
    if (pl->work8C9 > 0) {
        if ((u16)a == 0x581) {
            pl->work724[0] = 0x580;
            pl->work720[0] = 0;
        } else {
            pl->work724[0] = a;
        }
        pl->work724[1] = (u16)a + 100;
        pl->work72C[0] = pl->work8C9;
        pl->work72C[1] = pl->work8C9;
    } else {
        if ((u16)b == 0x582) {
            pl->work724[0] = 0x580;
            pl->work720[0] = 0;
        } else {
            pl->work724[0] = b;
        }
        pl->work724[1] = (u16)b + 100;
        pl->work72C[0] = -pl->work8C9;
        pl->work72C[1] = -pl->work8C9;
    }
    if (pl->work72C[0] > 0x64) pl->work72C[0] = 0x64;
    if (pl->work72C[1] > 0x64) pl->work72C[1] = 0x64;
}
#endif

#if !defined(G_PART) || G_PART == 1
static void blend_calc(PLW *pl, s8 d) {
    pl->work8C9 += d;
    if (pl->work8C9 >= 100) pl->work8C9 = 100;
    if (pl->work8C9 < -99) pl->work8C9 = -100;
}
#endif

/* timer_calc_sub_pl: original bytes kept (near-match C in src/main/pl/pl_nm.c, linked weak by the PC build): build/raw/timer_calc_sub_pl.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm void timer_calc_sub_pl(PLW *pl)
{
#include "timer_calc_sub_pl.inc"
}
#endif
#endif

/* Player code (SLPM_654.95 0x001361B0-0x00136378): timer_calc_sub_em, pl_timer_calc. */




#if !defined(G_PART) || G_PART == 39
static void timer_calc_sub_em(PLW *pl) {
    s16 temp_v1;
    u8 temp_a1;

    if ((pl->work8C3 == 0) && (temp_a1 = pl->work56A, (temp_a1 != 0xFF)) && (temp_a1 != 0)) {
        temp_v1 = pl->work572;
        if (temp_v1 != 0) {
            pl->work572 = (s16) (temp_v1 - 1);
            return;
        }
        pl->work56A = 0U;
    }
}
#endif

#if !defined(G_PART) || G_PART == 39
void pl_timer_calc(PLW *pl) {
    s16 temp_v0_3;
    s16 temp_v0_5;
    u16 temp_v0;
    u16 temp_v0_4;
    u8 temp_v0_2;
    u8 temp_v0_6;

    if (pl->x40A == 0) {
        if (pl_flag_ck(pl, 0x400001) != 0) {
            pl->work398 = (u16) (pl->work398 + 2);
        } else {
            pl->work398 = 0U;
        }
        pl->work39C = (s32) (pl->work39C + 2);
        if (pl->work39C >= 0x186A0) {
            pl->work39C = 0x2710;
        }
        temp_v0 = pl->work40C;
        if (temp_v0 != 0) {
            pl->work40C = (u16) (temp_v0 - 1);
        }
        temp_v0_2 = pl->work440;
        if (temp_v0_2 != 0) {
            pl->work440 = (u8) (temp_v0_2 - 1);
        }
        temp_v0_3 = pl->x43E;
        if (temp_v0_3 != 0) {
            pl->x43E = (s16) (temp_v0_3 - 1);
        }
        temp_v0_4 = pl->work40E;
        if (temp_v0_4 != 0) {
            pl->work40E = (u16) (temp_v0_4 - 1);
        }
        temp_v0_5 = pl->work4E0;
        if (temp_v0_5 != 0) {
            pl->work4E0 = (s16) (temp_v0_5 - 1);
        }
        temp_v0_6 = pl->work7D6;
        if (temp_v0_6 != 0) {
            pl->work7D6 = (u8) (temp_v0_6 - 1);
        }
        if (pl->x10 == 0) {
            timer_calc_sub_pl(pl);
        } else {
            timer_calc_sub_em(pl);
        }
    }
    pl_flag_clr(pl, 0x800);
    if (pl_flag_ck(pl, 1) != 0) {
        pl_flag_set(pl, 0x2000);
        return;
    }
    pl_flag_clr(pl, 0x2000);
}
#endif

/* Player code (SLPM_654.95 0x00136380-0x001365F0): pl_dm_value_sub, per-frame damage-over-time of a player: poison
   counter (x7BA / work7BE), the near-monster flash effect and the stage environment damage (stage env 1 = hot/cold). */





#if !defined(G_PART)
static void pl_dm_value_sub(PLW *pl) {
    f32 pos[3];
    s16 e, t;
    if (pl->flag14 == 3) return;
    if (pl->x7BA > 0) {
        t = pl->work7BE - 1;
        pl->work7BE = t;
        if (t <= 0) {
            pl->x7BA = pl->x7BA - 1;
            pl->work7BE = 0x1E;
            if (Pl_master_ck(pl) == 1) {
                Pl_vital_calc(pl, -1);
                pos[1] = 20.0f;
                pos[0] = 0;
                pos[2] = 0;
                Eft06_set2(1.0f, pl, 3, 0x14, pos);
            }
            if (pl->vital <= 0 && Pl_master_ck(pl) == 1) {
                pl->vital_red = 0;
                if (pl->st != 2) {
                    func_639F20(pl);
                } else if (act_ck(pl, 0, 9) == 0) {
                    Pl_act_set(pl, 0, 9, 0);
                }
            }
        }
    }
    if (Pl_master_ck(pl) == 0 && (pl->work4D5 & 1) && (*(u16 *)&game_w.x1E % 30) == 0) {
        pos[1] = 20.0f;
        pos[0] = 0;
        pos[2] = 0;
        Eft06_set2(1.0f, pl, 3, 0x14, pos);
    }
    if (Pl_master_ck(pl) == 1) {
        e = Stage_env_ck(pl->stg);
        switch (e) {
        case 1:
            if (Pl_Skill_ck(pl, 0x2D) != 1 && pl->work918 == 0) {
                t = pl->work90C - 1;
                pl->work90C = t;
                if (t <= 0) {
                    pl->work90C = 0x5A;
                    Pl_vital_calc(pl, -1);
                    if (pl->vital <= 0) {
                        pl->vital_red = 0;
                        if (pl->st != 2) {
                            func_639F20(pl);
                            return;
                        }
                        Pl_act_set(pl, 0, 9, 0);
                    }
                }
            }
            break;
        case 2:
            break;
        }
    }
}
#endif

/* Player code (SLPM_654.95 0x001365F0-0x00136D38): item_sel_sub, item_blank_ck, pl_item_sel, shell_chg_ck, pl_shell_sel, pl_status_ck, hit_stop_calc, stick_dir_set. */




#if !defined(G_PART) || G_PART == 40
u16 item_sel_sub(PLW *pl, u16 sel, int dir) {
    s16 n;
    s16 i;
    s16 found;
    found = 0;
    if (!(dir & 0xFF)) {
        n = 0;
        i = sel;
        for (;;) {
            i++;
            if (i >= 20) i = 0;
            if (pl->item[i].id != 0 && Item_data[pl->item[i].id][1] == 1) {
                sel = i;
                found = 1;
                break;
            }
            n++;
            if (n >= 20) break;
        }
    } else {
        n = 0;
        i = sel;
        for (;;) {
            if (i == 0) i = 19;
            else i--;
            if (pl->item[i].id != 0 && Item_data[pl->item[i].id][1] == 1) {
                sel = i;
                found = 1;
                break;
            }
            n++;
            if (n >= 20) break;
        }
    }
    if (found == 0) sel = 0;
    return sel;
}
#endif

#if !defined(G_PART) || G_PART == 2
static int item_blank_ck(PLW *pl) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (pl->item[i].id != 0 && pl->item[i].num > 0 && Item_data[pl->item[i].id][1] == 1) return 1;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 2
static void pl_item_sel(PLW *pl) {
    u16 t;
    if (Pl_master_ck(pl) == 0) return;
    if (act_ck(pl, 2, 0x13) == 0 && (u32)(pl->flag14 - 3) > 1 && Game_clear_ck(1) != 1) {
        if (pl->sw.pad08[0] & 4) {
            if (pl->work88C == 0) se_req(7, 0x11, 0);
            pl->work88C = 1;
            if (item_blank_ck(pl) != 0) {
                t = pl->sw.pad08[2];
                if (t & 0x20) {
                    se_req(7, 0x12, 0);
                    pl->work888 = item_sel_sub(pl, pl->work888, 0);
                    pl->work8F2 |= 2;
                } else if (t & 0x200) {
                    se_req(7, 0x12, 0);
                    pl->work888 = item_sel_sub(pl, pl->work888, 1);
                    pl->work8F2 |= 4;
                }
            }
        } else if (pl->work88C != 0) {
            se_req(7, 0x14, 0);
            pl->work88C = 0;
        }
        return;
    }
    pl->work88C = 0;
}
#endif

#if !defined(G_PART) || G_PART == 2
static int shell_chg_ck(PLW *pl) {
    u8 k = pl->kind;
    if (k != 1 && k != 5 && pl->work88E == 0xFF) return 0;
    if (pl->sw.pad08[0] & 4) return 0;
    if (pl->x56E == 0 && pl->work56D != pl->ammo_type && pl->flag12 != 0) return 1;
    return 0;
}
#endif

#if !defined(G_PART)
static int pl_shell_sel(PLW *pl) {
    u16 t;
    u8 k;
    if (Pl_master_ck(pl) == 0) return 0;
    if (act_ck(pl, 2, 0x13) != 0) return 0;
    if (pl->flag14 == 3 || pl->flag14 == 4) return 0;
    if (Game_clear_ck(1) == 1) return 0;
    if (pl->work8BE != 0) return 0;
    k = pl->kind;
    if (k != 1 && k != 5) return 0;
    if (pl->x56E == 0) {
        pl->x56E = 1;
        pl->work56D = pl->ammo_type;
        pl->work8CE = pl->work8BC;
        pl->work8D2 = pl->work01D;
        pl->work8D1 = pl->work01C;
    }
    if (pl->sw.pad08[0] & 4) {
        t = pl->sw.pad08[2];
        if (t & 0x40) {
            if (pl->work88E == 0xFF) pl->work88E = Pl_shell_set(pl, 0, 3);
            else pl->work88E = Pl_shell_set(pl, pl->work88E, 1);
            Shell_type_set(pl, 0);
            if (pl->work88E != 0xFF) {
                pl->work8F2 |= 8;
                se_req(7, 0x12, 0);
            }
        } else if (t & 0x100) {
            if (pl->work88E == 0xFF) pl->work88E = Pl_shell_set(pl, 0, 2);
            else pl->work88E = Pl_shell_set(pl, pl->work88E, 0);
            Shell_type_set(pl, 0);
            if (pl->work88E != 0xFF) {
                pl->work8F2 |= 0x10;
                se_req(7, 0x12, 0);
            }
        }
    } else if (pl->x56E != 0) {
        pl->x56E = 0;
        if (pl->work56D != pl->ammo_type) {
            Shell_type_set(pl, 0);
        } else {
            pl->work8BC = pl->work8CE;
            pl->work01D = pl->work8D2;
            pl->work01C = pl->work8D1;
        }
    }
    if (pl->work56D == pl->ammo_type) {
        pl->work8BC = pl->work8CE;
        pl->work01D = pl->work8D2;
        pl->work01C = pl->work8D1;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 3
static int pl_status_ck(PLW *pl) {
    if (pl->x7B2 > 0) return 1;
    if (pl->x7C4 > 0) return 2;
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 39
void hit_stop_calc(PLW *pl) {
    u8 a = pl->work409;
    if (a != 0) {
        pl->x40A += a;
        pl->work409 = 0;
        return;
    }
    if (pl->x40A != 0) pl->x40A--;
}
#endif

#if !defined(G_PART) || G_PART == 3
static int stick_dir_set(PLW *pl, int no) {
    return (pl->sw.ang[no] + (Get_view_dir() & 0xFFFF)) & 0xFFFF;
}
#endif

/* plx04 - stick_pow_get (SLPM_654.95 0x00136D40-0x00136E6C): strength class of the stick (0 none, 1 half, 3 full, 5 = pressed with a
   button; guess from use) for a player. Returns u8 with a masked test `(r & 0xFF)` (the original masks only the test). plf.h declares it
   s32, so the name is renamed around the include (no header edit). Whole file in pl_nm.c. Jump table in main:rodata. */






#if !defined(G_PART) || G_PART == 3
static u8 stick_pow_get(PLW *pl, int arg1) { /* arg1 unused, callers pass 0 */
    u8 r = 0;
    u16 p;
    switch (pl->kind) {
    case 1:
    case 5:
        if (pl->flag12 != 0 && pl->pch_on == 1) return 0;
    default:
    case 0:
    case 2:
    case 3:
    case 4:
#ifdef VERSION_MHG
    case 6:
#endif
        pl->x8C8 = 0;
        p = pl->sw.pow[0];
        if (p >= 0x78) {
            r = 3;
        } else if (p >= 0x55) {
            r = 1;
        } else if (p >= 0x28) {
            r = 1;
        }
        if (pl->st == 0 && pl->flag12 == 0 && (pl->sw.now & 0x10) && (r & 0xFF) && (act_ck(pl, 0, 0x24) == 0 || pl->work760 == 0)) return 5;
        return r;
    }
}
#endif

/* em_ninshiki_ck: written from the asm (agent B): marks the monsters on the hunter's stage that noticed him (x88F has his bit) as seen (x7EE), aims at the last one. */
#if !defined(G_PART) || G_PART == 3
static void em_ninshiki_ck(PLW *pl) {
    s16 i;
    u8 bit;
    EMW *em;

    em = em_work;
    bit = 1 << pl->id;
    pl->work81F = pl->work81E;
    pl->work81E = 0;
    for (i = 0; i < 20; i++, em++) {
        if (em->stg == pl->stg && em->be_flag != 0 && em->x01 != 0 && em->x9EC != 0 && em->mode != 5
            && (em->x88F & bit) && em->x888 == 1) {
            em->x7EE |= bit;
            pl->work81E = 1;
            pl->x3B0 = em;
            pl->x3A8 = ((calc_vec_ang2(em->pos, pl->pos) & 0xFFFF) + 0x4000) & 0xFFFF;
            if (em->kind == 0x1B || em->kind == 0x1C || em->kind == 0x1F) {
                pl->work7EE = 1;
            }
        }
    }
}
#endif

/* Player code (SLPM_654.95 0x00136FB0-0x001371B0): em_ninshiki_ck2, pl_ride_ck, ex_kabe_ck. */




#if !defined(G_PART) || G_PART == 3
static int em_ninshiki_ck2(PLW *pl) {
    u8 a = pl->work81E;
    if (a != 0) {
        if (a != pl->work81F && pl->work7EE == 0) {
            pl->work7EE = 1;
            return 1;
        }
        return 0xFF;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 3
static int pl_ride_ck(PLW *pl) {
    f32 d, lim;
    u8 k;
    d = flvecCalcLength((f32 *)((u8 *)pl + 0x618));
    k = pl->flag14;
    if (k == 0 && pl->flag15 == 0xC) lim = 10.0f;
    else if (k == 0 && pl->flag15 == 0x16) lim = 4.0f;
    else lim = 7.0f;
    if (!(d < lim)) {
        if (k == 0 && pl->flag15 == 0x16) return 1;
        Pl_act_set(pl, 0, 0x16, 0);
        return 1;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 3
static int ex_kabe_ck(PLW *pl) {
    u16 a = pl->sw.an_now;
    if (a & 0x2000) {
        if (act_ck(pl, 0, 0x28) == 0) Pl_act_set(pl, 0, 0x28, 0xC);
        return 1;
    }
    if (a & 0x1000) {
        if (act_ck(pl, 0, 0x29) == 0) Pl_act_set(pl, 0, 0x29, 0xC);
        return 1;
    }
    if (pl->sw.trg & 0x40) {
        Pl_act_set(pl, 0, 0x30, 0);
        return 1;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 3
static void unique_act_set(PLW *pl) {
    f32 sp90[3];
    u16 sp9C;
    u8 sp9F;
    f32 sp80[4];
    f32 sp70[4];
    s32 sp60[4];
    f32 sp20[16];
    int r;

    r = St_unique_ck(pl, sp90, &sp9C, &sp9F) & 0xFFFF;
    switch (r) {
    default:
        if (Sansai_talk_ck(pl) & 0xFF) {
            Pl_act_set2(pl, 0, 0x5B, 0);
        }
        break;
    case 1:
        flvecCopy(&pl->work800, sp90);
        Pl_adj_calc(pl, 0x14);
        pl->ang_y = sp9C;
        Pl_act_set2(pl, 0, 0x4C, 0);
        return;
    case 3:
        if (pl->work88C == 0) {
            Pl_act_set2(pl, 0, 0x57, 0);
            return;
        }
        break;
    case 4:
        pl->ang_y = sp9C;
        Pl_act_set2(pl, 0, 0x5C, 0);
        return;
    case 16:
        flvecCopy(&pl->work800, sp90);
        sp60[0] = 0;
        sp60[2] = 0;
        sp60[1] = sp9C;
        cpRotMatrix(sp60, sp20);
        SetVector(sp70, 120.0f * bed_ofs[pl->id], 0, 0);
        flvecApplyMat33(sp80, sp70, sp20);
        pl->work800 = pl->work800 + sp80[0];
        pl->work804 = pl->work804 + sp80[1];
        pl->work808 = pl->work808 + sp80[2];
        pl->ang_y = sp9C;
        Pl_adj_calc(pl, 0x14);
        Pl_act_set2(pl, 0, 0x33, 0);
        return;
    case 25:
        if (game_w.x1B2 == 0) {
            pl->ang_y = (u16)((calc_vec_ang2(pl->pos, sp90) & 0xFFFF) - 0x4000);
            Pl_act_set2(pl, 0, 0x56, 0);
            return;
        }
        break;
    case 21:
        Share_item_conv(pl);
        break;
    }
}
#endif

#if !defined(G_PART)
static void job_special_com_ck(PLW *pl, u8 mode) {
    u8 t;

    if (pl->sw.now & 0x80) {
        if (mode == 0 || mode == 2) {
            switch (pl->kind) {
            case 0:
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                Pl_act_set2(pl, 2, 3, 0);
                return;
            case 4:
            case 3:
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                Pl_act_set2(pl, 2, 0x17, 0);
                return;
            case 2:
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                Pl_act_set2(pl, 1, 0x18, 0);
                return;
#ifdef VERSION_MHG
            case 6:     /* G (game 0x54BC30): R1 turns the Dual Swords' demon mode on */
                if (mode == 2) {
                    t = pl->work56B;
                    if (t & 0xF) {
                        pl->work56B = t & 0xF0;
                        func_549200(pl, 4);
                    }
                }
                if (pl->work93D == 0) {
                    Pl_act_set2(pl, 1, 0x53, 0);
                    return;
                }
                break;
#endif
            }
        } else {
            switch (pl->kind) {
            case 0:
            case 4:
            case 3:
                Pl_act_set2(pl, 2, 3, 0);
                return;
            case 2:
                Pl_act_set2(pl, 1, 0x18, 0);
                break;
#ifdef VERSION_MHG
            case 6:
                if (pl->work93D == 0) {
                    Pl_act_set2(pl, 1, 0x53, 0);
                    return;
                }
                Pl_act_set2(pl, 1, 0x62, 0);
                break;
#endif
            }
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 4
static void search_act_set(PLW *pl, u8 mode) {
    u16 sp3E;
    f32 sp30[3];
    u16 id;

    if (Sansai_talk_ck(pl) & 0xFF) {
        if (mode == 2) {
            goto block_3;
        }
    } else {
block_3:
        if ((St_pick_ck(pl, &sp3E, sp30) & 0xFFFF) != 0xFFFF) {
            switch (sp3E) {
            case 0:
                if (mode != 2) {
                    if (mode == 0) {
                        Pl_act_set(pl, 0, 0x47, 0);
                    } else {
                        Pl_act_set(pl, 0, 0x5D, 0);
                    }
                } else {
                    goto block_24;
                }
                break;
            case 3:
                if (mode == 2) {
                    switch (pl->item[pl->work888].id) {
                    case PICKAXE0:
                    case PICKAXE0 + 1:
                    case PICKAXE0 + 2:
                        pl->ang_y = (s16)((calc_vec_ang2(pl->pos, sp30) & 0xFFFF) - 0x4000);
                        pl->work88A = pl->item[pl->work888].id;
                        Pl_act_set(pl, 0, 0x51, 0);
                        break;
                    case BUGNET0:
                    case BUGNET0 + 1:
                    case BUGNET0 + 2:
                        se_req(7, 0x15, 0);
                        break;
                    }
                }
                break;
            case 4:
                if (mode == 2) {
                    id = pl->item[pl->work888].id;
                    switch (id) {
                    case BUGNET0:
                    case BUGNET0 + 1:
                    case BUGNET0 + 2:
                        pl->work88A = id;
                        Pl_act_set(pl, 0, 0x52, 0);
                        break;
                    case PICKAXE0:
                    case PICKAXE0 + 1:
                    case PICKAXE0 + 2:
                        se_req(7, 0x15, 0);
                        break;
                    }
                }
                break;
            default:
                goto block_24;
            }
        } else {
block_24:
            switch (pl->item[pl->work888].id) {
            case PICKAXE0:
            case PICKAXE0 + 1:
            case PICKAXE0 + 2:
            case BUGNET0:
            case BUGNET0 + 1:
            case BUGNET0 + 2:
                if (mode == 2) {
                    se_req(7, 0x15, 0);
                }
                break;
            }
        }
        if (mode != 2 && (Ext_pick_point_ck(pl) & 0xFFFF) != 0xFFFF) {
            if (mode == 0) {
                Pl_act_set(pl, 0, 0x4A, 0);
                return;
            }
            Pl_act_set(pl, 0, 0x60, 0);
        }
    }
}
#endif

#if !defined(G_PART)
static s32 sit_com_ck(PLW *pl) {
    u8 temp_v1;

    if ((Pl_master_ck(pl) == 0) && (Online_ck() == 1)) {
        return 0;
    }
    switch (game_w.x0D5) {
    case 4:
        if (pl->work88D == 0) {
            Pl_act_set2(pl, 4, 2, 0);
            return 0;
        }
        goto def;
    case 6:
    case 5:
        Pl_act_set2(pl, 4, 3, 0);
        return 0;
    case 7:
    case 8:
        Pl_act_set2(pl, 4, 5, 0);
        return 0;
    default:
    def:
        pl->work8F0 = 1;
        temp_v1 = stick_pow_get(pl, 0);
        switch (temp_v1) {
        case 1:
        case 2:
            pl->ang_y = stick_dir_set(pl, 0);
            if (act_ck(pl, 0, 0x3B) == 0) {
                if (act_ck(pl, 0, 0x3D) != 0) {
                    Pl_act_set2(pl, 0, 0x3C, 0);
                } else {
                    Pl_act_set(pl, 0, 0x3B, 0);
                }
            }
            break;
        case 3:
            pl->ang_y = stick_dir_set(pl, 0);
            if (act_ck(pl, 0, 0x3D) == 0) {
                Pl_act_set(pl, 0, 0x3D, 0);
            }
            break;
        default:
        case 0:
            if (act_ck(pl, 0, 0x3B) == 0) {
                if (act_ck(pl, 0, 0x3D) != 0) {
                    goto block_31;
                }
            } else {
block_31:
                if (pl->work08 == 0) {
                    Pl_act_set2(pl, 0, 0x3C, 0);
                }
            }
            break;
        }
        if ((pl->sw.trg & 0x40) && (pl->x714 == 0)) {
            Pl_act_set2(pl, 0, 0x39, 0);
        }
        if (Game_clear_ck(1) == 1) {
            return 0;
        }
        if (pl->work8C4 != 0) {
            Pl_chat_act_set(pl);
        }
        if (pl->sw.an_trg & 0x3C) {
            Pl_act_set(pl, 0, 4, 0);
        }
        job_special_com_ck(pl, 0);
        if ((pl->sw.trg & 0x200) && (pl->work886 == 0)) {
            search_act_set(pl, 2);
            item_action_set(pl, 0);
            pl->work8F2 = (u8) (pl->work8F2 | 1);
        }
        if (pl->sw.trg & 0x20) {
            if (trade_get_ck_00139680(pl) == 0) {
                unique_act_set(pl);
                search_act_set(pl, 1);
            }
        } else if (pl->work908 != 0) {
            pl->work90B = 1;
            Pl_act_set2(pl, 0, 0x67, 0x10);
        }
        return 0;
    }
}
#endif

#if !defined(G_PART)
static void ex_atk_ck(PLW *pl, s32 arg1) {
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1_2;
    u8 temp_v1;

    if (((Pl_master_ck(pl) != 0) || (Online_ck() != 1)) && (Game_clear_ck(1) != 1) && (pl->work88C == 0)) {
        temp_v1 = pl->kind;
        switch (temp_v1) {
        case 0:
            if (pl->sw.an_trg & 0x3C) {
                temp_a0 = (pl->sw.ang[1] + 0x4000) & 0xFFFF;
                if ((temp_a0 >= 0) && (temp_a0 < 0x5555) && (arg1 != 1)) {
                    if (arg1 == 0) {
                        if (pl->sw.an_now & 0x400) {
                            Pl_act_set(pl, 1, 0x3F, 4);
                        } else {
                            Pl_act_set(pl, 1, 0xD, 4);
                        }
                    } else if (pl->sw.an_now & 0x400) {
                        Pl_act_set(pl, 1, 0x43, 4);
                    } else {
                        Pl_act_set(pl, 1, 0xF, 4);
                    }
                } else if ((temp_a0 >= 0x5555) && (temp_a0 < 0xAAAB) && (arg1 != 0)) {
                    if (arg1 == 1) {
                        if (pl->sw.an_now & 0x400) {
                            Pl_act_set(pl, 1, 0x44, 4);
                        } else {
                            Pl_act_set(pl, 1, 0x10, 4);
                        }
                    } else if (pl->sw.an_now & 0x800) {
                        Pl_act_set(pl, 1, 0x42, 4);
                    } else {
                        Pl_act_set(pl, 1, 0x11, 4);
                    }
                } else if ((temp_a0 >= 0xAAAB) && (arg1 != 2)) {
                    if (arg1 == 0) {
                        if (pl->sw.an_now & 0x800) {
                            Pl_act_set(pl, 1, 0x40, 4);
                        } else {
                            Pl_act_set(pl, 1, 0x12, 4);
                        }
                    } else if (pl->sw.an_now & 0x800) {
                        Pl_act_set(pl, 1, 0x45, 4);
                    } else {
                        Pl_act_set(pl, 1, 0x13, 4);
                    }
                }
            }
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                if (pl->sw.an_now & 0x3C00) {
                    temp_v0 = (pl->sw.ang[0] + 0x4000) & 0xFFFF;
                    if (temp_v0 < 0x5C72) {
                        Pl_act_set(pl, 0, 0x31, 4);
                        return;
                    }
                    if (temp_v0 < 0xA38E) {
                        Pl_act_set(pl, 0, 0x1C, 4);
                        return;
                    }
                    Pl_act_set(pl, 0, 0x32, 4);
                    return;
                }
                Pl_act_set(pl, 0, 0x1C, 4);
                return;
            }
            break;
#ifdef VERSION_MHG
        case 4:
        case 6:     /* G (game 0x54C9xx): the Dual Swords evade as the sword and shield; R1 turns the demon mode on / off */
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                if (pl->sw.an_now & 0x3C00) {
                    temp_v0_2 = (pl->sw.ang[0] + 0x4000) & 0xFFFF;
                    if (temp_v0_2 < 0x5C72) {
                        Pl_act_set(pl, 0, 0x31, 4);
                    } else if (temp_v0_2 < 0xA38E) {
                        Pl_act_set(pl, 0, 0x1C, 4);
                    } else {
                        Pl_act_set(pl, 0, 0x32, 4);
                    }
                } else {
                    Pl_act_set(pl, 0, 0x1C, 4);
                }
            }
            if (pl->kind == 6 && (pl->sw.trg & 0x80)) {
                if (pl->work93D == 0) {
                    Pl_act_set2(pl, 1, 0x63, 0);
                    return;
                }
                Pl_act_set2(pl, 1, 0x62, 0);
                return;
            }
            break;
#else
        case 4:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                if (pl->sw.an_now & 0x3C00) {
                    temp_v0_2 = (pl->sw.ang[0] + 0x4000) & 0xFFFF;
                    if (temp_v0_2 < 0x5C72) {
                        Pl_act_set(pl, 0, 0x31, 4);
                        return;
                    }
                    if (temp_v0_2 < 0xA38E) {
                        Pl_act_set(pl, 0, 0x1C, 4);
                        return;
                    }
                    Pl_act_set(pl, 0, 0x32, 4);
                    return;
                }
                Pl_act_set(pl, 0, 0x1C, 4);
                return;
            }
            break;
#endif
        case 2:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                if (pl->sw.an_now & 0x3C00) {
                    temp_v0_3 = (pl->sw.ang[0] + 0x4000) & 0xFFFF;
                    if (temp_v0_3 < 0x5C72) {
                        Pl_act_set(pl, 0, 0x31, 4);
                    } else if (temp_v0_3 < 0xA38E) {
                        Pl_act_set(pl, 0, 0x1C, 4);
                    } else {
                        Pl_act_set(pl, 0, 0x32, 4);
                    }
                } else {
                    Pl_act_set(pl, 0, 0x1C, 4);
                }
            }
            if ((pl->sw.an_trg & 0x3C) && (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC) && (arg1 == 1)) {
                Pl_act_set(pl, 1, 0x4B, 4);
                return;
            }
            break;
        case 3:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                if (pl->sw.an_now & 0x3C00) {
                    temp_v1_2 = (pl->sw.ang[0] + 0x2000) & 0xFFFF;
                    if (temp_v1_2 < 0x6000) {
                        Pl_act_set(pl, 0, 0x6C, 4);
                    } else if ((temp_v1_2 >= 0x6000) && (temp_v1_2 < 0xC001)) {
                        Pl_act_set(pl, 0, 0x6B, 4);
                    } else {
                        Pl_act_set(pl, 0, 0x6E, 4);
                    }
                } else {
                    Pl_act_set(pl, 0, 0x6D, 4);
                }
            }
            if (pl->sw.trg & 1) {
                Pl_act_set(pl, 1, 0x22, 0xC);
            }
            break;
        case 1:
        case 5:
            break;
        }
    }
}
#endif

/* G only (game 0x0054CBA0): the Dual Swords' next attack from the stick (dir 0 forward, 1 sideways, 2 back, 3 the
   combo's last sideways step) and the demon mode; combo = called from a running combo (the follow-up moves). */
#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 49)
static void ds_atk_set(PLW *pl, int dir, int combo) {
    switch (dir) {
    case 0:
        if (pl->work93D != 0) {
            Pl_act_set(pl, 1, 0x54, 4);
        } else if (combo == 0) {
            Pl_act_set(pl, 1, 0x5B, 4);
        } else {
            Pl_act_set(pl, 1, 0x64, 4);
        }
        break;
    case 1:
        if (pl->work93D != 0) {
            Pl_act_set(pl, 1, 0x5A, 4);
        } else if (combo == 0) {
            Pl_act_set(pl, 1, 0x5C, 4);
        } else {
            Pl_act_set(pl, 1, 0x65, 4);
        }
        break;
    case 2:
    case 3:
        if (pl->work93D != 0) {
            Pl_act_set(pl, 1, 0x59, 4);
        } else if (dir == 2) {
            Pl_act_set(pl, 1, 0x5D, 4);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void basic_atack_ck(PLW *pl) {
    s32 temp_v0;
    s32 temp_v0_2;
    u8 temp_a0;
    u8 temp_v1;

    if (((Pl_master_ck(pl) != 0) || (Online_ck() != 1)) && (pl->work88C == 0) && (Game_clear_ck(1) != 1)) {
        temp_v1 = pl->kind;
        switch (temp_v1) {
        case 4:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            if (pl->sw.an_trg & 0x3C) {
                temp_v0 = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
                if (temp_v0 < 0x4001) {
                    Pl_act_set(pl, 1, 0x33, 4);
                    return;
                }
                if (temp_v0 < 0x9556) {
                    Pl_act_set(pl, 1, 0x30, 4);
                    return;
                }
                if (temp_v0 < 0xD556) {
                    Pl_act_set(pl, 1, 0x33, 4);
                    return;
                }
                Pl_act_set(pl, 1, 0x31, 4);
                return;
            }
            break;
#ifdef VERSION_MHG
        case 6:     /* G: the Dual Swords (game 0x54CDD0) */
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            if (pl->sw.an_trg & 0x3C) {
                temp_v0 = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
                if (temp_v0 < 0x4001) {
                    ds_atk_set(pl, 0, 0);
                    return;
                }
                if (temp_v0 < 0x9556) {
                    Pl_act_set(pl, 1, 0x55, 4);
                    return;
                }
                if (temp_v0 < 0xD556) {
                    ds_atk_set(pl, 1, 0);
                    return;
                }
                ds_atk_set(pl, 2, 0);
                return;
            }
            break;
#endif
        case 0:
        default:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            if (pl->sw.trg & 1) {
                Pl_act_set(pl, 1, 0xE, 4);
            }
            if (pl->sw.an_trg & 0x3C) {
                temp_v0_2 = (pl->sw.ang[1] + 0x4000) & 0xFFFF;
                if (temp_v0_2 < 0x5555) {
                    Pl_act_set(pl, 1, 1, 4);
                    return;
                }
                if (temp_v0_2 < 0xAAAC) {
                    Pl_act_set(pl, 1, 4, 4);
                    return;
                }
                Pl_act_set(pl, 1, 0, 4);
                return;
            }
            break;
        case 3:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x6D, 4);
            }
            if (pl->sw.trg & 1) {
                Pl_act_set(pl, 1, 0x22, 0xC);
            }
            if (pl->sw.an_trg & 0x3C) {
                if (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC) {
                    Pl_act_set(pl, 1, 0x24, 0xC);
                    return;
                }
                Pl_act_set(pl, 1, 0x2D, 0xC);
                return;
            }
            break;
        case 2:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            if (pl->sw.an_trg & 0x3C) {
                if (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC) {
                    Pl_act_set(pl, 1, 0x17, 4);
                    return;
                }
                Pl_act_set(pl, 1, 0x16, 4);
                return;
            }
            break;
        case 1:
        case 5:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            temp_a0 = pl->work01C;
            if (temp_a0 != 0) {
                if (((s32) temp_a0 < (s32) pl->work01D) && ((s32) temp_a0 < pl->work8BC)) {
                    goto block_57;
                }
            } else {
block_57:
                if ((pl->sw.an_trg & 0x10) && (pl->work8BC > 0)) {
                    Pl_act_set(pl, 1, 9, 5);
                }
            }
            if ((pl->sw.an_trg & 0x20) && (pl->x763 == 0) && (pl->pch_on == 0)) {
                Pl_act_set(pl, 1, 0x3D, 4);
            }
            if (pl->work01C == 0) {
                if (pl->sw.trg & 1) {
                    Pl_act_set(pl, 1, 0xC, 5);
                    return;
                }
            } else {
                pl->work8ED = 1;
                if (pl->sw.trg & 1) {
#ifdef VERSION_MHG
                    switch (sub_main_func_004CE8E0(pl, pl->ammo_type)) {   /* G game 0x54D25C */
#else
                    switch (Shell_data[pl->ammo_type].se_kind) {
#endif
                    case 0:
                        Pl_act_set(pl, 1, 2, 5);
                        return;
                    default:
                    case 1:
                        Pl_act_set(pl, 1, 8, 5);
                        return;
                    case 2:
                        Pl_act_set(pl, 1, 0xA, 5);
                        break;
                    }
                }
            }
            break;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 5
static void guard_atk_ck(PLW *pl) {
    u8 temp_a0;

    if (((Pl_master_ck(pl) != 0) || (Online_ck() != 1)) && (Game_clear_ck(1) != 1)) {
        temp_a0 = pl->kind;
        switch (temp_a0) {
        case 3:
            if ((stick_pow_get(pl, 0) & 0xFF) && (pl->char0 != 0x3F3)) {
                Pl_act_set(pl, 0, 7, 4);
            }
            if (pl->sw.trg & 1) {
                Pl_act_set(pl, 1, 0x22, 0xC);
            }
            if (pl->sw.an_trg & 0x3C) {
                Pl_act_set(pl, 1, 0x28, 0xC);
                return;
            }
            break;
        case 4:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            if (pl->sw.an_trg & 0x20) {
                Pl_act_set(pl, 1, 0x34, 4);
                return;
            }
            break;
        case 0:
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x1C, 4);
            }
            break;
        }
    }
}
#endif

/* near-match: one commutated addu (original: (base+id*252)+off, ours off+(base+id*252)) */
#if !defined(G_PART) || G_PART == 5
static s32 wall_act_ck(PLW *pl, s16 mode) {
    s16 i;
    int off;
    u16 flag;
    u16 ang;
    PL_WALL *w;

    if (!(Pl_stg_ck(pl) & 0xFF)) {
        return 0;
    }
    if (pl->work74C != 0) {
        for (i = 0, off = 0; i < 20; i++, off += 12) {
            w = pl_wall_mat[pl->id];
            w = (PL_WALL *)((u8 *)w + off);
            flag = w->flag;
            if (flag == 0) {
                break;
            }
            ang = calc_vec_ang(w->vec[0], w->vec[2], 0.0f, 0.0f);
            if ((flag & 2) && (pl->work74C & 0xE0000007)) {
                if (mode == 0) {
                    pl->ang[1] = (u16)(ang - 0x4000);
                } else {
                    pl->ang[1] = (u16)(ang + 0x4000);
                }
                pl->ang_y = pl->ang[1];
                return 1;
            }
            if (flag != 0) {
                if (mode == 1) {
                    pl->ang[1] = (u16)(ang + 0x4000);
                    pl->ang_y = pl->ang[1];
                }
                return 2;
            }
        }
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 5
static s32 wall_vec_set(PLW *pl, s16 mode) {
    s16 i;
    int off;
    u16 flag;
    u16 ang;
    PL_WALL *w;

    if (!(Pl_stg_ck(pl) & 0xFF)) {
        return 0;
    }
    if (pl->work74C & (mode == 0 ? 0xE0000007 : 0x3E000)) {
        for (i = 0, off = 0; i < 20; i++, off += 12) {
            w = pl_wall_mat[pl->id];
            w = (PL_WALL *)((u8 *)w + off);
            flag = w->flag;
            if (flag == 0) {
                break;
            }
            ang = calc_vec_ang(w->vec[0], w->vec[2], 0.0f, 0.0f);
            if ((flag & 2) && mode == 0) {
                pl->ang[1] = (u16)(ang - 0x4000);
                pl->ang_y = pl->ang[1];
                return 1;
            }
            if (mode == 1) {
                pl->ang[1] = (u16)(ang + 0x4000);
                pl->ang_y = pl->ang[1];
                return 2;
            }
        }
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 5
static u8 basic_kabe_ck(PLW *pl) {
    int sp3C;
    u8 r;

    r = 0;
    switch ((s8)wall_act_ck(pl, 0)) {
    case 0:
        break;
    case 1:
        Pl_act_set(pl, 0, 0x2B, 0xC);
        r = 1;
        break;
    }
    if (front_land_ck(55.0f, 60.0f, 120.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1E, 0xC);
        r = 1;
    } else if (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1B, 0xC);
        r = 1;
    } else if (front_land_ck(55.0f, 210.0f, 90.0f, pl, &sp3C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x15, 0xC);
        r = 1;
    }
    return r;
}
#endif

#if !defined(G_PART) && defined(VERSION_MHG)
/* G's item_action_set (game 0x54D8C0), written from G's asm for the PC (not byte-checked): G renumbered the items
   (G ids in the comments' names), added G-only items (ration, first-aid med, mini whetstone, portable spit,
   tranquilizer ball, ballista ammo, G well-done steak / G barrel bombs) and the tools (pickaxes 0x70-0x72,
   bug nets 0x73-0x75) leave without a sound (search_act_set handles them). The acts are MH1's. */
static void item_action_fail(PLW *pl, u8 mode) {
    se_req(7, 0x15, 0);
    if (mode == 1) {
        pl_to_normal(pl, 0, 4, 0);
    }
}

static void item_action_set(PLW *pl, u8 mode) {
    u8 sp3F;
    u16 sp3C;
    f32 sp30[3];
    u16 id;

    if ((s16)Get_Active_itemnum() <= 0) {
        item_action_fail(pl, mode);
        return;
    }
    id = pl->item[pl->work888].id;
    switch (id) {
    case 0x70: case 0x71: case 0x72:            /* pickaxes */
    case 0x73: case 0x74: case 0x75:            /* bug nets */
        return;
    case 0x37:                                  /* dung bomb */
    case 0x38:                                  /* smoke bomb */
        pl->work88A = id;
        if (pl_flag_ck(pl, 0x02000600) == 0) {
            Pl_act_set2(pl, 0, 0xB, 0xC);
        } else {
            Pl_act_set2(pl, 0, 0xE, 0xC);
        }
        return;
    case 0x39:                                  /* poison smoke bomb (G): always the drop act */
        pl->work88A = id;
        Pl_act_set2(pl, 0, 0xB, 0xC);
        return;
    case 0x3A:                                  /* farcaster */
        if (Modori_dama_ck() == 1) {
            pl->work88A = id;
            Pl_act_set2(pl, 0, 0x72, 0xC);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x2E: case 0x2F: case 0x30: case 0x31: /* throwing knives (G) */
    case 0x1FF:                                 /* tranquilizer ball (G) */
    case 0x33:                                  /* paintball */
    case 0x34:                                  /* flash bomb */
    case 0x35:                                  /* sonic bomb */
    case 0x99:                                  /* stone (G) */
        pl->work88A = id;
        Pl_act_set2(pl, 0, 0xE, 0xC);
        return;
    case 0x25:                                  /* raw meat */
    case 0x2A: case 0x2B: case 0x2C:            /* poison / paralysis / sleep meat */
    case 0x42: case 0x43:                       /* small barrel bomb, small barrel bomb+ */
        if (Niku_ok_ck() == 1 || id == 0x42 || id == 0x43) {
            if (pl->st == 0) {
                pl->work88A = id;
                Pl_act_set2(pl, 0, 0x19, 0xC);
            } else if (pl->st == 1) {
                pl->work88A = id;
                Pl_act_set2(pl, 0, 0x59, 0xC);
            }
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x44: case 0x45:                       /* large barrel bomb, large barrel bomb+ */
        if (Taru_ok_ck() == 1) {
            pl->work88A = id;
            Pl_act_set2(pl, 0, 0x6F, 0xC);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x3D:                                  /* pitfall trap */
        if ((s16)Pl_trap_use_ck(pl) >= 0) {
            pl->work88A = id;
            Pl_act_set2(pl, 0, 0x2E, 0xC);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x68: case 0x69: case 0x6A: case 0x6B: /* fishing baits */
    case 0x6C: case 0x6D: case 0x6E: case 0x6F:
        if ((St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF) == 2) {
            pl->work88A = id;
            pl->ang_y = sp3C;
            Pl_act_set2(pl, 0, 0x4F, 0);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x200:                                 /* ballista ammo (G) */
        if ((St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF) == 0x11) {
            pl->work800 = sp30[0];
            pl->work804 = sp30[1];
            pl->work808 = sp30[2];
            pl->ang_y = sp3C;
            Pl_adj_calc(pl, 0x14);
            pl->cnt39A = (s16)sp3F;
            Pl_act_set2(pl, 0, 0x36, 0x20);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
    case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
    case 0x1A: case 0x1B: case 0x1C: case 0x20:
    case 0x81: case 0x82: case 0x8F: case 0x91: case 0x93: case 0x94: case 0xAB: case 0xB9:
    case 0x1FB: case 0x1FC:                     /* potions, drinks, pills, herbs, seeds, fish, ration, first-aid med */
        pl->work88A = id;
        Pl_act_set2(pl, 0, 0x58, 0);
        return;
    case 0x76: case 0x77: case 0x1FD:           /* BBQ spits (G sets work88A too) */
        if (Nikuyaki_ck(pl) == 1) {
            pl->work88A = id;
            Pl_act_set(pl, 0, 0x4C, 0);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x78: case 0xAA: case 0x1FE:           /* whetstone, whetfish, mini whetstone */
        if (pl->kind != 1 && pl->kind != 5) {
            pl->work88A = id;
            Pl_act_set2(pl, 0, 0x61, 0);
            return;
        }
        item_action_fail(pl, mode);
        return;
    case 0x7B:                                  /* horn */
        pl->work88A = id;
        Pl_act_set(pl, 0, 0x5E, 0xC);
        return;
    case 0x7C: case 0x7D: case 0x7E: case 0x7F: /* health / antidote / demon / armor horns */
        pl->work88A = id;
        Pl_act_set(pl, 0, 0x62, 0xC);
        return;
    case 0x26: case 0x27: case 0x28: case 0x29: /* rare / well-done / well-done G / burnt steak */
        pl->work88A = id;
        Pl_act_set2(pl, 0, 0x5A, 0);
        return;
    case 0x79:                                  /* binoculars */
        if (pl->st == 1) {
            Pl_act_set2(pl, 0, 0x66, 0);
            return;
        }
        Pl_act_set2(pl, 0, 0x65, 0);
        return;
    default:
        item_action_fail(pl, mode);
        return;
    }
}
#elif !defined(G_PART)
static void item_action_set(PLW *pl, u8 mode) {
    u8 sp3F;
    u16 sp3C;
    f32 sp30[3];
    u16 temp_v1;
    u8 temp_a0;
    u8 temp_a0_2;

    if ((s16)Get_Active_itemnum() <= 0) {
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
        }
        return;
    }
    temp_v1 = pl->item[pl->work888].id;
    switch (temp_v1) {
    case 0x1A:
    case 0x1C:
        pl->work88A = temp_v1;
        if (pl_flag_ck(pl, 0x02000600) == 0) {
            Pl_act_set2(pl, 0, 0xB, 0xC);
            return;
        }
        Pl_act_set2(pl, 0, 0xE, 0xC);
        return;
    case 0xA5:
        if (Modori_dama_ck() == 1) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            Pl_act_set2(pl, 0, 0x72, 0xC);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    case 0x1B:
    case 0x80:
    case 0x68:
    case 0x21:
    case 0x9F:
        pl->work88A = temp_v1;
        Pl_act_set2(pl, 0, 0xE, 0xC);
        return;
    case 0x1F:
    case 0x12:
    case 0x16:
    case 0x18:
    case 0x17:
        if ((Niku_ok_ck() == 1) || (pl->item[pl->work888].id == 0x1F)) {
            temp_a0 = pl->st;
            if (temp_a0 == 0) {
                pl->work88A = (u16) pl->item[pl->work888].id;
                Pl_act_set2(pl, 0, 0x19, 0xC);
                return;
            }
            if (temp_a0 == 1) {
                pl->work88A = (u16) pl->item[pl->work888].id;
                Pl_act_set2(pl, 0, 0x59, 0xC);
                return;
            }
        } else {
            se_req(7, 0x15, 0);
            if (mode == 1) {
                pl_to_normal(pl, 0, 4, 0);
                return;
            }
        }
        break;
    case 0x20:
        if (Taru_ok_ck() == 1) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            Pl_act_set2(pl, 0, 0x6F, 0xC);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x1E:
        if (Pl_trap_use_ck(pl) >= 0) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            Pl_act_set2(pl, 0, 0x2E, 0xC);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
        if ((St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF) == 2) {
            pl->work88A = (u16) pl->item[pl->work888].id;
            pl->ang_y = sp3C;
            Pl_act_set2(pl, 0, 0x4F, 0);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0xA2:
        if ((St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF) == 0x11) {
            pl->work800 = sp30[0];
            pl->work804 = sp30[1];
            pl->work808 = sp30[2];
            pl->ang_y = sp3C;
            Pl_adj_calc(pl, 0x14);
            pl->cnt39A = (s16) sp3F;
            Pl_act_set2(pl, 0, 0x36, 0x20);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x41:
    case 0x5:
    case 0x6:
    case 0x1:
    case 0x2:
    case 0x9C:
    case 0x3:
    case 0x4:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xD:
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x9D:
    case 0x5F:
    case 0x5B:
    case 0x42:
    case 0x4B:
    case 0x53:
    case 0x54:
    case 0xA0:
    case 0xA1:
    case 0x9A:
    case 0xA4:
    case 0x99:
        pl->work88A = temp_v1;
        Pl_act_set2(pl, 0, 0x58, 0);
        return;
    case 0x81:
    case 0x143:
        if (Nikuyaki_ck(pl) == 1) {
            Pl_act_set(pl, 0, 0x4C, 0);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x69:
    case 0x9B:
    case 0x5E:
    case 0x6A:
        temp_a0_2 = pl->kind;
        if ((temp_a0_2 != 1) && (temp_a0_2 != 5)) {
            pl->work88A = temp_v1;
            Pl_act_set2(pl, 0, 0x61, 0);
            return;
        }
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        break;
    case 0x89:
        pl->work88A = temp_v1;
        Pl_act_set(pl, 0, 0x5E, 0xC);
        return;
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
        pl->work88A = temp_v1;
        Pl_act_set(pl, 0, 0x62, 0xC);
        return;
    case 0x13:
    case 0x14:
    case 0x15:
        pl->work88A = temp_v1;
        Pl_act_set2(pl, 0, 0x5A, 0);
        return;
    case 0x82:
        if (pl->st == 1) {
            Pl_act_set2(pl, 0, 0x66, 0);
            return;
        }
        Pl_act_set2(pl, 0, 0x65, 0);
        return;
    case PICKAXE0:          /* (G ids: search_act_set) */
    case PICKAXE0 + 1:
    case PICKAXE0 + 2:
    case BUGNET0:
    case BUGNET0 + 1:
    case BUGNET0 + 2:
        break;
    default:
        se_req(7, 0x15, 0);
        if (mode == 1) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 6
static s32 trade_get_ck_00139680(PLW *pl) {
    s16 i;
    PLW *p;
    u16 id;

    if (Online_ck() == 0) {
        return 0;
    }
    if (pl->work936 != 0) {
        return 0;
    }
    p = player_work;
    for (i = 0; i < game_w.pl_num; i++, p++) {
        {
            if (i != pl->id && p->be_flag != 0 && (s16)act_ck(p, 0, 0x67) != 0 && p->work909 == pl->id
                && flvecCalcDistance(pl->pos, p->pos) <= 300.0f) {
                id = p->work904;
                if (Item_data[id][2] < 3) {
                    if ((s16)Pl_item_num_ck(pl, id) == 0) {
                        if ((s16)Pl_item_search_space(pl) != 0) {
                            pl->work904 = p->work904;
                            pl->work906 = p->work906;
                            if (Pl_master_ck(pl) == 1) {
                                net_send_pl(pl, 7, p->id);
                            }
                            Pl_act_set2(pl, 0, 0x68, 0);
                            return 1;
                        }
                        set01_set2(GSTR(lit_1830, gptr_00528410));
                        pl->work936 = 0x5A;
                        goto ret0;
                    }
                    if ((s16)Pl_item_num_ck2(pl, p->work904) >= p->work906) {
                        pl->work904 = p->work904;
                        pl->work906 = p->work906;
                        if (Pl_master_ck(pl) == 1) {
                            net_send_pl(pl, 7, p->id);
                        }
                        Pl_act_set2(pl, 0, 0x68, 0);
                        return 1;
                    }
                    set01_set(1, 0xF, (s16)p->work904);
                    pl->work936 = 0x5A;
ret0:
                    return 0;
                }
            }
        }
    }
    return 0;
}
#endif

/* basic_com_ck: `case 0: default:` on the kind switch, the game-mode 5/6 and 7/8 tests are ifs (`== 6 || == 5`), the stick switch lists case 0 .. 4 in order, `> 0x4000` gives slti at, and the flag is a u8. */
#if !defined(G_PART)
static void basic_com_ck(PLW *pl) {
    EMW *var_s0;
    s32 temp_s0;
    u8 var_s1;
    s16 var_a0;
    u16 temp_v1_2;
    u16 var_v0;
    u32 temp_s0_2;
    u32 temp_v1_6;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_3;
    u8 temp_v1_4;
    u8 temp_v1_5;

    var_s0 = em_work;
    if (Online_ck() == 0) {
        if (Pl_master_ck(pl) == 0) {
            return;
        }
    } else if (Pl_master_ck(pl) == 0) {
        return;
    }
    {
        var_s1 = 0;
        if ((game_w.x0D5 == 4) && (pl->work88D == 0)) {
            Pl_act_set2(pl, 4, 2, 0);
            return;
        }
        pl->work8F0 = 1;
        if (pl->flag604 != 0) {
            for (var_a0 = 0; var_a0 < 0x14; var_a0++, var_s0++) {
                if ((pl->work605 == var_s0->x13) && (var_s0->kind == 7)) {
                    if (var_s0->x87F != 0) {
                        Pl_act_set(pl, 2, 7, 0);
                        return;
                    }
                    break;
                }
            }
            {
                if (((stick_pow_get(pl, 1) & 0xFF) > 0) && (pl->flag15 != 0x12)) {
                    pl->ang_y = stick_dir_set(pl, 0);
                    Pl_act_set(pl, 0, 0x12, 0);
                }
                pl_ride_ck(pl);
                if ((Game_clear_ck(1) != 1) && (pl->sw.trg & 0x20) && (trade_get_ck_00139680(pl) == 0)) {
                    unique_act_set(pl);
                    search_act_set(pl, 0);
                }
            }
        } else {
            if (game_w.x0D5 == 6 || game_w.x0D5 == 5) {
                Pl_act_set2(pl, 4, 3, 0);
                return;
            }
            if (game_w.x0D5 == 7 || game_w.x0D5 == 8) {
                Pl_act_set2(pl, 4, 5, 0);
                return;
            }
            {
                if (pl->flag12 != 0) {
                    temp_s0 = stick_pow_get(pl, 1) & 0xFF;
                    switch (temp_s0) {
                    case 0:
                        break;
                    case 1:
                    case 2:
                    case 3:
                        temp_v1 = pl->flag15;
                        if ((temp_v1 != 0x24) && (temp_v1 != 3) && (pl->x714 == 0)) {
                            Pl_act_set(pl, 0, 3, 0);
                        }
                        break;
                    case 4:
                        break;
                    }
                    if ((temp_s0 != 0) && (pl->x714 != 0) && (pl->flag15 != 0x3B)) {
                        Pl_act_set(pl, 0, 0x3B, 0);
                    }
                    if (Game_clear_ck(1) != 1) {
                        if (pl->work8C4 != 0) {
                            Pl_chat_act_set(pl);
                        }
                        temp_v1_2 = pl->sw.trg;
                        if ((temp_v1_2 & 0x220) || ((temp_v1_2 & 0x10) && (pl->sw.pow[0] >= 0x28))) {
                            if (pl_flag_ck(pl, 0x1200) == 0) {
                                Pl_act_set(pl, 0, 5, 0);
                            } else {
                                Pl_act_set(pl, 0, 0xA, 0);
                            }
                        }
                        job_special_com_ck(pl, 1);
                        if (!(shell_chg_ck(pl) & 0xFF)) {
                            basic_atack_ck(pl);
                        }
                        if (pl->work908 != 0) {
                            pl->work90B = 1;
                            Pl_act_set2(pl, 0, 0x67, 0x10);
                        }
                        if (func_639DD0(pl) & 0xFF) {
                            Pl_act_set2(pl, 2, 0x13, 0);
                            return;
                        }
                    }
                } else {
                    temp_s0_2 = stick_pow_get(pl, 0) & 0xFF;
                    switch (temp_s0_2) {
                    case 0:
                    case 4:
                        break;
                    case 1:
                        temp_v1_3 = pl->flag15;
                        if ((temp_v1_3 != 0x49) && (temp_v1_3 != 0x24) && (pl->x714 == 0)) {
                            Pl_act_set(pl, 0, 0x49, 0);
                            pl->ang_y = stick_dir_set(pl, 0);
                        }
                        break;
                    case 2:
                        temp_v1_4 = pl->flag15;
                        if ((temp_v1_4 != 2) && (temp_v1_4 != 0x24) && (pl->x714 == 0)) {
                            pl->ang_y = stick_dir_set(pl, 0);
                            Pl_act_set(pl, 0, 2, 0);
                        }
                        break;
                    case 3:
                        temp_v1_5 = pl->flag15;
                        if ((temp_v1_5 != 1) && (temp_v1_5 != 0x3F) && (temp_v1_5 != 0x13) && (temp_v1_5 != 0x24) && (temp_v1_5 != 0x1F) && (pl->x714 == 0)) {
                            pl->ang_y = stick_dir_set(pl, 0);
                            temp_v1_6 = (((*(u16 *)&pl->ang_y) + 0x10000) - pl->ang[1]) & 0xFFFF;
                            if ((temp_v1_6 >= 0x6000U) && (temp_v1_6 < 0xA001U)) {
                                Pl_act_set(pl, 0, 0x3F, 0);
                            } else {
                                Pl_act_set(pl, 0, 1, 0);
                            }
                        }
                        break;
                    case 5:
                        if (pl_flag_ck(pl, 0x200) == 0) {
                            if ((pl->work81E != 0) && ((((pl->x3A8 - pl->ang[1]) + 0x2000) & 0xFFFF) > 0x4000)) {
                                Pl_act_set(pl, 0, 0x20, 0);
                            } else {
                                Pl_act_set(pl, 0, 0x1F, 0);
                            }
                            var_s1 = 1;
                        }
                        break;
                    }
                    if (pl->work8C4 != 0) {
                        Pl_chat_act_set(pl);
                    }
                    if ((temp_s0_2 != 0) && (pl->x714 != 0) && (pl->flag15 != 0x3B)) {
                        Pl_act_set(pl, 0, 0x3B, 0);
                    }
                    if (Game_clear_ck(1) != 1) {
                        if (pl->sw.trg & 0x40) {
                            if (pl->sw.pow[0] >= 0x55) {
                                if (pl->stamina >= 0x4B) {
                                    Pl_act_set(pl, 0, 0x1C, 4);
                                }
                            } else {
                                Pl_act_set(pl, 0, 8, 0);
                            }
                        }
                        if ((pl->sw.trg & 0x200) && (pl->work886 == 0)) {
                            search_act_set(pl, 2);
                            item_action_set(pl, 0);
                            pl->work8F2 = (u8) (pl->work8F2 | 1);
                        }
                        job_special_com_ck(pl, 0);
                        if (var_s1 == 0) {
                            var_v0 = pl->sw.trg;
                        } else {
                            var_v0 = pl->sw.now;
                        }
                        if (var_v0 & 0xFFFF & 0x20) {
                            basic_kabe_ck(pl);
                        }
                        if (pl->sw.an_trg & 0x3C) {
                            if (pl_flag_ck(pl, 0x02000600) == 0) {
                                Pl_act_set(pl, 0, 4, 0);
                            } else {
                                temp_v0 = pl->kind;
                                switch (temp_v0) {
                                case 0:
                                default:
                                    Pl_act_set(pl, 1, 3, 4);
                                    break;
                                case 1:
                                    Pl_act_set(pl, 0, 4, 0);
                                    break;
                                case 5:
                                    Pl_act_set(pl, 0, 0x17, 0);
                                    break;
                                case 4:
                                    Pl_act_set(pl, 1, 0x3E, 4);
                                    break;
                                case 3:
                                    Pl_act_set(pl, 1, 0x2E, 4);
                                    break;
                                case 2:
                                    Pl_act_set(pl, 1, 0x20, 4);
                                    break;
                                }
                            }
                        }
                        if (pl->sw.trg & 1) {
                            Pl_act_set(pl, 1, 0x14, 4);
                        }
                        if (pl->sw.trg & 0x20) {
                            if (trade_get_ck_00139680(pl) == 0) {
                                unique_act_set(pl);
                                search_act_set(pl, 0);
                            }
                        } else if (pl->work908 != 0) {
                            pl->work90B = 1;
                            Pl_act_set2(pl, 0, 0x67, 0x10);
                        }
                        if ((em_ninshiki_ck2(pl) & 0xFF) == 1) {
                            Pl_act_set(pl, 0, 0x3E, 0);
                            WyvernFindPlayer(pl);
                        }
                        if (func_639DD0(pl) & 0xFF) {
                            Pl_act_set2(pl, 2, 0x13, 0);
                        }
                    }
                }
            }
        }
    }
}
#endif

/* gun_adj_sub matches with `#pragma opt_propagation off` (the original negates the constant v at run time instead of
   folding it), blend_calc's s8 parameter, the angle read as u16 (lhu), `v = (s8)v` after the second negation (the original sign-extends v again at the join), an int `pw`. */
#pragma opt_propagation off
#if !defined(G_PART)
static void gun_adj_sub(PLW *pl) {
    s8 v;
    u16 step;
    u16 now;
    u16 an;
    int pw;

    if (Pl_master_ck(pl) != 0) {
        if (pl->kind == 1 || pl->kind == 5) {
            if (pl->char0 == 0x3E9) {
                if (pl->x763 != 0 || pl->pch_on != 0) {
                    pl->x763 = 10;
                }
                blend_set(pl, 0x3ED, 0x3EE);
                if (pl->x763 != 0 || pl->pch_on != 0 || ((now = pl->sw.now, (now & 4) == 0) && (now & 8))) {
                    now = pl->sw.now;
                    if (now & 0x3000) {
                        v = 5;
                        if (pl->pch_on != 0 && game_w.x1DD != 0) {
                            v = -v;
                        }
                        if (now & 0x2000) {
                            blend_calc(pl, v);
                        } else {
                            blend_calc(pl, -v);
                        }
                        pl->x763 = 5;
                    }
                    if (pl->sw.now & 0x800) {
                        pl->x763 = 10;
                        if (pl->pch_on != 0 && game_w.x1DD == 2) {
                            *(u16 *)&pl->ang_y -= 0x240;
                        } else {
                            *(u16 *)&pl->ang_y += 0x240;
                        }
                    }
                    if (pl->sw.now & 0x400) {
                        pl->x763 = 10;
                        if (pl->pch_on != 0 && game_w.x1DD == 2) {
                            *(u16 *)&pl->ang_y += 0x240;
                        } else {
                            *(u16 *)&pl->ang_y -= 0x240;
                        }
                    }
                    blend_set(pl, 0x3ED, 0x3EE);
                    if (pl->x763 != 0 || pl->pch_on != 0) {
                        an = pl->sw.an_now;
                        if (!(an & 0x3C00) || (pw = pl->sw.pow[0]) < 0x28) {
                            return;
                        }
                        v = 1;
                        step = 0x4C;
                        if (pw >= 0x78) {
                            v = 8;
                            step = 0x200;
                        } else if (pw >= 0x5A) {
                            v = 4;
                            step = 0xC0;
                        } else if (pw >= 0x3C) {
                            v = 2;
                            step = 0x80;
                        }
                        if (an & 0x3000) {
                            if (game_w.x1DD != 0) {
                                v = -v;
                            }
                            v = (s8)v;
                            if (an & 0x2000) {
                                blend_calc(pl, v);
                            } else {
                                blend_calc(pl, -v);
                            }
                            pl->x763 = 5;
                            blend_set(pl, 0x3ED, 0x3EE);
                        }
                        if (pl->sw.an_now & 0x800) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                *(u16 *)&pl->ang_y -= step;
                            } else {
                                *(u16 *)&pl->ang_y += step;
                            }
                        }
                        if (pl->sw.an_now & 0x400) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                *(u16 *)&pl->ang_y += step;
                            } else {
                                *(u16 *)&pl->ang_y -= step;
                            }
                        }
                    }
                    blend_set(pl, 0x3ED, 0x3EE);
                }
            }
        }
    }
}
#endif
#pragma opt_propagation reset

/* Player code (SLPM_654.95 0x0013A5D0-0x0013A620): scope_add, clamp the pachinger/scope yaw offset (x8EE) to +-0x2000. */




#if !defined(G_PART) || G_PART == 7
static void scope_add(PLW *pl, int arg1) {
    pl->x8EE = pl->x8EE + arg1;
    if ((s16)arg1 >= 0) {
        if (pl->x8EE >= 0x2000) {
            pl->x8EE = 0x2000;
        }
    } else if (pl->x8EE < -0x1FFF) {
        pl->x8EE = -0x2000;
    }
}
#endif

/* sougun_adj_sub matches with `#pragma opt_propagation off` (the original negates the constant v at run time instead of
   folding it), blend_calc's s8 parameter, the angle read as u16 (lhu), `v = (s8)v` after the second negation, an int `pw`, the angle difference d as u32 with unsigned compares. */
#pragma opt_propagation off
#if !defined(G_PART) || G_PART == 7
static void sougun_adj_sub(PLW *pl, u16 id) {
    s8 v;
    u16 step;
    u16 now;
    u16 an;
    int pw;
    u16 t;
    u32 d;

    if (Pl_master_ck(pl) != 0) {
        if (pl->char0 == id) {
            if (pl->x763 != 0 || pl->pch_on != 0) {
                pl->x763 = 10;
            }
            if (pl->x763 != 0 || pl->pch_on != 0 || ((now = pl->sw.now, (now & 4) == 0) && (now & 8))) {
                now = pl->sw.now;
                if ((now & 0x3000) && id != 0x19E) {
                    v = 4;
                    if (pl->pch_on != 0 && game_w.x1DD != 0) {
                        v = -v;
                    }
                    if (now & 0x2000) {
                        scope_add(pl, (s16)(v << 8));
                    } else {
                        scope_add(pl, (s16)(-v << 8));
                    }
                    pl->x763 = 5;
                }
                if (pl->sw.now & 0x800) {
                    pl->x763 = 10;
                    if (game_w.x1DD == 2) {
                        *(u16 *)&pl->ang_y -= 0x180;
                    } else {
                        *(u16 *)&pl->ang_y += 0x180;
                    }
                }
                if (pl->sw.now & 0x400) {
                    pl->x763 = 10;
                    if (game_w.x1DD == 2) {
                        *(u16 *)&pl->ang_y += 0x180;
                    } else {
                        *(u16 *)&pl->ang_y -= 0x180;
                    }
                }
                if (pl->x763 != 0 || pl->pch_on != 0) {
                    an = pl->sw.an_now;
                    if ((an & 0x3C00) && (pw = pl->sw.pow[0]) >= 0x28) {
                        v = 1;
                        step = 0x4C;
                        if (pw >= 0x78) {
                            v = 8;
                            step = 0x200;
                        } else if (pw >= 0x5A) {
                            v = 4;
                            step = 0xC0;
                        } else if (pw >= 0x3C) {
                            v = 2;
                            step = 0x80;
                        }
                        if ((an & 0x3000) && id != 0x19E) {
                            if (game_w.x1DD != 0) {
                                v = -v;
                            }
                            v = (s8)v;
                            if (an & 0x2000) {
                                scope_add(pl, (s16)(v << 8));
                            } else {
                                scope_add(pl, (s16)(-v << 8));
                            }
                            pl->x763 = 5;
                        }
                        if (pl->sw.an_now & 0x800) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                *(u16 *)&pl->ang_y -= step;
                            } else {
                                *(u16 *)&pl->ang_y += step;
                            }
                        }
                        if (pl->sw.an_now & 0x400) {
                            pl->x763 = 10;
                            if (game_w.x1DD == 2) {
                                *(u16 *)&pl->ang_y += step;
                            } else {
                                *(u16 *)&pl->ang_y -= step;
                            }
                        }
                    }
                }
                if (id == 0x19E) {
                    t = pl->work2D4;
                    d = (u16)(*(u16 *)&pl->ang_y - t);
                    if (d < 0x8001 && d > 0x4000) {
                        *(u16 *)&pl->ang_y = t + 0x4000;
                    } else if (d >= 0x8000 && d < 0xC000) {
                        *(u16 *)&pl->ang_y = t - 0x4000;
                    }
                }
            }
        }
    }
}
#endif
#pragma opt_propagation reset

/* Player code (SLPM_654.95 0x0013A9F0-0x0013B8CC): player action handlers (pl_mv*), called through the PLPROG
   table: pl_mv000 (stand: per-character tweaks, then gun_adj_sub/basic_com_ck), pl_mv001 (guard/crouch states),
   pl_mv004, pl_mv006 (jump/fall with landing checks). x05 = step inside the action, x06/x07/work08 = counters. */




#if !defined(G_PART) || G_PART == 7
static void pl_mv000(PLW *pl, s32 unused_arg) {
    u16 c;

    c = pl->char0;
    switch (c) {
    case 0x1:
    case 0x3F:
        if (pl->work882 < 0x4C) {
            pl_chr_set2(pl, 0x193, 6, 0);
        } else if (pl->work39C >= 0xF0 && pl->work011 == 1 && c != 0x3F) {
            pl_chr_set2(pl, 0x3F, 2, 0);
        }
        break;
    case 0x15:
        if (pl->work194 == 0) {
            pl_chr_set2(pl, 5, 4, 0x90);
        }
        break;
    case 0x5:
        if (!(*(u16 *)&game_w.x1E & 0x1F)) {
            Eft02_set6(0.1f, pl, 8, 0x14);
        }
        break;
    case 0x193:
        if (pl->work882 > 0x4B) {
            pl_chr_set2(pl, 1, 4, 0);
        }
        break;
    case 0x3E9:
        if (pl->flag12 != 0) {
            if (pl->kind == 1 || pl->kind == 5) {
                blend_set(pl, 0x3ED, 0x3EE);
            }
        }
        break;
    }
    gun_adj_sub(pl);
    basic_com_ck(pl);
}
#endif

#if !defined(G_PART)
static void pl_mv001(PLW *pl, u32 arg1) {
    u8 p;
    u8 t;

    if (pl->x05 == 0) {
        pl->x05++;
        pl->x06 = 0;
        pl->x07 = 10;
        pl->work08 = 10;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 3, 6, 0);
            pl_flag_set(pl, 0x02000000);
            break;
        case 1:
            pl_chr_set2(pl, 2, 4, 0);
            break;
        case 2:
            switch (pl->kind) {
            case 0:
            default:
                pl_chr_set2(pl, 0x3EC, 4, 0);
                break;
            case 3:
                pl_chr_set2(pl, 0x3EC, 4, 0);
                break;
            case 2:
                pl_chr_set2(pl, 0x3EC, 8, 0);
                break;
            case 1:
            case 4:
            case 5:
#ifdef VERSION_MHG
            case 6:     /* G (game 0x54F830, jump table of 7): the Dual Swords blend into the walk start over 6 like the
                         * great sword; MH1's code gave them the default 4 (the lockstep referee's ds scenario, tick 254) */
#endif
                pl_chr_set2(pl, 0x3EC, 6, 0);
                break;
            }
            break;
        case 3:
            pl_chr_set2(pl, 0xA, 6, 0);
            break;
        case 5:
            pl_chr_set2(pl, 0xA, 2, 0);
            pl->st = 1;
            break;
        case 6:
            pl->work760 = 0x1E;
            if (pl->char0 != 3) {
                pl_chr_set2(pl, 3, 0, 0x26);
            }
            pl_flag_set(pl, 0x02000000);
            break;
        case 7:
            pl_chr_set2(pl, 0x31, 6, 0);
            break;
        case 8:
            pl_flag_set(pl, 0x8000);
            pl_chr_set2(pl, 0x3F3, 4, 0);
            break;
        }
        pl->work39C = 0;
    }
    if (Pl_master_ck(pl) == 0) {
        if (Online_ck() != 1) {
            goto go;
        }
    } else {
go:
        p = stick_pow_get(pl, pl->flag12);
        if (pl->x07 != 0) {
            pl->x07--;
        }
        if (pl->work08 > 0) {
            pl->work08--;
        }
        t = p;
        if (t == 0 && pl->work08 == 0) {
            if (pl->char0 == 0x3EC) {
                pl_to_normal(pl, 0, mv001_tbl[pl->kind], 0);
            } else if (pl->char0 == 3 || pl->char0 == 4) {
                if (pl->x07 == 0) {
                    pl->work8F0 = 1;
                    Pl_act_set(pl, 0, 0x2C, 0);
                } else {
                    pl_to_normal(pl, 0, 4, 0);
                }
            } else if (pl->char0 == 0x3F3) {
                Pl_act_set2(pl, 2, 0xC, 0);
            } else {
                pl_to_normal(pl, 0, 4, 0);
            }
            return;
        }
        switch (pl->char0) {
        case 2:
            if (frame_check(108.0f, pl, 0) != 0) {
                pl->x06++;
                if (pl->x06 >= 5) {
                    pl->x06 = 0;
                    pl_chr_set2(pl, 0x1D, 2, 0);
                }
            }
            break;
        case 3:
            if (t < 3) {
                pl->work8F0 = 1;
                Pl_act_set(pl, 0, 0x2C, 0);
                return;
            }
            break;
        case 0x1D:
            if (pl->work194 == 0) {
                pl_chr_set2(pl, 2, 6, 0x24);
            }
            break;
        case 0x3F3:
            if (!(pl->sw.now & 0x80)) {
                Pl_act_set2(pl, 2, 0xC, 0);
            }
            break;
        }
        if (pl->x714 != 0 && (arg1 != 5 || arg1 != 8)) {
            Pl_act_set(pl, 0, 0x3B, 0);
        }
        if (t != 0) {
            pl->ang_y = stick_dir_set(pl, 0);
        }
        if (arg1 != 8) {
            basic_com_ck(pl);
        } else {
            guard_atk_ck(pl);
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 8
static void pl_mv004(PLW *pl, u32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        switch (arg1) {
        case 0:
            pl->flag12 = mv004_chr_tbl0[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl0[pl->kind].chr, mv004_chr_tbl0[pl->kind].mot, 0);
            break;
        case 1:
        case 3:
        case 4:
            pl->flag12 = mv004_chr_tbl1[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl1[pl->kind].chr, mv004_chr_tbl1[pl->kind].mot, 0);
            break;
        case 2:
            pl_flag_set(pl, 0x1000);
            pl->flag12 = mv004_chr_tbl2[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl2[pl->kind].chr, mv004_chr_tbl2[pl->kind].mot, 0);
            break;
        case 5:
            pl->flag12 = mv004_chr_tbl3[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl3[pl->kind].chr, mv004_chr_tbl3[pl->kind].mot, 0);
            break;
        }
        pl->x8C8 = 0;
        t = pl->work56B;
        if (t & 0xF) {
            pl->work56B = t & 0xF0;
            func_549200(pl, 4);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            switch (arg1) {
            case 0:
                pl_to_normal(pl, 0, mv004_chr_tbl0[pl->kind].next, 0);
                break;
            case 1:
                pl_to_normal(pl, 0, mv004_chr_tbl1[pl->kind].next, 0);
                break;
            case 2:
                pl_to_normal(pl, 0, mv004_chr_tbl2[pl->kind].next, 0);
                break;
            case 3:
                Pl_act_set2(pl, 0, 8, 2);
                break;
            case 4:
                if (Pl_master_ck(pl) == 1) {
                    item_action_set(pl, 1);
                } else {
                    pl_to_normal(pl, 0, mv004_chr_tbl0[pl->kind].next, 0);
                }
                break;
            case 5:
                pl_to_normal(pl, 0, mv004_chr_tbl3[pl->kind].next, 0);
                break;
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 8
static void pl_mv006(PLW *pl, s32 arg1) {
    int sp4C;
    f32 sp40[3];
    f32 sp30[3];
    f32 f;
    u8 s;

    pl->work08++;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->st = 2;
        pl->x763 = 0;
        Pl_view_reset(pl);
        switch (arg1) {
        default:
            pl_chr_set2(pl, 0xF, 0, 0);
            pl->vel[0] = 0.0f;
            pl->vel[2] = 20.0f;
            pl->vel[1] = 18.0f;
            pl->acc[0] = 0.0f;
            pl->acc[2] = -(pl->vel[2] / 80.0f);
            rate_g_calc(pl, 0x13);
            if (pl->flag604 != 0) {
                pl->flag604 = 2;
            }
            break;
        case 1:
            pl_chr_set2(pl, 0xF, 8, 0x20);
            pl->flag604 = 0;
            pl->vel[0] = 0.0f;
            pl->vel[2] = 8.0f;
            pl->vel[1] = -10.0f;
            pl->acc[2] = 0.0f;
            pl->acc[0] = 0.0f;
            pl->acc[1] = -0.72727275f;
            break;
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->work08 = 0;
        pl->x07 = 0;
        break;
    case 1:
        pl->vel[0] = pl->vel[0] + pl->acc[0];
        pl->vel[1] = pl->vel[1] + pl->acc[1];
        pl->vel[2] = pl->vel[2] + pl->acc[2];
        if (pl->vel[0] < 0.0f) {
            pl->vel[0] = 0.0f;
        }
        if (pl->vel[2] < 0.0f) {
            pl->vel[2] = 0.0f;
        }
        sp40[0] = pl->vel[0];
        sp40[1] = pl->vel[1];
        sp40[2] = pl->vel[2];
        flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp30[0];
        pl->pos[1] = pl->pos[1] + sp30[1];
        pl->pos[2] = pl->pos[2] + sp30[2];
        if (pl->x06 < 5) {
            f = pl->work19C;
            if (!(f < 10.0f) && (f <= 40.0f) && (pl->sw.trg & 0x4000)) {
                pl->x05 = 1;
                pl->x06++;
            }
        }
        if (pl->vel[1] < 0.0f) {
            if ((front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp4C) != 0) && (pl->vital > 0) && (pl->work937 < 0x15)) {
                pl->ang_y = pl->ang[1];
                Pl_act_set2(pl, 0, 0x1B, 4);
                break;
            }
            f = pl->x5AC;
            if (!(f < pl->pos[1])) {
                pl->pos[1] = f;
                pl->st = 0;
                pl->flag604 = 0;
                if ((pl->vital <= 0) && (Pl_master_ck(pl) == 1)) {
                    func_639F20(pl);
                    break;
                }
                if (pl->work08 >= 0x1E) {
                    Pl_act_set2(pl, 0, 0x26, 0);
                    break;
                }
                pl->x05++;
                if ((pl->flag12 == 0) && (pl->sw.now & 0x10) && (stick_pow_get(pl, 0) & 0xFF)) {
                    pl->x07 = 1;
                    pl_chr_set2(pl, 0x10, 0, 0);
                    break;
                }
                pl->x07 = 0;
                pl_chr_set2(pl, 0x3E, 0, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            if (pl->x07 == 0) {
                pl_to_normal(pl, 0, 8, 0);
                break;
            }
            if (pl->work81E == 0) {
                Pl_act_set2(pl, 0, 0x46, 0);
                break;
            }
            Pl_act_set2(pl, 0, 0x20, 0);
        }
        break;
    }
}
#endif

/* pl_mv021: `case 0: default:` on the arg1 switch. */
#if !defined(G_PART) || G_PART == 8
static void pl_mv021(PLW *pl, s32 arg1) {
    int sp4C;
    f32 sp40[3];
    f32 sp30[3];
    f32 f;
    u8 s;

    pl->work08++;
    s = pl->x05;
    switch (s) {
    case 0:
        switch (arg1) {
        case 0:
            default:
            pl->x05++;
            pl->x06 = 0;
            pl_chr_set2(pl, 0xB, 2, 0);
            Pl_basic_flagset(pl, 0, 0, 0);
            break;
        case 2:
            pl->pos[1] = pl->pos[1] - 195.0f;
            sp40[2] = -30.0f;
            sp40[0] = 0.0f;
            sp40[1] = 0.0f;
            flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x60));
            pl->pos[0] = pl->pos[0] + sp30[0];
            pl->pos[2] = pl->pos[2] + sp30[2];
        case 1:
            pl->x05 = 2;
            pl->x06 = 0;
            Pl_basic_flagset(pl, 2, 0, 0);
            rate_clear(pl);
            pl->vel[1] = -9.0f;
            pl->acc[1] = -0.72727275f;
            if (arg1 == 1) {
                pl_chr_set2(pl, 0xC, 4, 0x16);
            } else {
                pl_chr_set2(pl, 0xC, 0, 0x16);
            }
            break;
        }
        Pl_view_reset(pl);
        action_timer_calc(pl, 0);
        pl->work08 = 0;
        break;
    case 1:
        if (pl->work194 == 0 || (pl->x06 < 6 && pl->x06 != 0)) {
            pl->x05++;
            rate_clear(pl);
            if (pl->flag12 != 0) {
                f = 0.8f;
            } else {
                f = 1.0f;
            }
            pl->vel[1] = 2.0f * (15.0f * f);
            if (pl->x06 != 0) {
                pl->vel[1] = 50.0f;
            }
            rate_g_calc(pl, 0x14);
            pl_chr_set2(pl, 0xC, 2, 0);
            pl->st = 2;
            if (pl->flag604 != 0) {
                pl->flag604 = 2;
            }
            action_timer_calc(pl, 0);
            pl->work08 = 0;
        }
        break;
    case 2:
        rate_add_g(pl);
        if (pl->vel[1] < 0.0f) {
            if ((arg1 != 2) && (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp4C) != 0) && (pl->vital > 0)) {
                pl->ang_y = pl->ang[1];
                Pl_act_set2(pl, 0, 0x1B, 0xC);
                break;
            }
            f = pl->x5AC;
            if (!(f < pl->pos[1])) {
                pl->pos[1] = f;
                pl->st = 0;
                pl->flag604 = 0;
                if ((pl->vital <= 0) && (Pl_master_ck(pl) == 1)) {
                    func_639F20(pl);
                    break;
                }
                if (pl->work08 >= 0x1E) {
                    Pl_act_set(pl, 0, 0x26, 0);
                    break;
                }
                pl->x05++;
                pl_chr_set2(pl, 0xD, 0, 0);
            }
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 3, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013BCB0-0x0013BF20): pl_mv008 (sit down / stand up), pl_mv013. */




#if !defined(G_PART) || G_PART == 8
static void pl_mv008(PLW *pl, int arg1) {
    u8 s;
    u8 t;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        switch (arg1) {
        case 0:
            pl->x05 = s + 1;
            pl->x06 = 0;
            pl->work39C = 0;
            pl_chr_set2(pl, 7, 2, 0);
            Pl_basic_flagset(pl, 0, 0, 0);
            pl->flag12 = 0;
            t = pl->work56B;
            if (t & 0xF) {
                pl->work56B = t & 0xF0;
                func_549200(pl, 4);
            }
            break;
        case 1:
            pl->x05 = 2;
            pl->x06 = 0;
            if (pl->char0 == 0x19C) {
                pl_chr_set2(pl, 8, 8, 0);
            } else {
                pl_chr_set2(pl, 8, 2, 0);
            }
            action_timer_calc(pl, 0);
            Pl_basic_flagset(pl, 0x8001, 0, 0);
            pl->flag12 = 0;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 8, 6, 0);
            Pl_basic_flagset(pl, 0x8001, 0, 0);
            action_timer_calc(pl, 0);
        }
        break;
    case 2:
        sit_com_ck(pl);
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv013(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work603 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->x06 = 0x10;
            normal_char_set(pl, pl->x06, 0);
            break;
        }
        if (frame_check2(4.0f, pl, 0) != 0) {
            pl->work601 = 1;
        }
        break;
    case 2:
        s = pl->x06;
        pl->x06 = s - 1;
        if (s == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013BF20-0x0013C868): pl_mv014 (use an item: apply it, then stage start/return), pl_mv017, pl_mv022, pl_mv024, pl_mv025 (action handlers) */




#if !defined(G_PART) || G_PART == 41
static void pl_mv014(PLW *pl, s32 arg1) {
    int a2;
    f32 f;
    u8 s;
    u8 t;
    u8 idx;
    int off;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x191, 2, 0);
        } else {
            pl_chr_set2(pl, 0x1A1, 2, 0);
        }
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (arg1 != 0) {
            f = 20.0f;
        } else {
            f = 24.0f;
        }
        if (frame_check(f, pl, 0) != 0) {
            a2 = 0;
            if (pl->work016 == 0) {
                t = pl->work017;
                if (t == 1) {
                    a2 = 2;
                }
                if (t == 2) {
                    a2 = 1;
                }
            }
#ifdef VERSION_MHG
            switch (pl->work88A) {      /* G pl_mv014 (game 0x550C00): G item ids, shell03 kinds 10-15 are G's */
            case 0x38: func_628FB0(pl, 0, a2); break;       /* smoke bomb */
            case 0x3A: func_628FB0(pl, 7, a2); break;       /* farcaster */
            case 0x34: func_628FB0(pl, 1, a2); break;       /* flash bomb */
            case 0x37: func_628FB0(pl, 5, a2); break;       /* dung bomb */
            case 0x39: func_628FB0(pl, 0xB, a2); break;     /* poison smoke bomb */
            case 0x33: func_628FB0(pl, 6, a2); break;       /* paintball */
            case 0x99: func_628FB0(pl, 8, a2); break;       /* stone */
            case 0x35: func_628FB0(pl, 9, a2); break;       /* sonic bomb */
            case 0x1FF: func_628FB0(pl, 0xA, a2); break;    /* tranquilizer ball */
            case 0x2E: func_628FB0(pl, 0xC, a2); break;     /* throwing knife */
            case 0x2F: func_628FB0(pl, 0xD, a2); break;     /* poison knife */
            case 0x30: func_628FB0(pl, 0xE, a2); break;     /* sleep knife */
            case 0x31: func_628FB0(pl, 0xF, a2); break;     /* paralysis knife */
            }
#else
            switch (pl->work88A) {
            case 0x1A:
                func_628FB0(pl, 0, a2);
                break;
            case 0xA5:
                func_628FB0(pl, 7, a2);
                break;
            case 0x1B:
                func_628FB0(pl, 1, a2);
                break;
            case 0x1C:
                func_628FB0(pl, 5, a2);
                break;
            case 0x80:
                func_628FB0(pl, 6, a2);
                break;
            case 0x68:
                func_628FB0(pl, 8, a2);
                break;
            case 0x21:
                func_628FB0(pl, 9, a2);
                break;
            case 0x9F:
                func_628FB0(pl, 0xA, a2);
                break;
            }
#endif
            if (Game_clear_ck(1) == 0) {
                Pl_item_stack(pl, pl->work88A, -1);
            }
        }
        if (pl->work194 == 0) {
            switch (arg1) {
            case 0:
                pl_to_normal(pl, 0, 0xA, 0);
                break;
            case 1:
                pl_to_normal(pl, 0, 2, 0);
                break;
            case 2:
                if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
                    idx = game_w.x2F;
                    pl->x738 = 1;
                    pl->work73C = stage_start_pos[idx][0];
                    pl->work740 = stage_start_pos[idx][1];
                    pl->work744 = stage_start_pos[idx][2];
                    pl->work570 = stage_start_ang[idx];
                    pl->x73A = idx;
                    Pl_ofs_set(pl, &pl->work73C, pl->work570);
                    net_send_pl(pl, 5, 0);
                } else {
                    pl_to_normal(pl, 0, 2, 0);
                }
                break;
            }
        }
        break;
    case 2:
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 9
static void pl_mv017(PLW *pl, s32 arg1) {
    f32 spCC;
    s32 spC0[3];
    f32 spB0[3];
    f32 spA0[3];
    f32 sp90[3];
    f32 sp50[16];
    u8 s;
    s32 w;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->ang_y = pl->ang[1];
        pl_chr_set2(pl, 0x11, 0, 0);
        pl->work08 = 0x1E;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 1) {
            spC0[0] = 0;
            spC0[1] = pl->ang[1];
            spC0[2] = 0;
            cpRotMatrix(spC0, sp50);
            spB0[0] = 0.0f;
            spB0[1] = 0.0f;
            spB0[2] = 55.0f;
            flvecApplyMat33(sp90, spB0, sp50);
            spA0[0] = pl->pos[0] + sp90[0];
            spA0[1] = pl->pos[1] + sp90[1];
            spA0[2] = pl->pos[2] + sp90[2];
            if (GetGroundHitAreaUpper(pl, spA0, &spCC) == 1) {
                pl->pos[1] = spCC;
            }
            spB0[2] = 55.0f;
            flvecApplyMat33(sp90, spB0, (f32 *)((u8 *)pl + 0x60));
            pl->pos[0] = pl->pos[0] + sp90[0];
            pl->pos[1] = pl->pos[1] + sp90[1];
            pl->pos[2] = pl->pos[2] + sp90[2];
        }
        if ((act_ck(pl, 0, 0x1B) == 1) && (pl->work016 == 0) && (pl->work017 == 9)) {
            pl->work937++;
        }
        break;
    case 1:
        w = pl->work08;
        if (w != 0) {
            pl->work08 = w - 1;
            if (pl->work08 <= 0) {
                pl->work08 = 0;
            }
        }
        if (pl->work194 == 0 || (pl->sw.trg & 0x20) || (pl->sw.an_now & 0x2000)) {
            Pl_act_set2(pl, 0, 0x3A, 0xC);
        } else if ((Pl_master_ck(pl) == 1) && (pl->sw.an_trg & 0x1000) && (pl->work08 == 0)) {
            Pl_act_set2(pl, 0, 0x38, 4);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 9
static void pl_mv022(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x1A, 6, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if ((s16)pl_ride_ck(pl) == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 9
static void pl_mv024(PLW *pl, s32 arg1) {
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work39C = 0;
        pl->work08 = 0;
        pl->flag12 = 0;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 8, 6, 0);
            Pl_basic_flagset(pl, 0, 0, 0);
            break;
        case 1:
            pl->x05 = 3;
            Pl_basic_flagset(pl, 1, 0, 0);
            pl_chr_set2(pl, 0x199, 0, 0x14);
            break;
        }
        action_timer_calc(pl, 0);
        if (Pl_master_ck(pl) == 1) {
            Oki_item_set(pl);
        }
        break;
    case 1:
        w = pl->work08 + 1;
        pl->work08 = w;
        if (w >= 0xE) {
            pl->x05++;
            pl_chr_set2(pl, 9, 0, 0);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x1D, 2);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 9
static void pl_mv025(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->work08 = 0;
        pl_chr_set2(pl, 7, 2, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x18, 0xC);
            } else {
                pl->x05++;
                pl_chr_set2(pl, 8, 6, 0);
            }
        }
        break;
    case 2:
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013C870-0x0013D5E4): pl_mv028 (kick/dodge), pl_mv030, pl_mv031/032 (run and dash with stamina drain), pl_mv033 (action handlers) */




#if !defined(G_PART)
static void pl_mv028(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work39C = 0;
        pl->work40C = 6;
        pl->work08 = 0;
        Pl_view_reset(pl);
        pl->x43E = 0;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        t = pl->work56B;
        if (t & 0xF) {
            pl->work56B = t & 0xF0;
            func_549200(pl, 4);
        }
        if (arg1 == 0) {
            pl->x05 = 2;
            if ((pl->kind == 2) && (pl->flag12 != 0)) {
                pl_chr_set2(pl, 0x3F3, 2, 0);
                break;
            }
            pl_chr_set2(pl, 0x1C, 2, 0);
            break;
        }
        pl->x05++;
        pl_chr_set2(pl, 0x16, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x1C, 0, 6);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((Pl_master_ck(pl) == 1) && (arg1 == 0) && (pl->flag12 != 0)) {
            switch (pl->kind) {
            case 4:
                if ((pl->sw.an_trg & 0x20) && (frame_check3(10.0f, 44.0f, pl, 0) != 0) && (pl->x06 == 0)) {
                    pl->x06 = 1;
                    if (pl->sw.an_now & 0x800) {
                        pl->x06 = 2;
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->x06 = 3;
                    }
                }
                if (frame_check2(44.0f, pl, 0) != 0) {
                    t = pl->x06;
                    if (t != 0) {
                        if ((t & 0xFF) == 2) {
                            pl->ang[1] = pl->ang[1] + 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        if (pl->x06 == 3) {
                            pl->ang[1] = pl->ang[1] - 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        Pl_act_set2(pl, 1, 0x49, 0xC);
                    }
                }
                break;
            case 2:
                if ((frame_check2(40.0f, pl, 0) != 0) && (pl->sw.now & 0x80)) {
                    Pl_act_set(pl, 1, 0x18, 0);
                }
                break;
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 10
static void pl_mv030(PLW *pl, s32 unused_arg) {
    f32 sp7C;
    s32 sp70[3];
    f32 sp60[3];
    f32 sp50[3];
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x12, 0, 0x58);
        pl->ang_y = pl->ang[1];
        sp70[0] = 0;
        sp70[1] = 0;
        sp70[2] = 0x425C0000;
        flvecApplyMat33(sp50, (f32 *)sp70, (f32 *)pl->rot);
        sp60[0] = pl->pos[0] + sp50[0];
        sp60[1] = pl->pos[1] + sp50[1];
        sp60[2] = pl->pos[2] + sp50[2];
        if (GetGroundHitAreaUpper(pl, sp60, &sp7C) == 1) {
            pl->pos[1] = sp7C;
        }
        sp70[2] = 0x425C0000;
        flvecApplyMat33(sp50, (f32 *)sp70, (f32 *)pl->rot);
        pl->pos[0] = pl->pos[0] + sp50[0];
        pl->pos[1] = pl->pos[1] + sp50[1];
        pl->pos[2] = pl->pos[2] + sp50[2];
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv031(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    u8 s;
    s32 p;
    s16 st;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x600);
        pl->work39C = 0;
        pl->work760 = 0x1E;
        switch (arg1) {
        case 0:
            if (pl->char0 != 3) {
                pl_chr_set2(pl, 3, 0, 0);
            }
            break;
        case 1:
            pl_chr_set2(pl, 3, 4, 0x1E);
            break;
        }
        pl->chr_spd0 = 2.5f;
        pl->chr_spd1 = 2.5f;
        pl->ang_y = pl->ang[1];
        break;
    case 1:
        pl->chr_spd0 = 2.5f;
        pl->chr_spd1 = 2.5f;
        Pl_stamina_calc(pl, -1);
        st = pl->stamina;
        if (st < 0x4C) {
            Pl_act_set2(pl, 0, 0x22, 0);
            break;
        }
        if ((pl->sw.trg & 0x40) && (st >= 0x4B) && (Game_clear_ck(1) == 0)) {
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 0, 0x1C, 4);
            break;
        }
        if (pl->sw.pow[0] >= 0x28) {
            pl->ang_y = stick_dir_set(pl, 0);
        }
        if (front_land_ck2(85.0f, -60.0f, pl, 0) != 0) {
            pl->ang_y = pl->ang[1];
            Pl_act_set2(pl, 0, 6, 0);
            break;
        }
        if ((pl->work81E != 0) && ((((pl->x3A8 - pl->ang[1]) + 0x2000) & 0xFFFF) > 0x4000)) {
            Pl_act_set(pl, 0, 0x20, 0);
            break;
        }
        sp40[2] = 5.0f;
        sp40[0] = 0.0f;
        sp40[1] = 0.0f;
        flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp30[0];
        pl->pos[2] = pl->pos[2] + sp30[2];
        p = stick_pow_get(pl, 0) & 0xFF;
        if ((p != 5) && ((Pl_master_ck(pl) == 1) || (Online_ck() == 0))) {
            if (act_ck(pl, 0, 0x22) == 0 || pl->stamina > 0) {
                if (p == 3) {
                    if (pl->work760 == 0) {
                        Pl_act_set(pl, 0, 0x24, 0);
                    } else {
                        basic_com_ck(pl);
                    }
                } else {
                    pl->work8F0 = 1;
                    Pl_act_set(pl, 0, 0x2C, 0);
                }
            } else {
                Pl_act_set(pl, 0, 0x21, 0);
            }
        } else {
            basic_com_ck(pl);
        }
        if (pl->x714 != 0) {
            Pl_act_set(pl, 0, 0x3B, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv032(PLW *pl, s32 arg1) {
    f32 sp50[3];
    f32 sp40[3];
    u8 s;
    s32 p;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x600);
        pl->work39C = 0;
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x26, 0, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0x27, 0, 0);
            break;
        case 2:
            pl_chr_set2(pl, 0x26, 4, 0);
            break;
        }
        pl->ang_y = pl->ang[1];
        break;
    case 1:
        if (arg1 != 1) {
            Pl_stamina_calc(pl, -2);
            if (pl->stamina < 0x4C) {
                Pl_act_set2(pl, 0, 0x22, 0);
                break;
            }
            if (pl->work81E != 0) {
                if ((((pl->x3A8 - pl->ang[1]) + 0x2000) & 0xFFFF) < 0x4001) {
                    goto b22;
                }
                goto b25;
            }
b22:
            Pl_act_set(pl, 0, 0x46, 0);
            break;
        }
        Pl_stamina_calc(pl, -1);
b25:
        if ((pl->sw.trg & 0x40) && (arg1 != 1) && (pl->stamina >= 0x4B)) {
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 0, 0x44, 4);
            break;
        }
        if (pl->sw.pow[0] >= 0x28) {
            pl->ang_y = stick_dir_set(pl, 0);
        }
        if ((front_land_ck2(85.0f, -60.0f, pl, 0) != 0) && (arg1 != 1)) {
            pl->ang_y = pl->ang[1];
            Pl_act_set2(pl, 0, 6, 0);
            break;
        }
        sp50[2] = 1.5f;
        sp50[0] = 0.0f;
        sp50[1] = 0.0f;
        flvecApplyMat33(sp40, sp50, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp40[0];
        pl->pos[2] = pl->pos[2] + sp40[2];
        p = stick_pow_get(pl, 0) & 0xFF;
        if ((p != 5) && ((Pl_master_ck(pl) == 1) || (Online_ck() == 0))) {
            if (act_ck(pl, 0, 0x22) == 0 || pl->stamina > 0) {
                if ((p == 3) && (arg1 != 1)) {
                    Pl_act_set(pl, 0, 0x24, 0);
                } else {
                    pl->work8F0 = 1;
                    Pl_act_set(pl, 0, 0x2C, 0);
                }
            } else {
                Pl_act_set(pl, 0, 0x21, 0);
            }
        } else {
            basic_com_ck(pl);
        }
        if (pl->x714 != 0) {
            Pl_act_set(pl, 0, 0x3B, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv033(PLW *pl, s32 arg1) {
    u8 s;
    s32 w;

    Pl_stamina_calc(pl, 4);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->flag12 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 5, 4, 0);
            pl->x05 = 2;
            pl->work08 = 0x96;
            break;
        }
        pl_chr_set2(pl, 0x15, 8, 0);
        pl->work08 = 10;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 5, 4, 0x90);
        }
        break;
    case 2:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x05++;
            pl_chr_set2(pl, 6, 4, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            Pl_stamina_calc(pl, pl->work882);
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013D5F0-0x0013DB40): pl_mv037, 038, 039, 040, 041, 042 (wall/ladder-type action handlers) */




#if !defined(G_PART) || G_PART == 11
static void pl_mv037(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x27, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 5, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv038(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0x8000, 0, 0);
        pl->flag604 = 0;
        pl_chr_set2(pl, 0x13, 2, 0);
        vib_set_pl(pl, 1);
        Pl_set_quake_sub(pl, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv039(PLW *pl, s32 unused_arg) {
    pl->work40E = 2;
    Pl_basic_flagset(pl, 0x8002, 0, 0);
    kabe_hosei(pl, 1);
    ex_kabe_ck(pl);
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv040(PLW *pl, s32 unused_arg) {
    int sp2C;
    u16 c;

    pl->work40E = 2;
    if (pl->x05 == 0) {
        pl->x05++;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        c = pl->char0;
        if (c != 0x22) {
            if (c == 0x23) {
                if (frame_check2(46.0f, pl, 0) != 0) {
                    pl_chr_set2(pl, 0x22, 0, 0x5A);
                } else {
                    pl_chr_set2(pl, 0x22, 0, 0x2E);
                }
            } else {
                pl_chr_set2(pl, 0x22, 4, 0);
            }
        }
    } else if (frame_check(46.0f, pl, 0) != 0 || frame_check(90.0f, pl, 0) != 0) {
        if (!(ex_kabe_ck(pl) & 0xFF) && (Pl_master_ck(pl) == 1)) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
    }
    if (front_land_ck(55.0f, 180.0f, 30.0f, pl, &sp2C) != 0) {
        pl->ang_y = pl->ang[1];
        Pl_act_set2(pl, 0, 0x1B, 0);
    }
    kabe_hosei(pl, 1);
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv041(PLW *pl, s32 unused_arg) {
    u16 c;

    pl->work40E = 2;
    if (pl->x05 == 0) {
        pl->x05++;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        c = pl->char0;
        if (c != 0x23) {
            if (c == 0x22) {
                if (frame_check2(92.0f, pl, 0) != 0) {
                    pl_chr_set2(pl, 0x23, 0, 0x2C);
                } else {
                    pl_chr_set2(pl, 0x23, 0, 0);
                }
            } else {
                pl_chr_set2(pl, 0x23, 4, 0);
            }
        }
    } else if (frame_check(0.0f, pl, 0) != 0 || frame_check(44.0f, pl, 0) != 0) {
        if (!(ex_kabe_ck(pl) & 0xFF) && (Pl_master_ck(pl) == 1)) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
    }
    if (pl->pos[1] <= pl->x5AC) {
        Pl_act_set(pl, 0, 0x2A, 0);
    }
    kabe_hosei(pl, 1);
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv042(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x24, 6, 0);
        kabe_hosei(pl, 1);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        } else {
            kabe_hosei(pl, 1);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013DB40-0x0013E408): pl_mv043, 044, 046, 047, 049, 051, 052, 053 (action handlers: ladder/hole/gather states) */




#if !defined(G_PART) || G_PART == 11
static void pl_mv043(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        wall_vec_set(pl, 0);
        pl_chr_set2(pl, 0x22, 4, 0);
        break;
    case 1:
        if (frame_check2(96.0f, pl, 0) != 0) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
        break;
    }
    kabe_hosei(pl, 1);
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv044(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x1E, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv046(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x195, -0xE, 0);
        break;
    case 1:
        if (frame_check2(130.0f, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x19A, 4, 0);
        }
        break;
    case 2:
        if (frame_check2(60.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x2F, 0xC);
            } else {
                pl->x05++;
            }
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 11
static void pl_mv047(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19A, 0, 0x3C);
        if (Pl_master_ck(pl) == 1) {
            Ana_item_set(pl);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x195, -4, 0xCE);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv049(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x43E = 0;
        pl->work40C = 6;
        Pl_basic_flagset(pl, 0, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        pl->work39C = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x3F5, 2, 0);
            break;
        }
        pl_chr_set2(pl, 0x3F4, 2, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->flag12 != 0)) {
            switch (pl->kind) {
            case 4:
                if ((pl->sw.an_trg & 0x20) && (frame_check3(10.0f, 42.0f, pl, 0) != 0) && (pl->x06 == 0)) {
                    pl->x06 = 1;
                    if (pl->sw.an_now & 0x800) {
                        pl->x06 = 2;
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->x06 = 3;
                    }
                }
                if (frame_check2(42.0f, pl, 0) != 0) {
                    t = pl->x06;
                    if (t != 0) {
                        if ((t & 0xFF) == 2) {
                            pl->ang[1] = pl->ang[1] + 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        if (pl->x06 == 3) {
                            pl->ang[1] = pl->ang[1] - 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        Pl_act_set2(pl, 1, 0x49, 0xC);
                    }
                }
                break;
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 12
static void pl_mv051(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    pl->work4D4 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl_chr_set2(pl, 0x1AA, -6, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x34, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 12
static void pl_mv052(PLW *pl, s32 unused_arg) {
    f32 sp20[3];
    u8 s;
    s32 w;
    s32 h;
    u16 r;

    pl->work40E = 2;
    pl->work4D4 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl->work08 = 0x78;
        pl_chr_set2(pl, 0x1AB, 0, 0);
        Eft06_set(4.0f, pl, 0, 0, 0xA);
        pl->x7BA = 0;
        pl->x7AA = 0;
        break;
    case 1:
        w = pl->work08 - 1;
        pl->work08 = w;
        if ((w <= 0) && (Pl_master_ck(pl) == 1)) {
            Pl_act_set2(pl, 0, 0x35, 0);
            break;
        }
        if ((pl->char0 == 0x1AB) && !((r = ran_suu(1)) & 0x7F)) {
            pl_chr_set2(pl, 0x1AC, 4, 0);
        } else if ((pl->char0 == 0x1AC) && (pl->work194 == 0)) {
            pl_chr_set2(pl, 0x1AB, 4, 0);
        }
        sp20[0] = 0.0f;
        sp20[1] = 20.0f;
        h = *(u16 *)&game_w.x1E % 60;
        sp20[2] = 0.0f;
        switch (h) {
        case 0:
        case 0xA:
        case 0x14:
            Eft06_set2(0.8f, pl, 4, 0x14, sp20);
            break;
        }
        Pl_vital_calc(pl, 1);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 12
static void pl_mv053(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    pl->work4D4 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl->ang[1] = (pl->ang[1] + 0x8000) & 0xFFFF;
        pl->ang_y = pl->ang[1];
        pl_chr_set2(pl, 0x1AD, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 12
static void pl_mv054(PLW *pl, s32 unused_arg) {
    f32 sp20[3];
    u16 sp2C;
    u8 sp2F;
    u8 s;
    s32 w;
    u8 b;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19E, 4, 0);
        action_timer_calc(pl, 0);
        pl->work08 = 0x3C;
        pl->x06 = 0x1E;
        pl->x8EE = 0;
        if ((St_unique_ck(pl, sp20, &sp2C, &sp2F) & 0xFFFF) == 0x11) {
            pl->work2D4 = sp2C;
            break;
        }
        pl->work2D4 = *(u16 *)&pl->ang_y;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        b = pl->x06;
        if (b > 0) {
            pl->x06 = b - 1;
        }
        if (Pl_master_ck(pl) == 1) {
            if (Game_clear_ck(1) == 1) {
                pl_to_normal(pl, 0, 6, 0);
                break;
            }
            if (pl->x06 == 0) {
                sougun_adj_sub(pl, 0x19E);
#ifdef VERSION_MHG      /* G pl_mv054: ballista ammo is G item 512; G also fires on button bit 0x01 */
                if ((pl->sw.trg & 0x21) && ((s16)Pl_item_num_ck(pl, 0x200) != 0)) {
#else
                if ((pl->sw.trg & 0x20) && ((s16)Pl_item_num_ck(pl, 0xA2) != 0)) {
#endif
                    Pl_act_set2(pl, 0, 0x48, 0xC);
                }
            }
            if ((pl->work08 == 0) && (pl->sw.pad08[2] & 0x40) && (pl->work88C == 0)) {
                pl_to_normal(pl, 0, 6, 0);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 42
static void pl_mv072(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19E, 0, 0x42);
        action_timer_calc(pl, 0);
        pl->work08 = 0x3C;
#ifdef VERSION_MHG      /* G pl_mv072: G item 512, and the pad rumbles */
        if ((Pl_master_ck(pl) != 1) || ((s16)Pl_item_num_ck(pl, 0x200) != 0)) {
            Pl_item_stack(pl, 0x200, -1);
            vib_set_pl(pl, 2);
            func_637F60(pl, 0);
        }
#else
        if ((Pl_master_ck(pl) != 1) || ((s16)Pl_item_num_ck(pl, 0xA2) != 0)) {
            Pl_item_stack(pl, 0xA2, -1);
            func_637F60(pl, 0);
        }
#endif
        break;
    case 1:
        pl->work08 = pl->work08 - 1;
        if (pl->work08 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 13
static void pl_mv055(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0xD3, 4, 0);
        action_timer_calc(pl, 0);
        vib_set_pl(pl, 3);
        Pl_set_quake_sub(pl, 5);
        func_637F60(pl, 1);
        break;
    case 1:
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xD4, 2, 0);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 13
static void pl_mv057(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 1, 0, 0);
        pl_chr_set2(pl, 9, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 13
static void pl_mv058(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x12, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 13
static void pl_mv059(PLW *pl, s32 unused_arg) {
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0xA, 8, 0);
        Pl_basic_flagset(pl, 1, 0, 0);
        pl->flag12 = 0;
        pl_flag_set(pl, 0x1000);
        pl->work8F0 = 1;
        pl->work08 = 0xF;
        break;
    case 1:
        w = pl->work08;
        if (w != 0) {
            pl->work08 = w - 1;
        }
        if (Pl_master_ck(pl) == 1) {
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x2D, 4);
                break;
            }
            sit_com_ck(pl);
        }
        break;
    }
}
#endif

/* plx12 - SLPM_654.95 0x0013EA40-0x0013EB0C: pl_mv060 (a short held pose of the hunter: sets the action flags and the motion 8 (12 for the
 * character 0x25), waits 12 frames, then returns to the normal action 0x1D). Field names are guesses. */





#if !defined(G_PART)
static void pl_mv060(PLW *pl, s32 unused_arg) {
    s16 v;
    u8 s;

    pl->work08++;
    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->char0 == 0x25) {
            v = 0xC;
        } else {
            v = 8;
        }
        Pl_basic_flagset(pl, 0x8001, 0, 0);
        pl_chr_set2(pl, 8, v, 0);
        pl->work08 = 0;
        break;
    case 1:
        if (pl->work08 >= 0xC) {
            Pl_act_set(pl, 0, 0x1D, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013EB10-0x0013ED48): pl_mv061, 062, 063 (action handlers) */




#if !defined(G_PART) || G_PART == 14
static void pl_mv061(PLW *pl, s32 unused_arg) {
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x25, 6, 0);
        Pl_basic_flagset(pl, 1, 0, 0);
        pl_flag_set(pl, 0x02000000);
        pl->work8F0 = 1;
        pl->work08 = 0xF;
        break;
    case 1:
        w = pl->work08;
        if (w != 0) {
            pl->work08 = w - 1;
        }
        if (Pl_master_ck(pl) == 1) {
            if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
                Pl_act_set(pl, 0, 0x2D, 0);
                break;
            }
            sit_com_ck(pl);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 14
static void pl_mv062(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x29, 2, 0);
        Pl_basic_flagset(pl, 0x8000, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 14
static void pl_mv063(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->ang[1] = *(u16 *)&pl->ang_y;
        pl_chr_set2(pl, 0x2A, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x24, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013ED50-0x0013F1B0): kabe_hosei (push along the wall), kabe_com_ck (wall-hug command), pl_mv064, pl_mv065 */




#if !defined(G_PART) || G_PART == 14
static void kabe_hosei(PLW *pl, int arg1) {
    f32 sp90[3];
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];

    sp70[0] = 0;
    sp70[2] = 0;
    sp70[1] = pl->ang[1];
    cpRotMatrix(sp70, sp30);
    sp90[0] = 0.0f;
    sp90[1] = 0.0f;
    sp90[2] = -3.0f;
    if ((s16)arg1 != 0) {
        sp90[2] *= -1.0f;
    }
    flvecApplyMat33(sp80, sp90, sp30);
    pl->pos[0] = pl->pos[0] + sp80[0];
    pl->pos[2] = pl->pos[2] + sp80[2];
}
#endif

#if !defined(G_PART) || G_PART == 14
static void kabe_com_ck(PLW *pl) {
    s32 p;
    s32 d;

    if (Pl_master_ck(pl) != 0) {
        p = stick_pow_get(pl, 0) & 0xFF;
        switch (p) {
        case 1:
        case 2:
        case 3:
            d = ((stick_dir_set(pl, 0) & 0xFFFF) - *(u16 *)&pl->ang[1]) & 0xFFFF;
            if ((d >= 0x2000) && (d < 0x6001)) {
                if (act_ck(pl, 0, 0x42) == 0) {
                    Pl_act_set(pl, 0, 0x42, 0);
                }
            } else if ((d >= 0xA000) && (d < 0xE001)) {
                if (act_ck(pl, 0, 0x41) == 0) {
                    Pl_act_set(pl, 0, 0x41, 0);
                }
            } else if ((act_ck(pl, 0, 0x40) == 0) && (act_ck(pl, 0, 0x43) == 0)) {
                Pl_act_set(pl, 0, 0x43, 0);
            }
            break;
        default:
            if ((act_ck(pl, 0, 0x40) == 0) && (act_ck(pl, 0, 0x43) == 0)) {
                Pl_act_set(pl, 0, 0x43, 0);
            }
            break;
        }
        if (!(pl->sw.trg & 0x40)) {
            if (!(wall_vec_set(pl, 1) & 0xFF)) {
                goto go;
            }
        } else {
go:
            pl->work8F0 = 1;
            Pl_act_set2(pl, 0, 0x2C, 0);
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 14
static void pl_mv064(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x2B, 2, 0);
        } else {
            pl_chr_set2(pl, 0x2B, 2, 0x1E);
        }
        Pl_basic_flagset(pl, 0x8000, 0, 0);
        kabe_hosei(pl, 0);
        break;
    case 1:
        kabe_hosei(pl, 0);
        if (frame_check2(30.0f, pl, 0) != 0) {
            kabe_com_ck(pl);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 14
static void pl_mv065(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x2C, 2, 0);
        } else {
            pl_chr_set2(pl, 0x2D, 2, 0);
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        kabe_hosei(pl, 0);
        break;
    case 1:
        kabe_hosei(pl, 0);
        kabe_com_ck(pl);
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013F1B0-0x0013F568): pl_mv068 (jump over an obstacle), pick_set_sub (pick up an item/point: stock request and pouch selection) */




#if !defined(G_PART) || G_PART == 14
static void pl_mv068(PLW *pl, s32 unused_arg) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x2E, 4, 0);
        Pl_basic_flagset(pl, 2, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        pl->work40C = 3;
        break;
    case 1:
        pl->work40C = 3;
        if (pl->work194 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x2F, 0, 0);
            pl->pos[1] = pl->pos[1] + 100.0f;
            sp30[2] = 16.0f;
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x60));
            pl->vel[0] = sp20[0];
            pl->vel[1] = 3.0f;
            pl->vel[2] = sp20[2];
            rate_clear_g(pl);
            rate_g_calc(pl, 8);
        }
        break;
    case 2:
        pl->work40C = 3;
        rate_add_g(pl);
        if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= pl->x5AC)) {
            pl->x05++;
            pl->st = 0;
            pl->pos[1] = pl->x5AC;
            pl_chr_set2(pl, 0x30, 0, 0);
            vib_set_pl(pl, 1);
            Pl_set_quake_sub(pl, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl->work40C = 0;
            if (pl->sw.now & 0x10) {
                Pl_act_set2(pl, 0, 0x45, 0);
                break;
            }
            pl->work8F0 = 1;
            Pl_act_set2(pl, 0, 0x2C, 0);
            break;
        }
        if (frame_check2(30.0f, pl, 0) == 0) {
            pl->work40C = 2;
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 14
static void pick_set_sub(PLW *pl, u16 arg1) {
    s32 r;
    s16 q;

    if (Game_clear_ck(1) != 1) {
        if (arg1 == 0) {
            r = St_pick_ck2(pl);
        } else {
            r = Ext_pick_point_ck2(pl);
        }
        r &= 0xFFFF;
        if ((r == 0) || (r == 0xFFFE) || (r == 0xFFFF)) {
            if (r != 0xFFFE) {
                if (arg1 == 0) {
                    set01_set(0, 0, 0);
                    return;
                }
                set01_set(0, 0xE, 0);
                return;
            }
            adx_se_set(pl, 8);
            set01_set2(GSTR(lit_3557, gptr_00528414));
            return;
        }
        q = ItemStockRequest(pl, r, pl->x8E6, 3);
        switch (q) {
        case 0:
        case 1:
        case 2:
            if (Check_hold_item(r) & 0xFF) {
                pl->x07 = 1;
            }
            if (pl->item[pl->work888].id != 0 && Item_data[pl->item[pl->work888].id][1] != 1) {
                pl->work888 = item_sel_sub(pl, pl->work888, 0);
            }
            break;
        }
    }
}
#endif

/* Player code (SLPM_654.95 0x0013F570-0x0013F968): pl_mv071 (pick up / pull out an item or egg) */




#ifdef VERSION_MHG      /* G pl_mv071 (game 0x554280): skill 170 plays the gather motion at 2.6x */
#define PL_MV071_FAST(pl) do { if (Pl_Skill_ck(pl, 170) == 1) { (pl)->chr_spd0 = 2.6f; (pl)->chr_spd1 = 2.6f; } } while (0)
#endif
#if !defined(G_PART) || G_PART == 43
static void pl_mv071(PLW *pl, u32 arg1) {
    u16 n;
    u16 r;
    s32 h;
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        switch (arg1) {
        case 0:
        case 1:
            pl_chr_set2(pl, 0x195, -0xE, 0);
#ifdef VERSION_MHG
            PL_MV071_FAST(pl);
#endif
            break;
        case 2:
            if (Pl_master_ck(pl) == 1) {
                set01_set(0, 0xA, 0);
            }
            pl_chr_set2(pl, 0x195, -0xE, 0);
#ifdef VERSION_MHG
            PL_MV071_FAST(pl);
#endif
            break;
        case 3:
            pl_chr_set2(pl, 0x1A3, -6, 0);
#ifdef VERSION_MHG
            PL_MV071_FAST(pl);
#endif
            break;
        case 4:
            pl_chr_set2(pl, 0x19F, 0, 0);
            break;
        case 5:
            pl_chr_set2(pl, 0x1A2, 0, 0);
            break;
        }
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && (frame_check(120.0f, pl, 0) != 0) && (Game_clear_ck(1) == 0)) {
            n = 1;
            switch (arg1) {
            case 4:
            case 5:
                switch (pl->work88A) {
                case PICKAXE0:
                    n = 3;
                    break;
                case PICKAXE0 + 1:
                    n = 0xA;
                    break;
                case PICKAXE0 + 2:
                    n = 0xF;
                    break;
                case BUGNET0:
                    n = 3;
                    break;
                case BUGNET0 + 1:
                    n = 0xA;
                    break;
                case BUGNET0 + 2:
                    n = 0xF;
                    break;
                }
#ifdef VERSION_MHG
                /* G: skills 103 / 104 halve the tool's odds now and then (1 in 4 / 1 in 2) */
                if (Pl_Skill_ck(pl, 103) == 1 && ((r = ran_suu(1)) & 3) == 0) {
                    n = ((u32)n >> 1) + 1;
                }
                if (Pl_Skill_ck(pl, 104) == 1 && ((r = ran_suu(1)) & 1) == 0) {
                    n = ((u32)n >> 1) + 1;
                }
#define PL_MV071_SK0 48
#define PL_MV071_SK1 49
#else
#define PL_MV071_SK0 0x32
#define PL_MV071_SK1 0x33
#endif
                h = (ran_suu(1) & 0xFFFF) % n;
                if (h == 0) {
                    if ((Pl_Skill_ck(pl, PL_MV071_SK0) != 1 || ((r = ran_suu(1)) & 3)) && (Pl_Skill_ck(pl, PL_MV071_SK1) != 1 || ((r = ran_suu(1)) & 1))) {
                        set01_set(1, 0xE, (s16)pl->work88A);
                        Pl_item_stack(pl, pl->work88A, -1);
                    }
                }
            default:
                pick_set_sub(pl, 0);
                break;
            case 2:
                Item_regained(pl, 1);
                break;
            case 3:
                pick_set_sub(pl, 1);
                break;
            }
        } else if (pl->work194 == 0) {
            if (pl->x07 == 0) {
                switch (arg1) {
                case 0:
                case 1:
                case 2:
                    pl_to_normal(pl, 0, 0xA, 0);
                    break;
                case 3:
                case 5:
                    pl_to_normal(pl, 0, 6, 0);
                    break;
                default:
                    pl_to_normal(pl, 0, 4, 0);
                    break;
                }
            } else {
                egg_set(pl);
                Pl_act_set2(pl, 5, 9, 0);
            }
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0013F970-0x00140100): pl_mv093 (pick up/gather), pl_mv076, pl_mv077, fish_com_ck (fishing commands), pl_mv078, pl_mv079 (fishing handlers) */




#if !defined(G_PART)
static void pl_mv093(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 1, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x19B, -6, 0);
            break;
        }
        pl_chr_set2(pl, 0x1A6, -6, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            if (arg1 == 0) {
                pl_chr_set2(pl, 0x195, -4, 0x24);
                break;
            }
            pl_chr_set2(pl, 0x1A3, 0, 0x44);
        }
        break;
    case 2:
        if ((Pl_master_ck(pl) == 1) && (frame_check(180.0f, pl, 0) != 0)) {
            pick_set_sub(pl, arg1);
        }
        if (arg1 == 0) {
            if (frame_check2(200.0f, pl, 0) != 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x19C, -4, 0);
                break;
            }
        } else if (frame_check2(294.0f, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x1A7, -2, 0);
            break;
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            if (pl->x07 == 0) {
                Pl_act_set2(pl, 0, 0x1D, 2);
                break;
            }
            egg_set(pl);
            Pl_act_set2(pl, 5, 9, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 15
static void pl_mv076(PLW *pl, s32 unused_arg) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl->work8CA = 0;
        pl->work8D0 = 0;
        pl_chr_set2(pl, 0x320, -4, 0);
        BBQcamera_set(pl);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x55, 2);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 15
static void pl_mv077(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x321, 0, 0);
            break;
        }
        pl_chr_set2(pl, 0x322, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void fish_com_ck(PLW *pl) {
    if (Pl_master_ck(pl) != 0) {
        if (pl->flag14 != 0) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        if (Game_clear_ck(1) == 1) {
            pl_to_normal(pl, 0, 4, 0);
            return;
        }
        switch (pl->flag15) {
        case 0x50:
            if (pl->sw.trg & 0x20) {
                if (pl->x881 > 0) {
                    Pl_act_set2(pl, 0, 0x53, 0);
                    break;
                }
                if (pl->x8EA == 1) {
                    Pl_act_set2(pl, 0, 0x64, 0);
                    break;
                }
                Pl_act_set2(pl, 0, 0x54, 0);
            }
            break;
        case 0x4E:
            if (pl->sw.trg & 0x20) {
                Pl_act_set(pl, 0, 0x4F, 0);
            }
            if (pl->sw.trg & 0x40) {
                pl_to_normal(pl, 0, 4, 0);
            }
            break;
        case 0x51:
        case 0x52:
            if (pl->sw.trg & 0x20) {
                Pl_act_set2(pl, 0, 0x53, 0);
            }
            break;
        case 0x4F:
        case 0x53:
            break;
        }
    }
}
#endif

#if !defined(G_PART) || G_PART == 16
static void pl_mv078(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl->work08 = 0;
        pl->x07 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x324, 0, 0);
            break;
        }
        pl_chr_set2(pl, 0x326, 2, 0);
        break;
    case 1:
        if ((pl->x881 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
        }
        fish_com_ck(pl);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 16
static void pl_mv079(PLW *pl, s32 unused_arg) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl->x881 = 0;
        pl->fish_time = 0;
        pl->x8EA = 1;
        pl_chr_set2(pl, 0x325, -2, 0);
        break;
    case 1:
        if (frame_check(104.0f, pl, 0) != 0) {
#ifdef VERSION_MHG
            if (pl->work88A == 0x6B) {        /* G pl_mv079: frog (MH1 0x7D) */
#else
            if (pl->work88A == 0x7D) {
#endif
                func_555A90(pl, 1);
            } else {
                func_555A90(pl, 0);
            }
        }
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x50, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00140100-0x001414B4): pl_mv083..pl_mv101: fishing/BBQ/flute/pitfall and other item-use action handlers */





#if !defined(G_PART) || G_PART == 16
static void pl_mv083(PLW *pl, s32 unused_arg) {
    EMW *e;
    s16 i;
    u8 s;

    e = em_work;
    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        pl->work2DA = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        i = 0;
        do {
            if ((e->stg == pl->stg) && (e->x8B9 != 0) && (e->x87C == pl->id)) {
                pl->x06 = 1;
            }
            i++;
            e++;
        } while (i < 0x14);
        pl_chr_set2(pl, 0x327, 2, 0);
        break;
    case 1:
        if (pl->x06 == 0) {
            if (frame_check(118.0f, pl, 0) != 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x328, 2, 0);
                break;
            }
            i = 0;
            do {
                if ((e->stg == pl->stg) && (e->x8B9 != 0) && (e->x87C == pl->id)) {
                    pl->x06 = 1;
                }
                i++;
                e++;
            } while (i < 0x14);
            break;
        }
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x07++;
            if (pl->x07 >= 2) {
                pl->x05 = 3;
                pl_chr_set2(pl, 0x32A, 4, 0);
                if (Game_clear_ck(1) == 0) {
                    Pl_item_stack(pl, pl->work88A, -1);
                }
            }
        }
        break;
    case 2:
        if ((frame_check(120.0f, pl, 0) != 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(1, 0xB, (s16)pl->fish_time);
            adx_se_set(pl, 5);
            Pl_item_stack(pl, pl->work88A, -1);
            if ((s16)ItemStockRequest(pl, pl->fish_time, pl->x8E6, 0) == 3) {
#ifdef MHG_PS2
                game_func_0054B4B0(pl->fish_time);     /* G: its own 'box full' notice (20 bytes) */
#else
                set01_set(1, 3, (s16)pl->fish_time);
#endif
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 2, 0x24);
        }
        break;
    case 4:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 16
static void pl_mv084(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x329, 2, 0);
        break;
    case 1:
        if ((frame_check(120.0f, pl, 0) != 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            if (arg1 == 0) {
                adx_se_set(pl, 8);
                Pl_item_stack(pl, pl->work88A, -1);
                set01_set(0, 3, 0);
            } else {
                set01_set(0, 0xD, 0);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv085(PLW *pl, s32 unused_arg) {
    u8 s;
    u16 r;
    s16 t;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        r = ran_suu(1);
        pl_chr_set2(pl, 0x323, 4, r & 0x3F);
        pl->work8CA = 0;
        pl->work8D0 = 0;
        adx_se_set(pl, 0xA);
        break;
    case 1:
#ifdef VERSION_MHG
        /* G pl_mv085 (game 0x555380): G item ids (raw meat 37 -> rare 38 / well-done 39 / burnt 41); with the gourmet
           spit (119) the burn point is 20 later (MH1 10) and 275-284 gives well-done steak G (40, outcome 4) */
        pl->work8CA = pl->work8CA + 1;
        t = pl->work8CA;
        {
            s16 burn = 270 + (pl->work88A == 0x77 ? 20 : 10);
            if (t >= burn) {
                pl->work8D0 = 3;
            } else if (t >= 270) {
                pl->work8D0 = (pl->work88A == 0x77 && t >= 275 && t < 285) ? 4 : 2;
            } else if (t >= 0xB4) {
                pl->work8D0 = 1;
            } else {
                pl->work8D0 = 0;
            }
        }
        if (Pl_master_ck(pl) == 1 && ((pl->sw.trg & 0x20) || pl->work8CA >= 0x12C)) {
            u16 got = 0;
            adx_se_stop(pl);
            switch (pl->work8D0) {
            case 0:
                if (Game_clear_ck(1) == 0) {
                    set01_set(0, 0xB, 0);
                }
                Pl_act_set2(pl, 0, 0x4B, 0);
                break;
            case 1:
                if (Game_clear_ck(1) == 0) {
                    Pl_item_stack(pl, 0x25, -1);
                    set01_set(1, 9, 0x26);
                    adx_se_set(pl, 0xB);
                    got = 0x26;
                }
                Pl_act_set2(pl, 0, 0x4B, 0);
                break;
            case 2:
            case 4:
                if (Game_clear_ck(1) == 0) {
                    int se;
                    Pl_item_stack(pl, 0x25, -1);
                    switch (FLD8(system_w, 0x1A)) {
                    case 0:
                        se = 0xC;
                        break;
                    default:
                        se = 0xE;
                        break;
                    }
                    if (pl->work8D0 == 4) {
                        se = 0xF;
                    }
                    adx_se_set(pl, se);
                    got = pl->work8D0 == 4 ? 0x28 : 0x27;
                    set01_set(1, 9, got);
                }
                Pl_act_set2(pl, 0, 0x4D, 0);
                break;
            case 3:
                if (Game_clear_ck(1) == 0) {
                    Pl_item_stack(pl, 0x25, -1);
                    set01_set(1, 0xA, 0x29);
                    adx_se_set(pl, 0xD);
                    got = 0x29;
                }
                Pl_act_set2(pl, 0, 0x4B, 0);
                break;
            }
            /* G's ItemStockRequest (5th argument 1, func_0011CD20) answers 3 when the pouch cannot take it: message */
            if (got != 0 && (s16)ItemStockRequest(pl, got, pl->x8E6, 0) == 3) {
                set01_set(1, 3, (s16)got);
            }
        }
        break;
#else
        pl->work8CA = pl->work8CA + 1;
        t = pl->work8CA;
        if (t >= 0x118) {
            pl->work8D0 = 3;
        } else if (t >= 0x10E) {
            pl->work8D0 = 2;
        } else if (t >= 0xB4) {
            pl->work8D0 = 1;
        } else {
            pl->work8D0 = 0;
        }
        if (Pl_master_ck(pl) == 1) {
            if (!(pl->sw.trg & 0x20)) {
                if (pl->work8CA >= 0x12C) {
                    goto go;
                }
            } else {
go:
                adx_se_stop(pl);
                switch (pl->work8D0) {
                case 0:
                    if (Game_clear_ck(1) == 0) {
                        set01_set(0, 0xB, 0);
                    }
                    Pl_act_set2(pl, 0, 0x4B, 0);
                    break;
                case 1:
                    if (Game_clear_ck(1) == 0) {
                        Pl_item_stack(pl, 0x12, -1);
                        ItemStockRequest(pl, 0x13, pl->x8E6, 0);
                        set01_set(1, 9, 0x13);
                        adx_se_set(pl, 0xB);
                    }
                    Pl_act_set2(pl, 0, 0x4B, 0);
                    break;
                case 2:
                    if (Game_clear_ck(1) == 0) {
                        Pl_item_stack(pl, 0x12, -1);
                        ItemStockRequest(pl, 0x14, pl->x8E6, 0);
                        set01_set(1, 9, 0x14);
                        switch (FLD8(system_w, 0x1A)) {
                        case 0:
                            adx_se_set(pl, 0xC);
                            break;
                        default:
                        case 2:
                        case 3:
                            adx_se_set(pl, 0xE);
                            break;
                        }
                    }
                    Pl_act_set2(pl, 0, 0x4D, 0);
                    break;
                case 3:
                    if (Game_clear_ck(1) == 0) {
                        Pl_item_stack(pl, 0x12, -1);
                        ItemStockRequest(pl, 0x15, pl->x8E6, 0);
                        set01_set(1, 0xA, 0x15);
                        adx_se_set(pl, 0xD);
                    }
                    Pl_act_set2(pl, 0, 0x4B, 0);
                    break;
                }
            }
        }
        break;
#endif
    }
}
#endif

#if !defined(G_PART) || G_PART == 17
static void pl_mv086(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x19F, 0, 0);
        Pile_on(pl);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 17
static void pl_mv087(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40E = 2;
    pl->work40C = 5;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->work8C2 = 1;
        pl->work8F3 = 0;
        if (Pl_master_ck(pl) == 1) {
            adx_se_set(pl, 3);
        }
        break;
    case 1:
        if ((pl->work8C2 == 0) || (Game_clear_ck(1) == 1)) {
            pl->work40C = 0;
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 17
static void pl_mv088(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x196, 4, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0x1A0, 4, 0);
            break;
        }
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x71, 0);
            } else {
                pl_to_normal(pl, 0, 4, 0);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 17
static void pl_mv113(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        if (Pl_master_ck(pl) == 1) {
            Basic_item_set(pl);
        }
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 17
static void pl_mv089(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 1, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x199, 2, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (frame_check(20.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x14, 0xC);
            } else {
                pl->x05++;
                pl_chr_set2(pl, 8, 6, 0);
            }
        }
        break;
    case 2:
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 17
static void pl_mv091(PLW *pl, s32 unused_arg) {
    u8 s;
    u8 v;
    u16 r;

    pl->work40C = 3;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->x8C6 = 0x3C;
        v = pl->work8C7;
        if (v < 7) {
            pl->work8C7 = v + 1;
            if ((pl->work8C7 >= 4) && ((r = ran_suu(1)) & 1)) {
                pl->work8C7 = 7;
            }
        } else {
            pl->work8C7 = 7;
        }
        pl->work08 = 0x3C;
        break;
    case 1:
        if (pl->x8C6 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 44
static void pl_mv094(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
#ifdef VERSION_MHG
        pl->flag12 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x1A4, 4, 0);
            pl_flag_set(pl, 0x10000);   /* G: only the first variant sets the flag, after the motion */
        } else {
            pl_chr_set2(pl, 0x1A8, 4, 0);
        }
#else
        pl_flag_set(pl, 0x10000);
        pl->flag12 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x1A4, 4, 0);
        } else {
            pl_chr_set2(pl, 0x1A8, 4, 0);
        }
#endif
        pl->work39C = 0;
        break;
    case 1:
        switch (arg1) {
        case 0:
            if (pl->x06 >= 3 || ((frame_check2(44.0f, pl, 0) != 0) && (pl->sw.trg & 0x40))) {
                Pl_act_set2(pl, 0, 0x5F, 0);
            } else if (frame_check(pl->work1A8, pl, 0) != 0) {
                pl->x06++;
            }
            break;
        case 1:
            if (frame_check(pl->work1A8, pl, 0) != 0) {
                pl->x06++;
                if (pl->x06 >= 2) {
                    Pl_act_set2(pl, 0, 0x63, 0);
                }
            }
            break;
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv095(PLW *pl, s32 arg1) {
    u16 n;
    s32 h;
    u16 r;
    u8 s;

    s = pl->x05;
    n = 1;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        if (arg1 == 0) {
            n = 5;
            pl_chr_set2(pl, 0x1A5, 2, 0);
        } else {
            pl_chr_set2(pl, 0x1A9, 4, 0);
#ifdef VERSION_MHG      /* G pl_mv095 (game 0x555E80): only the master plays the horn's effect; G horn ids */
            if (Pl_master_ck(pl) == 1) {
                Fue_item_set(pl);
            }
            switch (pl->work88A) {
            case 0x7C:
            case 0x7D:
                n = 0xC;
                break;
            case 0x7E:
            case 0x7F:
                n = 8;
                break;
            }
#else
            Fue_item_set(pl);
            switch (pl->work88A) {
            case 0x8A:
                n = 0xC;
                break;
            case 0x8B:
                n = 0xC;
                break;
            case 0x8C:
                n = 8;
                break;
            case 0x8D:
                n = 8;
                break;
            }
#endif
        }
        if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            h = (ran_suu(1) & 0xFFFF) % n;
#ifdef VERSION_MHG      /* G: skill 48 or 108 saves the horn 1 time in 4, skill 49 1 in 2 */
            if ((h == 0) && ((Pl_Skill_ck(pl, 48) != 1 && Pl_Skill_ck(pl, 108) != 1) || ((r = ran_suu(1)) & 3)) && ((Pl_Skill_ck(pl, 49) != 1) || ((r = ran_suu(1)) & 1))) {
#else
            if ((h == 0) && ((Pl_Skill_ck(pl, 0x32) != 1) || ((r = ran_suu(1)) & 3)) && ((Pl_Skill_ck(pl, 0x33) != 1) || ((r = ran_suu(1)) & 1))) {
#endif
                set01_set(1, 0xE, (s16)pl->work88A);
                Pl_item_stack(pl, pl->work88A, -1);
            }
        }
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 45
static void pl_mv097(PLW *pl, s32 unused_arg) {
    s32 n;
    s32 t;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x19D, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
#ifdef VERSION_MHG      /* G pl_mv097 (game 0x5560A0): skill 74 skips ahead to frame 218 of the motion */
        if (Pl_Skill_ck(pl, 74) == 1 && frame_check(98.0f, pl, 0) != 0) {
            pl_chr_set2(pl, 0x19D, 0, 218);
        }
#endif
        if ((frame_check(260.0f, pl, 0) != 0) && (Game_clear_ck(1) == 0)) {
            Pl_item_stack(pl, pl->work88A, -1);
#ifdef VERSION_MHG
            switch (pl->work88A) {
            case 0x78:          /* whetstone */
            case 0xAA:          /* whetfish */
                n = 0x96;
                break;
            case 0x1FE:         /* mini whetstone */
                n = 0x64;
                break;
            case 0x9C:          /* earth crystal: MH1 let it sharpen; G still lists it here, but G item_action_set no longer sends it */
                n = 0xC8;
                break;
            }
#else
            switch (pl->work88A) {
            case 0x69:
            case 0x5E:
                n = 0x96;
                break;
            case 0x9B:
                n = 0x64;
                break;
            case 0x6A:
                n = 0xC8;
                break;
            }
#endif
            if (Pl_Skill_ck(pl, 0x18) == 1) {
                t = (s16)n;
                n = (s16)(t + t / 4);
            }
            if (Pl_Skill_ck(pl, 0x19) == 1) {
                t = (s16)n;
                n = (s16)(t + t / 2);
            }
#ifdef VERSION_MHG
            if (Pl_Skill_ck(pl, 73) == 1) {     /* G: skill 73 halves it */
                n = (s16)((s16)n / 2);
            }
#endif
            Pl_slash_calc(pl, n);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 18
static void pl_mv101(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 1, 0, 0);
        pl->pch_on = 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 7, 2, 0);
            break;
        }
        pl->x05++;
        pl_chr_set2(pl, 8, 2, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 8, 6, 0);
            sougun_adj_sub(pl, 8);
            break;
        }
        sougun_adj_sub(pl, 7);
        break;
    case 2:
        sougun_adj_sub(pl, 8);
        if (Pl_master_ck(pl) != 0) {
            if (!(pl->sw.trg & 0x40)) {
                if (Game_clear_ck(1) == 1) {
                    goto go;
                }
            } else {
go:
                Pl_view_reset(pl);
                Pl_act_set2(pl, 0, 0x1D, 0);
            }
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x001414C0-0x00141BA0): pl_mv103..pl_mv112 (kneel/pitfall/barrel/misc action handlers) */





#if !defined(G_PART) || G_PART == 18
static void pl_mv103(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x3C, 4, 0);
        pl->work90A = 0;
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && ((pl->sw.trg & 0x40) || (pl->work90A != 0))) {
            Pl_act_set2(pl, 0, 0x69, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 18
static void pl_mv104(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->work90A = 0;
        pl->work08 = 0;
        break;
    case 1:
        w = pl->work08 + 1;
        pl->work08 = w;
        if ((w >= 0x259) || (pl->work90A != 0)) {
            if (pl->work90A == 1) {
                Pl_act_set2(pl, 0, 0x6A, 0);
                break;
            }
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 18
static void pl_mv105(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x3D, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 18
static void pl_mv106(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work90B = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    Pl_view_reset(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x280, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_mv107(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl->work40C = 6;
        Pl_stamina_calc(pl, -0x4B);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x3F6, 2, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0x3F7, 2, 0);
            break;
        case 2:
        case 3:
            pl_chr_set2(pl, 0x3F8, 2, 0);
            break;
        }
        break;
    case 1:
        if ((arg1 == 3) && (frame_check2(28.0f, pl, 0) == 0)) {
            sp40[2] = -8.0f;
            sp40[0] = 0.0f;
            sp40[1] = 0.0f;
            flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp30[0];
            pl->pos[2] = pl->pos[2] + sp30[2];
        }
        if (pl->work194 == 0) {
            if (arg1 == 2) {
                pl_to_normal(pl, 0, 6, 0);
            } else {
                pl_to_normal(pl, 0, 8, 0);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 19
static void pl_mv111(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->work08 = 0;
        pl_chr_set2(pl, 0x1B0, 6, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (frame_check2(78.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x70, 0xC);
            } else {
                pl->x05++;
            }
        }
        break;
    case 2:
        pl_chr_set2(pl, 0x1B0, 0, 0x4E);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 19
static void pl_mv112(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->work08 = 0;
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x1B0, 0, 0x4E);
        Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        if (Pl_master_ck(pl) == 1) {
            Taru_item_set(pl);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 19
static void pl_normal(PLW *pl) {
    switch (pl->flag15) {
    case 0x0:
        pl_mv000(pl, 0);
        break;
    case 0x1:
        pl_mv001(pl, 0);
        break;
    case 0x2:
        pl_mv001(pl, 1);
        break;
    case 0x3:
        pl_mv001(pl, 2);
        break;
    case 0x4:
        pl_mv004(pl, 0);
        break;
    case 0x5:
        pl_mv004(pl, 1);
        break;
    case 0x6:
        pl_mv006(pl, 0);
        break;
    case 0x7:
        pl_mv001(pl, 8);
        break;
    case 0x8:
        pl_mv008(pl, 0);
        break;
    case 0x9:
        pl_mv006(pl, 1);
        break;
    case 0xA:
        pl_mv004(pl, 2);
        break;
    case 0xB:
        pl_mv014(pl, 1);
        break;
    case 0xD:
        pl_mv013(pl, 0);
        break;
    case 0xE:
        pl_mv014(pl, 0);
        break;
    case 0xF:
        pl_mv004(pl, 3);
        break;
    case 0x10:
        pl_mv004(pl, 4);
        break;
    case 0x11:
        pl_mv017(pl, 0);
        break;
    case 0x12:
        pl_mv001(pl, 3);
        break;
    case 0x13:
        pl_mv001(pl, 4);
        break;
    case 0x14:
        pl_mv024(pl, 1);
        break;
    case 0x15:
        pl_mv021(pl, 0);
        break;
    case 0x16:
        pl_mv022(pl, 0);
        break;
    case 0x17:
        pl_mv004(pl, 5);
        break;
    case 0x18:
        pl_mv024(pl, 0);
        break;
    case 0x19:
        pl_mv025(pl, 0);
        break;
    case 0x1A:
        pl_mv001(pl, 5);
        break;
    case 0x1B:
        pl_mv017(pl, 1);
        break;
    case 0x1C:
        pl_mv028(pl, 0);
        break;
    case 0x1D:
        pl_mv008(pl, 1);
        break;
    case 0x1E:
        pl_mv030(pl, 0);
        break;
    case 0x1F:
        pl_mv031(pl, 0);
        break;
    case 0x20:
        pl_mv032(pl, 0);
        break;
    case 0x21:
        pl_mv033(pl, 0);
        break;
    case 0x22:
        pl_mv032(pl, 1);
        break;
    case 0x24:
        pl_mv001(pl, 6);
        break;
    case 0x25:
        pl_mv037(pl, 0);
        break;
    case 0x26:
        pl_mv038(pl, 0);
        break;
    case 0x27:
        pl_mv039(pl, 0);
        break;
    case 0x28:
        pl_mv040(pl, 0);
        break;
    case 0x29:
        pl_mv041(pl, 0);
        break;
    case 0x2A:
        pl_mv042(pl, 0);
        break;
    case 0x2B:
        pl_mv043(pl, 0);
        break;
    case 0x2C:
        pl_mv044(pl, 0);
        break;
    case 0x2D:
        pl_mv028(pl, 1);
        break;
    case 0x2E:
        pl_mv046(pl, 0);
        break;
    case 0x2F:
        pl_mv047(pl, 0);
        break;
    case 0x30:
        pl_mv021(pl, 1);
        break;
    case 0x31:
        pl_mv049(pl, 0);
        break;
    case 0x32:
        pl_mv049(pl, 1);
        break;
    case 0x33:
        pl_mv051(pl, 0);
        break;
    case 0x34:
        pl_mv052(pl, 0);
        break;
    case 0x35:
        pl_mv053(pl, 0);
        break;
    case 0x36:
        pl_mv054(pl, 0);
        break;
    case 0x37:
        pl_mv055(pl, 0);
        break;
    case 0x38:
        pl_mv021(pl, 2);
        break;
    case 0x39:
        pl_mv057(pl, 0);
        break;
    case 0x3A:
        pl_mv058(pl, 0);
        break;
    case 0x3B:
        pl_mv059(pl, 0);
        break;
    case 0x3C:
        pl_mv060(pl, 0);
        break;
    case 0x3D:
        pl_mv061(pl, 0);
        break;
    case 0x3E:
        pl_mv062(pl, 0);
        break;
    case 0x3F:
        pl_mv063(pl, 0);
        break;
    case 0x40:
        pl_mv064(pl, 0);
        break;
    case 0x41:
        pl_mv065(pl, 0);
        break;
    case 0x42:
        pl_mv065(pl, 1);
        break;
    case 0x43:
        pl_mv064(pl, 1);
        break;
    case 0x44:
        pl_mv068(pl, 0);
        break;
    case 0x45:
        pl_mv032(pl, 2);
        break;
    case 0x46:
        pl_mv031(pl, 1);
        break;
    case 0x47:
        pl_mv071(pl, 0);
        break;
    case 0x48:
        pl_mv072(pl, 0);
        break;
    case 0x49:
        pl_mv001(pl, 7);
        break;
    case 0x4A:
        pl_mv071(pl, 3);
        break;
    case 0x4B:
        pl_mv077(pl, 1);
        break;
    case 0x4C:
        pl_mv076(pl, 0);
        break;
    case 0x4D:
        pl_mv077(pl, 0);
        break;
    case 0x4E:
        pl_mv078(pl, 0);
        break;
    case 0x4F:
        pl_mv079(pl, 0);
        break;
    case 0x50:
        pl_mv078(pl, 1);
        break;
    case 0x51:
        pl_mv071(pl, 4);
        break;
    case 0x52:
        pl_mv071(pl, 5);
        break;
    case 0x53:
        pl_mv083(pl, 0);
        break;
    case 0x54:
        pl_mv084(pl, 0);
        break;
    case 0x55:
        pl_mv085(pl, 0);
        break;
    case 0x56:
        pl_mv086(pl, 0);
        break;
    case 0x57:
        pl_mv087(pl, 0);
        break;
    case 0x58:
        pl_mv088(pl, 0);
        break;
    case 0x59:
        pl_mv089(pl, 0);
        break;
    case 0x5A:
        pl_mv088(pl, 1);
        break;
    case 0x5B:
        pl_mv091(pl, 0);
        break;
    case 0x5C:
        pl_mv071(pl, 2);
        break;
    case 0x5D:
        pl_mv093(pl, 0);
        break;
    case 0x5E:
        pl_mv094(pl, 0);
        break;
    case 0x5F:
        pl_mv095(pl, 0);
        break;
    case 0x60:
        pl_mv093(pl, 1);
        break;
    case 0x61:
        pl_mv097(pl, 0);
        break;
    case 0x62:
        pl_mv094(pl, 1);
        break;
    case 0x63:
        pl_mv095(pl, 1);
        break;
    case 0x64:
        pl_mv084(pl, 1);
        break;
    case 0x65:
        pl_mv101(pl, 0);
        break;
    case 0x66:
        pl_mv101(pl, 1);
        break;
    case 0x67:
        pl_mv103(pl, 0);
        break;
    case 0x68:
        pl_mv104(pl, 0);
        break;
    case 0x69:
        pl_mv105(pl, 0);
        break;
    case 0x6A:
        pl_mv106(pl, 0);
        break;
    case 0x6B:
        pl_mv107(pl, 0);
        break;
    case 0x6C:
        pl_mv107(pl, 1);
        break;
    case 0x6D:
        pl_mv107(pl, 2);
        break;
    case 0x6E:
        pl_mv107(pl, 3);
        break;
    case 0x6F:
        pl_mv111(pl, 0);
        break;
    case 0x70:
        pl_mv112(pl, 0);
        break;
    case 0x71:
        pl_mv113(pl, 0);
        break;
    case 0x72:
        pl_mv014(pl, 2);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00142300-0x00143420): pl_at000, pl_at001, pl_at004, pl_at006: weapon attack handlers (sword-type chains: arg1 selects the swing, x05/x06/x07 are step/sub-step/hit flag) */





#if !defined(G_PART) || G_PART == 19
static void pl_at000(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work615 = 1;
        pl->x07 = 0;
        pl->x43E = 0x3C;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x57E, 4, 0);
            action_timer_calc(pl, 0);
            func_6362B0(pl, 2);
            Pl_basic_flagset(pl, 0, 1, 0);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                switch (arg1) {
                case 1:
                    pl_chr_set2(pl, 0x589, 4, 0);
                    break;
                case 2:
                    pl_chr_set2(pl, 0x58D, 4, 0);
                    break;
                case 3:
                    pl->ang[1] = (pl->ang[1] + 0x2000) & 0xFFFF;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x58F, 0, 0);
                    break;
                case 4:
                    pl->ang[1] = pl->ang[1] + 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x593, 0, 0);
                    break;
                }
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    switch (arg1) {
                    case 2:
                        pl_chr_set2(pl, 0x57E, 2, 0x2C);
                        break;
                    case 4:
                        pl_chr_set2(pl, 0x57E, 0, 0x2C);
                        break;
                    default:
                        pl_chr_set2(pl, 0x57E, 2, 0x1A);
                        break;
                    }
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 2);
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if ((pl->work019 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                func_543690(pl, 0xF, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                pl->work615 = 0;
                Pl_act_set(pl, 1, 6, 0);
                break;
            case 2:
            case 3:
                pl->x05++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 2);
                break;
            }
        } else if ((frame_check2(78.0f, pl, 0) != 0) && (frame_check2(104.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 2);
        }
        break;
    case 2:
        if (pl->x610 <= 0) {
            pl->x05 = s + 1;
            func_543690(pl, 0x1C, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((frame_check2(78.0f, pl, 0) != 0) && (frame_check2(104.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 2);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 19
static void pl_at001(PLW *pl, s32 arg1) {
    f32 sp50[3];
    f32 sp40[3];
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work615 = 1;
        pl->x07 = 0;
        pl->x43E = 0x3C;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x57F, 4, 0);
            pl->work39C = 0;
            func_6362B0(pl, 1);
            Pl_basic_flagset(pl, 0, 1, 0);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                switch (arg1) {
                case 1:
                    pl_chr_set2(pl, 0x588, 4, 0);
                    break;
                case 2:
                    pl_chr_set2(pl, 0x58B, 4, 0);
                    break;
                case 3:
                    pl->ang[1] = pl->ang[1] - 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x58E, 0, 0);
                    break;
                case 4:
                    pl->ang[1] = pl->ang[1] - 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x591, 0, 0);
                    break;
                }
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    if (arg1 == 1) {
                        pl_chr_set2(pl, 0x57F, 2, 0x40);
                    } else {
                        pl_chr_set2(pl, 0x57F, 0, 0x42);
                    }
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 1);
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if ((pl->work019 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                func_543690(pl, 0xF, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                pl->work615 = 0;
                Pl_act_set(pl, 1, 6, 0);
                break;
            case 2:
            case 3:
                pl->x05++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 2);
                break;
            }
        } else {
            if ((pl->x07 == 0) && (frame_check(106.0f, pl, 0) != 0)) {
                sp50[0] = 30.0f;
                sp50[1] = 0.0f;
                sp50[2] = -150.0f;
                flvecApplyMat33(sp40, sp50, (f32 *)((u8 *)pl + 0x20));
                sp50[0] = pl->pos[0] + sp40[0];
                sp50[1] = 15.0f + (pl->pos[1] + sp40[1]);
                sp50[2] = pl->pos[2] + sp40[2];
                func_551790(pl, 0, sp50, 1);
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 0);
            }
            if ((frame_check2(116.0f, pl, 0) != 0) && (frame_check2(192.0f, pl, 0) == 0)) {
                ex_atk_ck(pl, 1);
            }
        }
        break;
    case 2:
        if (pl->x610 <= 0) {
            pl->x05 = s + 1;
            func_543690(pl, 0x1C, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((frame_check2(116.0f, pl, 0) != 0) && (frame_check2(192.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 1);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 19
static void pl_at004(PLW *pl, u32 arg1) {
    f32 sp50[3];
    f32 sp40[3];
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x579, 4, 0);
            action_timer_calc(pl, 0);
            func_6362B0(pl, 3);
            pl->work615 = 1;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                switch (arg1) {
                case 1:
                    pl_chr_set2(pl, 0x582, 2, 0);
                    pl->flag12 = 1;
                    pl->work615 = 0;
                    break;
                case 2:
                    pl->work615 = 1;
                    pl_chr_set2(pl, 0x58C, 4, 0);
                    break;
                case 3:
                    pl->work615 = 1;
                    pl_chr_set2(pl, 0x58A, 4, 0);
                    break;
                case 4:
                    pl->work615 = 1;
                    pl_chr_set2(pl, 0x57D, 4, 0);
                    break;
                case 5:
                    pl->work615 = 1;
                    pl->ang[1] = pl->ang[1] + 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x590, 0, 0);
                    break;
                case 6:
                    pl->work615 = 1;
                    pl->ang[1] = pl->ang[1] - 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x592, 0, 0);
                    break;
                }
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    switch (arg1) {
                    case 1:
                        pl_chr_set2(pl, 0x579, 0, 0x5C);
                        break;
                    case 2:
                        pl_chr_set2(pl, 0x579, 2, 0x32);
                        break;
                    case 3:
                        pl_chr_set2(pl, 0x579, 2, 0x44);
                        break;
                    case 4:
                        pl_chr_set2(pl, 0x579, 2, 0x4A);
                        break;
                    case 5:
                        pl_chr_set2(pl, 0x579, 0, 0x46);
                        break;
                    case 6:
                        pl_chr_set2(pl, 0x579, 0, 0x34);
                        break;
                    }
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 3);
                    pl->work615 = 1;
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((pl->work019 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                func_543690(pl, 0xF, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set(pl, 1, 6, 0);
                break;
            case 2:
            case 3:
                pl->x05++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 1);
                break;
            }
        } else {
            if ((pl->x07 == 0) && (frame_check(102.0f, pl, 0) != 0)) {
                sp50[0] = 0.0f;
                sp50[1] = 0.0f;
                sp50[2] = 150.0f;
                flvecApplyMat33(sp40, sp50, (f32 *)((u8 *)pl + 0x20));
                sp50[0] = pl->pos[0] + sp40[0];
                sp50[1] = 15.0f + (pl->pos[1] + sp40[1]);
                sp50[2] = pl->pos[2] + sp40[2];
                func_551790(pl, 0, sp50, 1);
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 0);
            }
            if ((frame_check2(108.0f, pl, 0) != 0) && (frame_check2(162.0f, pl, 0) == 0)) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    case 2:
        if (pl->x610 <= 0) {
            pl->x05 = s + 1;
            func_543690(pl, 0x1C, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((frame_check2(108.0f, pl, 0) != 0) && (frame_check2(162.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 19
static void pl_at006(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl_chr_set2(pl, 0x57C, 0, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(18.0f, pl, 0) != 0) {
                Pl_act_set(pl, 1, 7, 0xC);
            }
        } else if ((frame_check3(2.0f, 30.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
            pl->x07 = 0;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            switch (pl->x07) {
            case 0:
                Pl_act_set(pl, 1, 1, 0);
                pl_chr_set2(pl, 0x579, 0, 0x10);
                pl->work615 = 1;
                break;
            case 2:
                Pl_act_set(pl, 1, 1, 0);
                pl->work615 = 1;
                break;
            }
        }
        break;
    }
}
#endif

/* pl_at008: matches with blend_set taking u16 (agent B); the table reads are at008_tbl[arg1].field. */
#if !defined(G_PART)
static void pl_at008(PLW *pl, s32 arg1) {
    u8 s;

    if (pl->x763 != 0) {
        pl->x763 = 5;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        blend_set(pl, at008_tbl[arg1].a, at008_tbl[arg1].b);
        pl_chr_set2(pl, at008_tbl[arg1].chr, 0, 0);
        break;
    case 1:
        if (frame_check(8.0f, pl, 1) != 0) {
            vib_set_pl(pl, 0);
            Pachinger_set_quake_sub(pl, 1);
            func_62A6C0(pl, 0, 0xE);
            pl->work01C = pl->work01C - 1;
            if (Game_clear_ck(1) == 0) {
                Pl_item_stack(pl, pl->item[pl->work88E].id, -1);
            }
            if (pl->work8BC == 0) {
                pl->work01C = 0;
            }
            pl->work8CE = pl->work8BC;
            pl->work8D2 = pl->work01D;
            pl->work8D1 = pl->work01C;
        }
        if (pl->work194 == 0) {
            blend_set(pl, 0x3ED, 0x3EE);
            pl_to_normal_b(pl, 0, 0, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_at009(PLW *pl, s32 unused_arg) {
    u8 s;

    if (pl->x763 != 0) {
        pl->x763 = 5;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        blend_set(pl, 0x586, 0x587);
        pl_chr_set2(pl, 0x57C, 0, 0);
        pl->work01C = pl->work01D;
        if (pl->work8BC < pl->work01C) {
            pl->work01C = pl->work8BC;
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        switch (Get_string_pow(pl, pl->ammo_type) & 0xFF) {
        case 0:
            pl->chr_spd0 = 2.5f;
            pl->chr_spd1 = 2.5f;
            break;
        default:
        case 1:
            pl->chr_spd0 = 2.0f;
            pl->chr_spd1 = 2.0f;
            break;
        case 2:
            pl->chr_spd0 = 1.5f;
            pl->chr_spd1 = 1.5f;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal_b(pl, 0, 0, 0);
        }
        break;
    }
}
#endif

/* near-match (not built): pl_at012 - calls blend_set (same IPA effect as gun_adj_sub: the original keeps a0 live across blend_set, which
   MWCC only does for a static callee defined earlier in the same TU). Same cause for pl_at008/pl_at009's delay-slot differences. */
#if !defined(G_PART)
static void pl_at012(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        blend_set(pl, 0x588, 0x589);
        pl_chr_set2(pl, 0x584, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal_b(pl, 0, 0, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00143790-0x00144538): pl_at014..pl_at026: weapon attack handlers (bow, lance, hammer, hunting horn?: charge/release: tame_pow_ck, tame_release, tame_com_ck) */





#if !defined(G_PART) || G_PART == 20
static void pl_at014(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 0, 0);
        func_6362B0(pl, 4);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 20
static void pl_at020(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x32, 4, 0);
        func_6362B0(pl, 5);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 20
static void pl_at021(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->x763 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work01C = pl->work01D;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 20
static s32 we02_hit_sub(PLW *pl) {
    u8 s;
    u8 t;

    s = pl->x07;
    switch (s) {
    case 0:
        if (pl->work019 != 0) {
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                pl->x07 = pl->x07 + 1;
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set2(pl, 1, 0x1F, 0);
                return 1;
            case 2:
            case 3:
                pl->x07++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 1);
                break;
            }
        }
        break;
    case 1:
        if (pl->x610 <= 0) {
            pl->x07 = s + 1;
        }
        break;
    case 2:
        break;
    }
    return 0;
}
#endif

#if !defined(G_PART) || G_PART == 20
static void pl_at022(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x579, 4, 0);
        func_6362B0(pl, 6);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        break;
    case 1:
        if (we02_hit_sub(pl) != 1) {
            if (pl->work194 == 0) {
                pl_to_normal(pl, 0, 6, 0);
                break;
            }
            if (Pl_master_ck(pl) == 1) {
                if (frame_check3(50.0f, 80.0f, pl, 0) != 0) {
                    ex_atk_ck(pl, 1);
                }
                if ((frame_check2(72.0f, pl, 0) != 0) && (pl->sw.now & 0x80)) {
                    Pl_act_set(pl, 1, 0x4C, 0);
                }
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 20
static void pl_at023(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->work08 = 0;
        pl->x07 = 0;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x57A, 4, 0);
            action_timer_calc(pl, 0);
            Pl_basic_flagset(pl, 0, 1, 0);
            func_6362B0(pl, 7);
            pl->x43E = 0x1B;
            break;
        case 1:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                pl_chr_set2(pl, 0x581, 4, 0);
                Pl_basic_flagset(pl, 0, 1, 0);
                pl->flag12 = 1;
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    pl_chr_set2(pl, 0x57A, 0, 0x18);
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 7);
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (we02_hit_sub(pl) != 1) {
            if (frame_check(66.0f, pl, 0) != 0) {
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 2);
            }
            if ((Pl_master_ck(pl) == 1) && (frame_check3(30.0f, 88.0f, pl, 0) != 0) && (pl->sw.an_trg & 0x3C) && (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC)) {
                pl->work08 = 1;
            }
            if (pl->work194 == 0) {
                pl_to_normal(pl, 0, 6, 0);
                break;
            }
            if (pl->x610 == 0) {
                if (frame_check3(80.0f, 120.0f, pl, 0) != 0) {
                    ex_atk_ck(pl, 2);
                    break;
                }
                if ((frame_check2(70.0f, pl, 0) != 0) && (pl->work08 != 0)) {
                    Pl_act_set(pl, 1, 0x4E, 4);
                }
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 20
static s32 tame_pow_ck(PLW *pl) {
    return (pl->work87C / 30) & 0xFF;
}
#endif

#if !defined(G_PART) || G_PART == 20
static void tame_release(PLW *pl) {
    s32 p;

    p = tame_pow_ck(pl) & 0xFF;
    switch (p) {
    case 0:
        if (pl_flag_ck(pl, 0x1000) != 0) {
            Pl_act_set2(pl, 1, 0x39, 4);
            break;
        }
        Pl_act_set2(pl, 1, 0x21, 4);
        break;
    case 1:
        if (pl_flag_ck(pl, 0x1000) != 0) {
            Pl_act_set2(pl, 1, 0x3A, 4);
            break;
        }
        Pl_act_set2(pl, 1, 0x1D, 4);
        break;
    default:
    case 2:
        if (pl_flag_ck(pl, 0x1000) != 0) {
            pl->work880 = tame_pow_ck(pl);
            Pl_act_set2(pl, 1, 0x1C, 4);
            break;
        }
        Pl_act_set2(pl, 1, 0x1E, 4);
        break;
    }
}
#endif

#if !defined(G_PART)
static void tame_com_ck(PLW *pl) {
    if (Pl_master_ck(pl) != 0) {
        if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
            Pl_act_set2(pl, 0, 0x1C, 4);
            return;
        }
        if ((pl->sw.now & 0x80) && (pl->stamina > 0)) {
            if (stick_pow_get(pl, 0) & 0xFF) {
                if (act_ck(pl, 1, 0x1A) == 0) {
                    pl->ang_y = stick_dir_set(pl, 0);
                    Pl_act_set(pl, 1, 0x1A, 0);
                } else {
                    pl->ang_y = stick_dir_set(pl, 0);
                }
            } else if ((act_ck(pl, 1, 0x18) == 0) && (act_ck(pl, 1, 0x1B) == 0) && (act_ck(pl, 1, 0x4C) == 0)) {
                Pl_act_set(pl, 1, 0x1B, 0);
            }
            if ((pl->sw.an_trg & 0x20) && (pl->work88C == 0)) {
                tame_release(pl);
            }
        } else {
            tame_release(pl);
        }
    }
}
#endif

/* pl_at024: a charge ("tame") move (arg 0/1/2). G (game 0x559290) adds arg 3, the Dual Swords' (action 0x67): arg 0's
   charge with motion 0x3EE started at frame 6, blend 0xC. */
#if !defined(G_PART) || G_PART == 50
static void pl_at024(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->flag12 = 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->work08 = 0;
        switch (arg1) {
        case 0:
#ifdef VERSION_MHG
        case 3:
#endif
            pl->x05 = 2;
#ifdef VERSION_MHG
            if (arg1 == 3) {
                pl_chr_set2(pl, 0x3EE, 6, 0xC);
            } else
#endif
            pl_chr_set2(pl, 0x3EE, 4, 0);
            pl->work87C = 0;
            Eft06_set(4.0f, pl, 6, 0, 2);
            pl->x43E = 0x3C;
            break;
        case 1:
            pl->x05 = 3;
            pl_chr_set2(pl, 0x3EF, 8, 0);
            goto c3;
        case 2:
            pl->x05 = 1;
            pl_chr_set2(pl, 0x582, 4, 0);
            pl->work87C = 0;
            pl->x43E = 0x3C;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x3EE, 0, 0xE);
            pl->work87C = 0;
            Eft06_set(4.0f, pl, 6, 0, 2);
            pl->x43E = 0x3C;
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x3EF, 4, 0);
            pl->x43E = 1;
        }
        break;
    case 3:
c3:
            tame_cnt_up(pl);
            if (Pl_master_ck(pl) == 1) {
                pl->work08 = pl->work08 + 1;
                if (pl->work08 >= 2) {
                    Pl_stamina_calc(pl, -1);
                }
            }
            tame_com_ck(pl);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 21
static void pl_at025(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x3F0, 0x14, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 21
static void pl_at026(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x3ED, 8, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl_flag_set(pl, 0x02001000);
        pl->work08 = 0;
    case 1:
        tame_cnt_up(pl);
        if (Pl_master_ck(pl) == 1) {
            pl->work08 = pl->work08 + 1;
            if (pl->work08 >= 2) {
                Pl_stamina_calc(pl, -1);
            }
        }
        tame_com_ck(pl);
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00144540-0x00145330): pl_at028..pl_at035, lance_kan_ck: weapon attack handlers (long sword/lance/gun-lance family) */





#if !defined(G_PART) || G_PART == 21
static void pl_at028(PLW *pl, s32 unused_arg) {
    u16 v;
    u32 c;
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x57B, 6, 0);
        pl->x06 = pl->work880 + 2;
        if (pl->x06 >= 5) {
            pl->x06 = 5;
        }
        pl->x07 = 0;
        pl->work08 = 0;
        pl->ang_y = pl->ang[1];
        func_6362B0(pl, 0xC);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        break;
    case 1:
        pl->x43E = 2;
        if (frame_check(44.0f, pl, 0) != 0) {
            pl->work08 = (u32)pl->work08 + 1;
            t = pl->x06 - 1;
            pl->x06 = t;
            if (t <= 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x57C, 0, 0);
                pl->cnt39A = pl->cnt39A + 3;
                func_6362B0(pl, 0xF);
                break;
            }
            c = pl->work08;
            switch (c) {
            case 1:
            case 2:
                if (pl->x07 != 0) {
                    if (pl->sw.an_now & 0x800) {
                        pl->ang[1] = pl->ang[1] + 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->ang[1] = pl->ang[1] - 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    Pl_act_set2(pl, 1, 0x3A, 4);
                }
                break;
            case 3:
            case 4:
            case 5:
                if (pl->x07 != 0) {
                    if (pl->sw.an_now & 0x800) {
                        pl->ang[1] = pl->ang[1] + 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->ang[1] = pl->ang[1] - 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    Pl_act_set2(pl, 1, 0x4D, 4);
                }
                break;
            case 0:
            default:
                break;
            }
        } else {
            if (Pl_master_ck(pl) == 1) {
                v = pl->sw.an_now;
                if (v & 0x800) {
                    pl->ang_y = *(u16 *)&pl->ang_y + 0xC0;
                } else if (v & 0x400) {
                    pl->ang_y = *(u16 *)&pl->ang_y - 0xC0;
                }
                if (pl->sw.an_trg & 0x20) {
                    pl->x07++;
                }
            }
            if (frame_check(26.0f, pl, 0) != 0) {
                func_6362B0(pl, 0xD);
            }
            if (frame_check(52.0f, pl, 0) != 0) {
                func_6362B0(pl, 0xE);
            }
        }
        break;
    case 2:
        pl->x43E = 2;
        if (frame_check(38.0f, pl, 0) != 0) {
            vib_set_pl(pl, 2);
            Pl_set_quake_sub(pl, 1);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_at029(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        if (arg1 == 4) {
            pl_chr_set2(pl, 0x57D, 2, 0);
        } else {
            pl_chr_set2(pl, 0x57D, 4, 0);
        }
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 8);
        pl->x43E = 0x3C;
        break;
    case 1:
        if (frame_check2(30.0f, pl, 0) == 0) {
            if (arg1 != 2) {
                if (arg1 == 3) {
                    goto go;
                }
            } else {
go:
                sp40[0] = 0.0f;
                sp40[1] = 0.0f;
                sp40[2] = 18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 24.0f)) / 360.0f)));
                flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
                pl->pos[0] = pl->pos[0] + sp30[0];
                pl->pos[2] = pl->pos[2] + sp30[2];
            }
        }
        if ((arg1 == 0) || (arg1 == 3)) {
            if (frame_check(60.0f, pl, 0) != 0) {
                func_6362B0(pl, 9);
            }
            if (pl->work194 == 0) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (frame_check2(88.0f, pl, 0) != 0) {
                ex_atk_ck(pl, 0);
                break;
            }
        } else {
            if ((we02_hit_sub(pl) != 1) && (frame_check2(30.0f, pl, 0) != 0)) {
                pl->x05++;
                pl_chr_set2(pl, 0x57F, 0, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if (frame_check3(32.0f, 46.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at030(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x57E, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0xA);
        pl->x43E = 0x3C;
        break;
    case 1:
        if (frame_check(74.0f, pl, 0) != 0) {
            func_6362B0(pl, 0xB);
        }
        if (frame_check(80.0f, pl, 0) != 0) {
            vib_set_pl(pl, 2);
            Pl_set_quake_sub(pl, 2);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check3(84.0f, 170.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at031(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x580, 0, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(32.0f, pl, 0) != 0) {
                Pl_act_set(pl, 1, 0x51, 0xC);
            }
        } else if ((frame_check3(2.0f, 30.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at032(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x3F2, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->flag12 = 1;
        pl->work615 = 1;
        pl->x43E = 0x3C;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57D, 2, 0x2E);
            action_timer_calc(pl, 0);
            pl->work615 = 1;
        }
        break;
    case 2:
        if (frame_check(56.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x22);
            pl->x05++;
        }
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check2(88.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at034(PLW *pl, s32 arg1) {
    u16 v;
    u8 s;

    pl->x43E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        if (arg1 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x579, 4, 0);
        } else {
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57A, 0, 6);
            func_6362B0(pl, 0x11);
        }
        pl->work019 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        pl->x07 = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57A, 0, 4);
            func_6362B0(pl, 0x11);
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1) {
            if (pl->sw.an_now & 0x800) {
                pl->ang_y = *(u16 *)&pl->ang_y + 0x80;
            }
            if (pl->sw.an_now & 0x400) {
                pl->ang_y = *(u16 *)&pl->ang_y - 0x80;
            }
        }
        Pl_stamina_calc(pl, -2);
        if (pl->stamina <= 0) {
            Pl_act_set(pl, 1, 0x23, 4);
            break;
        }
        if (Pl_master_ck(pl) == 1) {
            v = pl->sw.an_trg;
            if ((v & 0x10) || (pl->sw.trg & 1)) {
                Pl_act_set(pl, 1, 0x23, 4);
            } else if (v & 0x20) {
                Pl_act_set(pl, 1, 0x4A, 4);
            }
        }
        if (pl->work019 != 0) {
            switch (pl->work887) {
            case 0:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set(pl, 2, 0, 0);
                break;
            case 2:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set2(pl, 1, 0x25, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set2(pl, 1, 0x2F, 0);
                break;
            }
        } else if (pl->work74C & 0xE0000007) {
            Pl_act_set2(pl, 1, 0x27, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at035(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        pl->x43E = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x57B, 2, 0);
            break;
        }
        pl_chr_set2(pl, 0x583, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static s32 lance_kan_ck(PLW *pl) {
    if ((pl->work019 != 0) && (pl->work018 == 1)) {
        Pl_set_quake_sub(pl, 0);
        pl->x40A = 0;
        pl->work409 = 0;
        Pl_act_set2(pl, 1, 0x41, 0);
        return 1;
    }
    return 0;
}
#endif

/* Player code (SLPM_654.95 0x00145330-0x00145DD0): pl_at036..pl_at045, we04_hit_sub: weapon attack handlers */





#if !defined(G_PART) || G_PART == 22
static void pl_at036(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x06 = 0;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        switch (arg1) {
        case 0:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57D, 4, 0);
            func_6362B0(pl, 0x10);
            break;
        case 1:
            pl->x05++;
            pl_chr_set2(pl, 0x3F2, 2, 0);
            pl->flag12 = 1;
            func_6362B0(pl, 0x14);
            break;
        case 2:
        case 3:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57D, 6, 0xA);
            if (arg1 == 3) {
                func_6362B0(pl, 0x24);
                break;
            }
            func_6362B0(pl, 0x10);
            break;
        case 4:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57D, 4, 0xA);
            func_6362B0(pl, 0x10);
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57D, 0, 0x22);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((lance_kan_ck(pl) != 1) && (Pl_master_ck(pl) == 1) && (frame_check3(36.0f, 62.0f, pl, 0) != 0)) {
            ex_atk_ck(pl, 0);
            if ((pl->sw.an_trg & 0x3C) && (arg1 != 3)) {
                if (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC) {
                    if (arg1 == 2) {
                        Pl_act_set(pl, 1, 0x36, 4);
                        break;
                    }
                    Pl_act_set(pl, 1, 0x35, 4);
                    break;
                }
                if (arg1 == 2) {
                    Pl_act_set(pl, 1, 0x3C, 4);
                    break;
                }
                Pl_act_set(pl, 1, 0x3B, 4);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at037(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x40A = 0;
        pl->work409 = 0;
        pl_chr_set2(pl, 0x57C, 0, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0;
        break;
    case 1:
        Pl_stamina_calc(pl, -2);
        if (pl->work194 == 0) {
            if ((arg1 == 0) && (pl->x06 == 0)) {
                Pl_act_set2(pl, 1, 0x26, 0);
                break;
            }
            Pl_act_set2(pl, 1, 0x23, 0);
            break;
        }
        if ((pl->sw.an_trg & 0x10) || (pl->sw.trg & 1)) {
            pl->x06 = 1;
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at040(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x588, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x12);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
            break;
        }
        if ((lance_kan_ck(pl) != 1) && (frame_check2(18.0f, pl, 0) != 0) && (frame_check2(28.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at041(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        if (arg1 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x57F, 6, 0);
            break;
        }
        pl->x05 = 2;
        pl_flag_set(pl, 0x8000);
        pl_chr_set2(pl, 0x580, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x580, 4, 0);
            pl_flag_set(pl, 0x8000);
            pl->work019 = 0;
            func_6362B0(pl, 0x13);
        }
        break;
    case 2:
        if (pl->work019 != 0) {
            pl->work409 = 0;
            pl->x610 = 0x1E;
            Pl_act_set(pl, 1, 0x2B, 0);
            break;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->sw.an_trg & 0x10)) {
            Pl_act_set(pl, 1, 0x2B, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at042(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x582, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x12);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 1, 0x2C, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at043(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x581, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at045(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        switch (arg1) {
        default:
            pl_chr_set2(pl, 0x57E, 4, 0);
            func_6362B0(pl, 0x1C);
            break;
        case 1:
        case 2:
            pl_chr_set2(pl, 0x57E, -6, 0xA);
            if (arg1 == 2) {
                func_6362B0(pl, 0x23);
                break;
            }
            func_6362B0(pl, 0x1C);
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((lance_kan_ck(pl) != 1) && (Pl_master_ck(pl) == 1) && (frame_check3(36.0f, 62.0f, pl, 0) != 0)) {
            ex_atk_ck(pl, 0);
            if ((pl->sw.an_trg & 0x3C) && (arg1 != 2)) {
                if (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC) {
                    if (arg1 == 1) {
                        Pl_act_set(pl, 1, 0x36, 4);
                        break;
                    }
                    Pl_act_set(pl, 1, 0x35, 4);
                    break;
                }
                if (arg1 == 1) {
                    Pl_act_set(pl, 1, 0x3C, 4);
                    break;
                }
                Pl_act_set(pl, 1, 0x3B, 4);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static s32 we04_hit_sub(PLW *pl) {
    if ((pl->work019 != 0) && (pl->x07 == 0)) {
        pl->x07 = 1;
        switch (pl->work018) {
        case 0:
            break;
        case 1:
            pl->x40A = 0;
            pl->work409 = 0;
            Pl_act_set(pl, 1, 0x32, 0);
            return 1;
        case 2:
        case 3:
            pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
            pl->work7ED = 2;
            break;
        }
    }
    return 0;
}
#endif

/* Player code (SLPM_654.95 0x00145DD0-0x00146F9C): pl_at048..pl_at078: weapon attack handlers */





#if !defined(G_PART) || G_PART == 22
static void pl_at048(PLW *pl, s32 arg1) {
    f32 a;
    f32 b;
    s32 t;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x579, 4, 0);
            func_6362B0(pl, 0x15);
            break;
        case 1:
            pl_chr_set2(pl, 0x57A, 2, 0);
            func_6362B0(pl, 0x19);
            break;
        case 2:
            pl_chr_set2(pl, 0x57B, 2, 0);
            func_6362B0(pl, 0x1A);
            break;
        case 3:
            pl_chr_set2(pl, 0x579, 0xC, 0);
            func_6362B0(pl, 0x15);
            break;
        case 4:
            pl_chr_set2(pl, 0x579, 2, 0xA);
            func_6362B0(pl, 0x15);
            func_543690(pl, 8, 1);
            break;
        }
        break;
    case 1:
        if (pl->x06 == 0) {
            a = 10.0f;
            b = 50.0f;
            if (arg1 == 2) {
                a = 30.0f;
                b = 70.0f;
            }
            if ((frame_check2(a, pl, 0) != 0) && (frame_check2(b, pl, 0) == 0) && (pl->sw.an_trg & 0x3C)) {
                t = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
                if (t < 0x4001) {
                    pl->x06 = 2;
                } else if (t < 0x9556) {
                    pl->x06 = 1;
                } else if (t < 0xD556) {
                    pl->x06 = 2;
                }
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (we04_hit_sub(pl) != 1) {
            switch (arg1) {
            case 0:
            case 3:
            case 4:
                if (frame_check2(24.0f, pl, 0) != 0) {
                    if (pl->x06 != 0) {
                        if (pl->x06 == 1) {
                            Pl_act_set(pl, 1, 0x37, 4);
                        } else {
                            Pl_act_set(pl, 1, 0x33, 4);
                        }
                    } else if (frame_check2(50.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                }
                break;
            case 1:
                if (frame_check2(20.0f, pl, 0) != 0) {
                    if (pl->x06 != 0) {
                        if (pl->x06 == 1) {
                            Pl_act_set(pl, 1, 0x38, 4);
                        } else {
                            Pl_act_set(pl, 1, 0x33, 4);
                        }
                    } else if (frame_check2(50.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                }
                break;
            case 2:
                if (frame_check2(64.0f, pl, 0) != 0) {
                    if (pl->x06 == 2) {
                        Pl_act_set(pl, 1, 0x33, 4);
                    } else if (frame_check2(92.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                }
                break;
            }
            if ((frame_check(52.0f, pl, 0) != 0) && (arg1 == 2)) {
                func_6362B0(pl, 0x1B);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 22
static void pl_at049(PLW *pl, s32 arg1) {
    s32 t;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        pl->x07 = 0;
        switch (arg1) {
        case 0:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57C, 2, 0);
            func_6362B0(pl, 0x16);
            break;
        case 1:
            pl->x05++;
            pl->flag12 = 1;
            pl_chr_set2(pl, 0x3F2, 2, 0);
            break;
        case 2:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57C, 4, 0);
            func_6362B0(pl, 0x16);
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57C, 0, 8);
            func_6362B0(pl, 0x16);
        }
        break;
    case 2:
        if ((pl->x06 == 0) && (frame_check2(20.0f, pl, 0) != 0) && (frame_check2(42.0f, pl, 0) == 0) && (pl->sw.an_trg & 0x3C)) {
            t = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
            if ((t > 0x4000) && (t < 0x9556)) {
                pl->x06 = 1;
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if ((we04_hit_sub(pl) != 1) && (frame_check2(34.0f, pl, 0) != 0)) {
            if (pl->x06 != 0) {
                Pl_act_set(pl, 1, 0x46, 4);
                break;
            }
            if (frame_check2(42.0f, pl, 0) == 0) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_at050(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x580, 4, 0);
        Pl_set_quake_sub(pl, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(32.0f, pl, 0) != 0) {
                Pl_act_set2(pl, 1, 0x50, 0xC);
            }
        } else if ((frame_check3(14.0f, 40.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at051(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, 0x57D, 4, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x17);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((we04_hit_sub(pl) != 1) && (frame_check2(20.0f, pl, 0) != 0) && (frame_check2(40.0f, pl, 0) != 0)) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at052(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, 0x588, 4, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x18);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
            break;
        }
        we04_hit_sub(pl);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at061(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x585, 4, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x1D);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at065(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x589, 0, 0);
        Pl_set_quake_sub(pl, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(42.0f, pl, 0) != 0) {
                Pl_act_set(pl, 1, 0x52, 0xC);
            }
        } else if ((frame_check3(10.0f, 40.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at070(PLW *pl, s32 arg1) {
    s32 t;
    u8 s;
    u8 u;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x57E, 2, 0);
        } else {
            pl_chr_set2(pl, 0x57E, 2, 8);
        }
        func_6362B0(pl, 0x1E);
        break;
    case 1:
        if ((pl->x06 == 0) && (frame_check2(26.0f, pl, 0) != 0) && (frame_check2(38.0f, pl, 0) == 0) && (pl->sw.an_trg & 0x3C)) {
            t = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
            if (t < 0x4001) {
                pl->x06 = 2;
            } else if (t < 0x9556) {
                pl->x06 = 1;
            } else if (t < 0xD556) {
                pl->x06 = 2;
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((we04_hit_sub(pl) != 1) && (frame_check2(26.0f, pl, 0) != 0)) {
            u = pl->x06;
            if (u == 1) {
                Pl_act_set(pl, 1, 0x48, 4);
                break;
            }
            if (u == 2) {
                Pl_act_set(pl, 1, 0x33, 4);
                break;
            }
            if (frame_check2(38.0f, pl, 0) == 0) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at074(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x584, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        func_6362B0(pl, 0x1F);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x43E = 0;
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check3(72.0f, 102.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at077(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 0, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x1C;
        func_6362B0(pl, 0x20);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check(40.0f, pl, 0) != 0) {
            vib_set_pl(pl, 2);
            Pl_set_quake_sub(pl, 1);
        }
        if (frame_check3(102.0f, 132.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 23
static void pl_at078(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, 0x584, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0;
        func_6362B0(pl, 0x21);
        pl->work08 = 0;
        break;
    case 1:
        if (we02_hit_sub(pl) != 1) {
            if (pl->work194 == 0) {
                if ((Pl_master_ck(pl) == 1) && (pl->work08 != 0) && (arg1 == 0)) {
                    Pl_act_set(pl, 1, 0x4F, 4);
                    break;
                }
                pl->x05++;
                pl_chr_set2(pl, 0x57A, 4, 0x4A);
                break;
            }
            if ((Pl_master_ck(pl) == 1) && (frame_check3(30.0f, 60.0f, pl, 0) != 0) && (pl->sw.an_trg & 0x3C) && (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC)) {
                pl->work08 = 1;
            }
            if (frame_check(52.0f, pl, 0) != 0) {
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 2);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check3(80.0f, 120.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 2);
        }
        break;
    }
}
#endif

/* G only (PS2 Monster Hunter G game 0x0055C160-0x0055D36C): the Dual Swords' (job 6) attack handlers, called by G's
   pl_attack for actions 0x53-0x66 (agent A, 16 Oct 2026; named pl_atNNN after their first action number, as MH1's).
   PLW+0x93D = demon mode ("kijinka") on; shell00_set2 (G game 0x6690D0) is shell00_set with the blade (0 right,
   1 left) in the shell's x07; ds_atk_set (G game 0x54CBA0) picks the next attack from the stick direction and the
   demon mode. pl->x06 holds the direction a combo input asked for. */
/* pl_at083: near-match (PC only; the G build keeps G's asm): G has an extra empty `beq arg1, 2` (+2 instructions) before
   the demon-mode-on block; switch orders and an empty case 2 tried (16 Oct 2026). Logic checked against the asm. */
#if defined(VERSION_MHG) && !defined(G_PART)
static void pl_at083(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->flag12 = 1;
        switch (arg1) {
        case 2:
        default:
            pl->work93D = 1;
            pl->x43E = 2;
            if (arg1 == 0) {
                pl_chr_set2(pl, 0x3FA, 4, 0);
            } else {
                pl_chr_set2(pl, 0x3FA, 4, 0xA);
            }
            break;
        case 1:
            pl->work93D = 0;
            pl_chr_set2(pl, 0x3FB, 4, 0);
            break;
        }
        break;
    case 1:
        if (arg1 == 0) {
            pl->x43E = 2;
        }
        if (pl->work194 == 0) {
            pl->x43E = 0;
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at084(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x583, 4, 0);
        } else {
            pl_chr_set2(pl, 0x57D, 4, 0);
        }
        shell00_set2(pl, 0x2D, (u8)arg1);
        break;
    case 1:
        if (frame_check2(40.0f, pl, 0) != 0) {
            pl->x05++;
            if (arg1 == 0) {
                pl_chr_set2(pl, 0x582, 0, 0);
            } else {
                pl_chr_set2(pl, 0x581, 0, 0);
            }
            shell00_set2(pl, 0x34, (u8)arg1);
            pl->x07 = 0;
            break;
        }
        if (we04_hit_sub(pl) == 1) {
            break;
        }
        if (frame_check(24.0f, pl, 0) != 0) {
            shell00_set2(pl, 0x2E, (u8)arg1);
        }
        if (frame_check(34.0f, pl, 0) != 0) {
            shell00_set2(pl, 0x2F, (u8)arg1);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if (we04_hit_sub(pl) == 1) {
            break;
        }
        if (frame_check(28.0f, pl, 0) != 0) {
            shell00_set2(pl, 0x35, (u8)arg1);
        }
        if (frame_check(38.0f, pl, 0) != 0) {
            shell00_set2(pl, 0x36, (u8)arg1);
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at085(PLW *pl, s32 arg1) {
    u8 s;
    f32 f0, f1;
    s32 a;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x579, 2, 0);
            func_6362B0(pl, 0x25);
            break;
        case 1:
            pl_chr_set2(pl, 0x57A, 2, 0);
            func_6362B0(pl, 0x26);
            break;
        case 2:
            pl_chr_set2(pl, 0x57B, 2, 0);
            func_6362B0(pl, 0x28);
            break;
        case 3:
            pl_chr_set2(pl, 0x579, 2, 0xA);
            func_6362B0(pl, 0x25);
            Eft05_set(pl, 8, 1);
            break;
        }
        break;
    case 1:
        if (pl->x06 == 0) {
            f0 = 10.0f;
            f1 = 50.0f;
            if (arg1 == 2) {
                f0 = 60.0f;
                f1 = 120.0f;
            }
            if (frame_check2(f0, pl, 0) != 0 && frame_check2(f1, pl, 0) == 0 && (pl->sw.an_trg & 0x3C)) {
                a = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
                if (a < 0x4001) {
                    pl->x06 = 2;
                } else if (a < 0x9556) {
                    pl->x06 = 1;
                } else if (a < 0xD556) {
                    pl->x06 = 3;
                } else {
                    pl->x06 = 4;
                }
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if (we04_hit_sub(pl) == 1) {
            break;
        }
        switch (arg1) {
        case 0:
        case 3:
            if (frame_check2(22.0f, pl, 0) != 0) {
                switch (pl->x06) {
                case 1:
                    Pl_act_set(pl, 1, 0x56, 4);
                    break;
                case 2:
                    ds_atk_set(pl, 0, 1);
                    break;
                case 3:
                    ds_atk_set(pl, 1, 1);
                    break;
                case 4:
                    ds_atk_set(pl, 3, 1);
                    break;
                default:
                    if (frame_check2(50.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                    break;
                }
            }
            break;
        case 1:
            if (frame_check2(28.0f, pl, 0) != 0) {
                switch (pl->x06) {
                case 1:
                    Pl_act_set(pl, 1, 0x57, 4);
                    break;
                case 2:
                    ds_atk_set(pl, 0, 1);
                    break;
                case 3:
                    ds_atk_set(pl, 1, 1);
                    break;
                case 4:
                    ds_atk_set(pl, 3, 1);
                    break;
                default:
                    if (frame_check2(50.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                    break;
                }
            }
            if (frame_check(18.0f, pl, 0) != 0) {
                func_6362B0(pl, 0x27);
            }
            break;
        case 2:
            if (frame_check2(88.0f, pl, 0) != 0) {
                switch (pl->x06) {
                case 2:
                    ds_atk_set(pl, 0, 1);
                    break;
                case 3:
                    ds_atk_set(pl, 1, 1);
                    break;
                case 4:
                    ds_atk_set(pl, 3, 1);
                    break;
                default:
                    if (frame_check2(120.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                    break;
                }
            }
            if (frame_check(58.0f, pl, 0) != 0) {
                func_6362B0(pl, 0x29);
            }
            if (frame_check(70.0f, pl, 0) != 0) {
                func_6362B0(pl, 0x2A);
            }
            break;
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at089(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x57F, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x1C;
        func_6362B0(pl, 0x30);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if (frame_check(58.0f, pl, 0) != 0 && pl->work887 < 2) {
            Pl_act_set(pl, 1, 0x66, 4);
            break;
        }
        if (frame_check(20.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x31);
        }
        if (frame_check(26.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x32);
        }
        if (frame_check(50.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x32);
        }
        if (frame_check(54.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x31);
        }
        if (frame_check(66.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x31);
        }
        if (frame_check(70.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x32);
        }
        if (frame_check(82.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x32);
        }
        if (frame_check(88.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x31);
        }
        if (frame_check(118.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x33);
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at091(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, ds_chr_tbl[arg1 * 2], 4, ds_chr_tbl[arg1 * 2 + 1]);
        Pl_basic_flagset(pl, 0, 1, 0);
        shell00_set2(pl, 0x2D, (u8)arg1);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (we04_hit_sub(pl) == 1) {
            break;
        }
        if (frame_check2(50.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        if (frame_check(24.0f, pl, 0) != 0) {
            shell00_set2(pl, 0x2E, (u8)arg1);
        }
        if (frame_check(34.0f, pl, 0) != 0) {
            shell00_set2(pl, 0x2F, (u8)arg1);
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at093(PLW *pl, s32 arg1) {
    u8 s;
    s32 a;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        pl->x07 = 0;
        pl->work08 = 0;
        switch (arg1) {
        case 0:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57C, 2, 0);
            func_6362B0(pl, 0x2B);
            break;
        case 1:
            pl->x05++;
            pl->flag12 = 1;
            pl_chr_set2(pl, 0x585, 4, 0);
            break;
        case 2:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57C, 4, 0);
            func_6362B0(pl, 0x2B);
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57C, 2, 0x1C);
            func_6362B0(pl, 0x37);
        }
        break;
    case 2:
        if (pl->x06 == 0 && frame_check2(20.0f, pl, 0) != 0 && frame_check2(42.0f, pl, 0) == 0 && (pl->sw.an_trg & 0x3C)) {
            a = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
            if (a > 0x4000 && a < 0x9556) {
                pl->x06 = 1;
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if (we04_hit_sub(pl) == 1) {
            break;
        }
        if (frame_check(36.0f, pl, 0) != 0 && pl->work08 == 0) {
            pl->work08++;
            func_6362B0(pl, 0x38);
        }
        if (frame_check2(42.0f, pl, 0) != 0) {
            if (pl->x06 != 0) {
                Pl_act_set(pl, 1, 0x5E, 4);
                break;
            }
            if (frame_check2(60.0f, pl, 0) == 0) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at094(PLW *pl, s32 arg1) {
    u8 s;
    s32 a;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        pl_chr_set2(pl, 0x57E, 2, arg1 != 0 ? 4 : 0);
        func_6362B0(pl, 0x2C);
        break;
    case 1:
        if (pl->x06 == 0 && frame_check2(26.0f, pl, 0) != 0 && frame_check2(38.0f, pl, 0) == 0 && (pl->sw.an_trg & 0x3C)) {
            a = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
            if (a < 0x4001) {
                pl->x06 = 2;
            } else if (a < 0x9556) {
                pl->x06 = 1;
            } else if (a < 0xD556) {
                pl->x06 = 3;
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (we04_hit_sub(pl) == 1) {
            break;
        }
        if (frame_check2(26.0f, pl, 0) != 0) {
            switch (pl->x06) {
            case 1:
                Pl_act_set(pl, 1, 0x58, 4);
                break;
            case 2:
                Pl_act_set(pl, 1, 0x64, 4);
                break;
            case 3:
                Pl_act_set(pl, 1, 0x65, 4);
                break;
            default:
                if (frame_check2(38.0f, pl, 0) == 0) {
                    ex_atk_ck(pl, 0);
                }
                break;
            }
        }
        break;
    }
}
#endif

#if defined(VERSION_MHG) && (!defined(G_PART) || G_PART == 48)
static void pl_at102(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x584, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_attack(PLW *pl) {
    pl->work350 = 0;
    switch (pl->flag15) {
    case 0x0:
        pl_at000(pl, 0);
        break;
    case 0x1:
        pl_at001(pl, 0);
        break;
    case 0x2:
        pl_at008(pl, 1);
        break;
    case 0x3:
        pl_at004(pl, 1);
        break;
    case 0x4:
        pl_at004(pl, 0);
        break;
    case 0x6:
        pl_at006(pl, 0);
        break;
    case 0x7:
        pl_at004(pl, 4);
        break;
    case 0x8:
        pl_at008(pl, 0);
        break;
    case 0x9:
        pl_at009(pl, 0);
        break;
    case 0xA:
        pl_at008(pl, 2);
        break;
    case 0xB:
        pl_at004(pl, 1);
        break;
    case 0xC:
        pl_at012(pl, 0);
        break;
    case 0xD:
        pl_at001(pl, 1);
        break;
    case 0xE:
        pl_at014(pl, 0);
        break;
    case 0xF:
        pl_at001(pl, 2);
        break;
    case 0x10:
        pl_at004(pl, 2);
        break;
    case 0x11:
        pl_at004(pl, 3);
        break;
    case 0x12:
        pl_at000(pl, 1);
        break;
    case 0x13:
        pl_at000(pl, 2);
        break;
    case 0x14:
        pl_at020(pl, 0);
        break;
    case 0x15:
        pl_at021(pl, 0);
        break;
    case 0x16:
        pl_at022(pl, 0);
        break;
    case 0x17:
        pl_at023(pl, 0);
        break;
    case 0x18:
        pl_at024(pl, 0);
        break;
    case 0x19:
        pl_at025(pl, 0);
        break;
    case 0x1A:
        pl_at026(pl, 0);
        break;
    case 0x1B:
        pl_at024(pl, 1);
        break;
    case 0x1C:
        pl_at028(pl, 0);
        break;
    case 0x1D:
        pl_at029(pl, 0);
        break;
    case 0x1E:
        pl_at030(pl, 0);
        break;
    case 0x1F:
        pl_at031(pl, 0);
        break;
    case 0x20:
        pl_at032(pl, 0);
        break;
    case 0x21:
        pl_at029(pl, 1);
        break;
    case 0x22:
        pl_at034(pl, 0);
        break;
    case 0x23:
        pl_at035(pl, 0);
        break;
    case 0x24:
        pl_at036(pl, 0);
        break;
    case 0x25:
        pl_at037(pl, 0);
        break;
    case 0x26:
        pl_at034(pl, 1);
        break;
    case 0x27:
        pl_at037(pl, 1);
        break;
    case 0x28:
        pl_at040(pl, 0);
        break;
    case 0x29:
        pl_at041(pl, 0);
        break;
    case 0x2A:
        pl_at042(pl, 0);
        break;
    case 0x2B:
        pl_at043(pl, 0);
        break;
    case 0x2C:
        pl_at041(pl, 1);
        break;
    case 0x2D:
        pl_at045(pl, 0);
        break;
    case 0x2E:
        pl_at036(pl, 1);
        break;
    case 0x2F:
        pl_at035(pl, 1);
        break;
    case 0x30:
        pl_at048(pl, 0);
        break;
    case 0x31:
        pl_at049(pl, 0);
        break;
    case 0x32:
        pl_at050(pl, 0);
        break;
    case 0x33:
        pl_at051(pl, 0);
        break;
    case 0x34:
        pl_at052(pl, 0);
        break;
    case 0x35:
        pl_at036(pl, 2);
        break;
    case 0x36:
        pl_at036(pl, 3);
        break;
    case 0x37:
        pl_at048(pl, 1);
        break;
    case 0x38:
        pl_at048(pl, 2);
        break;
    case 0x39:
        pl_at029(pl, 2);
        break;
    case 0x3A:
        pl_at029(pl, 3);
        break;
    case 0x3B:
        pl_at045(pl, 1);
        break;
    case 0x3C:
        pl_at045(pl, 2);
        break;
    case 0x3D:
        pl_at061(pl, 0);
        break;
    case 0x3E:
        pl_at049(pl, 1);
        break;
    case 0x3F:
        pl_at001(pl, 3);
        break;
    case 0x40:
        pl_at000(pl, 3);
        break;
    case 0x41:
        pl_at065(pl, 0);
        break;
    case 0x42:
        pl_at004(pl, 5);
        break;
    case 0x43:
        pl_at001(pl, 4);
        break;
    case 0x44:
        pl_at004(pl, 6);
        break;
    case 0x45:
        pl_at000(pl, 4);
        break;
    case 0x46:
        pl_at070(pl, 0);
        break;
    case 0x47:
        pl_at048(pl, 3);
        break;
    case 0x48:
        pl_at048(pl, 4);
        break;
    case 0x49:
        pl_at070(pl, 1);
        break;
    case 0x4A:
        pl_at074(pl, 0);
        break;
    case 0x4B:
        pl_at023(pl, 1);
        break;
    case 0x4C:
        pl_at024(pl, 2);
        break;
    case 0x4D:
        pl_at077(pl, 0);
        break;
    case 0x4E:
        pl_at078(pl, 0);
        break;
    case 0x4F:
        pl_at078(pl, 1);
        break;
    case 0x50:
        pl_at049(pl, 2);
        break;
    case 0x51:
        pl_at029(pl, 4);
        break;
    case 0x52:
        pl_at036(pl, 4);
        break;
#ifdef VERSION_MHG     /* G (game 0x55D370): the Dual Swords (0x53-0x66) and a fourth pl_at024 */
    case 0x53:
        pl_at083(pl, 0);
        break;
    case 0x54:
        pl_at084(pl, 0);
        break;
    case 0x55:
        pl_at085(pl, 0);
        break;
    case 0x56:
        pl_at085(pl, 1);
        break;
    case 0x57:
        pl_at085(pl, 2);
        break;
    case 0x58:
        pl_at085(pl, 3);
        break;
    case 0x59:
        pl_at089(pl, 0);
        break;
    case 0x5A:
        pl_at084(pl, 1);
        break;
    case 0x5B:
        pl_at091(pl, 0);
        break;
    case 0x5C:
        pl_at091(pl, 1);
        break;
    case 0x5D:
        pl_at093(pl, 0);
        break;
    case 0x5E:
        pl_at094(pl, 0);
        break;
    case 0x5F:
        pl_at094(pl, 1);
        break;
    case 0x60:
        pl_at093(pl, 1);
        break;
    case 0x61:
        pl_at093(pl, 2);
        break;
    case 0x62:
        pl_at083(pl, 1);
        break;
    case 0x63:
        pl_at083(pl, 2);
        break;
    case 0x64:
        pl_at091(pl, 2);
        break;
    case 0x65:
        pl_at091(pl, 3);
        break;
    case 0x66:
        pl_at102(pl, 0);
        break;
    case 0x67:
        pl_at024(pl, 3);
        break;
#endif
    default:
        pl_to_normal(pl, 0, 4, 0);
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00147520-0x00147E0C): pl_dm000..pl_dm002: damage reaction handlers (flinch, blow-back with joint position) */





#if !defined(G_PART) || G_PART == 24
static void pl_dm000(PLW *pl) {
    u8 s;
    u16 r;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0xCA, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        if (((ran_suu(1) & 0xFFFF) % 3) == 0) {
            pl_voice_req(pl, ((r = ran_suu(1)) & 1) + 0x20);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 6, 0x22);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            pl->work40C = 6;
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_dm001(PLW *pl, s32 arg1) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x586, 2, 0);
        } else {
            pl_chr_set2(pl, 0x587, 0, 0);
        }
        action_timer_calc(pl, 0);
        Pl_se_req2_com(pl, 0x67, 0, pl->pos, 1, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        vib_set_pl(pl, 1);
        pl_flag_set(pl, 2);
        pl_flag_set(pl, 0x8000);
        break;
    case 1:
        if (arg1 == 0) {
            if (frame_check2(30.0f, pl, 0) == 0) {
                sp30[0] = 0.0f;
                sp30[1] = 0.0f;
                switch (pl->kind) {
                case 3:
                    sp30[2] = -15.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 32.0f)) / 360.0f)));
                    break;
                case 0:
                case 4:
                    sp30[2] = -20.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 32.0f)) / 360.0f)));
                    break;
                }
                flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
                pl->pos[0] = pl->pos[0] + sp20[0];
                pl->pos[2] = pl->pos[2] + sp20[2];
            }
        } else {
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            switch (pl->kind) {
            case 3:
                if (frame_check2(104.0f, pl, 0) == 0) {
                    sp30[2] = -16.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 104.0f)) / 360.0f)));
                }
                break;
            case 0:
                if (frame_check2(104.0f, pl, 0) == 0) {
                    sp30[2] = -22.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 104.0f)) / 360.0f)));
                }
                break;
            case 4:
                if (frame_check2(94.0f, pl, 0) == 0) {
                    sp30[2] = -18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 94.0f)) / 360.0f)));
                }
                break;
            }
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp20[0];
            pl->pos[2] = pl->pos[2] + sp20[2];
        }
        if (pl->work194 == 0) {
            switch (pl->kind) {
            case 3:
                Pl_act_set2(pl, 2, 0xC, 0);
                break;
            case 4:
                pl_to_normal(pl, 0, 6, 0);
                break;
            default:
            case 0:
                pl_to_normal(pl, 0, 0xA, 0);
                break;
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_dm002(PLW *pl) {
    f32 sp90[3];
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];
    f32 f;
    u8 s;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
#ifdef VERSION_MHG
        pl->work93D = 0;    /* G: a knock-down ends the demon mode */
#endif
        pl_chr_set2(pl, 0xCD, 0, 0x10);
        get_joint_pos(pl, 2, sp90);
        pl->pos[1] = sp90[1];
        Pl_basic_flagset(pl, 2, 0, 1);
        pl->flag604 = 0;
        rate_clear_g(pl);
        sp70[0] = 0;
        sp70[1] = pl->ang[1];
        sp70[2] = 0;
        cpRotMatrix(sp70, sp30);
        sp90[0] = 0.0f;
        sp90[1] = 0.0f;
        sp90[2] = -11.17f;
        flvecApplyMat33(sp80, sp90, sp30);
        pl->vel[0] = 2.0f * sp80[0];
        pl->vel[2] = 2.0f * sp80[2];
        pl->vel[1] = 6.0f;
        rate_g_calc(pl, 0xC);
        pl->vel[1] = 0.0f;
        pl->acc[1] = -0.33333334f;
        pl->work08 = 0x10;
        break;
    case 1:
        if ((pl->work194 == 0) && (pl->char0 == 0xCD)) {
            pl_chr_set2(pl, 0xD6, 0, 0);
        }
        rate_add_g(pl);
        if (pl->work08 > 0) {
            pl->work08 = pl->work08 - 1;
            f = 63.0f + pl->x5AC;
            if (pl->pos[1] <= f) {
                pl->pos[1] = f;
            }
        } else if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= (63.0f + pl->x5AC))) {
            pl->x05++;
            pl->pos[1] = pl->x5AC;
            Pl_se_req2_com(pl, 0x42, 0, pl->pos, 1, 0);
            armor_sd_req(pl, 3);
            pl_chr_set2(pl, 0xCE, 0, 0);
            pl->st = 3;
            if (game_w.stage == 0) {
                func_546860(pl, pl->pos, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            if (pl->vital > 0) {
                pl->x05 = s + 1;
                pl_chr_set2(pl, 0xCF, 2, 0);
                break;
            }
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 3, 1, 0);
                break;
            }
            pl->x05 = 4;
            pl_flag_set(pl, 0x20000);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            pl->work40C = 6;
        }
        break;
    case 4:
        break;
    }
}
#endif

/* pl_dm003: `case 0: default:` on both kind switches; `work39C > 0x12` gives the original slti at. */
#if !defined(G_PART) || G_PART == 25
static void pl_dm003(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            switch (pl->kind) {
            case 0:
            default:
                pl_chr_set2(pl, 0x584, 3, 0);
                break;
            case 4:
                pl_chr_set2(pl, 0x3ED, 2, 0);
                break;
            case 3:
                pl_chr_set2(pl, 0x3ED, 2, 0);
                break;
            }
        } else {
            switch (pl->kind) {
            case 0:
            default:
                pl_chr_set2(pl, 0x584, 6, 0x28);
                break;
            case 4:
            case 3:
                pl_chr_set2(pl, 0x3EE, 2, 0);
                break;
            }
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x8000);
        pl->flag12 = 1;
        break;
    case 1:
        if (Pl_master_ck(pl) != 1) {
            if (Online_ck() == 0) {
                goto a;
            }
            goto b;
        }
a:
        if ((pl->work39C > 0x12) && !(pl->sw.now & 0x80)) {
            Pl_act_set2(pl, 2, 8, 0);
            break;
        }
b:
        switch (pl->kind) {
        case 3:
        case 4:
            if ((pl->char0 == 0x3ED) && (pl->work194 == 0)) {
                pl_chr_set2(pl, 0x3EE, 2, 0);
            }
            if (pl->work39C > 0x12) {
                guard_atk_ck(pl);
            }
            break;
        case 0:
            if (pl->work39C > 0x12) {
                guard_atk_ck(pl);
            }
            break;
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00148050-0x00148488): pl_dm005, pl_dm006: damage reaction handlers */





#if !defined(G_PART)
static void pl_dm005(PLW *pl) {
    f32 sp90[3];
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];
    f32 f;
    u8 s;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
#ifdef VERSION_MHG
        pl->work93D = 0;    /* G: a knock-down ends the demon mode */
#endif
        pl_chr_set2(pl, 0xD0, 0, 0);
        get_joint_pos(pl, 2, sp90);
        pl->pos[1] = sp90[1];
        pl->ang_y = pl->ang[1];
        Pl_basic_flagset(pl, 2, 0, 1);
        pl->flag604 = 0;
        rate_clear_g(pl);
        sp70[0] = 0;
        sp70[1] = (u16)pl->ang[1];
        sp70[2] = 0;
        cpRotMatrix(sp70, sp30);
        sp90[0] = 0.0f;
        sp90[1] = 0.0f;
        sp90[2] = 11.5f;
        flvecApplyMat33(sp80, sp90, sp30);
        pl->vel[0] = 2.0f * sp80[0];
        pl->vel[2] = 2.0f * sp80[2];
        pl->vel[1] = 0.0f;
        pl->acc[1] = -0.25f;
        pl->work08 = 0x10;
        break;
    case 1:
        if ((pl->work194 == 0) && (pl->char0 == 0xD0)) {
            pl_chr_set2(pl, 0xDC, 0, 0);
        }
        rate_add_g(pl);
        if (pl->work08 > 0) {
            pl->work08 = pl->work08 - 1;
            f = 63.0f + pl->x5AC;
            if (pl->pos[1] <= f) {
                pl->pos[1] = f;
            }
        } else if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= (63.0f + pl->x5AC))) {
            pl->x05++;
            pl->pos[1] = pl->x5AC;
            Pl_se_req2_com(pl, 0x42, 0, pl->pos, 1, 0);
            armor_sd_req(pl, 3);
            pl_chr_set2(pl, 0xD1, 0, 0);
            pl->st = 3;
            if (game_w.stage == 0) {
                func_546860(pl, pl->pos, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            if (pl->vital > 0) {
                pl->x05 = s + 1;
                pl_chr_set2(pl, 0xD8, 0, 0);
                break;
            }
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 3, 2, 0);
                break;
            }
            pl->x05 = 4;
            pl_flag_set(pl, 0x20000);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            pl->work40C = 6;
        }
        break;
    case 4:
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 26
static void pl_dm006(PLW *pl) {
    u8 s;
    u16 r;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0xCC, 0, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        if (((ran_suu(1) & 0xFFFF) % 3) == 0) {
            pl_voice_req(pl, ((r = ran_suu(1)) & 1) + 0x20);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xCF, 6, 0x3A);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            pl->work40C = 6;
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00148490-0x00148780): pl_dm007: damage reaction handler (knock-back jump) */





#if !defined(G_PART) || G_PART == 26
static void pl_dm007(PLW *pl, s32 arg1) {
    f32 spA0[3];
    f32 sp90[3];
    s32 sp80[3];
    f32 sp40[16];
    f32 v;
    u16 c;
    u8 s;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
#ifdef VERSION_MHG
        pl->work93D = 0;
#endif
        if (arg1 == 0) {
            pl_chr_set2(pl, 0xD5, 0, 0);
        } else {
            pl_chr_set2(pl, 0xDB, 0, 0);
        }
        get_joint_pos(pl, 2, spA0);
        pl->pos[1] = spA0[1];
        Pl_basic_flagset(pl, 2, 0, 1);
        pl->flag604 = 0;
        rate_clear_g(pl);
        sp80[0] = 0;
        sp80[1] = pl->ang[1];
        sp80[2] = 0;
        cpRotMatrix(sp80, sp40);
        spA0[0] = 0.0f;
        spA0[1] = 0.0f;
        if (arg1 != 0) {
            v = 7.0f;
        } else {
            v = -7.0f;
        }
        spA0[2] = v;
        flvecApplyMat33(sp90, spA0, sp40);
        pl->vel[0] = 2.0f * sp90[0];
        pl->vel[2] = 2.0f * sp90[2];
        pl->vel[1] = 20.0f;
        pl->acc[1] = -0.72727275f;
        break;
    case 1:
        if (pl->work194 == 0) {
            c = pl->char0;
            if (c != 0xD5) {
                if (c == 0xDB) {
                    goto go;
                }
            } else {
go:
                if (arg1 == 0) {
                    pl_chr_set2(pl, 0xD6, 0, 0);
                } else {
                    pl_chr_set2(pl, 0xDC, 0, 0);
                }
            }
        }
        rate_add_g(pl);
        if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= (63.0f + pl->x5AC))) {
            pl->x05++;
            Pl_se_req2_com(pl, 0x42, 0, pl->pos, 1, 0);
            armor_sd_req(pl, 3);
            pl_chr_set2(pl, 0xD7, 0, 0);
            pl->pos[1] = pl->x5AC;
            pl->st = 3;
            if (game_w.stage == 0) {
                func_546860(pl, pl->pos, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->ang[1] = (pl->ang[1] + 0x8000) & 0xFFFF;
            pl->ang_y = pl->ang[1];
            pl_chr_set2(pl, 0xD8, 0, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            pl->work40C = 6;
        }
        break;
    }
}
#endif

/* pl_dm008: the kind switch's default needs `case 0: default:` (a lone default reuses a0). */
#if !defined(G_PART) || G_PART == 26
static void pl_dm008(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        switch (pl->kind) {
        case 0:
        default:
            pl_chr_set2(pl, 0x586, 2, 0);
            break;
        case 4:
        case 3:
            pl_chr_set2(pl, 0x3EF, 2, 0);
            break;
        }
        pl_flag_set(pl, 0x8000);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00148870-0x00148AE0): pl_dm009, pl_dm011: damage reaction handlers */





#if !defined(G_PART) || G_PART == 26
static void pl_dm009(PLW *pl, s32 arg1) {
    f32 sp80[3];
    s32 sp70[3];
    f32 sp30[16];
    f32 v;
    u16 r;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        if (pl->flag12 != 0) {
            pl_chr_set2(pl, 0x4BB, 0, 0);
        } else {
            pl_chr_set2(pl, 0xC9, 0, 0);
        }
        Pl_basic_flagset(pl, 0, 0, 1);
        if (((ran_suu(1) & 0xFFFF) % 3) == 0) {
            pl_voice_req(pl, ((r = ran_suu(1)) & 1) + 0x20);
        }
        sp70[0] = 0;
        sp70[2] = 0;
        sp70[1] = *(u16 *)&pl->ang_y;
        cpRotMatrix(sp70, sp30);
        sp80[0] = 0.0f;
        sp80[1] = 0.0f;
        if (arg1 != 0) {
            v = 4.0f;
        } else {
            v = -4.0f;
        }
        sp80[2] = v;
        flvecApplyMat33(pl->vel, sp80, sp30);
        break;
    case 1:
        if (frame_check2(16.0f, pl, 0) == 0) {
            rate_add(pl);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 26
static void pl_dm011(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x585, 0, 0);
        action_timer_calc(pl, 0);
        Pl_se_req2_com(pl, 0x67, 0, pl->pos, 1, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl_flag_set(pl, 0x8000);
        vib_set_pl(pl, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x00148AE0-0x00149698): pl_dm015..pl_dm021, piyo_reset (static: MWCC then knows it leaves a0 alone, which pl_dm019/021 rely on) */





#if !defined(G_PART)
static void pl_dm015(PLW *pl, s32 unused_arg) {
    f32 sp30[3];
    f32 sp20[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xDA, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        break;
    case 1:
        if (frame_check2(40.0f, pl, 0) == 0) {
            sp30[0] = 0.0f;
            sp30[1] = 0.0f;
            sp30[2] = -18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 42.0f)) / 360.0f)));
            flvecApplyMat33(sp20, sp30, (f32 *)((u8 *)pl + 0x20));
            pl->pos[0] = pl->pos[0] + sp20[0];
            pl->pos[2] = pl->pos[2] + sp20[2];
            if ((pl->work39C % 7) == 0) {
                eft13_set(pl, 5, 9);
                eft13_set(pl, 8, 9);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xC, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm016(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        if (arg1 == 0) {
            pl->x07 = 4;
        } else {
            pl->x07 = 1;
        }
        pl_chr_set2(pl, 0xD3, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->work08 = 0;
        break;
    case 1:
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x06++;
            if (pl->x06 >= pl->x07) {
                pl->x05++;
                pl_chr_set2(pl, 0xD4, 2, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xC, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm017(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        if (pl->flag12 != 0) {
            pl_chr_set2(pl, 0x4BB, 0, 0);
        } else {
            pl_chr_set2(pl, 0xC9, 0, 0);
        }
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void piyo_reset(PLW *pl) {
    pl->x7AC = 0;
    pl->x7AA = 0;
    pl->work886 = 0x1E;
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm019(PLW *pl, s32 unused_arg) {
    s8 n;
    u8 s;
    s32 w;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        action_timer_calc(pl, 0);
        pl->flag12 = 0;
        pl->x43E = 0;
        pl_chr_set2(pl, 0x1A, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->work08 = 0x12C;
        pl->x06 = 0;
        pl->x07 = 5;
        pl->x7AC = 0;
        pl->x7AA = 0;
        pl->ang_y = pl->ang[1];
        func_558A80(pl, 0);
        break;
    case 1:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x06 = 0;
            pl->x07 = pl->x07 - 1;
            pl->work08 = 0x12C;
        } else if ((pl->sw.trg & 0x3FF) || (pl->sw.an_trg & 0x3C3C)) {
            pl->x06 = pl->x06 + 1;
            n = piyo_ret_tbl[pl->x07];
            if (Pl_Skill_ck(pl, 0x16) == 1) {
                n = n * 2;
            }
            if (pl->x06 >= n) {
                pl->x06 = 0;
                pl->x07 = pl->x07 - 1;
                pl->work08 = 0x12C;
            }
        }
        if (pl->x07 == 0) {
            piyo_reset(pl);
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm020(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x197, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm021(PLW *pl, s32 unused_arg) {
    f32 sp20[3];
    s32 w;
    s32 h;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->flag12 = 0;
        pl->x7B2 = 0;
        piyo_reset(pl);
        pl_chr_set2(pl, 0xE1, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->work08 = 0xF0;
            if (Pl_Skill_ck(pl, 0xB) == 1) {
                pl->work08 = pl->work08 / 2;
            }
            if (Pl_Skill_ck(pl, 0x15) == 1) {
                pl->work08 = pl->work08 * 2;
            }
            pl_chr_set2(pl, 0xD2, 0, 0x9A);
        }
        break;
    case 2:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xCF, 0, 0);
            break;
        }
        sp20[0] = 0.0f;
        sp20[1] = 20.0f;
        h = *(u16 *)&game_w.x1E % 60;
        sp20[2] = 0.0f;
        switch (h) {
        case 0:
        case 0xA:
        case 0x14:
            Eft06_set2(0.8f, pl, 4, 0x14, sp20);
            break;
        }
        Pl_stamina_calc(pl, 2);
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm022(PLW *pl, s32 arg1) {
    s16 v;
    u16 r;
    u8 s;

    pl->x06 = pl->x06 + 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 1);
        pl->flag12 = 0;
        pl->x7C4 = 0;
        pl->x7B2 = 0;
        piyo_reset(pl);
        if (arg1 == 0) {
            pl_chr_set2(pl, 0xE0, 4, 0);
            pl->work39C = 0;
            pl->x06 = 0;
            Eft06_set(0.75f, pl, 5, 0, 2);
            pl->work934 = 0x96;
            if (Pl_Skill_ck(pl, 0xA) == 1) {
                pl->work934 = pl->work934 / 2;
            }
            if (Pl_Skill_ck(pl, 0x14) == 1) {
                pl->work934 = pl->work934 * 2;
            }
            pl->work40C = 2;
            break;
        }
        pl->x05 = 2;
        if (pl->work934 <= 0) {
            pl->work934 = 0;
        }
        pl_chr_set2(pl, 0xD2, 2, 0x96);
        break;
    case 1:
        pl->work40C = 2;
        if (pl->work194 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xD2, 2, 0x96);
            pl->work40C = 0;
        }
        if ((pl->x05 < 3) && !(pl->x06 & 0x1F)) {
            Eft06_set(0.75f, pl, 5, 0, 2);
        }
        break;
    case 2:
        v = pl->work934 - 1;
        pl->work934 = v;
        if (v <= 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xCF, 0, 0);
            pl->work40C = 2;
            break;
        }
        if ((pl->char0 == 0xD2) && (frame_check(164.0f, pl, 0) != 0) && !((r = ran_suu(1)) & 0x3F)) {
            pl_chr_set2(pl, 0xDF, 0, 0);
        } else if ((pl->char0 == 0xDF) && (pl->work194 == 0)) {
            pl_chr_set2(pl, 0xD2, 4, 0xA4);
        }
        if ((pl->x05 < 3) && !(pl->x06 & 0x1F)) {
            Eft06_set(0.75f, pl, 5, 0, 2);
        }
        break;
    case 3:
        pl->work40C = 2;
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            pl->work40C = 6;
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 27
static void pl_dm023(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x3FA, 2, 1);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x8000);
        pl->flag12 = 1;
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_damage(PLW *pl) {
    u8 a;
    u8 b;

    Pl_view_reset(pl);
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    pl->x43E = 0;
    pl_flag_clr(pl, 0x80000);
    a = pl->flag15;
    if ((a != 0x11) && (a != 0x12)) {
        b = pl->work56B;
        if (b & 0xF) {
            pl->work56B = b & 0xF0;
            func_549200(pl, 4);
        }
    }
    switch (pl->flag15) {
    case 0:
        pl_dm000(pl);
        break;
    case 1:
        pl_dm001(pl, 0);
        break;
    case 2:
        pl_dm002(pl);
        break;
    case 3:
        pl_dm003(pl, 0);
        break;
    case 5:
        pl_dm005(pl);
        break;
    case 6:
        pl_dm006(pl);
        break;
    case 7:
        pl_dm007(pl, 0);
        break;
    case 8:
        pl_dm008(pl);
        break;
    case 9:
        pl_dm009(pl, 0);
        break;
    case 10:
        pl_dm001(pl, 1);
        break;
    case 11:
        pl_dm011(pl);
        break;
    case 12:
        pl_dm003(pl, 1);
        break;
    case 13:
        pl_dm009(pl, 1);
        break;
    case 14:
        pl_dm007(pl, 1);
        break;
    case 15:
        pl_dm015(pl, 0);
        break;
    case 16:
        pl_dm016(pl, 0);
        break;
    case 17:
        pl_dm017(pl, 0);
        break;
    case 18:
        pl_dm017(pl, 1);
        break;
    case 19:
        pl_dm019(pl, 0);
        break;
    case 20:
        pl_dm020(pl, 0);
        break;
    case 21:
        pl_dm021(pl, 0);
        break;
    case 22:
        pl_dm022(pl, 0);
        break;
    case 23:
        pl_dm023(pl, 0);
        break;
    case 24:
        pl_dm022(pl, 1);
        break;
    case 25:
        pl_dm016(pl, 1);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
#endif

#if !defined(G_PART) || G_PART == 28
static void pl_die000(PLW *pl, s32 arg1) {
    int c;
    u8 v;
    u8 idx;
    u16 r;
    u8 s;

    pl->work40E = 0xA;
    Pl_view_reset(pl);
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    pl_flag_clr(pl, 0x80000);
    v = pl->work56B;
    if (v & 0xF) {
        pl->work56B = v & 0xF0;
        func_549200(pl, 4);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0xD2, 0, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0xD2, 0, 0x8C);
            break;
        case 2:
            pl_chr_set2(pl, 0xD1, 0, 0x8C);
            pl_flag_set(pl, 0x20000);
            break;
        }
        vib_set_pl(pl, 3);
        action_timer_calc(pl, 0);
        pl_voice_req(pl, 0x22);
        pl_flag_clr(pl, 2);
        pl_flag_clr(pl, 0x8000);
        pl->flag12 = 0;
        c = (s8)Quest_remuneration_calc();
        if (c != 0) {
            pl->x05++;
        }
        if (Pl_master_ck(pl) == 0) {
            set01_set(3, 0, (s16)pl->id);
            if ((s8)c < 2) {
                Quest_forfeit_message();
            }
            if ((quest_w.x14 <= 0) && ((s8)c == 1)) {
                set01_set2(GSTR(lit_6243, gptr_00528418));
            }
            break;
        }
        v = pl->work91E;
        if (v < 0x3F) {
            pl->work91E = v + 1;
        } else {
            pl->work91E = 0x63;
        }
        PlayerDieCameraRequest();
        set01_set2(GSTR(lit_6244, gptr_0052841C));
        if ((s8)c < 2) {
            Quest_forfeit_message();
        }
        if (pl->x05 < 2) {
            Quest_restart();
            die_bgm_set();
            break;
        }
        if ((s8)c == 1) {
            set01_set2(GSTR(lit_6243, gptr_00528420));
        }
        set01_set2(GSTR(lit_6245, gptr_00528424));
        break;
    case 1:
        if ((pl->char0 == 0xD2) && (frame_check(164.0f, pl, 0) != 0) && !((r = ran_suu(1)) & 0x3F)) {
            pl_chr_set2(pl, 0xDF, 0, 0);
        } else if ((pl->char0 == 0xDF) && (pl->work194 == 0)) {
            pl_chr_set2(pl, 0xD2, 4, 0xA4);
        }
        if (Game_clear_ck(2) == 1) {
            pl->x05++;
            break;
        }
        if ((pl->work39C == 0x12C) && (Pl_master_ck(pl) == 1)) {
            set01_set(0, 7, 0);
        }
        if ((pl->work39C == 0x258) && (Pl_master_ck(pl) == 1)) {
            idx = game_w.x2F;
            pl->x738 = 2;
            pl->work73C = stage_start_pos[idx][0];
            pl->work740 = stage_start_pos[idx][1];
            pl->work744 = stage_start_pos[idx][2];
            pl->work570 = stage_start_ang[idx];
            pl->x73A = idx;
            net_send_pl(pl, 4, 0);
        }
        break;
    case 2:
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 28
static void pl_die(PLW *pl) {
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    switch (pl->flag15) {
    case 0:
        pl_die000(pl, 0);
        break;
    case 1:
        pl_die000(pl, 1);
        break;
    case 2:
        pl_die000(pl, 2);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
#endif

#if !defined(G_PART) || G_PART == 28
static void pl_demo000(PLW *pl, s32 unused_arg) {
    EMW *e;
    u8 *q;
    s16 i;
    s16 k;
    u8 *g;
    u8 s;

    pl->work40E = 0xA;
    pl->work40C = 0xA;
    pl->x01 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        k = -1;
        pl->x06 = 0;
        i = 0;
        g = (u8 *)&game_w;
        do {
            if ((g[0x28] == 0x12) && ((e = pull_enemy_work()) != 0)) {
                q = (u8 *)game_w.x28 + (int)i;
                e->mdl_no = i;
                e->kind = *q;
                e->stg = game_w.stage;
                e->type = 0;
                e->type = 0xFF;
                e->x616 = pl->id;
                enemy_mv(e);
                e->pos[0] = pl->pos[0];
                e->pos[1] = pl->pos[1];
                e->pos[2] = pl->pos[2];
                e->ang[1] = pl->ang[1];
                pl->x824 = e;
                k = *q;
                break;
            }
            i++;
            g++;
        } while (i < 4);
        if (k != 0x12) {
            pl_to_normal(pl, 0, 4, 0);
        } else {
            pl->x07 = 0;
            pl_chr_set2(pl, 0xDD, 0, 0);
        }
        if (Pl_master_ck(pl) == 1) {
            PlComebackCameraRequest();
        }
        break;
    case 1:
        e = pl->x824;
        if (e == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        get_joint_pos_em(e, 0x1D, pl->pos);
        if (e->char0 == 0x3EB) {
            Pl_act_set2(pl, 4, 1, 2);
        }
        pl->ang[1] = e->ang[1];
        pl->ang_y = pl->ang[1];
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0014A050-0x0014A548): pl_demo001..005: demo (cutscene) player states */





#if !defined(G_PART) || G_PART == 28
static void pl_demo001(PLW *pl, s32 unused_arg) {
    f32 f;
    u8 s;

    pl->work40E = 0xA;
    pl->work40C = 0xA;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xDE, 0, 0);
        pl->st = 2;
        break;
    case 1:
        if (frame_check2(28.0f, pl, 0) != 0) {
            pl->pos[1] = pl->pos[1] - 4.0f;
        }
        f = pl->x5AC;
        if (pl->pos[1] <= f) {
            pl->pos[1] = f;
        }
        if (pl->work194 == 0) {
            pl->x05++;
            pl_chr_set2(pl, 0xD1, 0, 0x46);
            pl->st = 0;
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 0, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 28
static void pl_demo002(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work88D = 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x197, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 28
static void pl_demo003(PLW *pl, s32 unused_arg) {
    u8 s;
    u8 v;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x26F, 6, 0);
        pl->work39C = 0;
        v = pl->work56B;
        if (v & 0xF) {
            pl->work56B = v & 0xF0;
            func_549200(pl, 4);
        }
        if (Pl_master_ck(pl) == 1) {
            PlayerDieCameraRequest();
        }
        break;
    case 1:
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 28
static void pl_demo004(PLW *pl, s32 unused_arg) {
    u8 s;

    pl->work40C = 4;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0xD8, 0, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->vital = pl->work792;
            pl->vital_red = pl->work792;
            pl->work882 = 0x12C;
            pl->work8C0 = 0x2A30;
            pl->stamina = pl->work882;
            pl->x7AC = 0;
            pl->work8CC = 0;
            pl->work918 = 0;
            pl->work91A = 0;
            pl->work91C = 0;
            pl->work930 = 0;
            pl->x7AA = 0;
            pl->x7BA = 0;
            pl->x7B2 = 0;
            pl->x7C4 = 0;
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_demo005(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 v;
    u8 s;

    pl->work40C = 0xA;
    pl->work40E = 0xA;
    Pl_view_reset(pl);
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    pl_flag_clr(pl, 0x80000);
    pl->pos[1] = pl->x5AC;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0xD2, 0, 0);
        action_timer_calc(pl, 0);
        pl_voice_req(pl, 0x22);
        pl_flag_clr(pl, 2);
        pl_flag_clr(pl, 0x8000);
        pl->flag12 = 0;
        pl->work08 = 0x12C;
        v = pl->work56B;
        if (v & 0xF) {
            pl->work56B = v & 0xF0;
            func_549200(pl, 4);
        }
        if (Pl_master_ck(pl) == 1) {
            PlayerDieCameraRequest();
        }
        break;
    case 1:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x05++;
        }
        break;
    case 2:
        pl->be_flag = 0;
        pl->x01 = 0;
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 29
static void pl_demo(PLW *pl) {
    pl->work8C2 = 0;
    pl->x8C6 = 0;
    switch (pl->flag15) {
    case 0:
        pl_demo000(pl, 0);
        break;
    case 1:
        pl_demo001(pl, 0);
        break;
    case 2:
        pl_demo002(pl, 0);
        break;
    case 3:
        pl_demo003(pl, 0);
        break;
    case 4:
        pl_demo004(pl, 0);
        break;
    case 5:
        pl_demo005(pl, 0);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
#endif

/* Player code (SLPM_654.95 0x0014A610-0x0014A6A8): egg_set (egg carrying: item bits in work56B) */





#if !defined(G_PART) || G_PART == 29
static void egg_set(PLW *pl) {
    s32 id;

    if ((Pl_master_ck(pl) == 1) && (act_ck(pl, 5, 0xA) == 0)) {
        id = Pl_hold_item_ck(pl) & 0xFFFF;
        if (id != 0xFFFF) {
            pl->work56B = ((Check_hold_item(id) & 0xFF) - 1) * 0x10;
        } else {
            pl->work56B = 0;
        }
    }
    pl->work56B = (pl->work56B & 0xF0) | 4;
}
#endif

#ifdef VERSION_MHG
#define HOLD_CANNONBALL 0x201
#else
#define HOLD_CANNONBALL 0xA3
#endif
#if !defined(G_PART) || G_PART == 46
static void egg_com_ck(PLW *pl, int arg1) {
    u8 sp3F;
    u16 sp3C;
    f32 sp30[3];
    s32 u;
    u16 v;

    if (Pl_master_ck(pl) != 0) {
        switch (game_w.x0D5) {
        case 6:
        case 5:
            Pl_act_set2(pl, 4, 3, 0);
            break;
        case 7:
        case 8:
            Pl_act_set2(pl, 4, 5, 0);
            break;
        default:
            if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xFFFF) {
                Pl_act_set2(pl, 5, 3, 0);
                break;
            }
            if (pl->sw.pow[0] >= 0x28) {
                pl->ang_y = stick_dir_set(pl, 0);
                if ((pl->sw.now & 0x10) && (pl->work760 == 0)) {
                    if (act_ck(pl, 5, 2) == 0) {
                        Pl_act_set2(pl, 5, 2, 0);
                    }
                } else if ((act_ck(pl, 5, 1) == 0) && (pl->work760 == 0)) {
                    Pl_act_set2(pl, 5, 1, 0);
                }
            } else if (pl_flag_ck(pl, 0x02001200) != 0) {
                if ((s16)arg1 == 1) {
                    if (pl->work08 == 0) {
                        goto go;
                    }
                } else {
go:
                    Pl_act_set2(pl, 5, 0, 0);
                }
            }
            if (Game_clear_ck(1) != 1) {
                v = pl->sw.trg;
                if (v & 0x40) {
                    if (pl->sw.pow[0] >= 0x55) {
                        if (pl->stamina >= 0x4B) {
                            Pl_act_set2(pl, 0, 0x1C, 4);
                        }
                    } else {
                        Pl_act_set2(pl, 0, 8, 0);
                    }
                    return;
                }
                if (pl->sw.an_trg & 0x3C) {
                    Pl_act_set2(pl, 0, 4, 0);
                    break;
                }
#ifdef VERSION_MHG      /* G egg_com_ck (game 0x560D20): not while work88C is set; the cannonball is G item 513 */
                if ((v & 0x200) && pl->work88C == 0) {
#else
                if (v & 0x200) {
#endif
                    Pl_act_set2(pl, 5, 3, 0);
                }
                job_special_com_ck(pl, 2);
                if (pl->sw.trg & 0x20) {
                    u = St_unique_ck(pl, sp30, &sp3C, &sp3F) & 0xFFFF;
                    switch (u) {
                    case 21:
                        Share_item_conv(pl);
                        if ((Pl_hold_item_ck(pl) & 0xFFFF) == 0xFFFF) {
                            Pl_act_set2(pl, 5, 0xA, 0);
                            return;
                        }
                        break;
                    case 24:
                        if ((Pl_hold_item_ck(pl) & 0xFFFF) == HOLD_CANNONBALL) {
                            pl->work56B = pl->work56B & 0xF0;
                            Pl_item_stack(pl, HOLD_CANNONBALL, -0xA);
                            pl->ang_y = sp3C;
                            pl->cnt39A = sp3F;
                            Pl_act_set2(pl, 0, 0x37, 0x20);
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
}
#endif

/* Player code (SLPM_654.95 0x0014AA50-0x0014AD78): pl_egg00, pl_egg01, pl_egg02: egg-carrying action states */





#if !defined(G_PART) || G_PART == 30
static void pl_egg00(PLW *pl, s32 arg1) {
    s32 w;
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work08 = 0x1E;
        pl->work760 = 0;
    case 1:
        if (pl->char0 != 0x33) {
            pl_chr_set2(pl, 0x33, 4, 0);
        }
        if (arg1 == 1) {
            w = pl->work08;
            if (w <= 0) {
                egg_com_ck(pl, 0);
                break;
            }
            pl->work08 = w - 1;
            break;
        }
        egg_com_ck(pl, 0);
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_egg01(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work08 = 0x14;
        pl->work760 = 0x1E;
        pl_chr_set2(pl, 0x34, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        egg_com_ck(pl, 1);
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_egg02(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->work011 == 0) {
            pl_chr_set2(pl, 0x35, 4, 0);
        } else {
            pl_chr_set2(pl, 0x3B, 4, 0);
        }
        pl->work08 = 0x14;
        pl->work760 = 0x1E;
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x02000200);
        break;
    case 1:
        if (front_land_ck2(85.0f, -60.0f, pl, 0) != 0) {
            pl->ang_y = pl->ang[1];
            Pl_act_set2(pl, 5, 5, 0);
            break;
        }
        Pl_stamina_calc(pl, -1);
        if (pl->stamina <= 0) {
            Pl_act_set2(pl, 5, 4, 0);
            break;
        }
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        egg_com_ck(pl, 1);
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 31
static void pl_egg03(PLW *pl, s32 arg1) {
    s32 v;
    s32 id;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 1) {
            pl_chr_set2(pl, 0x37, 4, 0);
        } else {
            pl_chr_set2(pl, 0x36, 4, 0);
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        v = (arg1 == 1) ? 4 : 0x72;
        if (frame_check((f32)v, pl, 0) != 0) {
            pl->x05++;
            pl->work56B = pl->work56B & 0xF0;
            if (arg1 != 2) {
                func_549200(pl, 4);
            }
            if ((arg1 == 0) && (Pl_master_ck(pl) == 1)) {
                id = Pl_hold_item_ck(pl) & 0xFFFF;
                if (id != 0xFFFF) {
                    set01_set(1, 0xC, (s16)id);
                    Pl_item_stack(pl, id, -0x64);
                }
            }
            break;
        }
        egg_set(pl);
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
        }
        break;
    }
}
#endif

/* pl_egg05: the arg1 test is a switch with default first (agent B; an if/else gives the same code minus the original's nop after the compare's delay slot). */
#if !defined(G_PART)
static void pl_egg05(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    f32 f;
    s32 w;
    u8 s;

    egg_set(pl);
    pl->work08++;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->st = 2;
        switch (arg1) {
        default:
            pl_chr_set2(pl, 0x38, 0, 0);
            pl->vel[0] = 0.0f;
            pl->vel[2] = 20.0f;
            pl->vel[1] = 18.0f;
            pl->acc[0] = 0.0f;
            pl->acc[2] = -(pl->vel[2] / 80.0f);
            rate_g_calc(pl, 0x13);
            break;
        case 1:
            pl_chr_set2(pl, 0x38, 8, 0x20);
            pl->flag604 = 0;
            pl->vel[0] = 0.0f;
            pl->vel[2] = 8.0f;
            pl->vel[1] = -10.0f;
            pl->acc[2] = 0.0f;
            pl->acc[0] = 0.0f;
            pl->acc[1] = -0.72727275f;
            break;
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->work08 = 0;
        pl->x07 = 0;
        break;
    case 1:
        pl->vel[0] = pl->vel[0] + pl->acc[0];
        pl->vel[1] = pl->vel[1] + pl->acc[1];
        pl->vel[2] = pl->vel[2] + pl->acc[2];
        if (pl->vel[0] < 0.0f) {
            pl->vel[0] = 0.0f;
        }
        if (pl->vel[2] < 0.0f) {
            pl->vel[2] = 0.0f;
        }
        sp40[0] = pl->vel[0];
        sp40[1] = pl->vel[1];
        sp40[2] = pl->vel[2];
        flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp30[0];
        pl->pos[1] = pl->pos[1] + sp30[1];
        pl->pos[2] = pl->pos[2] + sp30[2];
        if (pl->vel[1] < 0.0f) {
            pl->work08++;
            f = pl->x5AC;
            if (!(f < pl->pos[1])) {
                pl->pos[1] = f;
                pl->st = 0;
                pl->flag604 = 0;
                w = pl->work08;
                if (w >= 0x32) {
                    pl->work56B = pl->work56B & 0xF0;
                    func_549200(pl, 4);
                    Pl_act_set2(pl, 0, 0x26, 0);
                    break;
                }
                if (w >= 0x1E) {
                    Pl_act_set2(pl, 5, 7, 2);
                    break;
                }
                Pl_act_set2(pl, 5, 8, 2);
            }
        }
        break;
    }
}
#endif

/* Player code (SLPM_654.95 0x0014B200-0x0014B2DC): pl_egg07: egg-carrying action state */





#if !defined(G_PART) || G_PART == 32
static void pl_egg07(PLW *pl, s32 arg1) {
    u8 s;

    egg_set(pl);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x39, 0, 0);
        } else {
            pl_chr_set2(pl, 0x3A, 0, 0);
        }
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 5, 0, 2);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 32
static void pl_egg(PLW *pl) {
    pl->flag12 = 0;
    switch (pl->flag15) {
    case 0:
        pl_egg00(pl, 0);
        break;
    case 1:
        pl_egg01(pl, 0);
        break;
    case 2:
        pl_egg02(pl, 0);
        break;
    case 3:
        pl_egg03(pl, 0);
        break;
    case 4:
        pl_egg03(pl, 1);
        break;
    case 5:
        pl_egg05(pl, 0);
        break;
    case 6:
        pl_egg05(pl, 1);
        break;
    case 7:
        pl_egg07(pl, 0);
        break;
    case 8:
        pl_egg07(pl, 1);
        break;
    case 9:
        pl_egg00(pl, 1);
        break;
    case 10:
        pl_egg03(pl, 2);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
#endif

/* Player code (SLPM_654.95 0x0014B3F0-0x0014C1E0): pl_chat00..pl_chat16 (except dispatcher): chat/emote action states */





#if !defined(G_PART) || G_PART == 32
static void pl_chat00(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xD2, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (Pl_master_ck(pl) == 1) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                Pl_act_set2(pl, 6, 1, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 32
static void pl_chat01(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xCF, 0, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 32
static void pl_chat02(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x26C, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 32
static void pl_chat03(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x26F, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && (frame_check2(90.0f, pl, 0) != 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                Pl_act_set2(pl, 6, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_chat04(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x270, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xCF, 4, 0x3C);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 33
static void pl_chat05(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x274, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && (frame_check2(380.0f, pl, 0) != 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                Pl_act_set2(pl, 6, 6, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART)
static void pl_chat06(PLW *pl, s32 unused_arg) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x275, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 4, 0x36);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat07(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x26E, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat08(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x273, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat09(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, chat09_chr_tbl_002F17C0[arg1].a, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, chat09_chr_tbl_002F17C0[arg1].b, 0);
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat10(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x271, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat11(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x276, 0x10, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat12(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x272, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat16(PLW *pl, s32 unused_arg) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x279, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
#endif

#if !defined(G_PART) || G_PART == 34
static void pl_chat(PLW *pl) {
    Pl_view_reset(pl);
    switch (pl->flag15) {
    case 0:
        pl_chat00(pl, 0);
        break;
    case 1:
        pl_chat01(pl, 0);
        break;
    case 2:
        pl_chat02(pl, 0);
        break;
    case 3:
        pl_chat03(pl, 0);
        break;
    case 4:
        pl_chat04(pl, 0);
        break;
    case 5:
        pl_chat05(pl, 0);
        break;
    case 6:
        pl_chat06(pl, 0);
        break;
    case 7:
        pl_chat07(pl, 0);
        break;
    case 8:
        pl_chat08(pl, 0);
        break;
    case 9:
        pl_chat09(pl, 0);
        break;
    case 10:
        pl_chat10(pl, 0);
        break;
    case 11:
        pl_chat11(pl, 0);
        break;
    case 12:
        pl_chat12(pl, 0);
        break;
    case 13:
        pl_chat09(pl, 1);
        break;
    case 14:
        pl_chat09(pl, 2);
        break;
    case 15:
        pl_chat09(pl, 3);
        break;
    case 16:
        pl_chat16(pl, 0);
        break;
    default:
        pl_to_normal(pl, 0, 4, 0);
    }
}
#endif

/* Player code (SLPM_654.95 0x0014C3E0-0x0014C4F4): pl_move (loop over the 8 players), pl_move_sub_sub (state group dispatcher: flag14 selects normal/attack/damage/die/demo/egg/chat) */





#if !defined(G_PART) || G_PART == 47
void pl_move(void) {
    s32 i;
    PLW *p;

    pl_sw_set();
    i = 0;
    p = player_work;
    do {
        pl_move_sub(p);
        i++;
        p++;
    } while (i < GV(8, 10));
    hit_timer_calc_shl();
}
#endif

#if !defined(G_PART) || G_PART == 35
static void pl_move_sub_sub(PLW *pl) {
    pl->work8F0 = 0;
    pl->work4D4 = 1;
    switch (pl->flag14) {
    case 0:
        pl_normal(pl);
        break;
    case 1:
        pl_attack(pl);
        break;
    case 2:
        pl_damage(pl);
        break;
    case 3:
        pl_die(pl);
        break;
    case 4:
        pl_demo(pl);
        break;
    case 5:
        pl_egg(pl);
        break;
    case 6:
        pl_chat(pl);
    }
}
#endif

/* pl_move_sub: original bytes kept (near-match C in src/main/pl/pl_nm.c, linked weak by the PC build): build/raw/pl_move_sub.inc,
   config/c_rawfuncs.txt */
#ifdef __MWERKS__
#if !defined(G_PART)
asm void pl_move_sub(PLW *pl)
{
#include "pl_move_sub.inc"
#ifndef __MWERKS__
unsigned char hit_cap_cap3_m(void *, void *, f32 *);   /* (declared before its first use) */
unsigned char hit_cap_sphr_m(void *, f32 *, f32 *, f32);   /* (declared before its first use) */
unsigned char hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb);   /* (declared before its first use) */
#endif
}
#endif
#endif

/* pl_turn_sub: written from the asm (agent B). The rotation step r is a u16 used with -= / +=, the stick angle b is read as u16. */
#if !defined(G_PART) || G_PART == 36
static void pl_turn_sub(PLW *pl) {
    u8 k;
    int t;
    u32 d;
    int b;
    int a;
    u16 r;
    int v;

    if (pl->x738 == 0) {
        if ((pl->flag12 != 0) && (pl->flag14 == 0)) {
            k = pl->flag15;
            if ((u32)(k - 1) > 1) {
                if (k == 0xC) {
                    goto b71;
                }
                goto other;
            }
b71:
            t = 0x71C;
        } else {
other:
            if ((act_ck(pl, 0, 1) != 0) || (act_ck(pl, 0, 0x24) != 0)) {
                t = 0x71C;
            } else if (act_ck(pl, 0, 0x1F) != 0) {
                t = 0x5B0;
            } else if (act_ck(pl, 0, 0x3F) != 0) {
                t = 0;
            } else {
                t = 0xFA4;
            }
        }
        a = pl->ang[1];
        b = *(u16 *)&pl->ang_y;
        d = (b - (a & 0xFFFF)) & 0xFFFF;
        switch (pl->char0) {
        case 3:
            r = 0x80;
            break;
        case 4:
            r = 0xA0;
            break;
        default:
            r = 0;
            break;
        }
        if ((u32)((d + t) & 0xFFFF) < (u32)(t * 2)) {
            pl->ang[1] = b;
            pl->work2F8 = 0;
            v = pl->work750;
            if (v + 0x300 < 0x601) {
                pl->work750 = 0;
            } else if (v < 0x8000) {
                pl->work750 = v - 0x300;
            } else {
                pl->work750 = v + 0x300;
            }
        } else if (d < 0x8000) {
            pl->ang[1] = (a + t) & 0xFFFF;
            pl->work750 -= r;
        } else {
            pl->ang[1] = (a - t) & 0xFFFF;
            pl->work750 += r;
        }
        v = pl->work750;
        if (v < 0xF601 && v >= 0xA00) {
            if (v >= 0x8000) {
                pl->work750 = 0xF600;
            } else {
                pl->work750 = 0xA00;
            }
        }
    }
}
#endif

/* pl_horm_sub: written from the asm (agent B). work81A is read and written as u16 (lhu), the -= / += forms reload it. */
#define W81A (*(u16 *)&pl->work81A)
#if !defined(G_PART) || G_PART == 36
static void pl_horm_sub(PLW *pl) {
    f32 dmin;
    f32 d;
    u32 i;
    u32 best;
    EMW *e;
    int v;
    int t;
    u16 ang;

    best = 0xFF;
    pl->work81C = 0;
    dmin = 1500.0f;
    e = em_work;
    if (pl->char0 == 0x26) {
        i = 0;
        do {
            if (e->be_flag != 0 && e->x01 != 0) {
                d = flvecCalcDistance(pl->pos, e->pos);
                if (!(dmin < d)) {
                    best = i;
                    dmin = d;
                    pl->work81C = 1;
                }
            }
            i++;
            e++;
        } while (i < 20);
    }
    if (pl->work81C == 0) {
        v = W81A;
        if (v != 0) {
            t = v + 0x400;
            if (t < 0x801 && t >= 0) {
                W81A = 0;
            } else if ((s16)v >= 0) {
                W81A -= 0x400;
            } else {
                W81A += 0x400;
            }
        }
    } else {
        ang = (((calc_vec_ang2(em_work[best].pos, pl->pos) & 0xFFFF) + 0x4000) & 0xFFFF) + 0x10000 - pl->ang[1];
        if (ang >= 0x8000) {
            if (ang < 0xD556) {
                ang = 0xD556;
            }
            v = W81A;
            if (v >= 0x8000) {
                t = v - 0x800;
                if (t <= ang) {
                    W81A = ang;
                } else {
                    W81A = t;
                }
            } else {
                W81A = v - 0x800;
            }
        } else {
            if (ang > 0x2AAB) {
                ang = 0x2AAB;
            }
            v = W81A;
            if (v < 0x8000) {
                t = v + 0x800;
                if (t >= ang) {
                    W81A = ang;
                } else {
                    W81A = t;
                }
            } else {
                W81A = v + 0x800;
            }
        }
    }
}
#endif
#undef W81A
