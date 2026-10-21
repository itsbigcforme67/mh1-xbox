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
 *   fast approximation, written out below (ps2_sincos).
 * - flmatMul33_2(a, b): a = a * b on the 3x3 part (row 3 kept).
 * - flmatScaleFactor33(m, x, y, z): m = m * diag(x, y, z).
 * - flmatSetXYZ33(m, x, y, z): 3x3 = identity * Rx(x) * Ry(y) * Rz(z).
 */
#include "rt.h"
#include "types.h"
#include "fl.h"

#include <math.h>
#include "rt_vu.h"
#include <string.h>

/* flPS2SinFast / flPS2CosFast / flPS2SinCosFast (main 0x171240 / 0x171380 / 0x1714A0, VU0 macro code [read: asm]), so the
 * game's rotations come out as on the PS2 instead of libm's: an angle outside [-pi, pi) is brought back by
 * k = (int)(a * at_tbl[0]) (truncated), then k + (a > 0) rounded down to even, a -= k * at_tbl[1]; then
 * u = pi/2 - a (a >= 0) or pi/2 + a (a < 0, sine negative), and cos = u + SS5432[3] u^3 + [2] u^5 + [1] u^7 + [0] u^9
 * (the VU's multiply-add chain, kept in this order), sin = +-sqrt(1 - cos^2). The two four-float tables come from the
 * player's disc (src/pc/rt/tables.txt); without them (tools, no disc) libm stands in. Rounding: the game's float mode
 * (RT_FPU_PS2: toward zero as the VU). Agent D, 20 Oct 2026 (the lockstep referee: monster facings one unit apart). */
extern unsigned char at_tbl[], SS5432[];
static f32 ps2_sincos_vu(f32 a, int want_sin)
{
    const f32 *c = (const f32 *)at_tbl, *k = (const f32 *)SS5432;
    f32 u, u2, u4, u6, u8, k5[4], acc;
    int neg, i;
    if (c[1] == 0.0f)
        return want_sin ? sinf(a) : cosf(a);
    if (a <= -3.14159274f || !(a < 3.14159274f)) {
        union { f32 f; int32_t i; } b;
        int n;
        b.f = a;
        n = (int)(a * c[0]);
        n = (short)((n + (b.i > 0)) & ~1);
        a = a - (f32)n * c[1];
    }
    if (a < 0.0f) {
        u = 1.57079637f + a;
        neg = 1;
    } else {
        u = 1.57079637f - a;
        neg = 0;
    }
    u2 = u * u;
    for (i = 0; i < 4; i++)
        k5[i] = k[i] * u;
    u4 = u2 * u2;
    u6 = u2 * u4;
    u8 = u4 * u4;
    acc = u;
    acc = acc + u2 * k5[3];
    acc = acc + u4 * k5[2];
    acc = acc + u6 * k5[1];
    acc = acc + u8 * k5[0];
    if (!want_sin)
        return acc;
    u = sqrtf(fabsf(1.0f - acc * acc));      /* the VU's square root takes the absolute value */
    return neg ? -u : u;
}
static f32 ps2_sincos(f32 a, int want_sin)
{
    unsigned int o = rt_vu_begin();
    f32 r = ps2_sincos_vu(a, want_sin);
    rt_vu_end(o);
    return r;
}
#define sinf(a) ps2_sincos((a), 1)
#define cosf(a) ps2_sincos((a), 0)

FLMAT rview_mat;     /* 0x3F2060: inverse of the view matrix (camera world) */
FLMAT rview_matY;    /* 0x3F2020: rotation about Y that turns a billboard to the camera */
FLMAT view_mat;      /* 0x3F20A0: the view matrix (world -> camera) */

void flmatInit(FLMAT *m)
{
    memset(m, 0, sizeof *m);
    (*m)[0][0] = (*m)[1][1] = (*m)[2][2] = (*m)[3][3] = 1.0f;
}

/* flmatInit33 (0x171D20, VU0 asm in src/main/fl/flvu02.c): identity in the xyz of rows 0-2, the w of each row and
 * row 3 (translation) stay as they were. */
void flmatInit33(FLMAT *m)
{
    int r, c;
    for (r = 0; r < 3; r++)
        for (c = 0; c < 3; c++)
            (*m)[r][c] = (r == c) ? 1.0f : 0.0f;
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
    unsigned int rt_vu_o = rt_vu_begin();
    int i, k;
    for (i = 0; i < 3; i++) {
        f32 r[3];
        for (k = 0; k < 3; k++)
            r[k] = (*a)[i][0] * (*b)[0][k] + (*a)[i][1] * (*b)[1][k] + (*a)[i][2] * (*b)[2][k];
        for (k = 0; k < 3; k++)
            (*a)[i][k] = r[k];
    }
    rt_vu_end(rt_vu_o);
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

u16 calc_mat_angY(FLMAT *m);    /* src/main/cp/cp01.c */

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
    unsigned int rt_vu_o = rt_vu_begin();
    f32 l = flvecCalcLength(v);
    if (l > 0.0f) {
        v[0] /= l;
        v[1] /= l;
        v[2] /= l;
    }
    rt_vu_end(rt_vu_o);
}

f32 flSqrt(f32 x) { return sqrtf(x); }
f32 flAbs(f32 x) { return fabsf(x); }
f32 flArcSin(f32 x) { return asinf(x < -1.0f ? -1.0f : x > 1.0f ? 1.0f : x); }
f32 flArcCos(f32 x) { return acosf(x < -1.0f ? -1.0f : x > 1.0f ? 1.0f : x); }

/* ------------------------------------------------------------ more helpers (set09) */
f32 flSin(f32 a) { return sinf(a); }     /* flPS2SinFast (ps2_sincos above) */
f32 flCos(f32 a) { return cosf(a); }
/* flArcTan2 (0x1735E0) jumps to the PS2 C library's atan2f (newlib's single-precision __ieee754_atan2f 0x1ABB80 and
 * atanf 0x1AEF50, FPU code [read: asm]); written out here with its own constants and the tables from the player's disc
 * (atanhi / atanlo / aT, src/pc/rt/tables.txt), under the game's float rounding, so monster facings come out as on
 * the PS2 (libm's atan2f was one unit of angle off now and then: the lockstep referee). This library's pi and pi/2 end
 * in ..FDA, its pi_lo is +1.5099580e-07, and large quotients (k > 60) give pi/2 without a correction. */
extern unsigned char atanhi_0035E598[], atanlo_0035E5A8[], aT_0035E5B8[];
static f32 bits_f(uint32_t b) { union { uint32_t i; f32 f; } u; u.i = b; return u.f; }
static uint32_t f_bits(f32 f) { union { uint32_t i; f32 f; } u; u.f = f; return u.i; }
static f32 ps2_atanf(f32 x)
{
    const f32 *hi = (const f32 *)atanhi_0035E598, *lo = (const f32 *)atanlo_0035E5A8, *aT = (const f32 *)aT_0035E5B8;
    int32_t hx = (int32_t)f_bits(x), ix = hx & 0x7FFFFFFF, id;
    f32 z, w, s1, s2;
    if (ix > 0x507FFFFF)
        return hx > 0 ? hi[3] + lo[3] : -hi[3] - lo[3];
    if (ix <= 0x3EDFFFFF) {
        if (ix <= 0x30FFFFFF && bits_f(0x7149F2C9) + x > 1.0f)
            return x;
        id = -1;
    } else {
        x = fabsf(x);
        if (ix <= 0x3F97FFFF) {
            if (ix <= 0x3F2FFFFF) {
                id = 0;
                x = ((x + x) - 1.0f) / (x + 2.0f);
            } else {
                id = 1;
                x = (x - 1.0f) / (x + 1.0f);
            }
        } else if (ix > 0x401BFFFF) {
            id = 3;
            x = -1.0f / x;
        } else {
            id = 2;
            x = (x - 1.5f) / (x * 1.5f + 1.0f);
        }
    }
    z = x * x;
    w = z * z;
    s1 = z * (aT[0] + w * (aT[2] + w * (aT[4] + w * (aT[6] + w * (aT[8] + w * aT[10])))));
    s2 = w * (aT[1] + w * (aT[3] + w * (aT[5] + w * (aT[7] + w * aT[9]))));
    if (id < 0)
        return x - x * (s1 + s2);
    z = hi[id] - ((x * (s1 + s2) - lo[id]) - x);
    return hx < 0 ? -z : z;
}
f32 flArcTan2(f32 y, f32 x)
{
    int32_t hx = (int32_t)f_bits(x), hy = (int32_t)f_bits(y), ix = hx & 0x7FFFFFFF, iy = hy & 0x7FFFFFFF, m, k;
    const f32 pi = bits_f(0x40490FDA), pi_o_2 = bits_f(0x3FC90FDA), pi_lo = bits_f(0x34222168);
    f32 z;
    if (((const f32 *)aT_0035E5B8)[0] == 0.0f)
        return atan2f(y, x);            /* no disc tables (tools) */
    if (hx == 0x3F800000)
        return ps2_atanf(y);
    m = (int32_t)(((uint32_t)hy >> 31) | ((hx >> 30) & 2));
    if (iy <= 0x7FFFFF) {               /* y zero (or denormal) */
        if (m == 2)
            return pi;
        if (m == 3)
            return -pi;
        return y;
    }
    if (ix <= 0x7FFFFF)                 /* x zero */
        return hy < 0 ? -pi_o_2 : pi_o_2;
    k = (iy - ix) >> 23;
    if (k > 60)
        z = pi_o_2;
    else if (hx < 0 && k < -60)
        z = 0.0f;
    else
        z = ps2_atanf(fabsf(y / x));
    switch (m) {
    case 0:
        return z;
    case 1:
        return bits_f(f_bits(z) ^ 0x80000000u);
    case 2:
        return pi - (z - pi_lo);
    default:
        return (z - pi_lo) - pi;
    }
}

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
    unsigned int rt_vu_o = rt_vu_begin();
    FLMAT t;
    int i, k;
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            t[i][k] = (*a)[i][0] * (*b)[0][k] + (*a)[i][1] * (*b)[1][k] + (*a)[i][2] * (*b)[2][k];
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            (*dst)[i][k] = t[i][k];
    rt_vu_end(rt_vu_o);
}

/* flmatRotXYZ33(m, x, y, z): m = m * Rx(x) * Ry(y) * Rz(z) (same VU0 blocks
 * as flmatSetXYZ33, without the reset). */
void flmatRotXYZ33(FLMAT *m, f32 x, f32 y, f32 z)
{
    flmatRotX33(m, x);
    flmatRotY33(m, y);
    flmatRotZ33(m, z);
}

/* flvecApplyMat33(out, v, m): out = v * m (3x3). */
void flvecApplyMat33(f32 *out, f32 *v, FLMAT *m)
{
    unsigned int rt_vu_o = rt_vu_begin();
    f32 x = v[0], y = v[1], z = v[2];
    out[0] = x * (*m)[0][0] + y * (*m)[1][0] + z * (*m)[2][0];
    out[1] = x * (*m)[0][1] + y * (*m)[1][1] + z * (*m)[2][1];
    out[2] = x * (*m)[0][2] + y * (*m)[1][2] + z * (*m)[2][2];
    rt_vu_end(rt_vu_o);
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
    unsigned int rt_vu_o = rt_vu_begin();
    f32 x = v[0], y = v[1], z = v[2], w = v[3];
    int k;
    for (k = 0; k < 4; k++)
        out[k] = x * (*m)[0][k] + y * (*m)[1][k] + z * (*m)[2][k] + w * (*m)[3][k];
    rt_vu_end(rt_vu_o);
}

/* flvecRotY(v, a): v = v * Ry(a) (x' = c x + s z, z' = -s x + c z). */
void flvecRotY(f32 *v, f32 a)
{
    f32 s = sinf(a), c = cosf(a), x = v[0], z = v[2];
    v[0] = c * x + s * z;
    v[2] = -s * x + c * z;
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
    unsigned int rt_vu_o = rt_vu_begin();
    FLMAT t;
    int i, k;
    for (i = 0; i < 4; i++)
        for (k = 0; k < 4; k++)
            t[i][k] = (*a)[i][0] * (*b)[0][k] + (*a)[i][1] * (*b)[1][k] + (*a)[i][2] * (*b)[2][k]
                    + (*a)[i][3] * (*b)[3][k];
    memcpy(d, t, sizeof t);
    rt_vu_end(rt_vu_o);
}

/* flmatInvert(d, s): inverse of a rotation-and-scale + translation matrix:
 * d[i][j] = s[j][i] / |row j|^2, translation -t * that. */
void flmatInvert(FLMAT *d, FLMAT *s)
{
    unsigned int rt_vu_o = rt_vu_begin();
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
    rt_vu_end(rt_vu_o);
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
    unsigned int rt_vu_o = rt_vu_begin();
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
    rt_vu_end(rt_vu_o);
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

