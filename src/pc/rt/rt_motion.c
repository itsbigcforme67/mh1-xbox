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
#include "../fl/fl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------ motion sets */
typedef struct {
    aan_motion m;
} rt_mset;

static rt_mset *msets;
static int nmsets, capmsets;

static const aan_motion *mset_get(u32 h)
{
    if (h == 0 || (int)h > nmsets)
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

u32 flCreateMotionSetHandle(void *tmp)
{
    u8 *aan;
    fmt_blob b;
    memcpy(&aan, tmp, sizeof aan);
    if (!aan)
        return 0;
    if (nmsets == capmsets) {
        capmsets = capmsets ? capmsets * 2 : 256;
        msets = realloc(msets, capmsets * sizeof *msets);
    }
    b.p = aan;
    b.n = fmt_u32(aan + 8, FMT_LE);
    if (fmt_aan_load(&msets[nmsets].m, b, FMT_LE) != 0) {
        fprintf(stderr, "rt: motion set at %p does not parse\n", (void *)aan);
        return 0;
    }
    return (u32)++nmsets;
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
void cpApplyMatrix(FLMAT *m, f32 *v, f32 *out)
{
    flvecApplyMat33(out, v, m);
}

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

void rt_motion_load_plcom(const uint8_t *tbl)
{
    pl_area_top = (u8 *)tbl;
    create_plcom_motion();
}

/* the player's own motions (ids >= 1000) through create_pl_motion */
void create_pl_motion(int pl);
void rt_motion_load_pl(int no, const uint8_t *tbl)
{
    pl_area_top = (u8 *)tbl;
    create_pl_motion(no);
}

void *rt_motion_attach(void *work)
{
    rt_actor_motion *am = calloc(1, sizeof *am);
    FRW *w = work;
    am->mdl.skl = (FRSKL *)am->skl;
    am->mdl.mot0 = (FRMOT *)&am->mot0;
    am->mdl.mot1 = (FRMOT *)&am->mot1;
    am->mot0.xD0 = (s32)(intptr_t)&am->mot0;
    am->mot1.xD0 = (s32)(intptr_t)&am->mot1;
    w->mdl = &am->mdl;
    return am;
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
    create_em_motion(slot, kind);
}

void rt_monster_motion_start(int no, int mdl_no, const uint8_t *tbl, int kind, const int *ids, int layers)
{
    EMW *em = &em_work[no];
    FRW *w = (FRW *)em;
    int g;
    pl_area_top = (u8 *)tbl;
    create_em_motion(mdl_no, kind);
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
            const aan_motion *m = mset_get(motion_set_handle_tbl[mdl_no * 600 + 1700 + first + slot]);
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

void rt_monster_save_old(void *em);
void rt_monster_collide(void *em);
u8 Em_stg_ck(void *);

/* One tick of em_work[no]: em_move's order (old position, frame_move with
 * root motion, walls and ground: rt_hit.c) once it is placed on the stage
 * (rt_monster_place), else only the motion. */
int rt_monster_motion_tick(int no)
{
    EMW *em = &em_work[no];
    int r;
    if (!((FRW *)em)->be_flag)
        return frame_move((FRW *)em);
    rt_monster_save_old(em);
    rt_snd_monster_motion(no);      /* walk sounds (em01 ef_move_sub's list), before the frame steps */
    r = frame_move((FRW *)em);
    if (Em_stg_ck(em) & 0xFF)
        rt_monster_collide(em);
    return r;
}

/* Put em_work[no] on the current stage at pos facing ang_y (0x10000 per
 * turn): in use, monster kind `kind` (em+2, selects its wall spheres
 * em_hit_push_tbl[kind]), wall tests on (+0x4D4, as the em init code at
 * 0x10BE08 sets it). */
void rt_monster_place(int no, int kind, const float pos[3], int ang_y)
{
    EMW *em = &em_work[no];
    FRW *w = (FRW *)em;
    u8 *b = (u8 *)em;
    w->be_flag = 1;
    b[0x2] = (u8)kind;
    b[0x4D4] = 1;
    b[0x736] = game_w.stage;
    w->pos[0] = pos[0];
    w->pos[1] = pos[1];
    w->pos[2] = pos[2];
    w->ang[1] = ang_y & 0xFFFF;
    w->scl[0] = w->scl[1] = w->scl[2] = 1.0f;
    *(f32 *)(b + 0x5AC) = pos[1];
    /* hit points +0x302 / max +0x792: the quest's monster set-up is not
     * ported; 2000 is a stand-in [guess] so hit_check counts it alive */
    *(s16 *)(b + 0x302) = 2000;
    *(s16 *)(b + 0x792) = 2000;
}

void rt_monster_get(int no, float pos[3], int *ang_y)
{
    FRW *w = (FRW *)&em_work[no];
    pos[0] = w->pos[0];
    pos[1] = w->pos[1];
    pos[2] = w->pos[2];
    *ang_y = w->ang[1];
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
