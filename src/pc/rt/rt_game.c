/*
 * rt_game.c - game-side globals and services for the port runtime: the
 * game work areas (game_w, player_work, stage_work, set_mdlw), the set
 * object pool (pull/push_set_work), draw prims and ordering tables
 * (get_prim, add_prim, ot0..ot3) and the random number generator.
 *
 * These are native re-implementations of what the PS2 main program does,
 * written from the asm's behaviour; sizes and offsets follow the game's
 * include/ headers and are checked with static asserts so that a 32-bit
 * build lays the structs out like the PS2 (docs/pc.md "Port runtime").
 * What is a guess is marked as such.
 */
#include "rt.h"
#include "types.h"
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The game C stores pointers in u32 fields: only a 32-bit build works. */
_Static_assert(sizeof(void *) == 4, "build the port runtime and game C with -m32");
_Static_assert(sizeof(GAME_W) == 0x224, "GAME_W size");
_Static_assert(sizeof(PLW) == 0xA00, "PLW size");
_Static_assert(offsetof(SETW, move) == 0x20 && offsetof(SETW, prim) == 0x38, "SETW layout");
_Static_assert(offsetof(PRIM, trans) == 0x14 && offsetof(PRIM, owner) == 0x18, "PRIM layout");
_Static_assert(sizeof(CLAY) == 0x8C, "CLAY size");

/* ------------------------------------------------------------ work areas */
GAME_W game_w;                       /* 0x3F33F0 */
PLW player_work[8];                  /* 0x3E4BF0, 0x5000 bytes */

typedef struct {                     /* set model work (file-local SET_MDLW in set*.c) */
    u8 flag;                         /* 0x00 loaded */
    u8 _pad01[0x2F];
    CLAY *clay;                      /* 0x30 */
} RT_SET_MDLW;
_Static_assert(offsetof(RT_SET_MDLW, clay) == 0x30, "SET_MDLW layout");

typedef struct {                     /* stage_work, 0x64 bytes (set14_nm.c STAGE_WORK) */
    u8 _pad00[8];
    s16 timer;                       /* 0x08 counts up every tick (UV scrolling) */
    u8 _pad0A[0x3C - 0x0A];
    RT_SET_MDLW *mdl;                /* 0x3C stage model set */
    u8 _pad40[0x64 - 0x40];
} RT_STAGE_WORK;
_Static_assert(sizeof(RT_STAGE_WORK) == 0x64 && offsetof(RT_STAGE_WORK, mdl) == 0x3C, "STAGE_WORK layout");

RT_STAGE_WORK stage_work;            /* 0x3D8230 */
RT_SET_MDLW *set_mdlw;               /* 0x38A184 */
static RT_SET_MDLW set_mdl;
static CLAY set_clay[64];

/* ------------------------------------------------------------ clays */
#define MAX_CLAYS 1024
static gfx_clay *clays[MAX_CLAYS];
static unsigned char claimed[MAX_CLAYS];
static int nclays;

int rt_register_clay(gfx_clay *c)
{
    if (nclays >= MAX_CLAYS)
        return -1;
    clays[nclays] = c;
    return nclays++;
}

gfx_clay *rt_clay(int handle)
{
    if (handle < 0 || handle >= nclays)
        return NULL;
    claimed[handle] = 1;
    return clays[handle];
}

int rt_clay_claimed(int handle)
{
    return handle >= 0 && handle < nclays && claimed[handle];
}

int rt_bind_set_model(gfx_clay *const *c, const uint32_t *attr, int n)
{
    int i;
    memset(set_clay, 0, sizeof set_clay);
    for (i = 0; i < 64; i++) {
        set_clay[i].handle = i < n ? rt_register_clay(c[i]) : -1;
        set_clay[i].attr = i < n && attr ? (s32)attr[i] : 0;
    }
    set_mdl.flag = 1;
    set_mdl.clay = set_clay;
    set_mdlw = &set_mdl;
    stage_work.mdl = &set_mdl;
    return set_clay[0].handle;
}

/* ------------------------------------------------------------ random */
/* ran_suu (0x161230): per-channel Lehmer generator, x = x * 176 mod 65363,
 * seeded from the RTC (init_ran_suu); a zero state counts as 1. */
static u16 rnd_w[2] = { 1, 1 };

u32 ran_suu(int ch)
{
    u32 x = rnd_w[ch] ? rnd_w[ch] : 1;
    rnd_w[ch] = (u16)(x * 176 % 0xFF53);
    return rnd_w[ch];
}

/* ------------------------------------------------------------ set objects */
/* set_work (0x396A00) is 0x2000 bytes; 64 entries of 0x80 is a guess.
 * pull_set_work(n) (0x155290) also hands out n 512-byte blocks of the work
 * heap as sw->u.work (-1 when n <= 0) and notes them at +0x1C (first
 * block) / +0x1D (count); here each entry gets its own zeroed buffer. */
#define SET_MAX 64
#define SET_HEAP_BLOCK 0x200
static union { SETW w; u8 raw[0x80]; } set_pool[SET_MAX];
static unsigned char set_used[SET_MAX];
static void *set_heap[SET_MAX];

SETW *pull_set_work(int n)
{
    int i;
    for (i = 0; i < SET_MAX; i++)
        if (!set_used[i]) {
            memset(&set_pool[i], 0, sizeof set_pool[i]);
            set_used[i] = 1;
            set_pool[i].raw[0] = 1;
            if (n > 0) {
                set_heap[i] = calloc((size_t)n, SET_HEAP_BLOCK);
                set_pool[i].w.u.work = set_heap[i];
            } else {
                set_pool[i].w.u.work = (void *)-1;
            }
            set_pool[i].raw[0x1D] = (u8)n;
            return &set_pool[i].w;
        }
    return NULL;
}

void push_set_work(SETW *sw)
{
    int i = (int)((u8 *)sw - (u8 *)set_pool) / (int)sizeof set_pool[0];
    if (i >= 0 && i < SET_MAX) {
        set_used[i] = 0;
        free(set_heap[i]);
        set_heap[i] = NULL;
    }
}

void se_req2(int a, int b, int c, f32 *pos, int d, int e)
{
    (void)a; (void)b; (void)c; (void)pos; (void)d; (void)e;   /* no sound yet */
}

/* ------------------------------------------------------------ prims */
/* get_prim hands out slots, add_prim queues one on an ordering table for
 * this tick; rt_game_draw walks ot0..ot3 in order. Priority order inside a
 * table (low first) is a guess. */
#define PRIM_MAX 256
#define OT_N 4
#define QUEUE_MAX 512
u8 ot0[0x20], ot1[0x20], ot2[0x20], ot3[0x20];
static u8 *const ots[OT_N] = { ot0, ot1, ot2, ot3 };
static union { PRIM p; u8 raw[0x40]; } prim_pool[PRIM_MAX];
static unsigned char prim_used[PRIM_MAX];
static struct { PRIM *p; int pri; } queue[OT_N][QUEUE_MAX];
static int nqueue[OT_N];

int get_prim(void)
{
    int i;
    for (i = 0; i < PRIM_MAX; i++)
        if (!prim_used[i]) {
            prim_used[i] = 1;
            memset(&prim_pool[i], 0, sizeof prim_pool[i]);
            return i;
        }
    return -1;
}

PRIM *get_prim_ptr(s16 no)
{
    return no >= 0 && no < PRIM_MAX ? &prim_pool[no].p : NULL;
}

void add_prim(void *ot, PRIM *p, int pri, int kind)
{
    int t, k;
    (void)kind;
    for (t = 0; t < OT_N && ots[t] != ot; t++)
        ;
    if (t == OT_N || nqueue[t] >= QUEUE_MAX)
        return;
    for (k = nqueue[t]; k > 0 && queue[t][k - 1].pri > pri; k--)
        queue[t][k] = queue[t][k - 1];
    queue[t][k].p = p;
    queue[t][k].pri = pri;
    nqueue[t]++;
}

/* ------------------------------------------------------------ game loop */
void stage_set_set(int stage);
void rt_fl_reset_states(void);

void rt_game_init(int stage)
{
    memset(&game_w, 0, sizeof game_w);
    game_w.stage = (u8)stage;
    game_w.master = 0;
    stage_work.timer = 0;
    stage_set_set(stage);   /* the game's own spawn list (src/main/stage/stage_set.c) */
}

void rt_game_move(void)
{
    int i;
    for (i = 0; i < OT_N; i++)
        nqueue[i] = 0;
    stage_work.timer++;
    for (i = 0; i < SET_MAX; i++)
        if (set_used[i] && set_pool[i].w.move)
            set_pool[i].w.move(&set_pool[i].w);
}

void rt_game_draw(void)
{
    int t, k;
    for (t = 0; t < OT_N; t++)
        for (k = 0; k < nqueue[t]; k++) {
            PRIM *p = queue[t][k].p;
            if (p->trans)
                p->trans(p);
            rt_fl_reset_states();
        }
}
