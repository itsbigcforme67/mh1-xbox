/*
 * rt_mh2.c - host stand-ins for MH Dos (VERSION_MH2) on the way to a `--quest` hunt (docs/mh2/hunt.md).
 *
 * Quest_start (sub_main 0x3A11A0, no C yet): read from MH2's asm for the PC. It is G's Quest_start (f_quest.c) with MH2's
 * mission file: the same header offsets up to +0x30, the quest's rank byte at +0x48 instead of +0x34, two new tables at
 * +0x3C/+0x40, the info block's reward/fee/time at +0x0C/+0x10/+0x1C and its table at +0x24. Not byte-checked: a host
 * stand-in until CODE writes the function. What it skips is said at each place.
 */
#ifdef VERSION_MH2
#include "rt.h"
#include "types.h"
#include "game.h"
#include "gwx.h"
#include "quest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MU8(o) (((u8 *)mission_area)[o])
#define MS8(o) (((s8 *)mission_area)[o])
#define MW(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define QB(o) (((u8 *)&quest_w)[o])

extern u8 *mission_area;

void Ext_pick_point_init(void);
int ran_suu();
unsigned char Get_hunter_rank();
void quest_em_init();
void rt_quest_mem_init(void);

/* the mission file m<no><d|n><0-2>.mib, read by the viewer (it has DATA.BIN open): bytes into dst, 0 when missing */
static size_t (*mib_fn)(int no, u8 *dst, size_t max);
void rt_mh2_set_mib_loader(size_t (*fn)(int no, u8 *dst, size_t max)) { mib_fn = fn; }

/* the start stage, Quest_pl_stage_init(0xFF) (sub_main 0x3A1D00): file byte 0x5D non-zero = a weighted pick from gm_sub's list
 * 0x9316B0[info+0x20][pick - 1] ({u16 weight, u16 stage} ended by 0xFFFF) with quest_w.rnd[master] % the weights' sum; else
 * the player start table (header +4), 16 bytes per player, the stage its first word. Both into game_w+0x14 and +0x16. */
static uint32_t word_va(uint32_t va)
{
    const uint8_t *p = rt_addr(va, 4);
    uint32_t w = 0, h;
    if (!p)
        return 0;
    memcpy(&w, p, 4);
    h = w >= 0x02000000u ? rt_va_of_host((const void *)(uintptr_t)w) : 0;    /* the import made it a host pointer */
    return h ? h : w;
}
static int start_stage(void)
{
    u8 *m = mission_area, *info = m + MW(m, 0);
    int pick = MS8(0x5D);
    if (pick) {
        uint32_t l = word_va(0x9316B0 + 4 * (uint32_t)MW(info, 0x20)), e;
        const u16 *q;
        int sum = 0, r, acc = 0;
        if (l)
            l = word_va(l + 4 * (uint32_t)(pick - 1));
        q = l ? (const u16 *)rt_addr(l, 4) : NULL;
        if (!q) {
            rt_warn("MH2 quest: gm_sub start list not readable (info+0x20 = %d)", (int)MW(info, 0x20));
            return -1;
        }
        for (e = 0; q[2 * e] != 0xFFFF; e++)
            sum += q[2 * e];
        r = sum ? quest_w.rnd[game_w.master] % sum : 0;
        for (e = 0; q[2 * e] != 0xFFFF; e++) {
            acc += q[2 * e];
            if (r < acc)
                return q[2 * e + 1];
        }
        return q[1];
    }
    return MW(m + MW(m, 4), 16 * game_w.master);
}

/* Per-stage pointer tables whose words point into gm_sub (0x8FAE00-0x939600): tools/mh2_pcmeta.py's pointer scan does not let
 * sub_main data point into gm_sub, so the import leaves those words as PS2 addresses. Turned into host pointers here, once
 * (gm_sub is loaded as an extra image, rt_addr reads it). Words of other ranges were relocated by the import already. */
void *rt_host_sym(const char *n);
size_t rt_table_size(const char *name);
/* em_prog_tbl (gm_sub 0x90DE70, 35 kinds): each kind's table {local init, local init, ?, effect move, two more functions MH2
 * added, then data}. MH2's monster functions have no names the PC knows, so those slots import as NULL and em_create_model
 * refuses the kind. Stand-in: G's functions of the same kind number (emNN_local_init, emNN_effect_move, host C) in a copy of
 * the table. Whether MH2's kind N is G's kind N is checked for kind 1 only (its model is em01, the Rathian); for the other
 * kinds it is a guess. RT_MH2_EM_PROG=0 turns this off. */
extern void *em_prog_tbl[];
static void rt_nop_em(void *em) { (void)em; }
static void em_prog_fix(void)
{
    static void *copy[35][16];
    char a[48], b[48];
    int k, j;
    if (getenv("RT_MH2_EM_PROG") && !atoi(getenv("RT_MH2_EM_PROG")))
        return;
    for (k = 1; k < 35; k++) {
        void **t = em_prog_tbl[k], *init, *eff;
        if (!t || (t[0] && t[3]))
            continue;
        snprintf(a, sizeof a, "em%02d_local_init", k);
        snprintf(b, sizeof b, "em%02d_effect_move", k);
        init = rt_host_sym(a);
        eff = rt_host_sym(b);
        if (!init || !eff)
            continue;
        for (j = 0; j < 16; j++)
            copy[k][j] = t[j];
        copy[k][0] = copy[k][0] ? copy[k][0] : init;
        copy[k][1] = copy[k][1] ? copy[k][1] : init;
        copy[k][2] = copy[k][2] ? copy[k][2] : (void *)rt_nop_em;
        copy[k][3] = copy[k][3] ? copy[k][3] : eff;
        em_prog_tbl[k] = copy[k];
        rt_log_standin("MH2 em_prog_tbl: a kind's program slots from G's emNN functions");
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "rt_mh2: em_prog_tbl[%d]: G's %s / %s stand in\n", k, a, b);
    }
}
static void gm_sub_ptrs(void)
{
    static const char *const nm[] = { "ground_tbl_add", "wall_tbl_add", "shadow_tbl_add", "diffuse_tbl_add", "light_tbl_add" };
    static int done;
    size_t k, i, n;
    if (done++)
        return;
    for (k = 0; k < sizeof nm / sizeof nm[0]; k++) {
        uint32_t *w = rt_host_sym(nm[k]);
        n = rt_table_size(nm[k]) / 4;
        for (i = 0; w && i < n; i++)
            if (w[i] >= 0x8FAE00 && w[i] < 0x939600) {
                const uint8_t *h = rt_addr(w[i], 4);
                w[i] = h ? (uint32_t)(uintptr_t)h : 0;
            }
    }
    em_prog_fix();
}

int rt_mh2_quest_start(int no)
{
    u8 *m, *info, *p;
    int i, st;
    size_t n;
    gm_sub_ptrs();
    rt_quest_mem_init();
    if (!mib_fn || (n = mib_fn(no, mission_area, 0x40000)) < 0x74) {
        rt_warn("MH2 quest %d: no mission file", no);
        return -1;
    }
    /* 0x3A11A0: the quest number from the select work into game_w.quest and quest_w.no; memcpy of 24 bytes of that work into
     * quest_w+0x1F4 and the call 0x18D750 are skipped (what they hold is not known) */
    quest_w.no = game_w.quest = (u16)no;
    game_w.x0D0 = 0;
    game_w.x0D5 = 2;
    Ext_pick_point_init();
    /* 0x3A7FC0, 0x366950, 0x3A3DF0: not known, skipped */
    for (i = 0; i < 15; i++)    /* offline: 15 random bytes (online: the host's), then the quest's own generator 0x3A3850/0x3A39F0 */
        quest_w.rnd[i] = (u8)ran_suu(0);
    memset(quest_w.x1D4, 0, sizeof quest_w.x1D4);   /* 0x20C and 0x34C, 0x50 u16 each */
    memset(quest_w.x16C, 0, sizeof quest_w.x16C);
    quest_w.x140 = 0;
    quest_w.x13C = 0;
    quest_w.x150 = 0;
    quest_w.x14F = 0;
    quest_w.x0B = 0;
    memset(quest_w.xB4, 0, sizeof quest_w.xB4);
    quest_w.xB4[game_w.master].b = Get_hunter_rank(&User_data);
    quest_w.xAE = 1;
    memset(game_w.x28, 0, 4);
    memset(quest_w.x98, 0, sizeof quest_w.x98);
    quest_w.xAC = quest_w.xAD = 0;
    quest_w.x03 = quest_w.x00 = quest_w.x01 = 0;
    quest_w.x3A = 0;
    quest_w.x3C = 0;
    quest_w.x10 = 0;
    quest_w.x04 = 0;
    /* (the stage's file pick, 0x18D970 / 0x18DA70: day or night and the variant, is the viewer's: RT_MIB) */
    m = mission_area;
    quest_w.x64 = (void *)m;
    info = m + MW(m, 0);
    quest_w.x94 = (void *)info;
    QB(0x148) = 0;                  /* 0x39E400 returned 0 (offline): byte 0x148 = 0, u16 0x156 = 60 */
    *(u16 *)&QB(0x156) = 60;
    st = start_stage();
    if (st >= 0) {
        game_w.stage = (u16)st;
        *(u16 *)((u8 *)&game_w + 0x16) = (u16)st;
    }
    quest_w.x10 = MW(info, 0x1C);    /* Quest_time_get(1): info+0x1C */
    quest_w.xAF = (s8)((quest_w.x10 - 1) / 9000 > 2 ? 2 : (quest_w.x10 - 1) / 9000);
    quest_w.x38 = 0;
    quest_w.x36 = 0;
    quest_w.x6C = (s32 *)(m + MW(m, 0x10));
    quest_w.x7C = (s32 *)(m + MW(m, 0x1C));
    quest_w.x70 = (s32 *)(m + MW(m, 0x30));
    quest_w.x84 = (s32 *)(m + MW(info, 0x24));
    quest_w.x74 = (s32 *)(m + MW(m, 0x14));
    quest_w.x78 = (s32 *)(m + MW(m, 0x18));
    quest_w.x80 = (s32 *)(m + MW(m, 0x20));
    quest_w.x8C = (s32 *)(m + MW(m, 0x28));
    quest_w.x90 = (s32 *)(m + MW(m, 0x2C));
    *(void **)&QB(0x530) = m + MW(m, 0x3C);     /* new in MH2; what reads them is not known */
    *(void **)&QB(0x534) = m + MW(m, 0x40);
    quest_w.x14 = MW(info, 0x0C);
    quest_w.x18 = MW(info, 0x10);
    quest_w.x88 = (s32 *)(m + MW(m, 0x0C));
    quest_w.x40 = MW(info, 0);
    quest_w.x14E = (s8)MW(m, 0x48);
    MW(m, 0x38) += (s32)(intptr_t)m;            /* the word at +0x38 is made a pointer in place (a 32-bit host pointer) */
    p = m + MW(m, 0x24);
    game_w.x2E = (u8)MW(p, 0);
    game_w.x2F = (u8)MW(p, 4);
    quest_w.x165[0] = MS8(0x5E);
    quest_w.x165[1] = MS8(0x5F);
    quest_w.x165[2] = MS8(0x60);
    QB(0x9D) = (u8)((quest_w.x10 - 1) / 9000);   /* layouts.txt calls 0x9C x169 and 0x9D x168: the asm stores this at 0x9D */
    QB(0x9C) = 0;
    {   /* 0x15A: a byte of the table 0x44B450 by the quest's generator (bonus, guess); 0 online */
        const s8 *b = (const s8 *)rt_addr(0x44B450, 16);
        QB(0x15A) = b ? (u8)b[ran_suu(0) & 0xF] : 0;
    }
    /* 0x3A8060, 0x3A3E20: not known, skipped */
    quest_em_init();
    /* NPCZoomInCameraCancel, game 0x6EE380(0), gm_sub 0x90AAA0: skipped */
    rt_log("MH2 quest %d: mission %u bytes, start stage %d, time %d, reward %d, fee %d, flags 0x%X", no, (unsigned)n, st,
           quest_w.x10, quest_w.x14, quest_w.x18, (unsigned)quest_w.x40);
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_mh2: quest %d: start stage %d, time %d, reward %d, fee %d, flags 0x%X, models %d %d %d %d\n", no, st,
                quest_w.x10, quest_w.x14, quest_w.x18, (unsigned)quest_w.x40, game_w.x28[0], game_w.x28[1], game_w.x28[2],
                game_w.x28[3]);
    return 0;
}

/* ------------------------------------------------ lights (sub_main 0x332C70-0x3337D0, no C yet)
 * MH2 dropped G's per-stage pl_light_tbl: the actor lights come from the stage's environment block or fixed rows. light_nm.c's and
 * light_init_nm.c's G copies are renamed away for MH2 (tools/build_pc.sh). A light block of a set is at light_work + set * 0x140 +
 * 0x10 + i * 0x68: +0x0C colour, +0x1C second row, +0x2C third row, +0x3C direction, +0x48 fourth row (as G). */
extern u8 light_work[];
extern u8 *stg_light_tbl[5];
#define SW ((u8 *)&stage_work)
static void rows(u8 *dst, uint32_t va, int stride, int i)
{
    const uint8_t *p = rt_addr(va + (uint32_t)(stride * i), 12);
    if (p)
        memcpy(dst, p, 12);
}
/* 0x332C70(block + 8, stage, light, mode): mode 0 direction and third row, mode 0 or 1 the colour. From a stage other than 0
 * whose environment block is loaded (stage_work+0x4D > 0): +0x5C direction, +0xA4 third row, +0x80 colour (G's offsets); else
 * the rows 0x41EC00 (direction, 12 bytes a light), 0x41ECF0 (third, 16), 0x41EC60 (colour, 16). Stage 86 first writes rows
 * of its own (0x41EC30, 0x41ED20, 0x41ECC0), but every path then reaches the code above, which overwrites them: kept so */
static void light_rows(u8 *b, int st, int i, int mode)
{
    int env = st != 0 && SW[0x4D] > 0;
    if (mode == 0) {
        if (env) {
            memcpy(b + 0x3C, SW + 0x5C + 12 * i, 12);
            memcpy(b + 0x2C, SW + 0xA4 + 12 * i, 12);
        } else {
            rows(b + 0x3C, 0x41EC00, 12, i);
            rows(b + 0x2C, 0x41ECF0, 16, i);
        }
    }
    if (mode == 0 || mode == 1) {
        if (env)
            memcpy(b + 0x0C, SW + 0x80 + 12 * i, 12);
        else
            rows(b + 0x0C, 0x41EC60, 16, i);
    }
}
/* light_init (0x332F80): set 0 as G from stg_light_tbl (colours x 10); set 1 per light: fourth row 0, light_rows mode 0, second
 * row 1.0, the block's +8 word 0 */
void light_init(void)
{
    u8 *b;
    int i, k;
    memset(light_work, 0, 0x290);
    for (i = 0, b = light_work + 0x10; i < 3; i++, b += 0x68) {
        for (k = 0; k < 3; k++) {
            *(f32 *)(b + 0x48 + 4 * k) = ((f32 *)(stg_light_tbl[0] + 12 * i))[k];
            *(f32 *)(b + 0x3C + 4 * k) = ((f32 *)(stg_light_tbl[1] + 12 * i))[k];
            *(f32 *)(b + 0x0C + 4 * k) = 10.0f * ((f32 *)(stg_light_tbl[2] + 16 * i))[k];
            *(f32 *)(b + 0x2C + 4 * k) = ((f32 *)(stg_light_tbl[3] + 16 * i))[k];
            *(f32 *)(b + 0x1C + 4 * k) = ((f32 *)(stg_light_tbl[4] + 16 * i))[k];
        }
        *(s32 *)(b + 8) = 0;
    }
    for (i = 0, b = light_work + 0x150; i < 3; i++, b += 0x68) {
        memset(b + 0x48, 0, 12);
        light_rows(b, game_w.stage, i, 0);
        *(f32 *)(b + 0x1C) = *(f32 *)(b + 0x20) = *(f32 *)(b + 0x24) = 1.0f;
        *(s32 *)(b + 8) = 0;
    }
}
/* 0x333770(n): set n's colours, light_rows(mode 1) with n passed as the stage (as the asm does: a1 = n). G's light_change_normal
 * took the stage's colours; that this is its MH2 form is a guess from the place and the shape */
void light_change_normal(int n)
{
    int i;
    for (i = 0; i < 3; i++)
        light_rows(light_work + n * 0x140 + 0x10 + i * 0x68, n, i, 1);
}
/* pl_light_change (0x333640, G's at the same place): per light of set n, near a monster on stages 12/13/14/28/30
 * (PLW+0x597, G 0x613) the rows 0x41EC90; else the player's own rows (PLW+0x664, G 0x710) for lights 0 and 1; else light_rows
 * mode 1 */
void pl_light_change(u8 *pl, int n)
{
    int i, st = game_w.stage;
    for (i = 0; i < 3; i++) {
        u8 *b = light_work + n * 0x140 + 0x10 + i * 0x68;
        f32 *own = *(f32 **)(pl + 0x664);
        if ((st == 30 || st == 28 || st == 14 || st == 13 || st == 12) && pl[0x597])
            rows(b + 0x0C, 0x41EC90, 16, i);
        else if (own && i != 2)
            memcpy(b + 0x0C, (u8 *)own + 16 * i, 12);
        else
            light_rows(b, st, i, 1);
    }
}

/* ------------------------------------------------ monster command data (gm_sub 0x90AAA0-0x90AC24, game 0x7262E0, no C yet)
 * MH2 loads each kind's command tables from em_cmd.afs (DATA.BIN entry 6587) when its model is made: Quest_next_em_set
 * (0x3A4370) calls 0x90AB00(game_w.x2E, kind) before em_create_model. 0x932C70[game_w.x2E] is a per-kind list of em_cmd.afs
 * entries (-1 none); the file's last two non-zero words are the offsets of its outer table and of a table of byte counts per
 * outer entry; 0x90A9D0 turns the outer words and each entry's inner words into pointers (-1 to 0, words not below the
 * file's address left alone). 0x939450[kind] keeps the outer table: em_cmd_init (0x7262E0) puts it in EMW.cmd_tbl. The PC loads
 * from em_create_model (rt_quest.c), each kind once per run (the PS2 clears the list at Quest_start and the stage load). */
static size_t (*emcmd_fn)(int idx, u8 *dst, size_t max);
void rt_mh2_set_em_cmd_loader(size_t (*fn)(int idx, u8 *dst, size_t max)) { emcmd_fn = fn; }
static u8 *emcmd[64];
static void emcmd_reloc(u8 *base, u32 *outer, u32 *cnt)
{
    int n = (int)(cnt - outer), i, j;
    for (i = 0; i < n; i++) {
        u32 v = outer[i], *in;
        int m;
        if (v == 0xFFFFFFFFu) {
            outer[i] = 0;
            continue;
        }
        if (v == 0 || v >= (u32)(uintptr_t)base)
            continue;
        outer[i] = v + (u32)(uintptr_t)base;
        in = (u32 *)(uintptr_t)outer[i];
        m = (int)(cnt[i] >> 2);
        for (j = 0; j < m; j++) {
            if (in[j] == 0xFFFFFFFFu)
                in[j] = 0;
            else if (in[j] < (u32)(uintptr_t)base)
                in[j] += (u32)(uintptr_t)base;
            else
                break;
        }
    }
}
void rt_mh2_em_cmd_load(int level, int kind)
{
    const s32 *l;
    u32 lva;
    int idx;
    size_t n;
    u8 *f;
    u32 *e;
    if (kind <= 0 || kind >= 64 || emcmd[kind] || !emcmd_fn)
        return;
    lva = word_va(0x932C70 + 4 * (uint32_t)level);
    l = lva ? (const s32 *)rt_addr(lva + 4 * (uint32_t)kind, 4) : NULL;
    if (!l || (idx = *l) < 0)
        return;
    if (!(f = calloc(1, 0x10000)) || (n = emcmd_fn(idx, f, 0x10000)) < 8) {
        free(f);
        rt_warn("MH2 em_cmd.afs entry %d (kind %d) not read", idx, kind);
        return;
    }
    for (e = (u32 *)(f + (n & ~3u)) - 1; e > (u32 *)f && *e == 0; e--)
        ;
    if (e - 1 <= (u32 *)f || e[-1] >= n || e[0] >= n) {
        free(f);
        rt_warn("MH2 em_cmd.afs entry %d: no table offsets at its end", idx);
        return;
    }
    emcmd_reloc(f, (u32 *)(f + e[-1]), (u32 *)(f + e[0]));
    emcmd[kind] = f + e[-1];
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_mh2: em_cmd.afs entry %d for kind %d (level %d): %u bytes, %d tables\n", idx, kind, level, (unsigned)n,
                (int)((e[0] - e[-1]) / 4));
}
/* em_cmd_init (game 0x7262E0); em_cmd_nm.c's G copy is renamed away for MH2 (tools/build_pc.sh). Raw MH2 EMW offsets as the asm
 * has them; the named ones are mh2_emw.h's (cmd_tbl 0x754, cmd_pc 0x758, cmd_top 0x7C4) */
#define EB2(o) (*(u8 *)((u8 *)em + (o)))
#define EH2(o) (*(u16 *)((u8 *)em + (o)))
#define EW2(o) (*(u32 *)((u8 *)em + (o)))
extern s32 em_atk_bit;
u8 *em_cmd_top(void *em);
void em_cmd_init(u8 *em)
{
    EB2(0x774) = EB2(0x775) = EB2(0x776) = EB2(0x778) = 0;
    EB2(0x7A2) = 0;
    EB2(0x798) = 0xFF;
    EW2(0x754) = (u32)(uintptr_t)emcmd[em[2] < 64 ? em[2] : 0];
    EW2(0x758) = EW2(0x754) ? (u32)(uintptr_t)em_cmd_top(em) : 0;     /* PC: no tables (no em_cmd.afs entry for the kind): no program */
    EB2(0x87C) = EB2(0x87D) = EB2(0x881) = EB2(0x880) = 0xFF;
    EH2(0x986) = 0xFFFF;
    EH2(0x882) = 0xFFFF;
    EW2(0x7A8) = 0;
    em_atk_bit = 0;
    EW2(0x7C4) = EW2(0x758);
    EB2(0x779) = 0;
    EH2(0x77A) = EH2(0x77C) = 0;
    EB2(0x997) = 0;
    EH2(0x998) = EH2(0x99A) = 0;
}
#endif
