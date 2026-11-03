/*
 * rt_mh2_em.c - MH Dos's monster set-up on the PC (VERSION_MH2), host C written from our own disassembly where the
 * shared G-form C reads G's per-kind tables and G's EMW places (docs/mh2/quest_flow.md, "Monsters").
 *
 * em_status_init (gm_sub 0x904480) and what it calls: MH2 rewrote it (G's called em_def_attack_set / em_def_defence_set by
 * G's tables and set fields at MH1 places). These definitions are strong: the G copies (f_em_nm.c, linked weak) lose. Raw
 * MH2 EMW offsets on the host's em_work (MH2's 0x9D0-byte layout, include/mh2_emw.h); the per-kind tables are read from
 * MH2's game and gm_sub images by address. Not byte-checked; the field meanings are not named here.
 */
#ifdef VERSION_MH2
#include "rt.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern u8 quest_w[], game_w[], player_work[];
void em_work_set(), em_wall_bit_set(), em_search_data_set(), em_range_set(), Ikari_Data_Set(), em_tsuushin_set();
void em_init_sub(), em_dur_init(), World_calc(), enemy_mk();
u8 Em_max_parts_get();
void em_cmd_init(u8 *em);      /* rt_mh2.c (game 0x7262E0) */
void Ext_pick_point_clr();

#define B8(o) (*(u8 *)(em + (o)))
#define S8(o) (*(s8 *)(em + (o)))
#define H16(o) (*(u16 *)(em + (o)))
#define W32(o) (*(s32 *)(em + (o)))
#define F32(o) (*(f32 *)(em + (o)))
#define KIND (em[2])

/* a word of PS2 data that may be a host pointer after the import, as a host pointer */
static const void *ps2_ptr(uint32_t va)
{
    const uint8_t *p = rt_addr(va, 4);
    uint32_t w;
    if (!p)
        return NULL;
    memcpy(&w, p, 4);
    if (w >= 0x02000000u && rt_va_of_host((const void *)(uintptr_t)w))
        return (const void *)(uintptr_t)w;
    return w ? rt_addr(w, 4) : NULL;
}
static const void *kind_ptr(uint32_t tbl, int kind) { return ps2_ptr(tbl + 4 * (uint32_t)kind); }
static void missing(const char *what, int kind)
{
    static int said;
    if (!said++)
        rt_warn("MH2 monster set-up: %s for kind %d not readable", what, kind);
}

/* sub_main 0x379AC0: the stage a stage counts as (table 0x440E30 == 1: 0x440FC0's stage, unless 0xFFFF) */
static int stage_group(int st)
{
    const s16 *a;
    const u16 *b;
    st &= 0xFFFF;
    if (st == 0xFFFF)
        return st;
    a = (const s16 *)rt_addr(0x440E30 + 2 * (uint32_t)st, 2);
    if (!a || *a != 1)
        return st;
    b = (const u16 *)rt_addr(0x440FC0 + 2 * (uint32_t)st, 2);
    return b && *b != 0xFFFF ? *b : st;
}
/* game 0x7098A0 / 0x7097E0: {s16 stage, s16 value} ended by -1 (value of the end entry the default); a stage from 10000 is that
 * exact stage, below it the stage's group */
static int stage_value(u8 *em, const s16 *l)
{
    int st = H16(0x680);
    if (!l)
        return 0;
    for (; l[0] != -1; l += 2) {
        if (l[0] >= 10000) {
            if (st == l[0] - 10000)
                return l[1];
        } else if (stage_group(st) == stage_group(l[0]))
            return l[1];
    }
    return l[1];
}

/* game 0x708E10 / 0x708EE0 (em_def_attack_set / em_def_defence_set): per kind (0x90D470) a row of 12 bytes by the monster's
 * level (EMW+0x98A): +4 attack, +8 defence. Online quests 25001 scale them by quest_w+0x194/+0x196 (not on the PC). */
static f32 def_row(u8 *em, int off)
{
    const u8 *t = kind_ptr(0x90D470, KIND);
    if (!t) {
        missing("attack / defence row (0x90D470)", KIND);
        return 1.0f;
    }
    return *(const f32 *)(t + 12 * S8(0x98A) + off);     /* (the images are whole in host memory) */
}

/* Em_Taisei_Set (game 0x7077F0; the host's G copy keeps the name for the G-form callers, on config/mh2/pc_data.txt's tables): five per-kind tables of s16 (sleep, poison, paralysis, sleep2 and MH2's new one) */
static void taisei_set(u8 *em)
{
    static const struct { uint32_t tbl; u16 off; } t[] = {
        { 0x8BDB50, 0x716 }, { 0x8BDA00, 0x6FC }, { 0x8BCCB0, 0x704 }, { 0x8BD180, 0x70E }, { 0x8BD550, 0x7D4 } };
    size_t i;
    for (i = 0; i < sizeof t / sizeof t[0]; i++) {
        const s16 *v = kind_ptr(t[i].tbl, KIND);
        if (v)
            *(s16 *)(em + t[i].off) = *v;
        else
            missing("tolerance table", KIND);
    }
}

/* em_cmd_top (game 0x731C70): the program's start, (*cmd_tbl)[EMW+0x774], into +0x7C4; +0x778 cleared. 0 when the kind has no
 * command tables (the PC loads them from em_cmd.afs when the model is made, rt_mh2.c) */
static u32 cmd_top(u8 *em)
{
    u32 **t = (u32 **)(uintptr_t)W32(0x754);
    B8(0x778) = 0;
    W32(0x7C4) = t && *t ? (s32)(*t)[B8(0x774)] : 0;
    return (u32)W32(0x7C4);
}
/* em_cmd_reset (game 0x733200) with reset_flag_ck (0x734100) and game 0x709510(em, 2) written in: G's copy read the command
 * table at G's place and crashed on a kind without tables */
static void cmd_reset(u8 *em)
{
    if (!B8(0x817) && !B8(0x7C3))
        B8(0x7C3) = 2;
    B8(0x7A2) = 1;
    B8(0x78D) = 0;          /* reset_flag_ck */
    B8(0x7A2) = 0;
    B8(0x78F) = 0;
    H16(0x994) = 0;
    B8(0x774) = 0;
    W32(0x758) = (s32)cmd_top(em);
    S8(0x87C) = S8(0x87D) = S8(0x881) = S8(0x880) = -1;
    H16(0x986) = 0xFFFF;
    H16(0x882) = 0xFFFF;
    W32(0x7A8) = 0;
}

void em_status_init(u8 *em)
{
    int i;
    const f32 *scale = (const f32 *)rt_addr(0x8B1620 + 4 * (uint32_t)KIND, 4);
    const u8 *size = rt_addr(0x8B15D0 + KIND, 1);
    const s32 *hun, *sui, *thi;
    B8(0x971) = 0;
    for (i = 0; i < 2; i++) {
        u8 *p = em + 76 * i;
        *(u16 *)(p + 0x8B4) = 0;
        *(s32 *)(p + 0x8B8) = *(s32 *)(p + 0x8BC) = *(s32 *)(p + 0x8C0) = 0;
        *(u16 *)(p + 0x8C4) = *(u16 *)(p + 0x8C6) = *(u16 *)(p + 0x8C8) = 0;
        p[0x8CA] = p[0x8CB] = 0;
        *(u16 *)(p + 0x8CE) = *(u16 *)(p + 0x8D0) = 0;
        p[0x8CD] = p[0x8D4] = 0;
        *(u16 *)(p + 0x8D2) = *(u16 *)(p + 0x8D6) = 0;
    }
    em_cmd_init(em);
    cmd_reset(em);
    W32(0x7CC) = 0;
    H16(0x662) = 1;
    B8(0x414) = 0;
    em[1] = 1;
    F32(0x160) = F32(0x1B0) = F32(0x200) = F32(0x250) = 2.0f;
    F32(0x584) = F32(0xB8) = F32(0xBC) = F32(0xC0) = scale ? *scale : 1.0f;
    if (KIND != 18 && KIND != 10) {     /* gm_sub 0x9043C0: scaled by the size in percent (EMW+0x984) */
        f32 s = H16(0x984) / 100.0f;
        F32(0xB8) = F32(0x584) * s;
        F32(0xBC) = F32(0x584) * s;
        F32(0xC0) = F32(0x584) * s;
    }
    if (*(u16 *)(quest_w + 8) == 0)
        W32(0xA0) = W32(0xA4) = W32(0xA8) = 0;
    F32(0x6E4) = 1.0f;
    B8(0x532) = 0;
    W32(0x3A4) = 0;
    Ext_pick_point_clr(S8(0x7E1));      /* Em_hagi_point_clr (sub_main 0x3A69E0) */
    S8(0x7E1) = -1;
    B8(0x596) = size ? *size : 0;
    W32(0x63C) = W32(0x640) = W32(0x644) = W32(0x648) = 0;
    F32(0x884) = 1.0f;
    B8(0x8AE) = B8(0x7D3) = B8(0x7E2) = B8(0x89C) = B8(0x7D8) = B8(0x7D9) = 0;
    H16(0x7DA) = 0;
    B8(0x7DC) = B8(0x7DD) = B8(0x7DE) = B8(0x7E0) = B8(0x7E3) = 0;
    H16(0x7E4) = H16(0x7E6) = H16(0x7E8) = H16(0x7EA) = 0;
    hun = kind_ptr(0x8BC520, KIND);     /* hunger, sleep, thirst (MH1's em_hungry_tbl, em_suimin_tbl, em_thirst_tbl) */
    sui = kind_ptr(0x8BC7D0, KIND);
    thi = kind_ptr(0x8BC690, KIND);
    if (hun && sui && thi) {
        W32(0x7F8) = thi[0];
        W32(0x7FC) = hun[0];
        W32(0x800) = sui[0];
        if (W32(0x7EC) <= 0)
            W32(0x7EC) = thi[1];
        if (W32(0x7F0) <= 0)
            W32(0x7F0) = hun[1];
        if (W32(0x7F4) <= 0)
            W32(0x7F4) = sui[1];
    } else
        missing("hunger / sleep / thirst tables", KIND);
    B8(0x80A) = 0;
    H16(0x808) = H16(0x806) = 0;
    B8(0x80B) = B8(0x80C) = B8(0x80D) = B8(0x80F) = B8(0x810) = B8(0x811) = B8(0x812) = 0;
    B8(0x814) = B8(0x815) = B8(0x813) = B8(0x816) = 0;
    W32(0x848) = W32(0x84C) = W32(0x850) = W32(0x854) = 0;
    B8(0x868) = B8(0x869) = 0;
    B8(0x86A) = 1;
    B8(0x86B) = 0;
    W32(0x86C) = W32(0x870) = W32(0x874) = W32(0x878) = 0;
    {
        const u32 *v = (const u32 *)rt_addr(0x8BB280 + 4 * (uint32_t)KIND, 4);   /* a per-kind word (MH1's em_pos_tbl place, guess) */
        W32(0x894) = v ? (s32)*v : 0;
    }
    H16(0x8A2) = 0;
    B8(0x8AB) = B8(0x8AC) = B8(0x8AD) = 0;
    H16(0x98C) = 0;
    B8(0x8B0) = 0;
    B8(0x965) = B8(0x967) = B8(0x968) = B8(0x969) = B8(0x96B) = B8(0x96A) = B8(0x96C) = 0;
    B8(0x9A3) = B8(0x973) = B8(0x9AC) = B8(0x9AD) = B8(0x975) = 0;
    taisei_set(em);
    H16(0x6FE) = H16(0x702) = 0;
    B8(0x73B) = 0;
    H16(0x718) = H16(0x71C) = 0;
    B8(0x73E) = 0;
    H16(0x710) = H16(0x714) = 0;
    B8(0x73D) = B8(0x7D7) = 0;
    H16(0x706) = H16(0x70A) = 0;
    B8(0x73C) = 0;
    H16(0x70C) = 0;
    B8(0x71E) = B8(0x71F) = B8(0x6AE) = B8(0x7DF) = B8(0x722) = 0;
    F32(0x724) = def_row(em, 4);
    F32(0x728) = def_row(em, 8);
    B8(0x437) = 1;
    W32(0x958) = 0;
    B8(0x80E) = B8(0x7CB) = B8(0x989) = B8(0x89D) = B8(0x9A0) = B8(0x9A1) = B8(0x777) = 0;
    H16(0x994) = 0;
    B8(0x9A2) = 0;
    S8(0x9A8) = -1;
    H16(0x9AA) = 0;
    B8(0x9A9) = 1;
    /* game 0x709960: two per-stage values per kind (0x8BEB60 -> +0x8A0, 0x8BEC90 -> +0x89E) */
    *(s16 *)(em + 0x8A0) = (s16)stage_value(em, kind_ptr(0x8BEB60, KIND));
    *(s16 *)(em + 0x89E) = (s16)stage_value(em, kind_ptr(0x8BEC90, KIND));
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_mh2_em: em_status_init kind %d: scale %.2f size %d attack %.2f defence %.2f stage values %d %d\n", KIND,
                F32(0xB8), B8(0x596), F32(0x724), F32(0x728), *(s16 *)(em + 0x8A0), *(s16 *)(em + 0x89E));
}

/* em_init (gm_sub 0x904880), step 0 of enemy_mv. G's (f_em_nm.c, weak) set 32 bytes to 1 at MH1's 0x4E6 through MH2_OFS, which
 * put four of them over EMW.mdl (0x4EC): the next get_joint_pos crashed on 0x01010101. MH2 sets 0x4C8..0x4E7. The online
 * parts (sub_main 0x39F400: the level and size from quest_w+0x190 for quest 25001) are left out: the PC is offline. The calls
 * in between are the host's G-form functions of the same names (not checked against MH2's). */
void em_init(u8 *em)
{
    int i;
    em_work_set(em);
    B8(0x04)++;
    B8(0x10) = 1;
    B8(0x1E) = 0;
    B8(0x7E2) = game_w[0x3C];           /* game_w.master */
    em_status_init(em);
    H16(0x29C) = H16(0x29E) = H16(0x2A0) = H16(0x2A2) = 0;
    em_wall_bit_set(em);
    em_search_data_set(em, 0);
    em_range_set(em, 0);
    taisei_set(em);
    Ikari_Data_Set(em);
    em_tsuushin_set(em);
    for (i = 0; i < 32; i++)
        em[0x4C8 + i] = 1;
    H16(0x2C0) = (u16)(Em_max_parts_get(KIND) & 0xFF);
    em_init_sub(em);
    em_dur_init(em);
    B8(0x817) = 0;
    if (B8(0x966)) {                    /* a monster shared in a session: run by the first hunter on its stage */
        for (i = 0; i < game_w[0x3E]; i++)
            if (H16(0x680) == *(u16 *)(player_work + 0x970 * i + 0x680) && game_w[0x224 + i] == 1) {
                B8(0x817) = game_w[0x3C] != i;
                B8(0x7E2) = (u8)i;
                break;
            }
    }
    World_calc(em);
    enemy_mk(em);
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_mh2_em: em_init kind %d done\n", KIND);
}

/* em_effect_move (gm_sub 0x90A990): the kind's program slot 3 from EMW+0x3D0. emt02.c's (matched, weakened by the build
 * against this host copy) reads the table at MH1's 0x3CC */
void em_effect_move(u8 *em)
{
    void (**prog)(u8 *) = (void (**)(u8 *))(uintptr_t)W32(0x3D0);
    if (prog && prog[3])
        prog[3](em);
}
/* ---- em_move (gm_sub 0x904CF0), step 1 of enemy_mv ----
 * MH2 restructured G's: no Stage_Hate_Add (G's f_em_nm.c called it and crashed on stage_hate_add_tbl, which MH2 does not have),
 * the kind's main by a 74-entry jump table (0x904AB0), MH2's own counters and the hagi (carve point) effect (sub_main 0x3A66B0).
 * Raw MH2 EMW offsets from the asm. The callees are the host's G-form functions of the same names, except the small ones written
 * in below; which of those read MH1 places on MH2's EMW was not checked one by one. */
void Em_Master_Change(void *);
int Online_ck(void);
void net_receive_em_act(void *);
void pl_timer_calc(void *);
int GetGroundHitStatusAreaEm(void *, f32 *, void *, f32 *, f32 *);
void kehai_set(void *), kehai_ck(void *), ikari_flag_set(void *), em_escape_action_ck(void *);
s8 em_eye_search_set(void *);
int em_cancel_act_ck(void *, u8);
int smell_ck(void *, int);
u8 pl_ninshiki_ck(void *);
int em_pl_pos_set(void *, u8, f32 *);
void SetVector(f32 *, f32, f32, f32);
int Em_Unko_Smoke_Ck(void *);
void Em_Taisei_Ck(void *);
void cpRotMatrixYXZ2(s32 *, f32 *);
void em_neck_move(void *), Em_Damage_Hate_Set(void *), Em_Hate_Ck(void *);
void pl_flag_set(void *, int), pl_flag_clr(void *, int);
void frame_move(void *);
void HitWallPlayer(void *, int);
void GetEmMaterialData(void *);
void em01_act_set(void *, int, u16, u16), em08_act_set(void *, int, u16, u16), em21_act_set(void *, int, u16, u16);
void Eft13_set_pos(f32, f32 *, int);
void em01_main(void *), em02_main(void *), em03_main(void *), em04_main(void *), em07_main(void *), em08_main(void *);
void em09_main(void *), em10_main(void *), em12_main(void *), em14_main(void *), em15_main(void *), em16_main(void *);
void em17_main(void *), em18_main(void *), em19_main(void *), em20_main(void *), em21_main(void *), em27_main(void *);
void em29_main(void *), em33_main(void *), em40_main(void *);
extern f32 *em_ikari_data_tbl[];
extern s16 em_atk_mode_timer_tbl[];

#define G_STAGE (*(u16 *)(game_w + 0x14))     /* 0x6ADCC4 (game_w 0x6ADCB0) */
#define MODE (em[0x14])

/* sub_main 0x39C090 (G's hit_stop_calc place): the hit stop counter at +0x40B, added to from +0x40A */
static void hit_stop(u8 *em)
{
    if (B8(0x40A)) {
        B8(0x40B) += B8(0x40A);
        B8(0x40A) = 0;
    } else if (B8(0x40B))
        B8(0x40B)--;
}
/* game 0x709A40: the end of anger (+0x80A): attack and defence back to the kind's row, speed 1.0 */
static void ikari_end(u8 *em)
{
    H16(0x808) = 0;
    B8(0x80A) = 0;
    H16(0x806) = 0;
    F32(0x724) = def_row(em, 4);
    F32(0x728) = def_row(em, 8);
    F32(0x884) = 1.0f;
    B8(0x80C) = 0;
    B8(0x9A1) = 0;
}
/* sub_main 0x3799A0: the stage table 0x43ED80 (u16; 0xFFFF: the number itself) */
static int stage_map(int n)
{
    const u16 *t = (const u16 *)rt_addr(0x43ED80 + 2 * (uint32_t)(n & 0xFFFF), 2);
    return t && *t != 0xFFFF ? *t : n;
}
/* sub_main 0x3B5880(side): 1..7 when a hunter on the same side (0x3B5A40) is in action 0x2B0..0x2B6 (char0 0x73), else 0 */
static int side_action(int side)
{
    int i;
    for (i = 0; i < 4; i++) {
        u8 *pl = player_work + 0x970 * i;
        int a = pl[0x409], b = side & 0xFF;
        int same = (a == 0 || b == 0) ? 0 : ((a & 1) ^ (b & 1)) == 0;    /* 0x3B5A40(a, b, 1) */
        u16 act;
        if (!pl[0] || !same || pl[0x69E] || pl[0x15] != 0x73)
            continue;
        act = *(u16 *)(pl + 0x7E2);
        return act >= 0x2B0 && act <= 0x2B6 ? act - 0x2B0 + 1 : 0;
    }
    return 0;
}
/* game 0x724880 (em_eye_dmg_reset_act_set): MH2's kinds by the act_set each calls; 0x7F9080 / 0x7DE790 taken as em01_act_set /
 * em08_act_set (the arguments are G's for kinds 1 and 8: guess from that) */
static void eye_dmg_reset(u8 *em)
{
    switch (KIND) {
    case 0x01: case 0x0B: case 0x25: case 0x29: case 0x2A: case 0x31:
        em01_act_set(em, 0, 25, 4);
        break;
    case 0x08: case 0x22:
        em08_act_set(em, 0, 7, 4);
        break;
    case 0x15: case 0x2E:
        if (B8(0x390) == 0)
            em21_act_set(em, 0, 7, 4);
        break;
    }
}
/* gm_sub 0x904AB0: the kind's main by jump table 0x932FC0 (74 kinds). Kinds the host has no main for are skipped (MH2's game
 * 0x7588C0, 0x817CD0, 0x836470, 0x825B80, 0x83AE60, 0x83F680, 0x8489D0, 0x851C90, 0x856DF0, 0x85BA20); kind 40 (MH2 game
 * 0x74DFC0) gets G's em40_main (guess: G's kind 40 has the same place in G's table) */
static void kind_main(u8 *em)
{
    static void (*const tbl[74])(void *) = {
        em01_main, em01_main, em02_main, em03_main, em04_main, em04_main, em20_main, em07_main, em08_main, em09_main,
        em10_main, em01_main, em12_main, em16_main, em14_main, em15_main, em16_main, em17_main, em18_main, em19_main,
        em20_main, em21_main, em17_main, em09_main, em19_main, em12_main, em14_main, em27_main, em27_main, em29_main,
        em16_main, em27_main, em04_main, em33_main, em08_main, em16_main, em02_main, em01_main, em20_main, em20_main,
        em40_main, em01_main, em01_main, em14_main, em14_main, em15_main, em21_main, em17_main, NULL, em01_main,
        em07_main, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, em03_main,
        em12_main, em02_main, NULL, NULL };
    static u8 said[74];
    if (KIND >= 74)
        return;
    if (tbl[KIND])
        tbl[KIND](em);
    else if (!said[KIND]++)
        rt_warn("MH2 em_move: kind %d's main (MH2's own) is not on the PC: the monster does not act", KIND);
}
/* sub_main 0x3A66B0: a carve / pick point the monster carries (+0x9A8, StiEM_data entry of 0x20 bytes at 0x5D87C0: +0 pos,
 * +0x12 count); the count in +0x9A9, a timer in +0x9AA, a sparkle (Eft13_set_pos 0x15) every 64 ticks. Em_hagi_point_clr
 * (0x3A6680) written in */
static void hagi_point_clr(u8 *em)
{
    Ext_pick_point_clr((int)S8(0x9A8));
    S8(0x9A8) = -1;
}
static void hagi_move(u8 *em)
{
    const u8 *d;
    f32 pos[3];
    s16 t;
    if (S8(0x9A8) == -1)
        return;
    d = rt_addr(0x5D87C0 + 0x20 * (uint32_t)S8(0x9A8), 0x20);
    if (!d)
        return;
    if (*(const u16 *)(d + 0x12) < S8(0x9A9)) {
        S8(0x9A9) = (s8)*(const u16 *)(d + 0x12);
        if (S8(0x9A9) <= 0) {
            hagi_point_clr(em);
            return;
        }
    }
    t = (s16)(H16(0x9AA) - 1);
    H16(0x9AA) = (u16)t;
    if (t <= 0) {
        hagi_point_clr(em);
        return;
    }
    if (t & 0x3F)
        return;
    pos[0] = *(const f32 *)d;
    pos[1] = *(const f32 *)(d + 4) + 20.0f;
    pos[2] = *(const f32 *)(d + 8);
    Eft13_set_pos(t < 900 ? 0.2f + 0.3f * (t / 900.0f) : 0.5f, pos, 0x15);
}
#define DEC8(o) do { if (S8(o) > 0) S8(o)--; } while (0)

static unsigned trace_n[32];
void em_move(u8 *em)
{
    int was = 0, k, g;
    u8 c;
    f32 pos[3];

    Em_Master_Change(em);
    if (Online_ck() == 1 && B8(0x966))
        net_receive_em_act(em);
    if (MODE == 8)
        H16(0x40E) = 2;
    pl_timer_calc(em);
    hit_stop(em);
    if (!B8(0x7CB)) {
        if (G_STAGE == H16(0x680)) {
            B8(0x7CB) = 1;
            GetGroundHitStatusAreaEm(em, &F32(0xAC), em + 0x660, &F32(0x574), &F32(0x730));
            if (!B8(0x390))
                F32(0xB0) = F32(0x574);
        }
    } else if (G_STAGE != H16(0x680))
        B8(0x7CB) = 0;
    if (!B8(0x975)) {
        F32(0x568) = F32(0xAC);
        F32(0x56C) = F32(0xB0);
        F32(0x570) = F32(0xB4);
    }
    if (G_STAGE != H16(0x680))
        S8(0x80E) = 2;
    if (MODE != 5 && MODE != 6 && MODE != 7 && MODE != 8) {
        if (B8(0x7DC) == 1)
            S8(0x965) = 0;
        if (S8(0x965))
            S8(0x965)--;
        else {
            kehai_set(em);
            if (B8(0x816) != 1)
                kehai_ck(em);
            em_eye_search_set(em);
            k = KIND;
            if (k == 0x34 || k == 0x33 || k == 0x30 || k == 0x27 || k == 0x14 || k == 0x2D || k == 0x0F || k == 0x31 || k == 0x29
                || k == 0x0B || k == 0x08 || k == 0x25 || k == 0x2A || k == 0x01) {
                if (!em_cancel_act_ck(em, 2) && !B8(0x817) && !B8(0x7DD))
                    c = (u8)smell_ck(em, 0x23);     /* MH2's smell_ck takes the monster only; G's host copy wants G's 0x23 */
                else
                    c = 0;
                if ((!B8(0x7DC) || KIND == 0x34) && c == 1 && !em_cancel_act_ck(em, 2) && !B8(0x811))
                    B8(0x86B) |= 2;
            }
            if (B8(0x80C) && MODE != 4 && !B8(0x811) && !em_cancel_act_ck(em, 8))
                ikari_flag_set(em);
            em_escape_action_ck(em);
            c = pl_ninshiki_ck(em);
            if ((KIND == 9 || KIND == 4) && !B8(0x7DC))
                c = 0;
            if (!B8(0x7DC) && c && !em_cancel_act_ck(em, 0x20) && B8(0x816) != 1 && !B8(0x811)) {
                B8(0x78D) = 1;
                B8(0x86B) |= 0x20;
                B8(0x86B) &= 0xFD;
            }
            if (B8(0x7DC) == 1 && c)
                H16(0x7DA) = (u16)em_atk_mode_timer_tbl[KIND];
            if (B8(0x997) == 1 && !H16(0x998) && S8(0x59B) != -1) {
                em_pl_pos_set(em, (u8)S8(0x59B), pos);
                SetVector(&F32(0x888), pos[0], pos[1], pos[2]);
            }
        }
    }
    DEC8(0x7D8);
    DEC8(0x7D9);
    DEC8(0x96E);
    DEC8(0x9A3);
    DEC8(0x973);
    DEC8(0x9AC);
    DEC8(0x9AD);
    if (S8(0x80F)) {
        if (--S8(0x80F) <= 0)
            S8(0x80F) = 0;
    }
    if (B8(0x80A)) {
        const f32 *ik = em_ikari_data_tbl[KIND];
        if (ik)
            F32(0x884) = ik[1];
        H16(0x808)--;
        if ((s16)H16(0x808) <= 0)
            ikari_end(em);
    }
    g = stage_map(game_w[0x30]);
    switch (KIND) {
    case 0x01: case 0x06: case 0x0B: case 0x14: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x31:
        if ((g & 0xFFFF) == 8 || (g & 0xFFFF) == 9 || (g & 0xFFFF) == 10)
            break;
        c = (u8)Em_Unko_Smoke_Ck(em);
        if (!B8(0x7DC) && c == 1 && !em_cancel_act_ck(em, 0x40) && !B8(0x811)) {
            B8(0x78D) = 1;
            B8(0x86B) |= 0x40;
        }
        break;
    }
    if ((s16)H16(0x89E) > 0)
        H16(0x89E)--;
    if (B8(0x7DC) == 1 && (s16)H16(0x8A0) > 0)
        H16(0x8A0)--;
    if (MODE != 5) {
        if (H16(0x8A2)) {
            H16(0x8A2)--;
            if ((s16)H16(0x8A2) <= 0) {
                H16(0x8A2) = 0;
                if (B8(0x71E) != 2 && B8(0x71E) != 1) {
                    B8(0x7DF) = 0;
                    if (!B8(0x817) && MODE != 4 && !B8(0x390))
                        eye_dmg_reset(em);
                }
            }
        } else if (B8(0x7DF))
            B8(0x7DF)--;
        Em_Taisei_Ck(em);
    }
    if (W32(0x698)) {
        if (MODE == 2 && !(F32(0x3BC) < 0.0f) && !(F32(0x3C0) <= 0.0f))
            F32(0xB0) += 30.0f;
        H16(0x736) = 2;
    } else
        H16(0x736) = 0;
    if (B8(0x409)) {
        if (B8(0x9A1))
            B8(0x89D) = 0x88;
        else {
            c = (u8)side_action(B8(0x409));
            if (!c)
                B8(0x89D) = 0;
            else if (!(B8(0x89D) & 0x80))
                B8(0x89D) = c | 0x80;
        }
    }
    kind_main(em);
    hagi_move(em);
    W32(0xA0) = (u16)W32(0xA0);
    W32(0xA4) = (u16)W32(0xA4);
    W32(0xA8) = (u16)W32(0xA8);
    cpRotMatrixYXZ2((s32 *)(em + 0xA0), &F32(0x20));
    em_neck_move(em);
    Em_Damage_Hate_Set(em);
    Em_Hate_Ck(em);
    {
        f32 f = 2.0f * (F32(0x884) * ((f32)(B8(0x8AE) + 100) / 100.0f));
        F32(0x160) = F32(0x1B0) = F32(0x200) = F32(0x250) = f;
    }
    if (B8(0x975)) {
        if (W32(0x398) & 0x20000)
            was = 1;
        else
            pl_flag_set(em, 0x20000);
    }
    frame_move(em);
    if (B8(0x975) && !was)
        pl_flag_clr(em, 0x20000);
    HitWallPlayer(em, 0);
    GetGroundHitStatusAreaEm(em, &F32(0xAC), em + 0x660, &F32(0x574), &F32(0x730));
    if (!S8(0x8B0))
        B8(0x722) = (B8(0x723) || B8(0x390) == 4) ? 1 : 0;
    else if (--S8(0x8B0) <= 0)
        S8(0x8B0) = 0;
    if (B8(0x390) != 2 && B8(0x390) != 4) {
        k = KIND;
        if (B8(0x735) && k != 0x08 && k != 0x0E && k != 0x1A && k != 0x2B && k != 0x2C && k != 0x22)
            F32(0x574) = F32(0x730);
        if (!B8(0x817) || G_STAGE == H16(0x680)) {
            F32(0xB0) = F32(0x574);
            W32(0xA0) = 0;
            W32(0xA8) = 0;
        }
    }
    GetEmMaterialData(em);
    if (getenv("RT_QUEST_TRACE") && !(trace_n[em[0x13] & 31]++ % 120))     /* test aid: where each monster is, every 120th em_move */
        fprintf(stderr, "rt_mh2_em: em_move %d kind %d stage %d mode %d act %d/%d at %.0f %.0f %.0f ang %d\n", em[0x13], KIND, H16(0x680), MODE,
                em[0x15], em[0x16], F32(0xAC), F32(0xB0), F32(0xB4), W32(0xA4));
}

/* ---- the quest's monsters on a stage: Quest_em_init_set (sub_main 0x3A2030), station_em_set (0x3A7A00), Quest_next_em_set
 * (0x3A4370) ----
 * The G-form copies (f_quest.c, f_quest_nm.c) scan the model slots at G's game_w+0x28 by raw offset, while the host's
 * em_create_model reads game_w.x28, MH2's +0x2A: the Rathian's model was never made (slot 0 read kind 0) and kind 10 got kind
 * 18's model. MH2's own forms, with the slots at +0x2A as the asm has them. f_quest.c's Quest_em_init_set is renamed away for MH2
 * by tools/build_pc.sh; f_quest_nm.c's Quest_next_em_set is weak. Left out (each is said): gm_sub 0x90AAF0 (game 0x1942F0 on the
 * word at 0x6AE040: not known), the online parts (0x39E400 is 0 offline), sub_main 0x3E5680 (the stage's Eft23 effects),
 * em_herb_set (static in f_quest.c; only after 0x3AAE80's 1-in-16 roll), game 0x86BBC0(quest_w+0x1CC, 10) when quest_w+0x1C0
 * is the stage (not known). */
s32 *Em_data_com_adrs_get();
s32 *Em_data_st_adrs_get(s32 *, int, int, s8);
void *Em_direct_set(void *q);
void em_create_model();
u16 ran_suu(int);
#define GW_SLOT(i) (game_w[0x2A + (i)])
#define QW_PTR(o) (*(s32 **)(quest_w + (o)))

/* a model slot for kind v: nothing when one has it, else the first free one gets it (em_create_model loads its command
 * tables too, rt_quest.c) */
static void model_slot(int v)
{
    int i;
    for (i = 0; i < 4; i++)
        if (GW_SLOT(i) == (u8)v)
            return;
    for (i = 0; i < 4; i++)
        if (GW_SLOT(i) == 0) {
            GW_SLOT(i) = (u8)v;
            em_create_model(i);
            return;
        }
}
/* 0x3A7A00: the quest's own monsters (quest_w.x78: model list, then the entries) */
static void station_set(void)
{
    s32 *l = Em_data_com_adrs_get(QW_PTR(0x60), 0);
    u8 *e = (u8 *)Em_data_com_adrs_get(QW_PTR(0x60), 1);
    if (!l || !e)
        return;
    for (; *l > 0; l++)
        model_slot(*l);
    for (; *(s16 *)e > 0; e += 0x3C)
        Em_direct_set(e);
}
/* sub_main 0x3AAE80: quests 0x2B04..0x2B06 on a stage of group 110, 1 in 16: kind 0x48's model (and em_herb_set) */
static int herb_roll(int n)
{
    u16 q = *(u16 *)(quest_w + 8);
    if (q != 0x2B04 && q != 0x2B05 && q != 0x2B06)
        return 0;
    if ((stage_group(n) & 0xFFFF) != 0x6E)
        return 0;
    return (ran_suu(0) & 0xF) == 0;
}
/* Em_direct_set (sub_main 0x3A3E80): one entry of a quest's monster list (MH2's entry, 0x3C bytes: +0 kind, +2 type, +6, +8 stage
 * (u16), +0xC/+0x10/+0x14 hunger, thirst, sleep, +0x18, +0x1C angle, +0x20 position, +0x2C, +0x2E flags, +0x30 size, +0x32 hit
 * points (-1: the kind's own, then kept here), +0x34 the em_work id, +0x35 the quest's boss, +0x36 level). G's (f_quest_nm.c, weak)
 * found the model slot at G's game_w+0x28 (mdl_no 2 for the Rathian), took the stage from G's +7 and the hit points from G's +8.
 * The kinds marked in em_tsuushin_tbl (0x8BEE60) take the entry's stage, others the current stage. */
void *rt_pull_enemy_work(void);
int enemy_mv(void *);
void *Em_direct_set(void *qv)
{
    u8 *q = qv, *em;
    const u8 *ts;
    int i;
    for (i = 0; i < 4; i++)
        if (*(s16 *)q == GW_SLOT(i))
            break;
    if (i >= 4)
        return NULL;
    em = rt_pull_enemy_work();
    if (!em)
        return NULL;
    B8(0x30F) = (u8)i;                   /* mdl_no */
    KIND = (u8)*(s16 *)q;
    B8(0x1B) = (u8)*(s16 *)(q + 2);
    B8(0x96F) = (u8)*(s16 *)(q + 0x2C);
    ts = rt_addr(0x8BEE60 + KIND, 1);
    H16(0x680) = ts && *ts ? *(u16 *)(q + 8) : G_STAGE;
    if (getenv("RT_MH2_EM_HERE") && KIND == atoi(getenv("RT_MH2_EM_HERE")))   /* test aid: that kind on the hunter's stage, by his start */
        H16(0x680) = G_STAGE;
    W32(0x7F0) = *(s32 *)(q + 0xC);
    W32(0x7EC) = *(s32 *)(q + 0x10);
    W32(0x7F4) = *(s32 *)(q + 0x14);
    B8(0x8AF) = (u8)*(u16 *)(q + 6);
    B8(0x964) = (u8)*(s32 *)(q + 0x18);
    F32(0xAC) = *(f32 *)(q + 0x20);
    F32(0xB0) = *(f32 *)(q + 0x24);
    F32(0xB4) = *(f32 *)(q + 0x28);
    W32(0xA4) = *(s32 *)(q + 0x1C);
    H16(0x984) = *(u16 *)(q + 0x30);
    S8(0x98A) = *(s8 *)(q + 0x36);
    if (getenv("RT_MH2_EM_HERE") && KIND == atoi(getenv("RT_MH2_EM_HERE"))) {
        F32(0xAC) = *(f32 *)(player_work + 0xAC) + 1500.0f;
        F32(0xB4) = *(f32 *)(player_work + 0xB4) + 1500.0f;
    }
    q[0x34] = (u8)H16(0xC);
    enemy_mv(em);
    {   /* gm_sub 0x9043C0: the scale by the size in percent */
        f32 f = H16(0x984) / 100.0f;
        F32(0xB8) = F32(0x584) * f;
        F32(0xBC) = F32(0x584) * f;
        F32(0xC0) = F32(0x584) * f;
    }
    enemy_mk(em);
    if (*(s16 *)(q + 0x32) == -1) {
        *(s16 *)(q + 0x32) = (s16)H16(0x2C2);
        if (q[0x35]) {
            *(u8 **)(quest_w + 0x98) = em;     /* (a host pointer in a PS2 word: the PC is 32-bit) */
            *(s16 *)(quest_w + 0x134) = (s16)H16(0x2C2);
        }
    } else
        H16(0x2C2) = *(u16 *)(q + 0x32);
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_mh2_em: Em_direct_set kind %d slot %d stage %d (entry %d) hp %d size %d level %d at %.0f %.0f %.0f\n", KIND,
                B8(0x30F), H16(0x680), *(s16 *)(q + 8), (s16)H16(0x2C2), H16(0x984), S8(0x98A), F32(0xAC), F32(0xB0), F32(0xB4));
    return em;
}
/* quest_em_init (sub_main 0x3A7xxx: quest_em_init_sub 0x3A77E0 on quest_w.x78, quest_em_init_sub2 0x3A7920 on quest_w.x74), called
 * by Quest_start (rt_mh2.c) in place of G's: G's entry set-up (sub_main_func_004B78D0's form) wrote -1 to G's hit points at +8,
 * which in MH2's entry is the stage, and left MH2's +0x32 alone (the Rathian was made with 0 hit points and went straight to
 * dying). 0x3A73E0 per entry: +0x2E = 2, +0x32 = -1, +0x34 = -1, +0x2C = the running count, the level (0x3A7230), +0x35 from
 * 0x8BF2D0[kind] (the quest's boss: quest_w+0x1C += entry +4). Not written: the level roll 0x3A7230 (a weighted pick from
 * 0x90D5B8 / 0x90D5D0 / 0x90D670 by the quest generator 0x3A39F0): level 0 unless quest flag 0x4000 gives quest_w+0x136 (guess
 * that 0 is a fair stand-in); the size roll of 0x3A7170 / 0x3A7480 for the kinds that get one: 100 for all. */
static void qem_entry_init(u8 *e, int *cnt)
{
    const u8 *boss;
    *(s16 *)(e + 0x2E) = 2;
    *(s16 *)(e + 0x32) = -1;
    e[0x34] = 0xFF;
    *(s16 *)(e + 0x2C) = (s16)*cnt;
    e[0x36] = (*(u32 *)(quest_w + 0x28) & 0x4000) ? quest_w[0x136] : 0;
    boss = rt_addr(0x8BF2D0 + (uint32_t)*(s16 *)e, 1);
    e[0x35] = boss ? *boss : 0;
    if ((s8)e[0x35])
        *(s16 *)(quest_w + 0x1C) += (s8)e[4];
    (*cnt)++;
}
static void qem_block(s32 *blk, int *cnt)
{
    for (;; blk += 4) {
        s32 *l0 = Em_data_com_adrs_get(blk, 0);
        u8 *e = (u8 *)Em_data_com_adrs_get(blk, 1);
        if (!l0)
            return;
        for (; e && *(s16 *)e > 0; e += 0x3C) {
            qem_entry_init(e, cnt);
            *(u16 *)(e + 0x30) = 100;
        }
    }
}
extern u8 *mission_area;
void rt_mh2_quest_em_init(void)
{
    int n = 0;
    s32 *p;
    qem_block(QW_PTR(0x60), &n);
    for (p = QW_PTR(0x5C); p && *p; p++)     /* quest_em_init_sub2: a list of blocks, offsets into the mission file once a quest is set */
        qem_block((s32 *)((u8 *)(uintptr_t)(u32)*p + (*(u16 *)(quest_w + 8) ? (uintptr_t)mission_area : 0)), &n);
}
void Quest_next_em_set(int n)
{
    s32 *l0;
    u8 *l1;
    int herb;
    if (n == game_w[0x31])
        for (herb = 0; herb < 4; herb++)
            if (GW_SLOT(herb) == 0) {
                GW_SLOT(herb) = 0x12;
                em_create_model(herb);
                break;
            }
    herb = herb_roll(n);
    if (herb) {
        int i;
        for (i = 0; i < 4; i++)
            if (GW_SLOT(i) == 0) {
                GW_SLOT(i) = 0x48;
                em_create_model(i);
                break;
            }
    }
    l0 = Em_data_st_adrs_get(QW_PTR(0x5C), n, 0, (s8)quest_w[0x22]);
    l1 = (u8 *)Em_data_st_adrs_get(QW_PTR(0x5C), n, 1, (s8)quest_w[0x22]);
    if (l0) {
        for (; *l0 > 0; l0++)
            model_slot(*l0);
        for (; l1 && *(s16 *)l1 > 0; l1 += 0x3C) {
            s16 id = *(s16 *)l1;
            if (*(s16 *)(l1 + 0x2E) & 1)
                continue;
            if (id == 10 && ((s8)quest_w[0x15A] ? *(s16 *)(l1 + 2) == 0 : *(s16 *)(l1 + 2) != 0))
                continue;
            if (id < 100) {
                Em_direct_set(l1);
                *(u16 *)(l1 + 8) = *(u16 *)(game_w + 0x14);
            }
        }
    }
    if (herb)
        rt_warn("MH2 Quest_next_em_set: em_herb_set (stage %d) is not on the PC", n);
}
void Quest_em_init_set(int n)
{
    station_set();
    Quest_next_em_set(n);
}

#endif
