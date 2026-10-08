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
FLMAT view_mat;      /* 0x3F20A0: the view matrix (world -> camera) */

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
__attribute__((weak)) u16 calc_mat_angY(FLMAT *m)
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
    /* view_mat = inverse of the camera's rigid world matrix */
    {
        int i, k;
        for (i = 0; i < 3; i++)
            for (k = 0; k < 3; k++)
                view_mat[i][k] = rview_mat[k][i];
        for (k = 0; k < 3; k++) {
            view_mat[3][k] = -(rview_mat[3][0] * view_mat[0][k] + rview_mat[3][1] * view_mat[1][k]
                              + rview_mat[3][2] * view_mat[2][k]);
            view_mat[k][3] = 0.0f;
        }
        view_mat[3][3] = 1.0f;
    }
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
__attribute__((weak)) void RotateX(FLMAT *m, f32 a) { rotate_pre(m, flmatRotX33, a); }
__attribute__((weak)) void RotateY(FLMAT *m, f32 a) { rotate_pre(m, flmatRotY33, a); }
__attribute__((weak)) void RotateZ(FLMAT *m, f32 a) { rotate_pre(m, flmatRotZ33, a); }

/* flvecApplyMat33(out, v, m): out = v * m (3x3). */
void flvecApplyMat33(f32 *out, f32 *v, FLMAT *m)
{
    f32 x = v[0], y = v[1], z = v[2];
    out[0] = x * (*m)[0][0] + y * (*m)[1][0] + z * (*m)[2][0];
    out[1] = x * (*m)[0][1] + y * (*m)[1][1] + z * (*m)[2][1];
    out[2] = x * (*m)[0][2] + y * (*m)[1][2] + z * (*m)[2][2];
}

/* flMemcpy (0x16F680): forward byte copy, nothing returned (the village's chat/net buffers: lb_a.c, lb_n04.c).
 * A no-op stand-in until 8 Oct 2026. flExp (0x1735F0) is expf (src/main/fl/flm02.c): sysw.c builds its gauss table with it. */
void flMemcpy(unsigned char *d, const unsigned char *s, int n)
{
    int i;
    for (i = 0; i < n; i++)
        *d++ = *s++;
}
f32 flExp(f32 x);
f32 flExp(f32 x) { return expf(x); }

/* flvecApplyMat(out, v, m) (0x172EE0, VU0): the 4-vector v times the 4x4 matrix, all four components
 * (v.w = 1 picks up the translation row). It was a no-op stand-in until 8 Oct 2026: calc_mat_angY read garbage. */
void flvecApplyMat(f32 *out, f32 *v, FLMAT *m)
{
    f32 x = v[0], y = v[1], z = v[2], w = v[3];
    int k;
    for (k = 0; k < 4; k++)
        out[k] = x * (*m)[0][k] + y * (*m)[1][k] + z * (*m)[2][k] + w * (*m)[3][k];
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
__attribute__((weak)) u16 calc_vec_ang(f32 x0, f32 z0, f32 x1, f32 z1)
{
    f32 v[3] = { x0 - x1, 0.0f, z0 - z1 };
    flvecNormalize(v);
    return (u16)(int)(65536.0f * atan2f(-v[2], v[0]) / 6.2831855f + 0.5f);
}

/* ------------------------------------------------------------ more fl
 * (for the eft and shell game C; from the VU0 asm in flmatAddTrans2.s) */

/* flmatCopy33: the 3x3 part only */
void flmatCopy33(FLMAT *d, FLMAT *s)
{
    int i, k;
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            (*d)[i][k] = (*s)[i][k];
}

/* flmatMul(d, a, b): d = a * b, all four rows and columns */
void flmatMul(FLMAT *d, FLMAT *a, FLMAT *b)
{
    FLMAT t;
    int i, k;
    for (i = 0; i < 4; i++)
        for (k = 0; k < 4; k++)
            t[i][k] = (*a)[i][0] * (*b)[0][k] + (*a)[i][1] * (*b)[1][k] + (*a)[i][2] * (*b)[2][k]
                    + (*a)[i][3] * (*b)[3][k];
    memcpy(d, t, sizeof t);
}

/* flmatInvert(d, s): inverse of a rotation-and-scale + translation matrix:
 * d[i][j] = s[j][i] / |row j|^2, translation -t * that. */
void flmatInvert(FLMAT *d, FLMAT *s)
{
    FLMAT t;
    f32 q[3];
    int i, j;
    for (j = 0; j < 3; j++)
        q[j] = 1.0f / ((*s)[j][0] * (*s)[j][0] + (*s)[j][1] * (*s)[j][1] + (*s)[j][2] * (*s)[j][2]);
    memset(t, 0, sizeof t);
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            t[i][j] = (*s)[j][i] * q[j];
    for (j = 0; j < 3; j++)
        t[3][j] = -((*s)[3][0] * (*s)[j][0] + (*s)[3][1] * (*s)[j][1] + (*s)[3][2] * (*s)[j][2]) * q[j];
    t[3][3] = 1.0f;
    memcpy(d, t, sizeof t);
}

static void v3norm(f32 *v)
{
    f32 l = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (l > 0.0f) {
        v[0] /= l;
        v[1] /= l;
        v[2] /= l;
    }
}

static void v3cross(f32 *d, const f32 *a, const f32 *b)
{
    f32 x = a[1] * b[2] - a[2] * b[1], y = a[2] * b[0] - a[0] * b[2], z = a[0] * b[1] - a[1] * b[0];
    d[0] = x;
    d[1] = y;
    d[2] = z;
}

/* flmatBlend(d, a, b, ta, tb): rows x, y and translation mixed as
 * a * ta + b * tb, then re-orthonormalised (z = x cross y, y = z cross x). */
void flmatBlend(FLMAT *d, FLMAT *a, FLMAT *b, f32 ta, f32 tb)
{
    f32 x[3], y[3], z[3], t[3];
    int k;
    for (k = 0; k < 3; k++) {
        x[k] = (*a)[0][k] * ta + (*b)[0][k] * tb;
        y[k] = (*a)[1][k] * ta + (*b)[1][k] * tb;
        t[k] = (*a)[3][k] * ta + (*b)[3][k] * tb;
    }
    v3norm(x);
    v3norm(y);
    v3cross(z, x, y);
    v3norm(z);
    v3cross(y, z, x);
    memset(d, 0, sizeof *d);
    for (k = 0; k < 3; k++) {
        (*d)[0][k] = x[k];
        (*d)[1][k] = y[k];
        (*d)[2][k] = z[k];
        (*d)[3][k] = t[k];
    }
    (*d)[3][3] = 1.0f;
}

/* flmatRotZXY33(m, x, y, z): m = m * Rz(z) * Rx(x) * Ry(y) */
void flmatRotZXY33(FLMAT *m, f32 x, f32 y, f32 z)
{
    flmatRotZ33(m, z);
    flmatRotX33(m, x);
    flmatRotY33(m, y);
}

/* flConvertStoR (0x1733C0) / cpAng2Rad: 0x10000-per-turn angle to radians
 * in (-pi, pi]. */
f32 flConvertStoR(u32 a)
{
    f32 r = 3.1415927f * ((f32)a * 0.0054931640625f) / 180.0f;
    if (!(r <= 3.1415927f))
        r += -6.2831855f;
    return r;
}

/* cpRotMatrix (0x1202C0): m = Rx Ry Rz of three 0x10000-per-turn angles */
__attribute__((weak)) FLMAT *cpRotMatrix(s32 *ang, FLMAT *m)
{
    flmatInit(m);
    flmatSetXYZ33(m, flConvertStoR((u32)ang[0]), flConvertStoR((u32)ang[1]), flConvertStoR((u32)ang[2]));
    return m;
}

/* CalcDistanceXZ (0x120F20): distance in the XZ plane */
__attribute__((weak)) f32 CalcDistanceXZ(f32 *a, f32 *b)
{
    f32 dx = a[0] - b[0], dz = a[2] - b[2];
    return sqrtf(dx * dx + dz * dz);
}

/* RotMatVec (main 0x120570): rotation matrix m whose row `axis` (0 X,
 * 1 Y, 2 Z) is the normalised v; another row comes from a cross product
 * with a fixed axis (a second one when v is parallel to the first,
 * |cross|^2 < 0.001), the third completes the frame. */
__attribute__((weak)) void RotMatVec(f32 *v, FLMAT *m, int axis)
{
    f32 u[3], x[3], y[3], z[3];
    flvecNormalize(v);
    switch (axis & 0xFF) {
    case 0:
        u[0] = 0; u[1] = 0; u[2] = 1.0f;
        flvecOuterProduct(y, u, v);
        if (flvecInnerProduct(y, y) < 0.001f) {
            u[2] = 0; u[0] = 1.0f;
            flvecOuterProduct(y, u, v);
        }
        flvecNormalize(y);
        flvecCopy(x, v);
        flvecOuterProduct(z, x, y);
        break;
    case 1:
        u[0] = 0; u[1] = 0; u[2] = 1.0f;
        flvecOuterProduct(x, v, u);
        if (flvecInnerProduct(x, x) < 0.001f) {
            u[2] = 0; u[1] = 1.0f;
            flvecOuterProduct(x, u, v);
        }
        flvecNormalize(x);
        flvecCopy(y, v);
        flvecOuterProduct(z, x, y);
        break;
    case 2:
        u[0] = 0; u[1] = 1.0f; u[2] = 0;
        flvecOuterProduct(x, u, v);
        if (flvecInnerProduct(x, x) < 0.001f) {
            u[2] = 1.0f; u[1] = 0;
            flvecOuterProduct(x, v, u);
        }
        flvecNormalize(x);
        flvecCopy(z, v);
        flvecOuterProduct(y, z, x);
        break;
    default:   /* the original leaves the rows uninitialised */
        x[0] = 1; x[1] = x[2] = 0; y[1] = 1; y[0] = y[2] = 0; z[2] = 1; z[0] = z[1] = 0;
        break;
    }
    flmatInit(m);
    flvecCopy((*m)[0], x);
    flvecCopy((*m)[1], y);
    flvecCopy((*m)[2], z);
}
