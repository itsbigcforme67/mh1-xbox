/*
 * flmat.h - fl-style matrices: 4x4 float, row vectors (v' = v * M),
 * translation in row 3 (m[12..14]), as on the PS2 (graphics.md 6).
 * The same memory layout is what OpenGL's column-major functions expect.
 */
#ifndef MH_FLMAT_H
#define MH_FLMAT_H

#include <math.h>
#include <string.h>

typedef float flmat[16];

static inline void flmat_identity(flmat m)
{
    memset(m, 0, sizeof(flmat));
    m[0] = m[5] = m[10] = m[15] = 1;
}

/* out = a * b (apply a first, then b); out may alias neither */
static inline void flmat_mul(flmat out, const flmat a, const flmat b)
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            out[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] + a[i * 4 + 1] * b[1 * 4 + j]
                           + a[i * 4 + 2] * b[2 * 4 + j] + a[i * 4 + 3] * b[3 * 4 + j];
}

/* flGetMotionMatrix (0x174300): Scale, then flmatRotXYZ33 (M*Rx*Ry*Rz),
 * then translation into row 3. */
static inline void flmat_srt(flmat m, const float s[3], const float r[3], const float t[3])
{
    float sx = sinf(r[0]), cx = cosf(r[0]), sy = sinf(r[1]), cy = cosf(r[1]);
    float sz = sinf(r[2]), cz = cosf(r[2]);
    /* rows of Rx*Ry*Rz */
    float R[9];
    R[0] = cy * cz;                   R[1] = cy * sz;                   R[2] = -sy;
    R[3] = sx * sy * cz - cx * sz;    R[4] = sx * sy * sz + cx * cz;    R[5] = sx * cy;
    R[6] = cx * sy * cz + sx * sz;    R[7] = cx * sy * sz - sx * cz;    R[8] = cx * cy;
    m[0] = s[0] * R[0]; m[1] = s[0] * R[1]; m[2] = s[0] * R[2]; m[3] = 0;
    m[4] = s[1] * R[3]; m[5] = s[1] * R[4]; m[6] = s[1] * R[5]; m[7] = 0;
    m[8] = s[2] * R[6]; m[9] = s[2] * R[7]; m[10] = s[2] * R[8]; m[11] = 0;
    m[12] = t[0]; m[13] = t[1]; m[14] = t[2]; m[15] = 1;
}

/* Inverse of an affine matrix (3x3 + translation). */
static inline void flmat_invert_affine(flmat out, const flmat m)
{
    float a = m[0], b = m[1], c = m[2], d = m[4], e = m[5], f = m[6], g = m[8], h = m[9], i = m[10];
    float A = e * i - f * h, B = -(d * i - f * g), C = d * h - e * g;
    float det = a * A + b * B + c * C, id;
    if (det == 0)
        det = 1;
    id = 1.0f / det;
    out[0] = A * id; out[1] = -(b * i - c * h) * id; out[2] = (b * f - c * e) * id; out[3] = 0;
    out[4] = B * id; out[5] = (a * i - c * g) * id;  out[6] = -(a * f - c * d) * id; out[7] = 0;
    out[8] = C * id; out[9] = -(a * h - b * g) * id; out[10] = (a * e - b * d) * id; out[11] = 0;
    out[12] = -(m[12] * out[0] + m[13] * out[4] + m[14] * out[8]);
    out[13] = -(m[12] * out[1] + m[13] * out[5] + m[14] * out[9]);
    out[14] = -(m[12] * out[2] + m[13] * out[6] + m[14] * out[10]);
    out[15] = 1;
}

static inline void flmat_apply(float out[3], const float v[3], const flmat m)
{
    out[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + m[12];
    out[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + m[13];
    out[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + m[14];
}

static inline void flmat_apply33(float out[3], const float v[3], const flmat m)
{
    out[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8];
    out[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9];
    out[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10];
}

/* Perspective projection (OpenGL clip conventions), row-vector form. */
static inline void flmat_perspective(flmat m, float fovy_rad, float aspect, float znear, float zfar)
{
    float f = 1.0f / tanf(fovy_rad / 2);
    memset(m, 0, sizeof(flmat));
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (zfar + znear) / (znear - zfar);
    m[11] = -1;
    m[14] = 2 * zfar * znear / (znear - zfar);
}

#endif
