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
#include "rt_prof.h"
#include "rt_pick.h"
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
#include <time.h>

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

typedef struct {                     /* model work (MDLW, get_mdlw_ptr; SET_MDLW in set*.c) */
    u8 flag;                         /* 0x00 loaded */
    u8 _pad01[0x2B];
    s16 nclay;                       /* 0x2C clay count (trans_stage) */
    u8 _pad2E[2];
    CLAY *clay;                      /* 0x30 */
} RT_SET_MDLW;
_Static_assert(offsetof(RT_SET_MDLW, clay) == 0x30, "SET_MDLW layout");

typedef struct {                     /* stage_work, 0x64 bytes (set14_nm.c STAGE_WORK,
                                      * trans_stage.c STAGE_W) */
    u8 flag;                         /* 0x00 trans_stage draws while flag and x01 are set */
    u8 x01;                          /* 0x01 */
    u8 stage;                        /* 0x02 stage_w_init: game_w.stage */
    u8 _pad03[5];
    s16 timer;                       /* 0x08 counts up every tick (UV scrolling) */
    u8 _pad0A[0x3C - 0x0A];
    RT_SET_MDLW *mdl;                /* 0x3C stage model set */
    u8 _pad40[8];
    void *data;                      /* 0x48 stage_w_init: Stage_data_get(stage) (floor heights) */
    u8 _pad4C[0x64 - 0x4C];
} RT_STAGE_WORK;
_Static_assert(sizeof(RT_STAGE_WORK) == 0x64 && offsetof(RT_STAGE_WORK, mdl) == 0x3C, "STAGE_WORK layout");

RT_STAGE_WORK stage_work;            /* 0x3D8230 */
RT_SET_MDLW *set_mdlw;               /* 0x38A184 */
static RT_SET_MDLW set_mdl, stage_mdl;
static CLAY stage_clay[64];
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

/* a model's clays bound again (stage change): the handles of the previous
 * model are reused, so handles kept by game C stay valid */
static int rebind_clay(int old, gfx_clay *c)
{
    if (old >= 0 && old < nclays) {
        clays[old] = c;
        claimed[old] = 0;
        return old;
    }
    return rt_register_clay(c);
}

unsigned char rt_stage_sky_flags[64];
int rt_stage_model_no;                  /* the stage the models being bound belong to (viewer.c load_stage_models) */     /* set by the viewer before rt_bind_stage_model: part k is a sky part */

extern int rt_stage_model_no;
int rt_bind_set_model(gfx_clay *const *c, const uint32_t *attr, int n)
{
    int i, old[64];
    for (i = 0; i < 64; i++)
        old[i] = set_mdl.flag && set_clay[i].handle >= 0 ? set_clay[i].handle : -1;
    memset(set_clay, 0, sizeof set_clay);
    for (i = 0; i < 64; i++) {
        set_clay[i].handle = i < n ? rebind_clay(old[i], c[i]) : -1;
        set_clay[i].attr = i < n && attr ? (s32)attr[i] : 0;
        if (i < n)
            pick_note_handle(set_clay[i].handle, PK_SET, rt_stage_model_no, i, 1);
    }
    set_mdl.flag = 1;
    set_mdl.nclay = (s16)(n < 64 ? n : 64);
    set_mdl.clay = set_clay;
    set_mdlw = &set_mdl;
    return set_clay[0].handle;
}

int rt_bind_stage_model(gfx_clay *const *c, const uint32_t *attr, int n)
{
    int i, old[64];
    for (i = 0; i < 64; i++)
        old[i] = stage_mdl.flag && stage_clay[i].handle >= 0 ? stage_clay[i].handle : -1;
    memset(stage_clay, 0, sizeof stage_clay);
    if (n > 64)
        n = 64;
    for (i = 0; i < 64; i++) {
        stage_clay[i].handle = i < n ? rebind_clay(old[i], c[i]) : -1;
        stage_clay[i].attr = i < n && attr ? (s32)attr[i] : 0;
        if (i < n)
            pick_note_handle(stage_clay[i].handle, rt_stage_sky_flags[i] ? PK_SKY : PK_STAGE, rt_stage_model_no, i, 0);
    }
    stage_mdl.flag = 1;
    stage_mdl.nclay = (s16)n;
    stage_mdl.clay = stage_clay;
    stage_work.mdl = &stage_mdl;     /* trans_stage draws it; set14 uses it on stages 0/0x1A */
    return stage_clay[0].handle;
}

/* ------------------------------------------------------------ random */
/* ran_suu (0x161230): per-channel Lehmer generator, x = x * 176 mod 65363, state Rnd_w[ch]
 * (the game's own variable: the co-op start seeds it); a zero state counts as 1. The PS2 seeds
 * it from the RTC (init_ran_suu); viewer.c seeds it from the clock unless a test script runs
 * (scripted runs stay deterministic, RT_SEED=n forces a seed). */
extern u16 Rnd_w[2];

u32 ran_suu(int ch)
{
    u32 x = Rnd_w[ch] ? Rnd_w[ch] : 1;
    Rnd_w[ch] = (u16)(x * 176 % 0xFF53);
    return Rnd_w[ch];
}

static unsigned rnd_seed;
static void rnd_apply(void)
{
    Rnd_w[0] = Rnd_w[1] = 0;
    if (rnd_seed) {
        unsigned m = rnd_seed * 2654435761u;      /* (small seeds like 1, 2, 3 would give near-identical Lehmer streams) */
        Rnd_w[0] = (u16)((m >> 8) % 0xFF52 + 1);
        Rnd_w[1] = (u16)((m >> 3) % 0xFF52 + 1);
    }
}

int rt_seed_get(void) { return (int)rnd_seed; }

void rt_seed_random(int scripted)
{
    const char *e = getenv("RT_SEED");
    if (e)
        rnd_seed = (unsigned)strtoul(e, NULL, 0);
    else if (!scripted)
        rnd_seed = (unsigned)time(NULL) * 2654435761u >> 8;
    rnd_apply();
}

/* init_ran_suu (sysw.c, WEAK): the game calls it at power-on; same seed again */
void init_ran_suu(void) { rnd_apply(); }

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
static unsigned char set_seen[SET_MAX];     /* RT_TRACE: reported once */

SETW *pull_set_work(int n)
{
    int i;
    for (i = 0; i < SET_MAX; i++)
        if (!set_used[i]) {
            memset(&set_pool[i], 0, sizeof set_pool[i]);
            set_used[i] = 1;
            set_seen[i] = 0;
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

/* clr_set_work (main, via all_reset -> clr_stg_work): every set object
 * gone (the PS2 zeroes set_work); the host frees its pool entries */
void clr_set_work(void)
{
    int i;
    for (i = 0; i < SET_MAX; i++)
        if (set_used[i])
            push_set_work(&set_pool[i].w);
}

/* se_req2 and the other sound calls: rt_snd.c */

/* ------------------------------------------------------------ prims */
/* get_prim hands out slots, add_prim queues one on an ordering table for
 * this tick; rt_game_draw walks ot0..ot4 in order. Priority order inside a
 * table (low first) is a guess. */
#define PRIM_MAX 512   /* the PS2 has 0x200 prim slots (prim2.c); 256 ran out in quests 160/167 (set14_m then wrote through NULL) */
#define OT_N 9
#define QUEUE_MAX 512
u8 ot0[0x20], ot1[0x20], ot2[0x20], ot3[0x20], ot4[0x20];
u8 ot5[0x20], ot6[0x20], ot7[0x20], ot8[0x20];   /* screen layers: HUD / menus (rt_game_draw_2d) */
static u8 *const ots[OT_N] = { ot0, ot1, ot2, ot3, ot4, ot5, ot6, ot7, ot8 };   /* ot4: set13 glare, drawn last (guess) */
static union { PRIM p; u8 raw[0x40]; } prim_pool[PRIM_MAX];
static unsigned char prim_used[PRIM_MAX];
static struct { PRIM *p; int pri; } queue[OT_N][QUEUE_MAX];
static int nqueue[OT_N];

static void *prim_caller[PRIM_MAX];
int get_prim(void);
static void prim_report(void)       /* RT_PRIM_TRACE=1: prims still held at exit, by the get_prim caller (leak hunting) */
{
    int i, n = 0;
    for (i = 0; i < PRIM_MAX; i++)
        if (prim_used[i]) {
            n++;
            fprintf(stderr, "rt_prim: slot %d held, allocated from get_prim%+ld, draw get_prim%+ld\n", i, (long)((char *)prim_caller[i] - (char *)get_prim),
                    (long)((char *)prim_pool[i].p.trans - (char *)get_prim));
        }
    fprintf(stderr, "rt_prim: %d of %d slots held\n", n, PRIM_MAX);
}

int get_prim(void)
{
    int i;
    static int reg;
    if (!reg++ && getenv("RT_PRIM_TRACE"))
        atexit(prim_report);
    for (i = 0; i < PRIM_MAX; i++)
        if (!prim_used[i]) {
            prim_used[i] = 1;
            prim_caller[i] = __builtin_return_address(0);
            memset(&prim_pool[i], 0, sizeof prim_pool[i]);
            return i;
        }
    return -1;
}

/* prim_init (0x169230, src/main/prim/prim_init.c): every prim slot free again. The PS2 calls it from all_reset and from
 * game2's stage change (step 2), and the sets/effects that held prims are rebuilt after it (stage_set_set, pl_init(1)); the
 * PC's pool is its own, so without this every stage change leaked the old stage's slots (set14 never releases its prim) and
 * a long hunt through 20+ areas ran the pool dry (quests 160/167 crashed in set14_m / enemy_mv at tick ~11000). */
void prim_init(void)
{
    memset(prim_used, 0, sizeof prim_used);
    memset(prim_pool, 0, sizeof prim_pool);
}

void release_prim(s16 no)
{
    if (no >= 0 && no < PRIM_MAX)
        prim_used[no] = 0;
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

/* ------------------------------------------------------------ players */
void rt_set_player(int no, const float pos[3])
{
    PLW *pl;
    if (no < 0 || no >= 8)
        return;
    pl = &player_work[no];
    pl->be_flag = 1;
    pl->id = (u16)no;
    pl->stg = game_w.stage;
    pl->pos[0] = pos[0];
    pl->pos[1] = pos[1];
    pl->pos[2] = pos[2];
    pl->scl[0] = pl->scl[1] = pl->scl[2] = 1.0f;   /* pl_init_sub */
    pl->work4D4 = 1;                                /* pl05.c: wall tests on */
    pl->x5AC = pos[1];
}

/* ------------------------------------------------------------ game loop */
void stage_set_set(int stage);
void rt_fl_reset_states(void);
void rt_eft_init(void);
void rt_eft_move(void);
void rt_eft_draw(void);
void rt_eft_trace(void);

void *Stage_data_get(int stg);

extern u8 quest_w[];
void rt_prims_reset(void);
void rt_game_init(int stage)
{
    /* a quest (rt_quest_load: Quest_start) has already set game_w up, as
     * game11 does before the stage is loaded */
    if (*(s16 *)(quest_w + 8) == 0)
        memset(&game_w, 0, sizeof game_w);
    game_w.stage = (u8)stage;
    game_w.master = 0;
    game_w.pl_num = 1;      /* one player, offline (monster sight/hate loops over pl_num) */
    stage_work.timer = 0;
    stage_work.flag = 1;
    stage_work.x01 = 1;
    stage_work.stage = (u8)stage;
    stage_work.data = Stage_data_get(stage);
    rt_prims_reset();       /* prims queued by the last tick belong to effects and
                             * set objects that are gone now (a windowed frame would
                             * draw them: eft13_t on a freed effect, village -> quest) */
    rt_eft_init();          /* init_eft_work / init_shell_work */
    stage_set_set(stage);   /* the game's own spawn list (src/main/stage/stage_set.c) */
    if (getenv("RT_SPOT_TRACE")) {  /* test aid: the stage's unique spots (3 = supply box, 6 = exit ...) */
        void *Stage_unique_data_get(int st);
        u8 *r = Stage_unique_data_get(stage);
        for (; r && *(float *)(r + 4) != -1.0f; r += 0x18)
            fprintf(stderr, "rt_game: stage %d spot kind %d at %.0f %.0f %.0f r %.0f ang %04X\n", stage, *(u16 *)(r + 2),
                    *(float *)(r + 4), *(float *)(r + 8), *(float *)(r + 0xC), *(float *)(r + 0x10), *(u16 *)(r + 0x14));
        {   /* gathering points (St_pick_ck's ST_ITEM list: x14 3 = mining, 4 = bugs) */
            void *Stage_item_data_get(int st);
            u8 *d = Stage_item_data_get(stage);
            u16 *Stage_item_probability_get(int);
            for (; d && *(float *)d != -1.0f; d += 0x18) {
                u16 *pr = Stage_item_probability_get(*(u16 *)(d + 0x10) & 0x7FFF);
                fprintf(stderr, "rt_game: stage %d pick id %d kind %d num %d at %.0f %.0f %.0f r %.0f items", stage,
                        *(u16 *)(d + 0x10), *(u16 *)(d + 0x14), *(u16 *)(d + 0x12), *(float *)d, *(float *)(d + 4),
                        *(float *)(d + 8), *(float *)(d + 0xC));
                for (; pr && *pr != 0xFFFF; pr += 2)
                    fprintf(stderr, " %d:%d%%", pr[1], pr[0]);
                fprintf(stderr, "\n");
            }
        }
        {   /* the exits (stage_mv_ck's STG_MV list, 0x34 bytes each) */
            void *Stage_mv_data_get(int st, int pl);
            u8 *m = Stage_mv_data_get(stage, 0);
            for (; m && *(u16 *)m != 0xFFFF; m += 0x34)
                fprintf(stderr, "rt_game: stage %d exit to %d kind %d at %.0f %.0f %.0f r %.0f h %.0f box %.0f %.0f %.0f\n", stage,
                        *(u16 *)m, *(s16 *)(m + 2), *(float *)(m + 4), *(float *)(m + 8), *(float *)(m + 0xC),
                        *(float *)(m + 0x10), *(float *)(m + 0x14), *(float *)(m + 0x18), *(float *)(m + 0x1C), *(float *)(m + 0x20));
        }
    }
}

void rt_font_tick_begin(void);
/* ot_init: the ordering tables are emptied at the start of every tick */
extern s16 spr_list_no;
extern u8 *sprite_area;
void rt_prims_reset(void)
{
    int i;
    for (i = 0; i < OT_N; i++)
        nqueue[i] = 0;
    spr_list_no = 0;        /* the Scheduler empties the sprite list every tick too */
    if (!sprite_area)       /* 0x100 entries of 0x50 (ioread: app_mem_top + 0x10D200) */
        sprite_area = calloc(0x101, 0x50);
}
void rt_game_move(void)
{
    int i;
    if (*(s16 *)(quest_w + 8) == 0)
        rt_font_tick_begin();   /* text printed by the previous tick is replaced (quests: rt_flow_tick) */
    for (i = 0; i < OT_N; i++)
        nqueue[i] = 0;
    stage_work.timer++;
    (*(u16 *)((u8 *)&game_w + 0x1E))++;     /* per-tick counter (0x10F060) */
    rt_prof_begin(RTP_SETS);
    for (i = 0; i < SET_MAX; i++)
        if (set_used[i] && set_pool[i].w.move) {
            if (!set_seen[i] && getenv("RT_TRACE")) {
                set_seen[i] = 1;
                fprintf(stderr, "rt: set object %d: type %d arg %d\n", i, set_pool[i].w.type, set_pool[i].w.arg);
            }
            set_pool[i].w.move(&set_pool[i].w);
        }
    rt_prof_end(RTP_SETS);
    rt_prof_begin(RTP_EFT_MOVE);
    rt_eft_move();          /* move_shell, move_eft (order after sets: a guess) */
    rt_prof_end(RTP_EFT_MOVE);
    {   /* move() (0x125xxx) then: move_item, move_senko, move_smoke */
        void move_senko(void), move_smoke(void);
        move_senko();
        move_smoke();
    }
}

/* the bug reporter's tag for a prim about to be drawn: its owner decides what it is (effect work, set object) */
extern u8 eft_work[];
static void pick_prim_tag(int ot, int k, PRIM *p)
{
    u8 *o = (u8 *)p->owner;
    float pos[3] = { p->pos[0], p->pos[1], p->pos[2] };
    int kind = PK_PRIM, w = k, ty = 0, ar = 0;
    if (o >= eft_work && o < eft_work + 128 * 0x40) {
        kind = PK_EFT;
        w = (int)(o - eft_work) / 0x40;
        ty = o[2];
        ar = o[3];
    } else if (o >= (u8 *)set_pool && o < (u8 *)(set_pool + SET_MAX)) {
        kind = PK_SET;
        w = (int)(o - (u8 *)set_pool) / (int)sizeof set_pool[0];
        ty = ((SETW *)o)->type;
        ar = ((SETW *)o)->arg;
    } else if (o) {
        kind = PK_SHELL;            /* shell works are neither: name the draw function */
        ty = o[2];
        ar = o[3];
    }
    PICK_FN(kind, w, ty, ar, ot, (const void *)p->trans, 0, pos);
}

void trans_stage(void);

void rt_stage_draw(void)
{
    PICK(PK_OTHER, 0, 0, 0, 0);
    trans_stage();
    rt_fl_reset_states();
}

void rt_game_draw(void)
{
    int t, k;
    static int traced;
    if (!traced && getenv("RT_TRACE")) {
        traced = 1;
        rt_eft_trace();
        for (t = 0; t < OT_N; t++)
            for (k = 0; k < nqueue[t]; k++) {
                SETW *o = (SETW *)queue[t][k].p->owner;
                if ((uintptr_t)o < 0x10000)     /* the lobby's player prims keep the player number there (Lb_set_player) */
                    o = NULL;
                fprintf(stderr, "rt: ot%d prim pri %d owner type %d arg %d pos %.0f,%.0f,%.0f\n", t, queue[t][k].pri,
                        o ? o->type : -1, o ? o->arg : -1, queue[t][k].p->pos[0], queue[t][k].p->pos[1], queue[t][k].p->pos[2]);
            }
    }
    rt_prof_begin(RTP_EFT_DRAW);
    rt_eft_draw();          /* trans_shell, trans_eft, trans_eft_up (before the prims: a guess) */
    rt_prof_end(RTP_EFT_DRAW);
    rt_fl_reset_states();
    for (t = 0; t < 5; t++)
        for (k = 0; k < (t == 2 ? 0 : nqueue[t]); k++) {   /* ot2: screen layer (rt_game_draw_2d) */
            PRIM *p = queue[t][k].p;
            static int skip = -2;
            if (skip == -2) skip = getenv("RT_SKIP_TYPE") ? atoi(getenv("RT_SKIP_TYPE")) : -1;
            if (skip >= 0 && p->owner && ((SETW *)p->owner)->type == skip)
                continue;           /* RT_SKIP_TYPE=n: debugging, skip prims of owner type n */
            {   /* A prim queued this tick whose effect work has been freed (or freed and handed out again: eft_take zeroes the
                 * work, so its owner is NULL) before the draw: the PS2 keeps the old bytes and draws a last stale frame, the PC
                 * would read through NULL (crash report 22:20 on stage 39: eft05_t, the weapon trail, pl = ew->owner = NULL). Cause was the stubbed
                 * clr_eft_work (rt_eft_clear_stage); this stays as a safety net. */
                extern u8 eft_work[];
                u8 *o = (u8 *)p->owner;
                if (o >= eft_work && o < eft_work + 128 * 0x40 && (!o[0] || (o[2] == 5 && !*(void **)(o + 0x34)))) {
                    static int warned;
                    if (!warned++)
                        fprintf(stderr, "rt_game_draw: skipped a prim whose effect work %d (type %d) is free or has no owner\n",
                                (int)(o - eft_work) / 0x40, o[2]);
                    continue;
                }
            }
            if (gfx_pick_pass)
                pick_prim_tag(t, k, p);
            if (p->trans)
                p->trans(p);
            rt_fl_reset_states();
        }
    PICK(PK_OTHER, 0, 0, 0, 0);
}

/* draw_prim (one ordering table now): the boot screens' trans() (rt_boot.c) */
void rt_draw_ot(int t)
{
    int k;
    if (t < 0 || t >= OT_N)
        return;
    for (k = 0; k < nqueue[t]; k++) {
        PRIM *p = queue[t][k].p;
        if (p->trans)
            p->trans(p);
        rt_fl_reset_states();
    }
}

/* add_prim2 (0x169710): queue on a multi-entry ordering table; entry idx
 * of n (drawn high idx first on the PS2: plplAdd(ot + n - 1 - idx)) */
int add_prim2(void *ot, PRIM *p, int idx, int n)
{
    if (!(idx < n && idx >= 0))
        return -1;
    add_prim(ot, p, n - 1 - idx, 0);
    return idx;
}

/* The screen layers in trans()'s order (main 0x163BC0): ot5, font stack
 * 0, ot6, stack 1, ot7, stack 2, ot8, stack 4, ot2, stack 3. */
void font_draw_stack_no(int n);
void rt_font_frame_begin(void);
void rt_font_frame_end(void);
void rt_game_draw_2d(void)
{
    static const int order[5] = { 5, 6, 7, 8, 2 }, fstack[5] = { 0, 1, 2, 4, 3 };
    int i, k;
    void InitRenderState(int soft);
    InitRenderState(1);     /* trans() ends with it: forgets the cached texture stage etc.
                             * (the host's 3D draws bound other textures since) */
    {   /* trans_sprite (sprite/trans2.c): SpritePut's list, before ot5 as in trans() */
        void trans_sprite(void);
        if (sprite_area && spr_list_no > 0) {
            PICK(PK_HUD, 100, 0, 0, 0);
            trans_sprite();
            rt_fl_reset_states();
        }
    }
    rt_font_frame_begin();
    for (i = 0; i < 5; i++) {
        int t = order[i];
        for (k = 0; k < nqueue[t]; k++) {
            PRIM *p = queue[t][k].p;
            PICK_FN(PK_HUD, t, k, queue[t][k].pri, 0, (const void *)p->trans, 0, 0);
            if (p->trans)
                p->trans(p);
            rt_fl_reset_states();
            if (gfx_2d_anchor)
                gfx_set_2d_anchor(GFX_A_CENTER);    /* the HUD prims (menu_nm.c) anchor their widgets; the rest stays 4:3 */
        }
        PICK(PK_TEXT, fstack[i], 0, 0, 0);
        font_draw_stack_no(fstack[i]);
        rt_fl_reset_states();
    }
    /* game3 / game4 / game5 call font_draw() after trans(): every stack
     * again, with what the prims printed meanwhile */
    if (game_w.mode >= 3 && game_w.mode <= 5)
        for (i = 0; i < 5; i++) {
            font_draw_stack_no(i);
            rt_fl_reset_states();
        }
    rt_font_frame_end();
    PICK(PK_OTHER, 0, 0, 0, 0);
}
