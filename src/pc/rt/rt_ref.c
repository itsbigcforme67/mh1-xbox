/*
 * rt_ref.c - the PC side of the lockstep referee (tools/referee.py, docs/referee.md).
 *
 * The real game in PCSX2 and this port play the same quest with the same pad, tick by tick; the referee compares
 * the game state. Here, in a --quest run:
 *   RT_REF_OUT=file   one record per game tick of the quest (game_w mode 2..5) with the state at the START of the tick
 *                     (what the previous tick left): player_work[0], em_work[0..19], game_w, quest_w, Rnd_w, Psw[0].
 *                     The PS2 side (tools/referee.py) writes the same records from the PS2's RAM through PINE.
 *   RT_REF_PAD=file   the pad per play tick: one line "bits lx ly rx ry" (fl pad bits, sticks -127..127) per tick in
 *                     which the quest runs (game_w mode 2, step 0/1: not while an area loads); idle after the last line
 *   RT_REF_UD=file    User_data from the PS2 (the hunter's save: equipment, pouch, rank), copied in before the quest
 *                     loads
 *   RT_REF_RND=a,b    Rnd_w (the game's random state, ran_suu) set at the first play tick
 *   RT_REF_POKE=file  writes at play ticks, the same ones the PS2 side makes: lines "PTICK REGION OFFSET TYPE VALUE"
 *                     (REGION plw / emN / gw / qw / ud; TYPE u8 u16 u32 f32): scenario aids (an area change)
 *   RT_REF_RNDS=file  the PS2's Rnd_w at the start of each play tick ("PTICK A B" lines), set after this side's record
 *                     of the tick: a difference in random calls then only shows in the tick where it happens instead
 *                     of moving everything random afterwards (the record still shows the PC's own value)
 *   RT_REF_RNG=file   every ran_suu call: "PTICK CHANNEL OFFSET" (the caller's return address minus ran_suu's own, so
 *                     the referee names the caller from the binary's symbols although the program is position
 *                     independent); the PS2 side logs its calls the same way (a hook on ran_suu)
 * A play tick is a tick that starts in game_w mode 2 with step 0 or 1 (game2 runs game_core). Loading ticks (game2
 * steps 2-6) take one tick here and many on the PS2, so the referee lines the two runs up by play ticks.
 * Record layout: "REF1" u32 ptick (-1 outside play) u32 raw tick, then the regions in the order of the header line.
 */
#include "rt.h"
#include "types.h"
#include "../pad/pad.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern u8 player_work[], em_work[], game_w[], quest_w[], User_data[], Psw[];
extern u16 Rnd_w[2];

#ifdef VERSION_MHG
#define GW_SIZE 0x240
#define QW_SIZE 0x290
#define UD_SIZE 0xE00
#else
#define GW_SIZE 0x224
#define QW_SIZE 0x190
#define UD_SIZE 0x480
#endif
#define EM_STRIDE 0xA10         /* the PC's em_work stride (MH1's; G's PS2 has 0xA00, em.h) */
#define EM_N 20
#define PLW_SIZE 0xA00

static FILE *out;
static int on, started, ptick, rawtick;
static pad_state *pads;
static int npads;
typedef struct { int pt; char reg[8]; int off; char type[4]; double v; } poke;
static poke *pokes;
static int npokes;

static void load_pads(const char *path)
{
    FILE *f = fopen(path, "r");
    int cap = 0, b, lx, ly, rx, ry;
    if (!f) {
        rt_warn("referee: no pad file %s", path);
        return;
    }
    while (fscanf(f, "%i %d %d %d %d", &b, &lx, &ly, &rx, &ry) == 5) {
        if (npads == cap)
            pads = realloc(pads, sizeof *pads * (size_t)(cap = cap ? cap * 2 : 1024));
        pads[npads].bits = (uint16_t)b;
        pads[npads].lx = lx;
        pads[npads].ly = ly;
        pads[npads].rx = rx;
        pads[npads].ry = ry;
        npads++;
    }
    fclose(f);
}

static void load_pokes(const char *path)
{
    FILE *f = fopen(path, "r");
    int cap = 0;
    poke p;
    if (!f)
        return;
    while (fscanf(f, "%d %7s %i %3s %lf", &p.pt, p.reg, &p.off, p.type, &p.v) == 5) {
        if (npokes == cap)
            pokes = realloc(pokes, sizeof *pokes * (size_t)(cap = cap ? cap * 2 : 64));
        pokes[npokes++] = p;
    }
    fclose(f);
}

static FILE *rng;
static u16 (*rnds)[2];
static int nrnds;
extern int rt_ref_rng_on;
u32 ran_suu(int ch);
void rt_ref_rng(void *ra, int ch)
{
    if (rng && started)
        fprintf(rng, "%d %d %ld\n", game_w[0] == 2 && game_w[1] <= 1 ? ptick : -1, ch, (long)((char *)ra - (char *)ran_suu));
}

/* The PS2's float rounding for the game's float math, ON BY DEFAULT (owner, 18 Oct 2026; RT_FPU_PS2=0 turns it off):
 * the EE FPU rounds toward zero and has no denormals; SSE does the same with MXCSR's rounding bits at "toward zero"
 * and flush-to-zero / denormals-are-zero. The game C is built with SSE math (build_pc.sh FPSSE, build_win.sh,
 * build_xbox.py -msse), so the mode reaches all of it. The host's own float code on 32-bit x86 (rt_*.c, the
 * renderer, libm, the mixer) is x87 code and does not read MXCSR, so it keeps round-to-nearest.
 * The referee showed why it matters: a Velociprey's run motion counts its frame up by 2.4 a tick; on the PS2 the sum
 * reaches 47.99998 and the 48-frame loop wraps a tick later than the PC's exact 48.0, and the monster lands 5 units
 * elsewhere (docs/referee.md).
 * DAZ (bit 6) is only set when the CPU has it: the Xbox's Pentium III has no DAZ, and ldmxcsr with a bit outside
 * MXCSR_MASK raises #GP (the mask comes from fxsave; 0 there means the old default mask 0xFFBF, no DAZ).
 * MXCSR is per thread: this runs on the game thread (viewer.c's main) before the game starts. */
static unsigned int fpu_mode = 0x1F80;
static unsigned int fpu_mxcsr(void)
{
    unsigned int m = 0x1F80;
#if defined(__i386__) || defined(__x86_64__)
    __asm__ volatile("stmxcsr %0" : "=m"(m));
#endif
    return m;
}
static void fpu_ps2(void)
{
#if defined(__i386__) || defined(__x86_64__)
    const char *e = getenv("RT_FPU_PS2");
    unsigned int m, mask;
    static unsigned char fx[512 + 16];
    unsigned char *p = (unsigned char *)(((unsigned long)fx + 15) & ~15ul);
    if (e && *e == '0') {
        rt_log("fpu: PC rounding (RT_FPU_PS2=0)");
        fpu_mode = fpu_mxcsr();
        return;
    }
    __asm__ volatile("fxsave %0" : "=m"(*(unsigned char (*)[512])p));
    memcpy(&mask, p + 28, 4);
    if (!mask)
        mask = 0xFFBF;
    m = fpu_mxcsr();
    m |= 0x6000u | 0x8000u;     /* RC = 11 (toward zero), FTZ */
    if (mask & 0x40u)
        m |= 0x40u;             /* DAZ */
    m &= mask;
    __asm__ volatile("ldmxcsr %0" : : "m"(m));
    fpu_mode = fpu_mxcsr();
    rt_log("fpu: PS2 rounding (toward zero, flush to zero%s), mxcsr %04X", mask & 0x40u ? ", denormals are zero" : "",
           fpu_mode);
#endif
}

/* at each quest start (rt_log.c): the game thread's MXCSR, and the mode set again if something changed it (a
 * library, a kernel that does not keep MXCSR across thread switches) */
void rt_fpu_check(void)
{
    unsigned int m = fpu_mxcsr();
    if ((m & ~0x3Fu) != (fpu_mode & ~0x3Fu)) {     /* bits 0-5: sticky exception flags, not the mode */
        rt_log("fpu: mxcsr %04X, expected %04X: set again", m, fpu_mode);
#if defined(__i386__) || defined(__x86_64__)
        { unsigned int w = fpu_mode; __asm__ volatile("ldmxcsr %0" : : "m"(w)); }
#endif
    } else
        rt_log("fpu: mxcsr %04X", m);
}

void rt_ref_init(void)
{
    const char *o = getenv("RT_REF_OUT"), *p = getenv("RT_REF_PAD"), *k = getenv("RT_REF_POKE");
    fpu_ps2();
    if (!o && !p)
        return;
    on = 1;
    {   /* the PS2's own layout: circle confirms (the settings menu's "Western" swap off) */
        extern int rt_pad_swap_confirm;
        rt_pad_swap_confirm = 0;
    }
    if (p)
        load_pads(p);
    if (k)
        load_pokes(k);
    if (getenv("RT_REF_RNDS")) {
        FILE *f = fopen(getenv("RT_REF_RNDS"), "r");
        int t, a, b, cap = 0;
        while (f && fscanf(f, "%d %d %d", &t, &a, &b) == 3) {
            if (t < 0)
                continue;
            while (t >= cap) {
                int nc = cap ? cap * 2 : 4096;
                rnds = realloc(rnds, sizeof *rnds * (size_t)nc);
                memset(rnds + cap, 0xFF, sizeof *rnds * (size_t)(nc - cap));
                cap = nc;
            }
            rnds[t][0] = (u16)a;
            rnds[t][1] = (u16)b;
            if (t >= nrnds)
                nrnds = t + 1;
        }
        if (f)
            fclose(f);
    }
    if (getenv("RT_REF_RNG") && (rng = fopen(getenv("RT_REF_RNG"), "w")) != NULL)
        rt_ref_rng_on = 1;
    if (o) {
        out = fopen(o, "wb");
        if (!out)
            rt_warn("referee: cannot write %s", o);
        else
            fprintf(out, "REF %s plw=0x%X em=%d:0x%X gw=0x%X qw=0x%X rnd=4 psw=0x24\n",
                    rt_ver_mhg() ? "mhg" : "mh1", PLW_SIZE, EM_N, EM_STRIDE, GW_SIZE, QW_SIZE);
    }
}

/* before the quest loads: the PS2's User_data */
void rt_ref_user_data(void)
{
    const char *u = getenv("RT_REF_UD");
    FILE *f;
    if (!u)
        return;
    f = fopen(u, "rb");
    if (!f) {
        rt_warn("referee: no User_data file %s", u);
        return;
    }
    if (fread(User_data, 1, UD_SIZE, f) != UD_SIZE)
        rt_warn("referee: short User_data file %s", u);
    fclose(f);
    rt_log("referee: User_data from %s", u);
}

static u8 *region(const char *r)
{
    if (!strcmp(r, "plw")) return player_work;
    if (!strcmp(r, "gw")) return game_w;
    if (!strcmp(r, "qw")) return quest_w;
    if (!strcmp(r, "ud")) return User_data;
    if (r[0] == 'e' && r[1] == 'm') return em_work + EM_STRIDE * atoi(r + 2);
    return NULL;
}

static void do_pokes(void)
{
    int i;
    for (i = 0; i < npokes; i++) {
        u8 *b;
        if (pokes[i].pt != ptick || !(b = region(pokes[i].reg)))
            continue;
        b += pokes[i].off;
        if (!strcmp(pokes[i].type, "u8")) *b = (u8)(int)pokes[i].v;
        else if (!strcmp(pokes[i].type, "u16")) *(u16 *)b = (u16)(int)pokes[i].v;
        else if (!strcmp(pokes[i].type, "u32")) *(u32 *)b = (u32)(long long)pokes[i].v;
        else if (!strcmp(pokes[i].type, "f32")) *(f32 *)b = (f32)pokes[i].v;
        rt_log("referee: play tick %d poke %s+0x%X %s %g", ptick, pokes[i].reg, pokes[i].off, pokes[i].type, pokes[i].v);
    }
}

/* at the start of each quest tick (before the game runs it) */
void rt_ref_tick(int raw)
{
    int play;
    if (!on)
        return;
    rawtick = raw;
    play = game_w[0] == 2 && game_w[1] <= 1;
    if (play && !started) {
        const char *r = getenv("RT_REF_RND");
        started = 1;
        ptick = 0;
        if (r) {
            unsigned a = 0, b = 0;
            sscanf(r, "%i,%i", &a, &b);
            Rnd_w[0] = (u16)a;
            Rnd_w[1] = (u16)b;
        }
        rt_log("referee: play tick 0 at tick %d (Rnd_w %04X %04X)", raw, Rnd_w[0], Rnd_w[1]);
    } else if (play && started) {
        ptick++;
    }
    if (!started)
        return;
    if (out) {
        u32 h[3];
        static const u8 z[4];
        memcpy(h, "REF1", 4);
        h[1] = play ? (u32)ptick : 0xFFFFFFFFu;
        h[2] = (u32)raw;
        fwrite(h, 4, 3, out);
        fwrite(player_work, 1, PLW_SIZE, out);
        fwrite(em_work, 1, (size_t)EM_STRIDE * EM_N, out);
        fwrite(game_w, 1, GW_SIZE, out);
        fwrite(quest_w, 1, QW_SIZE, out);
        fwrite(Rnd_w, 1, 4, out);
        fwrite(Psw, 1, 0x22, out);
        fwrite(z, 1, 2, out);
        fflush(out);
    }
    if (play && getenv("RT_REF_NODES") && getenv("RT_REF_NODES_OUT")) {
        /* RT_REF_NODES=SLOT:COUNT:FIRST-LAST (debug aid, tools/referee.py REF_NODES): the world matrices of monster
         * SLOT's first COUNT skeleton nodes at the start of these play ticks, as the PS2 side reads them */
        static FILE *nf;
        int sl, cnt, a, b, j, k;
        u8 *rt_actor_nodes(const void *work, int *max);
        if (sscanf(getenv("RT_REF_NODES"), "%d:%d:%d-%d", &sl, &cnt, &a, &b) == 4 && ptick >= a && ptick <= b && sl >= 0 &&
            sl < EM_N) {
            int max;
            u8 *nd = rt_actor_nodes(em_work + EM_STRIDE * sl, &max);
            if (!nf)
                nf = fopen(getenv("RT_REF_NODES_OUT"), "w");
            if (nf && nd) {
                fprintf(nf, "%d", ptick);
                for (j = 0; j < cnt && j < max; j++)
                    for (k = 0; k < 16; k++)
                        fprintf(nf, " %g", (double)((f32 *)(nd + 0x190 * j))[k]);
                fprintf(nf, "\n");
                fflush(nf);
            }
        }
    }
    if (play)
        do_pokes();     /* after the record, as the PS2 side writes them with the go */
    if (play && ptick < nrnds && !(rnds[ptick][0] == 0xFFFF && rnds[ptick][1] == 0xFFFF)) {
        Rnd_w[0] = rnds[ptick][0];
        Rnd_w[1] = rnds[ptick][1];
    }
}

/* this tick's pad (read_pad in viewer.c): 1 when the referee gives it */
int rt_ref_pad(pad_state *ps)
{
    if (!on || !pads)
        return 0;
    memset(ps, 0, sizeof *ps);
    if (started && game_w[0] == 2 && game_w[1] <= 1 && ptick < npads)
        *ps = pads[ptick];
    return 1;
}

int rt_ref_on(void) { return on; }
