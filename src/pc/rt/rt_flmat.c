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

/* ------------------------------------------------------------ vectors, maths */
/* fl vector helpers (flvec*, VU0 on the PS2) and the fl maths wrappers. */
void flvecCopy(f32 *dst, f32 *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

f32 flvecInnerProduct(f32 *a, f32 *b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void flvecOuterProduct(f32 *out, f32 *a, f32 *b)
{
    f32 x = a[1] * b[2] - a[2] * b[1];
    f32 y = a[2] * b[0] - a[0] * b[2];
    f32 z = a[0] * b[1] - a[1] * b[0];
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

f32 flvecCalcLength(f32 *v)
{
    return sqrtf(flvecInnerProduct(v, v));
}

f32 flvecCalcDistance(f32 *a, f32 *b)
{
    f32 d[3] = { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
    return flvecCalcLength(d);
}

void flvecNormalize(f32 *v)
{
    f32 l = flvecCalcLength(v);
    if (l > 0.0f) {
        v[0] /= l;
        v[1] /= l;
        v[2] /= l;
    }
}

f32 flSqrt(f32 x) { return sqrtf(x); }
f32 flAbs(f32 x) { return fabsf(x); }
f32 flArcSin(f32 x) { return asinf(x < -1.0f ? -1.0f : x > 1.0f ? 1.0f : x); }
f32 flArcCos(f32 x) { return acosf(x < -1.0f ? -1.0f : x > 1.0f ? 1.0f : x); }

/* ------------------------------------------------------------ more helpers (set09) */
f32 flSin(f32 a) { return sinf(a); }     /* flPS2SinFast: an approximation on the PS2 */
f32 flCos(f32 a) { return cosf(a); }
f32 flArcTan2(f32 y, f32 x) { return atan2f(y, x); }   /* j atan2f */

void flmatGetTrans(f32 *v, FLMAT *m)
{
    v[0] = (*m)[3][0];
    v[1] = (*m)[3][1];
    v[2] = (*m)[3][2];
}

/* flmatMakeScale: identity with the scale on the diagonal (row 3 = 0,0,0,1). */
void flmatMakeScale(FLMAT *m, f32 x, f32 y, f32 z)
{
    flmatInit(m);
    (*m)[0][0] = x;
    (*m)[1][1] = y;
    (*m)[2][2] = z;
}

/* flmatMul33(dst, a, b): dst 3x3 = a * b; dst's w column and row 3 are kept. */
void flmatMul33(FLMAT *dst, FLMAT *a, FLMAT *b)
{
    FLMAT t;
    int i, k;
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            t[i][k] = (*a)[i][0] * (*b)[0][k] + (*a)[i][1] * (*b)[1][k] + (*a)[i][2] * (*b)[2][k];
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            (*dst)[i][k] = t[i][k];
}

/* flmatRotXYZ33(m, x, y, z): m = m * Rx(x) * Ry(y) * Rz(z) (same VU0 blocks
 * as flmatSetXYZ33, without the reset). */
void flmatRotXYZ33(FLMAT *m, f32 x, f32 y, f32 z)
{
    flmatRotX33(m, x);
    flmatRotY33(m, y);
    flmatRotZ33(m, z);
}

/* RotateX/Y/Z (g_cpAng2Rad): m = R(a) * m on the 3x3 part. */
static void rotate_pre(FLMAT *m, void (*rot)(FLMAT *, f32), f32 a)
{
    FLMAT r;
    flmatInit(&r);
    rot(&r, a);
    flmatMul33(m, &r, m);
}
void RotateX(FLMAT *m, f32 a) { rotate_pre(m, flmatRotX33, a); }
void RotateY(FLMAT *m, f32 a) { rotate_pre(m, flmatRotY33, a); }
void RotateZ(FLMAT *m, f32 a) { rotate_pre(m, flmatRotZ33, a); }

/* flvecApplyMat33(out, v, m): out = v * m (3x3). */
void flvecApplyMat33(f32 *out, f32 *v, FLMAT *m)
{
    f32 x = v[0], y = v[1], z = v[2];
    out[0] = x * (*m)[0][0] + y * (*m)[1][0] + z * (*m)[2][0];
    out[1] = x * (*m)[0][1] + y * (*m)[1][1] + z * (*m)[2][1];
    out[2] = x * (*m)[0][2] + y * (*m)[1][2] + z * (*m)[2][2];
}

/* flvecRotY(v, a): v = v * Ry(a) (x' = c x + s z, z' = -s x + c z). */
void flvecRotY(f32 *v, f32 a)
{
    f32 s = sinf(a), c = cosf(a), x = v[0], z = v[2];
    v[0] = c * x + s * z;
    v[2] = -s * x + c * z;
}

/* calc_vec_ang (g_cpAng2Rad): angle of (x0 - x1, z0 - z1), 0x10000 per
 * turn: atan2(-dz, dx) of the normalised vector. */
u16 calc_vec_ang(f32 x0, f32 z0, f32 x1, f32 z1)
{
    f32 v[3] = { x0 - x1, 0.0f, z0 - z1 };
    flvecNormalize(v);
    return (u16)(int)(65536.0f * atan2f(-v[2], v[0]) / 6.2831855f + 0.5f);
}
