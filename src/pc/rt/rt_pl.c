/*
 * rt_pl.c - the player helpers the decompiled player code (src/main/pl,
 * agent F) calls that are not decompiled yet, written from the asm of
 * SLPM_654.95 main (addresses per function) so the game's own pl_move /
 * pl_normal / pl_attack state machines can run on the PC.
 *
 * These are host versions in the style of rt_main.c: logic read from the
 * asm (m2c drafts from tools/pl_draft.py, checked by hand against the
 * asm), raw offsets into PLW because several fields have no name yet.
 * They are NOT matching C and are not part of the PS2 build; when the
 * real files (g_act_set, g_pl_voice_req, World_calc, Pl_bari_ck, f_pl's
 * 0x1510xx part, g_Pl_hold_item_ck ...) are decompiled, drop the copy here.
 *
 * Functions with no visible effect on a single-player test (network,
 * items picked from the stage, menus, messages) are stubs and say so.
 */
#include "rt.h"
#include "types.h"
#include "game.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PS8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define PU8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PU16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define PS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PU32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define PF32(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define GW8(o)  PU8(&game_w, o)
#define GW16(o) PU16(&game_w, o)

#define STUB(name) { static int o_; if (!o_++ && rt_pl_trace()) fprintf(stderr, "rt_pl: %s not ported (stub)\n", name); }

typedef u8 PL;   /* the player work, PLW (0xA00 bytes), by raw offset */

/* game C and runtime helpers */
int Pl_master_ck(void *pl);
int ran_suu(int);
void net_send_pl(void *pl, int a, int b);
int act_ck(void *chr, int a, int b);
void pl_to_normal(void *pl, int a, int b, int c);
void pl_to_normal_b(void *pl, int a, int b, int c);
void frame_init(void *w, int frame, int blend, int n);
void frame_move(void *w);
void cpRotMatrix(s32 *ang, f32 *m);
int softdip_ck(int);
void Pl_se_req2(void *w, int code, int id, f32 *pos, int type, int chg);
void *Stage_data_get(int stg);
f32 flvecCalcDistance(f32 *a, f32 *b);
void flvecNormalize(f32 *v);
f32 flArcTan2(f32 y, f32 x);
f32 flSqrt(f32);
void flvecApplyMat33(f32 *d, f32 *s, f32 (*m)[4]);
int GetGroundHitAreaUpper(void *pl, f32 *pos, f32 *out);
int Game_clear_ck(int);
s16 Stage_env_ck(u8);
extern f32 *lpView;
extern u8 em_work[];
extern u8 Ken_data[][0x18];
extern u8 Item_data[][0x10];
extern u8 Shell_data[][4];
extern u8 Gun_data[][0x14];
extern u8 Battle_type[];
extern s16 *Pl_slash_tbl[];
extern u8 User_data[];

extern u8 player_work[];
static void *player_work_ptr(int n) { return player_work + 0xA00 * (n & 7); }

static int rt_pl_trace(void)
{
    static int t = -1;
    if (t < 0) t = getenv("RT_TRACE") != NULL || getenv("RT_PL_TRACE") != NULL;
    return t;
}

/* ------------------------------------------------ g_act_set (0x14ECB0..) */




/* ------------------------------------------------ motion requests (0x151220..) */
/* pl_chr_set_com (0x151220): queue motion chr on layer n: +0x2DC[n] = chr,
 * +0x2EC[n] = blend / 2 (the layers run at speed 2), +0x2E4[n] = start
 * frame, +0x2FC[n] = 0 (pl_chr_sub calls frame_init for it). The fifth
 * argument (t0) is the layer. */
static void pl_chr_set_com(void *pl, int chr, int blend, int frame, int n)
{
    PS16(pl, 0x2DC + n * 2) = (s16)chr;
    PS16(pl, 0x2EC + n * 2) = (s16)(blend / 2);
    PS16(pl, 0x2E4 + n * 2) = (s16)frame;
    PU8(pl, 0x2FC + n) = 0;
}





/* ------------------------------------------------ Pl_bari_ck file (0x14FC40..) */

/* Get_equip_value (0x2A1F70?, f_ud): the master's equipment stat n from
 * User_data (0 attack, 1 defence, 2-5 resistances, 6-12 sword element) */
s16 Get_equip_value(int n);

int Pl_item_num_ck(void *pl, int id);





/* Get_equip_value (f_ud? 0x2A..; read from the asm): sums from the
 * equipment tables for the master player's User_data equipment. User_data
 * +0x3CD equipment type (6 sword, 7 gun), +0x3CE weapon id, +0x3D0 gun
 * upgrades, +0x3D2..+0x3D6 armour ids. Only the sword path and the armour
 * sums are written; guns give their base value [partial port]. */
s16 Get_equip_value(int n)
{
    extern u8 Armor_Head_Data[][0x14], Armor_Body_Data[][0x14], Armor_Arm_Data[][0x14],
              Armor_Waist_Data[][0x14], Armor_Leg_Data[][0x14];
    u8 *u = User_data;
    u16 wid = PU16(u, 0x3CE);
    int type = PU8(u, 0x3CD);
    int k;
    switch (n & 0xFF) {
    case 0:
        if (type == 7) return (s16)PS16(Gun_data[wid], 8);
        return Ken_data[wid][8];
    case 1: case 2: case 3: case 4: case 5:
        k = n == 1 ? 8 : 9 + (n - 2);
        {
            s16 v = (s16)((s8)Armor_Head_Data[PU8(u, 0x3D3)][k] + (s8)Armor_Body_Data[PU8(u, 0x3D4)][k]
                        + (s8)Armor_Arm_Data[PU8(u, 0x3D5)][k] + (s8)Armor_Waist_Data[PU8(u, 0x3D6)][k]
                        + (s8)Armor_Leg_Data[PU8(u, 0x3D2)][k]);
            if (n == 1)
                v += type == 7 ? (s8)Gun_data[wid][0xA] : (s8)Ken_data[wid][0xA];
            return v;
        }
    default:
        if (n <= 12 && type == 6) return Ken_data[wid][0xB + n - 6];
        return 0;
    }
}

/* ------------------------------------------------ f_pl 0x1513B0.. (rates) */


/* ------------------------------------------------ World_calc file (0x152050..) */









/* ------------------------------------------------ misc (g_Pl_hold_item_ck ..) */

/* Get_dist_to_view (0x169DC0) / Get_view_dir (0x169DD0): distance from
 * the camera eye; camera heading (0x10000 per turn) */
f32 Get_dist_to_view(f32 *p)
{
    return flvecCalcDistance(p, lpView);
}
int Get_view_dir(void)
{
    f32 a = flArcTan2(-(lpView[5] - lpView[2]), lpView[3] - lpView[0]);
    return (int)(0.5f + 65536.0f * a / 6.2831855f) & 0xFFFF;
}
/* calc_vec_ang2 (0x120430): heading from b to a */
int calc_vec_ang2(f32 *a, f32 *b)
{
    f32 v[3];
    v[0] = a[0] - b[0];
    v[1] = 0.0f;
    v[2] = a[2] - b[2];
    flvecNormalize(v);
    return (int)(0.5f + 65536.0f * flArcTan2(-v[2], v[0]) / 6.2831855f) & 0xFFFF;
}

/* small game_w tests */
int Ana_ok_ck(void) { return GW8(0x212) < 1; }
int Taru_ok_ck(void) { return game_w.stage == GW8(0x2F) ? 0 : GW8(0x210) < 2; }
int Niku_ok_ck(void) { return GW8(0x211) < 3; }
void Pl_light_init(void *pl)
{
    PU32(pl, 0x578) = 0;
    PU32(pl, 0x57C) = 0;
    PU32(pl, 0x580) = 0;
}

/* ------------------------------------------------ stubs */
/* network (single player on the PC) */
void net_send_pl(void *pl, int a, int b) { (void)pl; (void)a; (void)b; }
/* models: the viewer builds the hunter's models itself */
void weapon_create_model(int a, int b, int c) { (void)a; (void)b; (void)c; }
void armor_create_model(void *pl) { (void)pl; }
void yure_init(void *pl) { (void)pl; }        /* hair/cloth sway */
/* the player's draw callbacks (trans_pl_sub, weapon_nm.c, calls these):
 * the viewer draws the hunter and the weapon itself */
void player_trans(void *pl, int a) { (void)pl; (void)a; }
void Lb_player_trans(void *pl, int a) { (void)pl; (void)a; }
void Ed_player_trans(void *pl, int a) { (void)pl; (void)a; }
/* lighting from the ground material (GetGroundCameraData ...) */
void GetPlayerMaterialData(void *pl) { (void)pl; }
/* messages, sounds not ported */
void adx_se_set(void *pl, int a) { (void)pl; (void)a; STUB("adx_se_set") }
void adx_se_stop(void *pl) { (void)pl; }
void die_bgm_set(void) { STUB("die_bgm_set") }
void armor_sd_req(void *pl, int a) { (void)pl; (void)a; }
/* camera requests (death / come back / pile bunker) */
void PlayerDieCameraRequest(void) {}
void PlComebackCameraRequest(void) {}
void PilebunkerCameraRequest(void) {}
/* Item_regained: f_quest_nm.c */
void *rt_pull_enemy_work(void);
void *pull_enemy_work(void) { return rt_pull_enemy_work(); }   /* rt_em.c */

/* ------------------------------------------------ attack data / hit ids (f_pl 0x151800..) */
/* Get_hit_id / Hit_id_init: a counter in game_w+0xD4 that skips 0 */
int Get_hit_id(void)
{
    GW8(0xD4)++;
    if (GW8(0xD4) == 0) GW8(0xD4) = 1;
    return GW8(0xD4);
}
void Hit_id_init(void) { GW8(0xD4) = 0; }






/* pl_body_make (0x14F9B0): the player's body capsule from the joint
 * matrices at +0x124 / +0x130 (hips) and +0x160 (head), widened by r
 * (RotMatVec not ported: the axis tilt is left out; NULL joints fall
 * back to the position) [partial port] */
void flmatGetTrans(f32 *out, void *m);

/* Code_Make (0x159840?): sound code a with chance n/8, else b with chance
 * m/8, else none (0xFFFF) */
int Code_Make(int a, int n, int b, int m)
{
    int r = (s16)(ran_suu(1) & 7);
    if (r < (s16)n) return a;
    if (r < (s16)n + (s16)m) return b;
    return 0xFFFF;
}

/* Pl_poison_add (game 0x639DF0): poison gauge +0x7BA (skills 0xE immune,
 * 9 half, 0x13 double), timer +0x7BE = 30 */
int Pl_Skill_ck(void *, int);
void Pl_poison_add(void *pl, int v)
{
    s16 d = (s16)v;
    if (Pl_Skill_ck(pl, 0xE) == 1) {
        PS16(pl, 0x7BA) = 0;
        return;
    }
    if (Pl_Skill_ck(pl, 9) == 1) d = (s16)(d / 2);
    if (Pl_Skill_ck(pl, 0x13) == 1) d = (s16)(d * 2);
    PS16(pl, 0x7BA) += d;
    PS16(pl, 0x7BE) = 30;
}

/* Stage_mv_data_get (f_quest): the stage's exits list; none on the PC
 * (stage changes are not ported) */

/* fptodp: the PS2 libc float->double helper (debug printf in hit_nm.c) */
int fptodp(float f) { (void)f; return 0; }   /* hit_nm.c declares it int; debug output only */

/* pl01 program (pl_local_init, pl01_effect_move, ef_move_sub ...):
 * src/main/sound/f_sound_nm.c. parts_chg (swap a hand's part model,
 * 0x1213xx?) is not ported: the viewer draws fixed parts. */
void parts_chg(void *pl, int part, int no) { (void)pl; (void)part; (void)no; }
void func_60E2B0(void *pl, int a) { (void)pl; (void)a; }   /* lobby Eft25_set */

/* ------------------------------------------------ parts (0x120F90) */
/* parts_init: 32 part blocks of 0xB0 bytes (parts_work + id * 0x1600) at
 * PLW+0x110.. with a local (+0x00) and world (+0x40) matrix and a scale
 * (+0x98); flags +0x4E6[32] = 1. The motion-player scale words it also
 * sets are not used by the host motion layer. The world matrices are
 * filled each frame from the host skeleton (rt_player_parts). */
static u8 parts_work_h[8][0x1600] __attribute__((aligned(16)));
void flmatInit(f32 *m);
void parts_init(void *pl)
{
    int i;
    u8 *b = parts_work_h[PU16(pl, 0xC) & 7];
    for (i = 0; i < 32; i++, b += 0xB0) {
        PU32(pl, 0x110 + i * 4) = (u32)(uintptr_t)b;
        memset(b, 0, 0xB0);
        b[0] = 0;
        ((f32 *)b)[0] = ((f32 *)b)[5] = ((f32 *)b)[10] = ((f32 *)b)[15] = 1.0f;
        ((f32 *)b)[16] = ((f32 *)b)[21] = ((f32 *)b)[26] = ((f32 *)b)[31] = 1.0f;
        PF32(b, 0x98) = PF32(b, 0x9C) = PF32(b, 0xA0) = 1.0f;
        PU8(pl, 0x4E6 + i) = 1;
    }
}

/* the viewer's posed skeleton of player no: world matrices (row-vector,
 * 16 floats each) of the master skeleton's bones; part i = bone i */
void rt_actor_joints(const void *chr, const float *mats, int n);
void rt_player_parts(int no, const float *world, int n)
{
    u8 *pl = player_work_ptr(no);
    int i;
    rt_actor_joints(pl, world, n);
    for (i = 0; i < 32 && i < n; i++) {
        u8 *b = (u8 *)(uintptr_t)PU32(pl, 0x110 + i * 4);
        if (b)
            memcpy(b + 0x40, world + 16 * i, 64);
    }
}

/* ------------------------------------------------ callees of agent F's pl49..pl83 not ported */
void clr_eft_work(void) { STUB("clr_eft_work") }
void clr_item_work(void) { STUB("clr_item_work") }
void clr_set_work(void) { STUB("clr_set_work") }
void clr_shell_work(void) { STUB("clr_shell_work") }
void clr_used_heap(void) { STUB("clr_used_heap") }
void init_eft_work(void) { STUB("init_eft_work") }
void init_item_work(void) { STUB("init_item_work") }
void init_set_work(void) { STUB("init_set_work") }
void init_shell_work(void) { STUB("init_shell_work") }
void net_send_host(void) { STUB("net_send_host") }
/* overlay calls by address (as rt_overlay.c): Eft12_set4, Shell12_set, Pl_poison_add */
void Eft12_set4(void *, int, int);
void Shell12_set(void *, int);
void Pl_poison_add(void *, int);
void func_5496E0(void *pl, int a, int b) { Eft12_set4(pl, a, b); }
void func_634460(void *pl, int a) { Shell12_set(pl, a); }
void func_639DF0(void *pl, int a) { Pl_poison_add(pl, a); }
/* eft20_nm.c calls game.bin by address: shell04_set2, shell01_set3 */
void shell04_set2(void *src, int arg);
void shell01_set3(void *em, f32 *pos, int arg);
void func_629C20(void *ew, int a) { shell04_set2(ew, a); }
void func_628750(void *em, f32 *pos, int a) { shell01_set3(em, pos, a); }
