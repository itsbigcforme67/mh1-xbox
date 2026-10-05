/*
 * rt_player.c - a stand-in for the player's "normal" state (pl_normal,
 * main 0x141BA0, and its pl_mv### actions: not decompiled yet, agent F's
 * area) so the hunter can stand, turn and run with the pad.
 *
 * What runs as game C each tick:
 *   rt_pad_tick -> swset (pad_get.c) -> pl_sw_set / sw_set_sub
 *   (pl_normal2.c) fill player_work[0].sw from the host pad;
 *   frame_init / frame_move (f_frame_nm.c) play the motions and move the
 *   player by the motion's root translation (pl_velocity_sub).
 * What is host code here (a guess at the game's behaviour, to be replaced
 * by the decompiled pl_normal):
 *   - the left stick turns the hunter towards the stick direction relative
 *     to the camera, at most 0x800 (11 degrees) per tick;
 *   - stick pushed: legs/upper motions 3/103 (the run loop: plcom 3 moves
 *     bone 1 537 units forward over 78 frames, and pl_mv001, which uses it,
 *     is the first action pl_normal calls), else idle 1/101 (as
 *     normal_char_set picks on a plain stage); 4-tick cross-fade;
 *   - y follows the ground (GetGroundHit). No walls yet.
 */
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "fl.h"
#include "frame.h"

#include <math.h>

extern FLMAT rview_mat;
void pl_sw_set(void);
void rt_pad_tick(void);
f32 GetGroundHit(f32 *pos);

static int moving[8];

static void set_motion(PLW *pl, int legs, int upper, int blend)
{
    FRW *w = (FRW *)pl;
    w->chr[0] = (u16)legs;
    w->chr[1] = (u16)upper;
    w->mt[0].spd = 1.0f;
    w->mt[1].spd = 1.0f;
    frame_init(w, 0, blend, 0);
    frame_init(w, 0, blend, 1);
}

/* Shortest signed difference b - a of two 0x10000-per-turn angles. */
static int ang_diff(int a, int b)
{
    return (s16)(u16)(b - a);
}

void rt_player_tick(int no)
{
    PLW *pl = &player_work[no];
    int pow, want;

    rt_pad_tick();
    pl_sw_set();
    pow = pl->sw.pow[0];
    want = pow > 0;
    if (want) {
        /* stick angle: 0 = right, 0x4000 = up; up = away from the camera */
        float a = (float)pl->sw.ang[0] * (6.2831853f / 65536.0f);
        float sx = cosf(a), sy = sinf(a);
        float rx = rview_mat[0][0], rz = rview_mat[0][2];      /* camera right */
        float fx = -rview_mat[2][0], fz = -rview_mat[2][2];    /* camera forward */
        float dx = rx * sx + fx * sy, dz = rz * sx + fz * sy;
        int target = (int)(atan2f(dx, dz) * (65536.0f / 6.2831853f)) & 0xFFFF;
        int d = ang_diff(pl->ang[1], target);
        if (d > 0x800) d = 0x800;
        if (d < -0x800) d = -0x800;
        pl->ang[1] = (pl->ang[1] + d) & 0xFFFF;
        pl->ang_y = (s16)pl->ang[1];
    }
    if (want != moving[no]) {
        moving[no] = want;
        if (want)
            set_motion(pl, 3, 103, 4);
        else
            set_motion(pl, 1, 101, 4);
    }
    frame_move((FRW *)pl);
    pl->pos[1] = GetGroundHit(pl->pos);
}

/* Read back what the game code saw (for tests): buttons, stick. */
void rt_player_sw(int no, int *now, int *ang, int *pow)
{
    PLW *pl = &player_work[no];
    *now = pl->sw.now;
    *ang = pl->sw.ang[0];
    *pow = pl->sw.pow[0];
}

void rt_player_set_ang(int no, int ang_y)
{
    player_work[no].ang[1] = ang_y & 0xFFFF;
    player_work[no].ang_y = (s16)ang_y;
}
