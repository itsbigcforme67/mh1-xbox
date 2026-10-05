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
/* action_timer_calc (0x151710): +0x39C = start frame - |blend|; with b set
 * also moves the +0x3D0 counter. */
void action_timer_calc(void *pl, int b)
{
    s16 bl = PS16(pl, 0x2EC);
    s16 fr = PS16(pl, 0x2E4);
    PS32(pl, 0x39C) = 0;
    if (bl < 0) bl = (s16)-bl;
    PS32(pl, 0x39C) += fr - bl;
    if ((s16)b != 0) {
        if (bl != 0) bl = (s16)(bl - 1);
        PU8(pl, 0x3D0) += (u8)(bl - fr);
    }
}

/* Pl_act_set (0x14ECB0): start action (group, number); mode bits: 1 keep
 * the +0x720.. timers, 2 no network send, 0x10 send kind 6, 0x20 keep the
 * low byte of the random seed. */
void Pl_act_set(void *pl, int grp, int no, int mode)
{
    u32 m = mode & 0xFFFF;
    PU8(pl, 0x16) = PU8(pl, 0x14);
    PU8(pl, 0x17) = PU8(pl, 0x15);
    PU8(pl, 0x14) = (u8)grp;
    PU8(pl, 0x15) = (u8)no;
    PU8(pl, 5) = 0;
    PU8(pl, 6) = 0;
    PU8(pl, 7) = 0;
    if (m & 0x20) {
        u8 lo = PU8(pl, 0x39A);
        PU16(pl, 0x39A) = (u16)(ran_suu(1) & 0xFF00);
        PU16(pl, 0x39A) |= lo;
    } else {
        PU16(pl, 0x39A) = (u16)ran_suu(1);
    }
    PU8(pl, 0x6FF) = 0;
    if (!(m & 1)) {
        PU8(pl, 0x720) = 0; PU16(pl, 0x724) = 0; PU16(pl, 0x72C) = 0;
        PU8(pl, 0x721) = 0; PU16(pl, 0x726) = 0; PU16(pl, 0x72E) = 0;
        PU8(pl, 0x722) = 0; PU16(pl, 0x728) = 0; PU16(pl, 0x730) = 0;
        PU8(pl, 0x723) = 0; PU16(pl, 0x72A) = 0; PU16(pl, 0x732) = 0;
        PU8(pl, 0x8C9) = 0;
    }
    if (PU8(pl, 0x14) == 2)
        PU8(pl, 0x615) = 0;
    action_timer_calc(pl, 0);
    PU32(pl, 0x390) &= 0x80100;
    PU32(pl, 0x394) = 0;
    PF32(pl, 0x1A0) = 2.0f;         /* both motion layers at speed 2 */
    PF32(pl, 0x1F0) = 2.0f;
    if (Pl_master_ck(pl) == 1) {
        if (!(m & 2))
            net_send_pl(pl, (m & 0x10) ? 6 : 1, mode);
    } else if ((s16)act_ck(pl, 0, 0) != 0) {
        if (!(m & 1))
            pl_to_normal(pl, 0, 4, 0);
        else
            pl_to_normal_b(pl, 0, 4, 0);
    }
}

/* Pl_act_set2 (0x14EE90): Pl_act_set unless this is the master player
 * waiting to change stage (+0x738); marks +0x6FF. */
void Pl_act_set2(void *pl, int grp, int no, int mode)
{
    if (Pl_master_ck(pl) == 1 && PU8(pl, 0x738) != 0)
        return;
    Pl_act_set(pl, grp, no, mode);
    PU8(pl, 0x6FF) = 1;
}

/* pl_flag_set / pl_flag_clr (0x14EFA0 / 0x14EFE0): bit 31 selects the
 * second word (+0x394). pl_flag_ck is in rt_eft.c. */
void pl_flag_set(void *pl, u32 f)
{
    if (!(f & 0x80000000u)) PU32(pl, 0x390) |= f;
    else PU32(pl, 0x394) |= f & 0x7FFFFFFFu;
}
void pl_flag_clr(void *pl, u32 f)
{
    if (!(f & 0x80000000u)) PU32(pl, 0x390) &= ~f;
    else PU32(pl, 0x394) &= ~(f & 0x7FFFFFFFu);
}

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
/* pl_chr_set (0x151260): one layer (t0 = layer, as the callers pass it) */
void pl_chr_set(void *pl, int chr, int blend, int frame, int n)
{
    PU8(pl, 0x81D) = 0;
    pl_chr_set_com(pl, chr, blend, frame, n);
}
/* pl_chr_set2 (0x151270): legs chr on layer 0 and chr + 100 (upper body)
 * on layer 1 */
void pl_chr_set2(void *pl, int chr, int blend, int frame)
{
    PU8(pl, 0x81D) = 0;
    pl_chr_set_com(pl, chr, blend, frame, 0);
    pl_chr_set_com(pl, chr + 100, blend, frame, 1);
}
/* pl_chr_set3 (0x1512A0): set and start at once, for layers < +0x300 */
void pl_chr_set3(void *pl, int chr, int blend, int frame, int n)
{
    if (n < PU16(pl, 0x300)) {
        cpRotMatrix((s32 *)((u8 *)pl + 0xA0), (f32 *)((u8 *)pl + 0x20));
        PS16(pl, 0x2DC + n * 2) = (s16)chr;
        PS16(pl, 0x2EC + n * 2) = (s16)(blend / 2);
        PU16(pl, 0x2E4 + n * 2) = (u16)frame;
        frame_init(pl, PU16(pl, 0x2E4 + n * 2), PS16(pl, 0x2EC + n * 2), n);
    }
}

/* pl_chr_sub (0x14D1D0, g_pl_chr_sub): per tick, set the motion speed
 * (0 while +0x40A, 0.2 while the +0x610 slow timer runs, else 2; job 1/5
 * motion 0x57C keeps at most 1.5), start queued motions, frame_move. */
void pl_chr_sub(void *pl)
{
    if (softdip_ck(0x18))
        return;
    cpRotMatrix((s32 *)((u8 *)pl + 0xA0), (f32 *)((u8 *)pl + 0x20));
    if (PU8(pl, 0x40A)) {
        PF32(pl, 0x1A0) = 0.0f;
        PF32(pl, 0x1F0) = 0.0f;
    } else if (PS16(pl, 0x610) > 0) {
        PS16(pl, 0x610)--;
        PF32(pl, 0x1A0) = 0.2f;
        PF32(pl, 0x1F0) = 0.2f;
    } else {
        PS16(pl, 0x610) = 0;
        if (PU16(pl, 0x2DC) != 3) {
            u8 k = PU8(pl, 2);
            if ((k == 1 || k == 5) && PU16(pl, 0x2DC) == 0x57C) {
                if (PF32(pl, 0x1A0) <= 1.5f) {
                    PF32(pl, 0x1A0) = 1.5f;
                    PF32(pl, 0x1F0) = 1.5f;
                }
            } else {
                PF32(pl, 0x1A0) = 2.0f;
                PF32(pl, 0x1F0) = 2.0f;
            }
        }
    }
    if (PU8(pl, 0x2FC) == 0) {
        PU8(pl, 0x2FC) = 1;
        frame_init(pl, PU16(pl, 0x2E4), PS16(pl, 0x2EC), 0);
    }
    if (PU8(pl, 0x2FD) == 0) {
        PU8(pl, 0x2FD) = 1;
        frame_init(pl, PU16(pl, 0x2E6), PS16(pl, 0x2EE), 1);
    }
    frame_move(pl);
}

/* ------------------------------------------------ g_pl_voice_req (0x14F850..) */
void pl_voice_req(void *pl, int code)
{
    Pl_se_req2(pl, code, 0, (f32 *)((u8 *)pl + 0xAC), 1, 0);
}

/* pl_light_ck (0x14F870): +0x613 = 1 when on the ground and near (XZ 300,
 * or 500 for kind > 2) a monster whose +0x612 >= 2 and not 300 below it. */
void pl_light_ck(void *pl)
{
    int i;
    u8 *em = em_work;
    if (PU8(pl, 0x604) == 0 && PF32(pl, 0xB0) - PF32(pl, 0x5AC) <= 100.0f) {
        for (i = 0; i < 20; i++, em += 0xA10) {
            if (em[0] && em[1] && PU8(em, 0x612) >= 2) {
                f32 dx = PF32(pl, 0xAC) - PF32(em, 0xAC), dz = PF32(pl, 0xB4) - PF32(em, 0xB4);
                f32 d = flSqrt(dx * dx + dz * dz);
                f32 r = PU8(em, 0x612) == 2 ? 300.0f : 500.0f;
                if (d <= r && PF32(pl, 0xB0) - PF32(em, 0xB4) < 300.0f) {
                    PU8(pl, 0x613) = 1;
                    return;
                }
            }
        }
    }
    PU8(pl, 0x613) = 0;
}

/* Pl_basic_flagset (0x14FB40): +0x388 stance (2 / 1 / else 0); flag 8 set
 * unless bit 0x8000 of b; flag 1 cleared; flag 2 from c. */
void Pl_basic_flagset(void *pl, int a, int b, int c)
{
    u8 s = (u8)a;
    PU8(pl, 0x388) = s == 2 ? 2 : s == 1 ? 1 : 0;
    if ((b & 0xFFFF) & 0x8000) pl_flag_clr(pl, 8);
    else pl_flag_set(pl, 8);
    pl_flag_clr(pl, 1);
    if ((s16)c == 0) pl_flag_clr(pl, 2);
    else pl_flag_set(pl, 2);
}

/* ------------------------------------------------ Pl_bari_ck file (0x14FC40..) */
/* Pl_Skill_ck (0x150150): skill id in the 5 slots at +0x910 */
int Pl_Skill_ck(void *pl, int sk)
{
    int i;
    for (i = 0; i < 5; i++)
        if (PU8(pl, 0x910 + i) == (u8)sk)
            return 1;
    return 0;
}

/* Get_equip_value (0x2A1F70?, f_ud): the master's equipment stat n from
 * User_data (0 attack, 1 defence, 2-5 resistances, 6-12 sword element) */
s16 Get_equip_value(int n);

int Pl_item_num_ck(void *pl, int id);

/* Pl_status_set (0x14FCB0): status icon bits at +0x4D5 */
void Pl_status_set(void *pl)
{
    u8 s = 0;
    int v;
    if (Pl_master_ck(pl) == 0)
        return;
    if (PS16(pl, 0x7BA) != 0) s = 1;
    if ((s16)act_ck(pl, 2, 0x15)) s |= 2;
    if ((s16)act_ck(pl, 2, 0x16)) s |= 4;
    if ((s16)act_ck(pl, 2, 0x13)) s |= 8;
    v = (s16)(PS8(pl, 0x6A4) + PS8(pl, 0x6A5));
    if (v > 0) s |= 0x10; else if (v < 0) s |= 0x40;
    v = (s16)(PS8(pl, 0x6A8) + PS8(pl, 0x6A9));
    if (v > 0) s |= 0x20; else if (v < 0) s |= 0x80;
    PU8(pl, 0x4D5) = s;
}

/* Pl_atck_adj_calc (0x14FDF0): attack value +0x6AC, rate +0x7D8 */
void Pl_atck_adj_calc(void *pl)
{
    if (Pl_master_ck(pl) == 1) {
        int v = Get_equip_value(0);
        s16 a;
        if ((s16)Pl_item_num_ck(pl, 0xA6) != 0) v += 5;
        a = (s16)(v + PS8(pl, 0x6A4) + PS8(pl, 0x6A5));
        if (Pl_Skill_ck(pl, 0x20) == 1) a += 3;
        else if (Pl_Skill_ck(pl, 0x21) == 1) a += 5;
        if (a < 0) a = 1;
        PS16(pl, 0x6AC) = a;
        if (PU16(pl, 0x6AC) >= 0x190) PS16(pl, 0x6AC) = 0x190;
    }
    PF32(pl, 0x7D8) = (f32)PU16(pl, 0x6AC) / 100.0f;
}

/* Pl_def_adj_calc (0x14FF20): defence +0x6AE, +0x7DC */
void Pl_def_adj_calc(void *pl)
{
    int v = (s16)Get_equip_value(1) + 1;
    if ((s16)Pl_item_num_ck(pl, 0xA7) != 0) v += 5;
    v += PS8(pl, 0x6A8) + PS8(pl, 0x6A9);
    if (Pl_Skill_ck(pl, 0x25) == 1) v += 5;
    else if (Pl_Skill_ck(pl, 0x26) == 1) v += 10;
    if (v < 0) v = 1;
    if ((f32)PS16(pl, 0x302) / (f32)PS16(pl, 0x792) <= 0.4f) v += 30;
    PU16(pl, 0x6AE) = (u16)(v & 0xFF);
    PF32(pl, 0x7DC) = (f32)PU16(pl, 0x6AE);
}

/* Pl_reg_calc (0x1501A0): element resistances +0x920..+0x92C */
void Pl_reg_calc(void *pl)
{
    int i;
    for (i = 0; i < 4; i++)
        PF32(pl, 0x920 + i * 4) = (f32)Get_equip_value(2 + i);
    for (i = 0; i < 4; i++)
        if (Pl_Skill_ck(pl, 1 + i) == 1) PF32(pl, 0x920 + i * 4) += 25.0f;
    for (i = 0; i < 4; i++)
        if (Pl_Skill_ck(pl, 5 + i) == 1) PF32(pl, 0x920 + i * 4) -= 10.0f;
    for (i = 0; i < 4; i++)
        if (PF32(pl, 0x920 + i * 4) < -100.0f) PF32(pl, 0x920 + i * 4) = -100.0f;
    for (i = 0; i < 4; i++)
        if (!(PF32(pl, 0x920 + i * 4) <= 100.0f)) PF32(pl, 0x920 + i * 4) = 100.0f;
}

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
/* rate_g_calc (0x1513B0): gravity for a jump that lands after t ticks */
int rate_g_calc(void *pl, int t)
{
    s16 h = (s16)((s16)t / 2);
    f32 v = PF32(pl, 0x3B8), g = -1.0f * v;
    int r;
    if (h >= 2 && !(v < 0.0f)) {
        g /= (f32)h;
        r = 0;
    } else
        r = 1;
    PF32(pl, 0x3C4) = g;
    return r;
}
void rate_add(void *pl)
{
    PF32(pl, 0xAC) += PF32(pl, 0x3B4);
    PF32(pl, 0xB0) += PF32(pl, 0x3B8);
    PF32(pl, 0xB4) += PF32(pl, 0x3BC);
}
void rate_add_g(void *pl)
{
    PF32(pl, 0x3B4) += PF32(pl, 0x3C0);
    PF32(pl, 0x3B8) += PF32(pl, 0x3C4);
    PF32(pl, 0x3BC) += PF32(pl, 0x3C8);
    rate_add(pl);
}
void rate_clear(void *pl)
{
    memset((u8 *)pl + 0x3B4, 0, 0x18);
}
void rate_clear_g(void *pl)
{
    memset((u8 *)pl + 0x3C0, 0, 0xC);
}

/* front_land_ck (0x151D90): ground ahead (dist) higher than y + lo and at
 * most y + lo + hi: writes the height, 1. No ground: 1. */
int front_land_ck(f32 dist, f32 lo, f32 hi, void *pl, f32 *out)
{
    s32 ang[3];
    f32 m[4][4], d[4], v[4], p[3], gy, top;
    v[0] = 0.0f; v[1] = 0.0f; v[2] = dist;
    ang[0] = 0; ang[1] = PS32(pl, 0xA4); ang[2] = 0;
    cpRotMatrix(ang, &m[0][0]);
    flvecApplyMat33(d, v, m);
    p[0] = PF32(pl, 0xAC) + d[0];
    p[1] = PF32(pl, 0xB0) + d[1];
    p[2] = PF32(pl, 0xB4) + d[2];
    top = p[1] + lo;
    if (GetGroundHitAreaUpper(pl, p, &gy) != 1)
        return 1;
    if (!(gy < top) && gy <= top + hi) {
        *out = gy;
        return 1;
    }
    return 0;
}
/* front_land_ck2 (0x151EC0): mode 0: ground ahead below y + lo; else not
 * above it. No ground: 0. */
int front_land_ck2(f32 dist, f32 lo, void *pl, int mode)
{
    s32 ang[3];
    f32 m[4][4], d[4], v[4], p[3], gy, lim;
    v[0] = 0.0f; v[1] = 0.0f; v[2] = dist;
    ang[0] = 0; ang[1] = PS32(pl, 0xA4); ang[2] = 0;
    cpRotMatrix(ang, &m[0][0]);
    flvecApplyMat33(d, v, m);
    p[0] = PF32(pl, 0xAC) + d[0];
    p[1] = PF32(pl, 0xB0) + d[1];
    p[2] = PF32(pl, 0xB4) + d[2];
    lim = PF32(pl, 0xB0) + lo;
    if (GetGroundHitAreaUpper(pl, p, &gy) != 1)
        return 0;
    if ((mode & 0xFFFF) == 0)
        return gy < lim;
    return !(gy <= lim);
}

/* ------------------------------------------------ World_calc file (0x152050..) */
/* World_calc (0x152050): +0x754 = position in stage-data coordinates */
void World_calc(void *pl)
{
    f32 *sd = Stage_data_get(PU8(pl, 0x736));
    PF32(pl, 0x754) = PF32(pl, 0xAC) + sd[0];
    PF32(pl, 0x758) = PF32(pl, 0xB0) + (sd[6] + sd[7]) / 2.0f;
    PF32(pl, 0x75C) = PF32(pl, 0xB4) + sd[1];
}

/* Pl_item_idx_calc (0x1526F0): +0x888 = first pouch slot holding a
 * usable item (Item_data[id][1] == 1) */
void Pl_item_idx_calc(void *pl)
{
    int i;
    PS16(pl, 0x888) = 0;
    for (i = 0; i < 20; i++) {
        u16 id = PU16(pl, 0x828 + i * 4);
        if (id != 0 && PS16(pl, 0x82A + i * 4) > 0 && Item_data[id][1] == 1) {
            PS16(pl, 0x888) = (s16)i;
            return;
        }
    }
}
/* Pl_item_num_ck (0x152B60): count of item id in the pouch */
int Pl_item_num_ck(void *pl, int id)
{
    int i;
    for (i = 0; i < 20; i++)
        if (PU16(pl, 0x828 + i * 4) == (u16)id)
            return PS16(pl, 0x82A + i * 4);
    return 0;
}
/* Pl_item_num_ck2 (0x152BC0): free space for item id (0xFF: no limit) */
int Pl_item_num_ck2(void *pl, int id)
{
    int i;
    u8 max = Item_data[(u16)id][3];
    for (i = 0; i < 20; i++)
        if (PU16(pl, 0x828 + i * 4) == (u16)id)
            return max == 0xFF ? 0xFF : (s16)(max - PS16(pl, 0x82A + i * 4));
    return max;
}
/* Get_Active_itemnum (0x152D50): count in the selected slot. On the PS2
 * it reads the player from a0, which its one caller (item_action_set,
 * pl10.c) leaves holding pl and does not pass; x86 has no such register,
 * so the host version uses the master player [port difference]. */
int Get_Active_itemnum(void)
{
    u8 *pl = (u8 *)player_work_ptr(game_w.master);
    return PS16(pl, 0x82A + PU16(pl, 0x888) * 4);
}

/* Pl_adj_calc (0x1532A0) / Pl_pos_adj (0x153360): move to +0x800 over n
 * ticks (snap when farther than 800) */
void Pl_adj_calc(void *pl, int n)
{
    f32 t;
    if (!(flvecCalcDistance((f32 *)((u8 *)pl + 0xAC), (f32 *)((u8 *)pl + 0x800)) < 800.0f)) {
        PS16(pl, 0x818) = 0;
        PF32(pl, 0xAC) = PF32(pl, 0x800);
        PF32(pl, 0xB0) = PF32(pl, 0x804);
        PF32(pl, 0xB4) = PF32(pl, 0x808);
        return;
    }
    PS16(pl, 0x818) = (s16)n;
    t = (f32)(s16)n;
    PF32(pl, 0x80C) = (PF32(pl, 0x800) - PF32(pl, 0xAC)) / t;
    PF32(pl, 0x810) = (PF32(pl, 0x804) - PF32(pl, 0xB0)) / t;
    PF32(pl, 0x814) = (PF32(pl, 0x808) - PF32(pl, 0xB4)) / t;
}
void Pl_pos_adj(void *pl)
{
    s16 n = PS16(pl, 0x818);
    if (n != 0) {
        PS16(pl, 0x818) = (s16)(n - 1);
        if (n > 0) {
            PF32(pl, 0xAC) += PF32(pl, 0x80C);
            PF32(pl, 0xB0) += PF32(pl, 0x810);
            PF32(pl, 0xB4) += PF32(pl, 0x814);
        }
    }
}

/* Pl_vital_calc (0x153420): hit points +0x302 (max +0x792, red +0x790) */
void Pl_vital_calc(void *pl, int v)
{
    s16 d = (s16)v;
    if (Pl_master_ck(pl) == 0)
        return;
    if (Game_clear_ck(0) == 1 && d < 0)
        return;
    if (PU8(pl, 0x14) == 3)
        return;
    PS16(pl, 0x302) += d;
    if (PS16(pl, 0x302) <= 0) PS16(pl, 0x302) = 0;
    if (PS16(pl, 0x302) >= PS16(pl, 0x792)) PS16(pl, 0x302) = PS16(pl, 0x792);
    if (PS16(pl, 0x302) >= PS16(pl, 0x790)) PS16(pl, 0x790) = PS16(pl, 0x302);
}

/* Pl_slash_lv_ck (0x153610): sharpness level 0-3 of +0x87E for the
 * weapon (Pl_slash_tbl[job][Ken_data[id][2]]); guns 0 */
int Pl_slash_lv_ck(void *pl, int unused)
{
    u8 k = PU8(pl, 2);
    s16 v = PS16(pl, 0x87E);
    s16 *t;
    (void)unused;
    if (k == 1 || k == 5 || !Pl_slash_tbl[k])
        return 0;
    t = Pl_slash_tbl[k] + Ken_data[PU16(pl, 0x360)][2] * 4;
    if (t[0] >= v) return 0;
    if (t[1] >= v) return 1;
    if (t[2] >= v) return 2;
    return 3;
}
/* Pl_slash_calc (0x1536D0): sharpness +0x87E += d (max +0x884); the
 * level-change messages (set01_set2) are not shown */
void Pl_slash_calc(void *pl, int d)
{
    u8 k;
    int lv;
    if (Pl_master_ck(pl) == 0)
        return;
    k = PU8(pl, 2);
    if (k == 1 || k == 5)
        return;
    PS16(pl, 0x87E) += (s16)d;
    if (PS16(pl, 0x87E) <= 0) PS16(pl, 0x87E) = 0;
    if (PS16(pl, 0x884) < PS16(pl, 0x87E)) PS16(pl, 0x87E) = PS16(pl, 0x884);
    lv = Pl_slash_lv_ck(pl, 0) & 0xFF;
    if (lv != PU8(pl, 0x887))
        PU8(pl, 0x887) = (u8)lv;
}

/* Shell_type_set (0x1539D0): shot type of the selected ammo slot */
void Shell_type_set(void *pl, int unused)
{
    u16 s = PU16(pl, 0x88E);
    (void)unused;
    if (s == 0xFF)
        return;
    PU8(pl, 0x56C) = Item_data[PU16(pl, 0x828 + s * 4)][8];
    if (Item_data[PU16(pl, 0x828 + s * 4)][3] == 0xFF)
        PS16(pl, 0x8BC) = 0xFF;
    else
        PS16(pl, 0x8BC) = PS16(pl, 0x82A + s * 4);
    PU8(pl, 0x1D) = Shell_data[PU8(pl, 0x56C)][0];
    PU8(pl, 0x1C) = 0;
}

/* Pl_shell_set (0x1537D0): next pouch slot with ammo the gun can load
 * (guns only; 0xFF none) */
int Pl_shell_set(void *pl, int from, int dir)
{
    u8 k = PU8(pl, 2);
    u16 i = (u16)from;
    int n, d = dir & 0xFF;
    if ((k != 1 && k != 5) || PU8(pl, 0x35F) != 7)
        return 0xFF;
    if (i >= 20) i = 0;
    if (d == 0) i = (u16)((i + 1) % 20);
    else if (d == 1) i = i == 0 ? 19 : (u16)(i - 1);
    for (n = 0; n < 20; n++) {
        u16 id = PU16(pl, 0x828 + i * 4);
        if (id != 0 && PS16(pl, 0x82A + i * 4) > 0 && Item_data[id][1] == 2
            && (PS32(Gun_data[PU16(pl, 0x360)], 0x10) & (1 << PS16(Item_data[id], 8))))
            return i;
        if (d == 0 || d == 2) i = (u16)((i + 1) % 20);
        else i = i == 0 ? 19 : (u16)(i - 1);
    }
    return 0xFF;
}

/* tame_cnt_up (0x153A70): charge counter +0x87C (sound at 60) */
void tame_cnt_up(void *pl)
{
    s16 t = PS16(pl, 0x87C);
    if (t < 1000) {
        PS16(pl, 0x87C) = (s16)(t + 1);
        if (PS16(pl, 0x87C) == 60)
            Pl_se_req2(pl, 9, 0, (f32 *)((u8 *)pl + 0xAC), 1, 0);
    } else
        PS16(pl, 0x87C) = 1000;
}

/* Pl_stamina_calc (0x153AE0) / Pl_max_stamina_calc (0x153B70) /
 * Pl_stamina_reduce (0x153C20): stamina +0x748 (max +0x882, hunger timer
 * +0x8C0) */
void Pl_stamina_calc(void *pl, int v)
{
    s16 d = (s16)v;
    if (Pl_master_ck(pl) == 0)
        return;
    if (d < 0 && PS16(pl, 0x8CC) != 0)
        return;
    PS16(pl, 0x748) += d;
    if (PS16(pl, 0x748) <= 0) PS16(pl, 0x748) = 0;
    if (PS16(pl, 0x748) >= PS16(pl, 0x882)) PS16(pl, 0x748) = PS16(pl, 0x882);
}
void Pl_max_stamina_calc(void *pl, int v)
{
    s16 d = (s16)v;
    if (Pl_master_ck(pl) == 0)
        return;
    if (d < 0 && PS16(pl, 0x8CC) != 0)
        return;
    PS16(pl, 0x882) += d;
    if (PS16(pl, 0x882) < 0x4C) PS16(pl, 0x882) = 0x4B;
    else if (PS16(pl, 0x882) >= 0x1C3) {
        PS16(pl, 0x882) = 0x1C2;
        PS16(pl, 0x8C0) = 0x2A30;
    }
    if (PS16(pl, 0x882) < PS16(pl, 0x748)) PS16(pl, 0x748) = PS16(pl, 0x882);
}
void Pl_stamina_reduce(void *pl)
{
    s16 n;
    if (Pl_master_ck(pl) == 0 || Pl_Skill_ck(pl, 0x12) == 1)
        return;
    if ((s16)Stage_env_ck(PU8(pl, 0x736)) == 2 && Pl_Skill_ck(pl, 0x2C) == 0 && PU16(pl, 0x91A) == 0)
        n = 3;
    else
        n = 1;
    if (Pl_Skill_ck(pl, 0xD) == 1 && !(GW16(0x1E) & 1))
        return;
    if (Pl_Skill_ck(pl, 0x17) == 1) n = (s16)(n * 2);
    PS16(pl, 0x8C0) -= n;
    if (PS16(pl, 0x8C0) <= 0) {
        PS16(pl, 0x8C0) = 0x2A30;
        Pl_max_stamina_calc(pl, -0x4B);
    }
}

/* ------------------------------------------------ misc (g_Pl_hold_item_ck ..) */
/* Pl_hold_item_ck (0x154BD0): a carried item (egg, ore...) in the pouch */
int Pl_hold_item_ck(void *pl)
{
    int i, r = 0xFFFF;
    for (i = 0; i < 20; i++)
        if (PS16(pl, 0x82A + i * 4) != 0) {
            u16 id = PU16(pl, 0x828 + i * 4);
            if (id == 0x95 || id == 0x93 || id == 0x94 || id == 0xA3 || id == 0x92 || id == 0x91)
                r = id;
        }
    return r;
}
int Check_hold_item(int id)
{
    switch (id & 0xFFFF) {
    case 0x91: return 1;
    case 0x92: return 2;
    case 0xA3: return 3;
    case 0x94: return 4;
    case 0x93: return 5;
    case 0x95: return 6;
    }
    return 0;
}
/* Get_string_pow (0x154D90): bow draw level for shot type t */
int Get_string_pow(void *pl, int t)
{
    s16 v = (s16)(Shell_data[t & 0xFF][2] - Gun_data[PU16(pl, 0x360)][3]);
    if (Pl_Skill_ck(pl, 0x30) == 1) v--;
    else if (Pl_Skill_ck(pl, 0x31) == 1) v -= 2;
    if (v < 5) return 0;
    if (v < 8) return 1;
    return 2;
}
/* Pl_view_reset (0x154E90) */
void Pl_view_reset(void *pl)
{
    PU8(pl, 0x764) = 0;
    PU8(pl, 0x763) = 0;
    PU16(pl, 0x8EE) = 0;
}

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
int Nikuyaki_ck(void *pl) { return (s16)Pl_item_num_ck(pl, 0x12) != 0; }
int Sansai_talk_ck(void *pl)
{
    int i;
    u8 *em = em_work;
    for (i = 0; i < 20; i++, em += 0xA10)
        if (em[2] == 0xA && flvecCalcDistance((f32 *)((u8 *)pl + 0xAC), (f32 *)(em + 0xAC)) <= 300.0f)
            return 1;
    return 0;
}
/* Pl_trap_use_ck (0x1533B0): trap model of the stage, -1 when not allowed */
int Pl_trap_use_ck(void *pl)
{
    extern s16 *stg_eft_mdl_no[];
    if (Ana_ok_ck() == 0)
        return -1;
    return stg_eft_mdl_no[PU8(pl, 0x736)] ? *stg_eft_mdl_no[PU8(pl, 0x736)] : -1;
}
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
void set01_set(int a, int b, int c) { (void)a; (void)b; (void)c; }
void set01_set2(void *msg) { (void)msg; }
void unmei_se(void *pl) { (void)pl; STUB("unmei_se") }
void adx_se_set(void *pl, int a) { (void)pl; (void)a; STUB("adx_se_set") }
void adx_se_stop(void *pl) { (void)pl; }
void die_bgm_set(void) { STUB("die_bgm_set") }
void armor_sd_req(void *pl, int a) { (void)pl; (void)a; }
/* camera requests (death / come back / pile bunker) */
void PlayerDieCameraRequest(void) {}
void PlComebackCameraRequest(void) {}
void PilebunkerCameraRequest(void) {}
/* items, gathering, quest */
int Pl_item_stack(void *pl, int a, int b) { (void)pl; (void)a; (void)b; STUB("Pl_item_stack") return 3; }
void Pl_item_supply(void *pl, int a, int b, int c) { (void)pl; (void)a; (void)b; (void)c; }
int Pl_item_search_space(void *pl) { (void)pl; return -1; }
void Pl_chat_act_set(void *pl) { (void)pl; STUB("Pl_chat_act_set") }
void ItemPickingDeclaration(void *pl, void *p) { (void)pl; (void)p; }
long ItemStockRequest(void *pl, int a, int b, int c) { (void)pl; (void)a; (void)b; (void)c; return 0; }
int St_pick_ck(void *pl, u16 *a, f32 *b) { (void)pl; (void)a; (void)b; return 0; }
int St_pick_ck2(void *pl) { (void)pl; return 0; }
int St_unique_ck(void *pl, f32 *a, u16 *b, u8 *c) { (void)pl; (void)a; (void)b; (void)c; return 0; }
void St_unique_adr_set(void *pl) { (void)pl; }
int Ext_pick_point_ck(void *pl) { (void)pl; return 0; }
int Ext_pick_point_ck2(void *pl) { (void)pl; return 0; }
void Item_regained(void *pl, int a) { (void)pl; (void)a; }
void Share_item_conv(void *pl) { (void)pl; }
void Basic_item_set(void *pl) { (void)pl; STUB("Basic_item_set") }
void Oki_item_set(void *pl) { (void)pl; STUB("Oki_item_set") }
void Taru_item_set(void *pl) { (void)pl; STUB("Taru_item_set") }
void Ana_item_set(void *pl) { (void)pl; STUB("Ana_item_set") }
void Fue_item_set(void *pl) { (void)pl; STUB("Fue_item_set") }
int Modori_dama_ck(void) { return 0; }
void Quest_restart(void) { STUB("Quest_restart") }
int Quest_remuneration_calc(void) { return 0; }
void Quest_forfeit_message(void) {}
void *pull_enemy_work(void) { STUB("pull_enemy_work") return NULL; }

/* ------------------------------------------------ attack data / hit ids (f_pl 0x151800..) */
/* Get_hit_id / Hit_id_init: a counter in game_w+0xD4 that skips 0 */
int Get_hit_id(void)
{
    GW8(0xD4)++;
    if (GW8(0xD4) == 0) GW8(0xD4) = 1;
    return GW8(0xD4);
}
void Hit_id_init(void) { GW8(0xD4) = 0; }

/* pl_atck_data_set2 (0x151830): empty in this build */
void pl_atck_data_set2(void) {}

/* pl_atck_data_set_shl2 (0x151840): copy the attack row n (0x18 bytes) of
 * the shell's attack table (*(sh+0x90)) to sh+0x60, halve the power
 * bytes (+0x60, +0x61, +0x75 unless 0xFF, +0x76), new hit id. b+0xC is
 * the hit-stop byte. */
void pl_atck_data_set_shl2(void *sh, void *b, int n)
{
    u8 f = PU8(sh, 0x6C) & 0xF7, el = f ? PU8(sh, 0x6D) : 0;
    u8 *row = *(u8 **)PU32(sh, 0x90) + n * 0x18;
    memcpy((u8 *)sh + 0x60, row, 0x18);
    PU8(sh, 8) = (u8)PU16(b, 0xC);
    PU8(sh, 0xB) = 1;
    PU8(sh, 0x1E) = 0;
    PU32(sh, 0x9C) = 0;
    PU32(sh, 0xA0) = 0;
    if (el) {
        PU8(sh, 0x6C) |= f;
        PU8(sh, 0x6D) = el;
    }
    PU8(sh, 0x60) >>= 1;
    PU8(sh, 0x61) >>= 1;
    if (PU8(sh, 0x75) != 0xFF) PU8(sh, 0x75) >>= 1;
    PU8(sh, 0x76) >>= 1;
    PU8(sh, 0xB5) = (u8)Get_hit_id();
}

/* hit_data_expand2 (0x151C70): one body/attack entry (s16 type at +2) to a
 * sphere (type 0: centre + radius at +0xC) or capsule (type 1) around
 * the base position */
int hit_data_expand2(f32 *base, u8 *e, f32 *cap, f32 *sph)
{
    switch (PS16(e, 2)) {
    case 0:
        sph[3] = PF32(e, 0xC);
        sph[0] = base[0] + PF32(e, 0x10);
        sph[1] = base[1] + PF32(e, 0x14);
        sph[2] = base[2] + PF32(e, 0x18);
        return 0;
    case 1:
        cap[6] = PF32(e, 0xC);
        cap[0] = base[0] + PF32(e, 0x10);
        cap[1] = base[1] + PF32(e, 0x14);
        cap[2] = base[2] + PF32(e, 0x18);
        cap[3] = base[0] + PF32(e, 0x1C);
        cap[4] = base[1] + PF32(e, 0x20);
        cap[5] = base[2] + PF32(e, 0x24);
        return 1;
    }
    return -1;
}
/* hit_data_expand3 (0x151D50): capsule a -> b with the radius of r[3] */
int hit_data_expand3(f32 *a, f32 *b, f32 *r, f32 *cap)
{
    cap[6] = r[3];
    cap[0] = a[0]; cap[1] = a[1]; cap[2] = a[2];
    cap[3] = b[0]; cap[4] = b[1]; cap[5] = b[2];
    return 1;
}

/* pl_body_make (0x14F9B0): the player's body capsule from the joint
 * matrices at +0x124 / +0x130 (hips) and +0x160 (head), widened by r
 * (RotMatVec not ported: the axis tilt is left out; NULL joints fall
 * back to the position) [partial port] */
void flmatGetTrans(f32 *out, void *m);
void pl_body_make(void *pl, f32 *cap, f32 r)
{
    f32 a[3], b[3];
    void *j0 = (void *)PU32(pl, 0x124), *j1 = (void *)PU32(pl, 0x130), *j2 = (void *)PU32(pl, 0x160);
    if (!j0 || !j1 || !j2) {
        cap[0] = PF32(pl, 0xAC); cap[1] = PF32(pl, 0xB0) + 40.0f; cap[2] = PF32(pl, 0xB4);
        cap[3] = cap[0]; cap[4] = PF32(pl, 0xB0) + 150.0f; cap[5] = cap[2];
        cap[6] = r;
        return;
    }
    flmatGetTrans(a, (u8 *)j0 + 0x40);
    flmatGetTrans(b, (u8 *)j1 + 0x40);
    cap[0] = (a[0] + b[0]) / 2.0f;
    cap[1] = (a[1] + b[1]) / 2.0f;
    cap[2] = (a[2] + b[2]) / 2.0f;
    flmatGetTrans(cap + 3, (u8 *)j2 + 0x40);
    cap[6] = r;
}

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
void *Stage_mv_data_get(int stg, int master) { (void)stg; (void)master; return NULL; }

/* fptodp: the PS2 libc float->double helper (debug printf in hit_nm.c) */
int fptodp(float f) { (void)f; return 0; }   /* hit_nm.c declares it int; debug output only */

/* ------------------------------------------------ pl01 program (0x24A240..) */
/* pl_prog_tbl[*] -> pl01_adr_tbl = { pl_local_init x3, pl01_effect_move }:
 * init (0x24A240) does nothing; pl01_effect_move (0x24A250) runs the
 * per-motion sound/effect list (+0x444 work: +0x445 step, +0x456 timer).
 * ef_move_sub (0x24A790, 40 KB) is not decompiled: the host keeps the
 * run-loop footsteps it already had (rt_snd_player_motion). */
void pl_local_init(void *pl) { (void)pl; }
void rt_snd_player_motion_pl(void *pl);
void pl01_effect_move(void *pl)
{
    u8 *w = (u8 *)pl + 0x444;
    switch (w[1]) {
    case 0:
        w[1] = 1;
        PS16(w, 0x12) = 0;
        break;
    case 1:
        rt_snd_player_motion_pl(pl);
        break;
    }
}

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
