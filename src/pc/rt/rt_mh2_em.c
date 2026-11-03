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
#endif
