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

typedef struct {
    FRMDL mdl;
    u8 _pad[0x60 - sizeof(FRMDL)];
    u8 skl[0x210];
    RT_MPLAY mot0, mot1;
} rt_actor_motion;

void rt_motion_load_plcom(const uint8_t *tbl)
{
    pl_area_top = (u8 *)tbl;
    create_plcom_motion();
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
        rt_motion_attach(pl);
    pl->scl[0] = pl->scl[1] = pl->scl[2] = 1.0f;
    w->layers = 2;
    w->chr[0] = (u16)legs_id;
    w->chr[1] = (u16)upper_id;
    w->mt[0].spd = 1.0f;
    w->mt[1].spd = 1.0f;
    frame_init(w, 0, 0, 0);
    frame_init(w, 0, 0, 1);
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
