/*
 * rt_flmat.c - Capcom fl matrix helpers (flmat*, VU0 macro code on the PS2)
 * re-implemented in plain C for the port runtime, plus the game's camera
 * matrices rview_mat / rview_matY (View_move 0x169A90).
 *
 * fl matrices are row-vector 4x4 (v' = v * M, translation in row 3). Each
 * function here follows its PS2 version's VU0 code [read: asm]:
 * - flmatRotX33/Y33/Z33(m, a): m = m * R(a) on the 3x3 part, with
 *   Rx rows (1,0,0) (0,c,s) (0,-s,c); Ry rows (c,0,-s) (0,1,0) (s,0,c);
 *   Rz rows (c,s,0) (-s,c,0) (0,0,1). s, c come from flPS2SinCosFast, a
 *   fast approximation; sinf/cosf are used here.
 * - flmatMul33_2(a, b): a = a * b on the 3x3 part (row 3 kept).
 * - flmatScaleFactor33(m, x, y, z): m = m * diag(x, y, z).
 * - flmatSetXYZ33(m, x, y, z): 3x3 = identity * Rx(x) * Ry(y) * Rz(z).
 */
#include "rt.h"
#include "types.h"
#include "fl.h"

#include <math.h>
#include <string.h>

FLMAT rview_mat;     /* 0x3F2060: inverse of the view matrix (camera world) */
FLMAT rview_matY;    /* 0x3F2020: rotation about Y that turns a billboard to the camera */

void flmatInit(FLMAT *m)
{
    memset(m, 0, sizeof *m);
    (*m)[0][0] = (*m)[1][1] = (*m)[2][2] = (*m)[3][3] = 1.0f;
}

void flmatCopy(FLMAT *dst, FLMAT *src)
{
    memmove(dst, src, sizeof *dst);
}

void flmatSetTrans(FLMAT *m, f32 x, f32 y, f32 z)
{
    (*m)[3][0] = x;
    (*m)[3][1] = y;
    (*m)[3][2] = z;
}

void flmatMul33_2(FLMAT *a, FLMAT *b)
{
    int i, k;
    for (i = 0; i < 3; i++) {
        f32 r[3];
        for (k = 0; k < 3; k++)
            r[k] = (*a)[i][0] * (*b)[0][k] + (*a)[i][1] * (*b)[1][k] + (*a)[i][2] * (*b)[2][k];
        for (k = 0; k < 3; k++)
            (*a)[i][k] = r[k];
    }
}

static void mul_rot(FLMAT *m, const f32 r[3][3])
{
    FLMAT t;
    int i, k;
    memset(t, 0, sizeof t);
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            t[i][k] = r[i][k];
    flmatMul33_2(m, &t);
}

void flmatRotX33(FLMAT *m, f32 a)
{
    f32 s = sinf(a), c = cosf(a);
    const f32 r[3][3] = { { 1, 0, 0 }, { 0, c, s }, { 0, -s, c } };
    mul_rot(m, r);
}

void flmatRotY33(FLMAT *m, f32 a)
{
    f32 s = sinf(a), c = cosf(a);
    const f32 r[3][3] = { { c, 0, -s }, { 0, 1, 0 }, { s, 0, c } };
    mul_rot(m, r);
}

void flmatRotZ33(FLMAT *m, f32 a)
{
    f32 s = sinf(a), c = cosf(a);
    const f32 r[3][3] = { { c, s, 0 }, { -s, c, 0 }, { 0, 0, 1 } };
    mul_rot(m, r);
}

void flmatScaleFactor33(FLMAT *m, f32 x, f32 y, f32 z)
{
    const f32 r[3][3] = { { x, 0, 0 }, { 0, y, 0 }, { 0, 0, z } };
    mul_rot(m, r);
}

void flmatSetXYZ33(FLMAT *m, f32 x, f32 y, f32 z)
{
    int i;
    for (i = 0; i < 3; i++) {
        (*m)[i][0] = (*m)[i][1] = (*m)[i][2] = 0.0f;
        (*m)[i][i] = 1.0f;
    }
    flmatRotX33(m, x);
    flmatRotY33(m, y);
    flmatRotZ33(m, z);
}

/* calc_mat_angY (0x120520): yaw of the matrix's local +z axis, 0x10000 per
 * turn: atan2(-dz, dx) of (0,0,1) * m - m's translation. */
static u16 calc_mat_angY(FLMAT *m)
{
    f32 dx = (*m)[2][0], dz = (*m)[2][2];
    return (u16)(int)(65536.0f * atan2f(-dz, dx) / 6.2831855f + 0.5f);
}

/* View_move's camera part: rview_mat = inverse(view) = the camera's world
 * matrix (flmatMakeLookAt: rows right, up, back (eye - target), eye), and
 * rview_matY = Ry(yaw + 90 degrees), so a billboard's +z faces the camera. */
void rt_set_camera(const float cam_world[16])
{
    u16 ang;
    memcpy(rview_mat, cam_world, sizeof rview_mat);
    ang = (u16)(calc_mat_angY(&rview_mat) + 0x4000);
    flmatInit(&rview_matY);
    flmatSetXYZ33(&rview_matY, 0.0f, 2.0f * (3.1415927f * (360.0f * (f32)ang / 65536.0f / 360.0f)), 0.0f);
}
