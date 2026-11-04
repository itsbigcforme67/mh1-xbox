/*
 * rt_motion.c - the fl motion layer under the decompiled motion system
 * (src/main/frame/f_frame_nm.c: create_*_motion, frame_init, frame_move,
 * frame_check*), written natively on top of the port's AAN reader.
 *
 * On the PS2 (main 0x173A50-0x1746A0, read from the asm):
 * - a motion-set handle is a system-memory copy of the set built by
 *   plCreateMotionSetFromAAN; flGetMotionSetTime reads its end frame
 *   (+0x08), flGetMotionSetLoopInfo its loop flag (+0x00 & 0x8000) and loop
 *   start (+0x0C).
 * - the model's motion players (model work +0x44 and +0x54) are trees of
 *   0x160-byte bone nodes (+0xC6 group, +0xC8 parent, +0xCC sibling, +0xD0
 *   child, +0xD4 motion-set handle, +0xD8 curve data, +0x104 S/R/T).
 *   flSetMotionEx gives the nodes of one group a motion set,
 *   flPlayMotionExSI poses them at a frame, flBlendMotionEx mixes the
 *   second tree into the first (flmatBlend per node), flCalcTransVelocity
 *   returns how far the root's child node (passed as node+0xD0) moves
 *   between two frames: the root motion frame_move adds to the actor.
 *
 * Here a motion player is an RT_MPLAY: the same "+0xD0" word (pointing to
 * the player itself, so flCalcTransVelocity finds it) followed by the host
 * state: per group the motion set and frame, and the blend. The host draw
 * poses an fl_skel from it with rt_motion_pose().
 */
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "frame.h"
#include "em.h"
#include "../fl/fl.h"
#include "rt_prof.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------ motion sets */
typedef struct {
    aan_motion m;
    int used;                   /* 0: freed (rt_motion_gc), the slot is free */
} rt_mset;

static rt_mset *msets;
static int nmsets, capmsets;

static const aan_motion *mset_get(u32 h)
{
    if (h == 0 || (int)h > nmsets || !msets[h - 1].used)
        return NULL;
    return &msets[h - 1].m;
}

/* flGetFrame / flReleaseFrame bracket a temporary allocation on the PS2. */
void flGetFrame(void *frame) { (void)frame; }
void flReleaseFrame(void *frame) { (void)frame; }

/* Remember which AAN the set is built from (tmp is the caller's buffer). */
void plCreateMotionSetFromAAN(void *tmp, u8 *base, u8 *aan)
{
    (void)base;
    memcpy(tmp, &aan, sizeof aan);
}

static u32 flCreateMotionSetHandle_lt(void *tmp)
{
    u8 *aan;
    fmt_blob b;
    int h;
    memcpy(&aan, tmp, sizeof aan);
    if (!aan)
        return 0;
    for (h = 0; h < nmsets && msets[h].used; h++)     /* a released slot first */
        ;
    if (h == capmsets) {
        capmsets = capmsets ? capmsets * 2 : 256;
        msets = realloc(msets, capmsets * sizeof *msets);
    }
    b.p = aan;
    b.n = fmt_u32(aan + 8, FMT_LE);
    if (fmt_aan_load(&msets[h].m, b, FMT_LE) != 0) {
        fprintf(stderr, "rt: motion set at %p does not parse\n", (void *)aan);
        memset(&msets[h].m, 0, sizeof msets[h].m);
        return 0;
    }
    msets[h].used = 1;
    if (h == nmsets)
        nmsets++;
    return (u32)h + 1;
}

u32 flCreateMotionSetHandle(void *tmp)
{
    u32 r;
    rt_lt_begin(RTL_MOTION);
    r = flCreateMotionSetHandle_lt(tmp);
    rt_lt_end(RTL_MOTION);
    return r;
}

f32 flGetMotionSetTime(u32 h)
{
    const aan_motion *m = mset_get(h);
    return m ? m->end : 0.0f;
}

s32 flGetMotionSetLoopInfo(u32 h, f32 *loop_start)
{
    const aan_motion *m = mset_get(h);
    if (m && m->loop) {
        *loop_start = m->loop_start;
        return 1;
    }
    *loop_start = 0.0f;
    return 0;
}

/* a set's curves freed, its slot free for the next flCreateMotionSetHandle (rt_motion_gc).
 * Not flReleaseMotionSetHandle: the game's all_motion_free (character screen) would then
 * free the player's weapon motions, which the host loads once (rt_motion_load_pl) and the
 * game does not create again (the quest loop's carving broke). */
static void mset_free(int h)
{
    if (h <= 0 || h > nmsets || !msets[h - 1].used)
        return;
    fmt_aan_free(&msets[h - 1].m);
    msets[h - 1].used = 0;
}

/* 1 when a live motion set reads its keys from [p, p + n) (a motion file: the sets point into it,
 * so the file can go only after rt_motion_gc freed them all) */
int rt_motion_refs(const void *p, size_t n)
{
    const uint8_t *lo = p, *hi = lo + n;
    int h, b, c;
    for (h = 0; h < nmsets; h++) {
        const aan_motion *m = &msets[h].m;
        if (!msets[h].used)
            continue;
        for (b = 0; b < m->nbone; b++)
            for (c = 0; c < m->ncurve[b]; c++)
                if (m->curve[b][c].keys) {
                    if (m->curve[b][c].keys >= lo && m->curve[b][c].keys < hi)
                        return 1;
                    goto next;          /* all keys of a set come from one file */
                }
    next:;
    }
    return 0;
}

/* ------------------------------------------------------------ players */
typedef struct RT_MPLAY {
    u8 _pad00[0xD0];
    s32 xD0;                /* 0xD0 the game passes this to flCalcTransVelocity */
    u32 set[4];             /* motion set per group */
    f32 t[4];               /* frame per group */
    int blend[4];           /* 1: mixed with bset/bt */
    u32 bset[4];
    f32 bt[4], wa[4], wb[4];
} RT_MPLAY;

void flSetMotionEx(FRMOT *m, u32 h, u16 n)
{
    RT_MPLAY *mp = (RT_MPLAY *)m;
    if (n < 4)
        mp->set[n] = h;
}

void flPlayMotionExSI(f32 frame, FRMOT *m, u16 n)
{
    RT_MPLAY *mp = (RT_MPLAY *)m;
    if (n >= 4)
        return;
    mp->t[n] = frame;
    mp->blend[n] = 0;
}

void flBlendMotionEx(FRMOT *ma, FRMOT *mb, u16 n, f32 wa, f32 wb)
{
    RT_MPLAY *a = (RT_MPLAY *)ma, *b = (RT_MPLAY *)mb;
    if (n >= 4)
        return;
    a->blend[n] = 1;
    a->bset[n] = b->set[n];
    a->bt[n] = b->t[n];
    a->wa[n] = wa;
    a->wb[n] = wb;
}

/* Translation of the root's child bone (AAN bone 1 of group 0) at frame b
 * minus at frame a. Channels the motion does not animate stay 0. */
void flCalcTransVelocity(f32 a, f32 b, f32 *out, s32 node)
{
    RT_MPLAY *mp = (RT_MPLAY *)(intptr_t)node;
    const aan_motion *m = mp ? mset_get(mp->set[0]) : NULL;
    float ca[9] = { 0 }, cb[9] = { 0 };
    if (!m || m->nbone < 2) {
        out[0] = out[1] = out[2] = 0.0f;
        return;
    }
    fmt_aan_eval(m, 1, a, ca);
    fmt_aan_eval(m, 1, b, cb);
    out[0] = cb[6] - ca[6];
    out[1] = cb[7] - ca[7];
    out[2] = cb[8] - ca[8];
}

/* plFCVFcurveInterpolateHermite (0x192E40): cubic Hermite between
 * (t0, v0, slope s0) and (t1, v1, slope s1), transcribed from the asm. */
f32 plFCVFcurveInterpolateHermite(f32 t, f32 v0, f32 t0, f32 s0, f32 v1, f32 t1, f32 s1)
{
    f32 d = t - t0, r = 1.0f / (t1 - t0);
    f32 d2 = d * d, r2 = r * r;
    f32 h3 = 3.0f * d2 * r2;            /* 3 u^2 */
    f32 q = d2 * r;                     /* d^2 / dt */
    f32 c = r2 * (d2 * d);              /* d^3 / dt^2 */
    f32 h2 = 2.0f * c * r;              /* 2 u^3 */
    f32 e = c - q;
    f32 hv0 = 1.0f + (h2 - h3);
    f32 hs0 = d + (e - q);
    f32 hv1 = -h2 + h3;
    return v0 * hv0 + v1 * hv1 + s0 * hs0 + s1 * e;
}

/* cpApplyMatrix (0x120370): out = v * m (3x3); returns out on the PS2. */
void flvecApplyMat33(f32 *out, f32 *v, FLMAT *m);

void system_error(char *fmt, int a, int b, int c)
{
    fprintf(stderr, "system_error: ");
    fprintf(stderr, fmt, a, b, c);
    fprintf(stderr, "\n");
}

/* ------------------------------------------------------------ host side */
/* main's pointers to the loaded player-area files (pl_area_top: the motion
 * table create_*_motion reads) */
u8 *pl_area_top;
u8 *data_load_ptr;
void create_plcom_motion(void);

#define RT_NODE_SIZE 0x190
#define RT_NODE_MAX 128
typedef struct {
    FRMDL mdl;
    u8 _pad[0x60 - sizeof(FRMDL)];
    RT_MPLAY mot0, mot1;
    /* mdl+0x24: the skeleton's nodes, RT_NODE_SIZE bytes each as on the PS2:
     * +0x00 world matrix (filled from the host skeleton each tick by
     * rt_actor_joints: hit_data_expand reads it), +0x40 local matrix (rows
     * turned by get_joint_mat callers; row 3 of node 1 is FRSKL.vel) */
    u8 skl[RT_NODE_MAX * RT_NODE_SIZE];
} rt_actor_motion;

/* A model's part of motion_set_handle_tbl emptied before create_*_motion fills it again: the
 * new file may hold fewer motions than the last one (the quest's plcom and the village's
 * lbcom, a Rathian's slot taken by an Aptonoth), and the entries past its end would keep the
 * old sets reachable for rt_motion_gc, and their files loaded. On the PS2 those old sets
 * were released before (all_motion_free, em_motion_free: stand-ins here). Regions: common
 * 0..499, player n 500 + 300n, monster model slot n 1700 + 600n (create_*_motion). */
#ifdef VERSION_MH2
/* MH2 moved the regions (create_pl_motion / create_em_motion in f_frame.c: MOT_PL 710, MOT_EM 1910; MH2's frame_init
 * sub_main 0x2E5190 adds 710 and 1910): common 0..709, player n 710 + 300n, monster slot n 1910 + 600n (4310 entries,
 * motion_set_handle_tbl 0x5DA080 up to em_mot_han_ofs 0x5DE3E0; config/mh2/pc_data.txt sizes the host array) */
#define MOT_COM_N 710
#define MOT_PL 710
#define MOT_EM 1910
#else
#define MOT_COM_N 500
#define MOT_PL 500
#define MOT_EM 1700
#endif
static void tbl_clear(int first, int n)
{
    memset(&motion_set_handle_tbl[first], 0, (size_t)n * sizeof motion_set_handle_tbl[0]);
}

/* the monster and NPC model slots' motions released: the village's NPCs when a quest starts,
 * the quest's monsters when the village begins (the game creates its own again) */
void rt_motion_em_release(void)
{
    tbl_clear(MOT_EM, 4 * 600);
}

void rt_motion_load_plcom(const uint8_t *tbl)
{
    pl_area_top = (u8 *)tbl;
    tbl_clear(0, MOT_COM_N);
    create_plcom_motion();
}

/* the player's own motions (ids >= 1000) through create_pl_motion */
void create_pl_motion(int pl);
void rt_motion_load_pl(int no, const uint8_t *tbl)
{
    pl_area_top = (u8 *)tbl;
    if (no >= 0 && no < 4)
        tbl_clear(MOT_PL + 300 * no, 300);
    create_pl_motion(no);
}

/* Model works are kept per em_work slot: clr_em_work / the village entry clear the whole monster work (model pointer
 * included) and em_work_set / rt_actor_nodes_fill then attach a new one; the slot's old one is reused (cleared, as a
 * new one would be) instead of allocating another ~52 KB each time. */
static rt_actor_motion *em_am[20];
static int am_allocs, am_reuses;
void *rt_motion_attach(void *work)
{
    int slot = (int)(((u8 *)work - (u8 *)em_work) / (int)sizeof(EMW));
    int is_em = (u8 *)work >= (u8 *)em_work && slot < 20 && (u8 *)work == (u8 *)&em_work[slot];
    rt_actor_motion *am = is_em ? em_am[slot] : NULL;
    FRW *w = work;
    if (am) {
        memset(am, 0, sizeof *am);
        am_reuses++;
    } else {
        am = calloc(1, sizeof *am);
        am_allocs++;
        if (is_em)
            em_am[slot] = am;
    }
    am->mdl.skl = (FRSKL *)am->skl;
    am->mdl.mot0 = (FRMOT *)&am->mot0;
    am->mdl.mot1 = (FRMOT *)&am->mot1;
    am->mot0.xD0 = (s32)(intptr_t)&am->mot0;
    am->mot1.xD0 = (s32)(intptr_t)&am->mot1;
    w->mdl = &am->mdl;
    return am;
}

void rt_motion_report(void)        /* RT_MEM */
{
    fprintf(stderr, "memstat: host model works: %d allocated, %d reused (%u bytes each)\n", am_allocs, am_reuses,
            (unsigned)sizeof(rt_actor_motion));
}

void rt_motion_pose(fl_skel *s, const void *work)
{
    const FRW *w = work;
    const RT_MPLAY *mp;
    fl_group_pose g[FL_MAX_GROUPS];
    int i;
    memset(g, 0, sizeof g);
    if (!w->mdl)
        return;
    mp = (const RT_MPLAY *)w->mdl->mot0;
    for (i = 0; i < FL_MAX_GROUPS && i < 4; i++) {
        g[i].m = mset_get(mp->set[i]);
        g[i].t = mp->t[i];
        if (mp->blend[i]) {
            g[i].m2 = mset_get(mp->bset[i]);
            g[i].t2 = mp->bt[i];
            g[i].wa = mp->wa[i];
            g[i].wb = mp->wb[i];
        }
    }
    fl_skel_pose_groups(s, g);
}

/* ------------------------------------------------------------ players */
extern PLW player_work[];
void rt_motion_scan(void);
static int plcom_loaded;

void pl_create_model(int no);
void rt_player_motion_start(int no, const uint8_t *plcom_tbl, int legs_id, int upper_id)
{
    PLW *pl = &player_work[no];
    FRW *w = (FRW *)pl;
    if (!plcom_loaded) {
        rt_motion_load_plcom(plcom_tbl);
        plcom_loaded = 1;
        if (getenv("RT_MOTION_SCAN"))
            rt_motion_scan();
    }
    if (!w->mdl)
        pl_create_model(no);
    pl->scl[0] = pl->scl[1] = pl->scl[2] = 1.0f;
    w->layers = 2;
    w->chr[0] = (u16)legs_id;
    w->chr[1] = (u16)upper_id;
    w->mt[0].spd = 1.0f;
    w->mt[1].spd = 1.0f;
    frame_init(w, 0, 0, 0);
    frame_init(w, 0, 0, 1);
}

/* pl_create_model (main f_model): on the PS2 it builds player no's model
 * work (MDLW: skeleton nodes, the two motion players) and points PLW+0x50C
 * at it; get_mdlw_ptr(index) gives a model work by its pool index (the
 * character screen sets PLW+0x50C from it right after). The PC keeps one
 * host model work per player (the hunter's parts are drawn by the host),
 * so a cleared player work (clr_pl_work) gets the same one back. */
static rt_actor_motion *pl_am[8];
static int pl_am_last = -1;
void pl_create_model(int no)
{
    FRW *w;
    if (no < 0 || no >= 8)
        return;
    w = (FRW *)&player_work[no];
    if (!pl_am[no])
        pl_am[no] = rt_motion_attach(w);
    else
        w->mdl = &pl_am[no]->mdl;
    pl_am_last = no;
}
void *get_mdlw_ptr(int idx)
{
    (void)idx;
    return pl_am_last >= 0 && pl_am[pl_am_last] ? &pl_am[pl_am_last]->mdl : NULL;
}

/* One tick of the game's motion player (frame_move) for player no. */
int rt_player_motion_tick(int no)
{
    return frame_move((FRW *)&player_work[no]);
}

void rt_player_pose(int no, void *skel)
{
    rt_motion_pose(skel, &player_work[no]);
}

/* the PLW of player no (the host's lighting reads its colour override and near-monster flag) */
void *rt_player_ptr(int no) { return &player_work[no]; }

/* Position and Y angle (0x10000 = 360 degrees) of player no. */
void rt_player_get(int no, float pos[3], int *ang_y)
{
    PLW *pl = &player_work[no];
    pos[0] = pl->pos[0];
    pos[1] = pl->pos[1];
    pos[2] = pl->pos[2];
    *ang_y = pl->ang[1];
}

/* RT_MOTION_SCAN=1: list the common motions (bank, slot, frames, loop and
 * how far bone 1 moves over the motion: root motion) to find walk/run. */
void rt_motion_scan(void)
{
    extern s32 com_mot_han_ofs[];
    int bank, slot;
    for (bank = 0; bank < 10; bank += 2) {
        int first = com_mot_han_ofs[bank], last = bank < 9 ? com_mot_han_ofs[bank + 1] : first;
        for (slot = 0; slot < last - first && slot < 100; slot++) {
            u32 h = motion_set_handle_tbl[first + slot];
            const aan_motion *m = mset_get(h);
            float c0[9] = { 0 }, c1[9] = { 0 };
            if (!m)
                continue;
            if (m->nbone > 1) {
                fmt_aan_eval(m, 1, 0, c0);
                fmt_aan_eval(m, 1, m->end, c1);
            }
            printf("plcom id %d: end %.0f loop %d(%.0f) bones %d d=(%.1f %.1f %.1f)\n",
                   bank * 100 + slot, m->end, m->loop, m->loop_start, m->nbone,
                   c1[6] - c0[6], c1[7] - c0[7], c1[8] - c0[8]);
        }
    }
}

/* ------------------------------------------------------------ monsters */
#include "em.h"
void create_em_motion(int no, int em);

/* em_work[no] played by the game's motion code: builds monster model
 * mdl_no's handles from its *_tbl.bin (create_em_motion, kind em: number
 * of part groups from em_parts_num) and starts ids[g] on layer g. */
void rt_em_motion_create(int slot, int kind, const uint8_t *tbl)
{
    pl_area_top = (u8 *)tbl;
    if (slot >= 0 && slot < 4)
        tbl_clear(MOT_EM + 600 * slot, 600);
    create_em_motion(slot, kind);
}

void rt_monster_motion_start(int no, int mdl_no, const uint8_t *tbl, int kind, const int *ids, int layers)
{
    EMW *em = &em_work[no];
    FRW *w = (FRW *)em;
    int g;
    rt_em_motion_create(mdl_no, kind, tbl);
    if (!w->mdl)
        rt_motion_attach(em);
    w->x10 = 1;                 /* not a player: em_mot_han_ofs path */
    w->x1E = 0;
    w->mdl_no = (u8)mdl_no;
    w->scl[0] = w->scl[1] = w->scl[2] = 1.0f;
    w->layers = (u16)layers;
    if (getenv("RT_MOTION_SCAN")) {     /* bank 0 (body) motions of this model */
        int first = em_mot_han_ofs[mdl_no][0], last = em_mot_han_ofs[mdl_no][1], slot;
        for (slot = 0; slot < last - first && slot < 100; slot++) {
            const aan_motion *m = mset_get(motion_set_handle_tbl[mdl_no * 600 + MOT_EM + first + slot]);
            float c0[9] = { 0 }, c1[9] = { 0 };
            if (!m)
                continue;
            if (m->nbone > 1) {
                fmt_aan_eval(m, 1, 0, c0);
                fmt_aan_eval(m, 1, m->end, c1);
            }
            printf("em mdl %d id %d: end %.0f loop %d(%.0f) bones %d d=(%.1f %.1f %.1f)\n",
                   mdl_no, 1000 + slot, m->end, m->loop, m->loop_start, m->nbone,
                   c1[6] - c0[6], c1[7] - c0[7], c1[8] - c0[8]);
        }
    }
    for (g = 0; g < layers && g < 4; g++) {
        w->chr[g] = (u16)ids[g];
        w->mt[g].spd = 1.0f;
        frame_init(w, 0, 0, g);
    }
}

#ifdef VERSION_MH2
/* frame_init / frame_init_b on MH2 (sub_main 0x2E5190 / 0x2E5520, not carried): src/main/frame/f_frameb.c's with MH2's
 * regions (MOT_PL / MOT_EM above). f_frameb.c's copies index MH1's 500 / 1700 and are renamed away for MH2
 * (tools/build_pc.sh): a monster read empty handles (its motion layers had end 0 and never moved, docs/mh2/host.md Next 1)
 * and the hunter read the common motions' tail as its own. Host C, not byte-checked: the shared fix is two constants. */
static u32 *mh2_han(FRW *w, u16 id)
{
    int no = id % 1000;
    u32 *han;
    if (w->x10 == 0 || (w->x1E != 0 && w->x1F != 0)) {
        if (id >= 1000)
            han = &motion_set_handle_tbl[w->id * 300 + MOT_PL] + pl_mot_han_ofs[w->id][no / 100];
        else
            han = motion_set_handle_tbl + com_mot_han_ofs[no / 100];
    } else if (id >= 1000) {
        han = &motion_set_handle_tbl[w->mdl_no * 600 + MOT_EM] + em_mot_han_ofs[w->mdl_no][no / 100];
    } else {
        system_error(lit_277_003584C0, (s16)id, 0, 0);
        return NULL;
    }
    return han + no % 100;      /* (f_frameb.c's two forms of the step are the same) */
}

void frame_init(FRW *w, int frame, int blend, int n)
{
    FRMDL *mdl = w->mdl;
    u32 *han = mh2_han(w, w->chr[n]);
    if (!han)
        return;
    if (blend != 0) {
        if (blend < 0) {
            blend = -blend;
            w->mt[n].b_dir = -1;
        } else {
            w->mt[n].b_dir = 1;
        }
        flSetMotionEx(mdl->mot1, *han, n);
        w->mt[n].b_end = flGetMotionSetTime(*han);
        w->mt[n].b_loop = flGetMotionSetLoopInfo(*han, &w->mt[n].b_loopfr);
        w->mt[n].b_frame = frame;
        w->mt[n].b_cnt = blend + 1;
        w->mt[n].b_rate = w->mt[n].b_step = 1.0f / (blend + 1);
        w->mt[n].b_han = han;
    } else {
        flSetMotionEx(mdl->mot0, *han, n);
        w->mt[n].end = flGetMotionSetTime(*han);
        w->mt[n].loop = flGetMotionSetLoopInfo(*han, &w->mt[n].loopfr);
        w->mt[n].frame = frame;
        w->mt[n].b_cnt = 0;
        w->mt[n].b_dir = 0;
        w->mt[n].b_han = han;
    }
    w->mt[n].stat = 1;
    if (n == 0) {
        w->mt[0].ofs[0] = 0.0f;
        w->mt[0].ofs[1] = 0.0f;
        w->mt[0].ofs[2] = 0.0f;
    }
}

void frame_init_b(FRW *w, int n)
{
    u32 *han = mh2_han(w, w->sub_chr[n]);
    if (han)
        flSetMotionEx(w->mdl->mot1, *han, n);
}
#endif

void rt_monster_get(int no, float pos[3], int *ang_y)
{
    FRW *w = (FRW *)&em_work[no];
    pos[0] = w->pos[0];
    pos[1] = w->pos[1];
    pos[2] = w->pos[2];
    *ang_y = w->ang[1];
}

/* 0 while the monster's motion has not been started yet (a monster respawned in the slot: its first ticks pose the bind pose,
 * which has the body 120 units below the ground) */
int rt_monster_motion_ready(int no)
{
    const FRW *w = (const FRW *)&em_work[no];
    return w->mdl && ((const RT_MPLAY *)w->mdl->mot0)->set[0] != 0;
}

void rt_monster_pose(int no, void *skel)
{
    rt_motion_pose(skel, &em_work[no]);
}

/* the node array at mdl+0x24 of a player/monster work (NULL without one) */
u8 *rt_actor_nodes(const void *work, int *max)
{
    const FRW *w = work;
    if (max)
        *max = RT_NODE_MAX;
    return w->mdl ? (u8 *)w->mdl->skl : NULL;
}

/* Motion sets nobody can reach any more, freed (called at each stage load, rt_flow.c).
 * create_plcom_motion / create_pl_motion / create_em_motion (f_frame.c) write new handles
 * over the table entries of the last load; on the PS2 those sets were released first
 * (all_reset / stage_free / em_motion_free), here those callers are stand-ins, so every
 * quest and village visit left its sets behind (~0.5 MB a loop; the Xbox ran out after
 * one quest loop, xemu 12 Oct 2026). all_motion_free's release stays a stand-in (mset_free). A set is kept while motion_set_handle_tbl holds its
 * handle or a player's / monster's motion player (pl_am, em_am) still plays it. */
void rt_motion_gc(void)
{
    static u8 *mark;
    static int cap;
    int i, k, g, freed = 0;
    if (!nmsets)
        return;
    if (cap < nmsets + 1) {
        free(mark);
        cap = capmsets + 1;
        mark = malloc((size_t)cap);
        if (!mark) {
            cap = 0;
            return;
        }
    }
    memset(mark, 0, (size_t)nmsets + 1);
    for (i = 0; i < 0x1004; i++)
        if (motion_set_handle_tbl[i] && (int)motion_set_handle_tbl[i] <= nmsets)
            mark[motion_set_handle_tbl[i]] = 1;
    for (k = 0; k < 28; k++) {
        rt_actor_motion *am = k < 8 ? pl_am[k] : em_am[k - 8];
        const FRW *w = k < 8 ? (const FRW *)&player_work[k] : (const FRW *)&em_work[k - 8];
        const RT_MPLAY *mp[2];
        int j;
        if (!am || !w->be_flag || w->mdl != &am->mdl)
            continue;       /* a hunter / monster / NPC work not in use: its motion player is set again before it plays */
        mp[0] = (const RT_MPLAY *)&am->mot0;
        mp[1] = (const RT_MPLAY *)&am->mot1;
        for (j = 0; j < 2; j++)
            for (g = 0; g < 4; g++) {
                if (j == 1 && !w->mt[g].b_dir && !w->sub_on[g])
                    continue;   /* mot1 plays only while a layer blends (frame_move); frame_init sets it first */
                if (mp[j]->set[g] && (int)mp[j]->set[g] <= nmsets)
                    mark[mp[j]->set[g]] = 1;
                if (mp[j]->blend[g] && mp[j]->bset[g] && (int)mp[j]->bset[g] <= nmsets)     /* bset: only while it is mixed in */
                    mark[mp[j]->bset[g]] = 1;
            }
    }
    for (i = 1; i <= nmsets; i++)
        if (msets[i - 1].used && !mark[i]) {
            mset_free(i);
            freed++;
        }
    if (freed && getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_motion: %d unreachable motion sets freed\n", freed);
}
