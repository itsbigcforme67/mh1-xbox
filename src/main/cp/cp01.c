/* cp01 - vector and angle math 0x00120240-0x001209F4: cpAng2Rad, cpAng2Rad_all, cpRotMatrix, cpRotMatrixYXZ2, cpApplyMatrix, calc_vec_ang, calc_vec_ang2, calc_mat_angY, RotMatVec, SetVector, PointToPoint, AddVector, SubVector, ScaleVector, UnitNormalVectorCCW, NormalClipF3. Whole file in cpmath_nm.c. */
#include "types.h"

typedef f32 MAT4[4][4];

void flmatInit(void *);
void flmatSetXYZ33(void *, f32, f32, f32);
void flvecApplyMat33(void *, void *, void *);
void flvecApplyMat(void *, void *, void *);
void flvecNormalize(f32 *);
f32 flArcTan2(f32, f32);
void RotateX(MAT4 *, f32);
void RotateY(MAT4 *, f32);
void RotateZ(MAT4 *, f32);









void flvecOuterProduct(f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
f32 flAbs(f32);
void flmatRotX33(void *, f32);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
void flmatMul33(void *, void *, void *);





















f32 cpAng2Rad(s32 a) {
    return 0.0000958738f * (f32)(a & 0xFFFF);
}

void cpAng2Rad_all(s32 *a, f32 *out) {
    out[0] = cpAng2Rad(a[0]);
    out[1] = cpAng2Rad(a[1]);
    out[2] = cpAng2Rad(a[2]);
}

void *cpRotMatrix(s32 *ang, MAT4 *m) {
    f32 r[3];

    cpAng2Rad_all(ang, r);
    flmatInit(m);
    flmatSetXYZ33(m, r[0], r[1], r[2]);
    return m;
}

void *cpRotMatrixYXZ2(s32 *ang, MAT4 *m) {
    f32 r[3];

    cpAng2Rad_all(ang, r);
    flmatInit(m);
    RotateY(m, r[1]);
    RotateX(m, r[0]);
    RotateZ(m, r[2]);
    return m;
}

void *cpApplyMatrix(MAT4 *m, f32 *in, f32 *out) {
    flvecApplyMat33(out, in, m);
    return out;
}

/* Yaw of the direction (x0,z0) -> (x1,z1) as a 16 bit angle. */
u16 calc_vec_ang(f32 x0, f32 z0, f32 x1, f32 z1) {
    f32 d[3];

    d[1] = 0;
    d[0] = x0 - x1;
    d[2] = z0 - z1;
    flvecNormalize(d);
    return (s32)(0.5f + (65536.0f * flArcTan2(-d[2], d[0])) / 6.2831855f);
}

u16 calc_vec_ang2(f32 *a, f32 *b) {
    f32 d[3];

    d[0] = a[0] - b[0];
    d[1] = 0;
    d[2] = a[2] - b[2];
    flvecNormalize(d);
    return (s32)(0.5f + (65536.0f * flArcTan2(-d[2], d[0])) / 6.2831855f);
}

u16 calc_mat_angY(MAT4 *m) {
    f32 in[4];
    f32 out[4];

    in[0] = 0;
    in[1] = 0;
    in[2] = 1.0f;
    in[3] = 1.0f;
    flvecApplyMat(out, in, m);
    return (s32)(0.5f + (65536.0f * flArcTan2(-(out[2] - (*m)[3][2]), out[0] - (*m)[3][0])) / 6.2831855f);
}

/* Builds an orthonormal frame around normal n into m (rows 0, 1, 2 = three axes). `type`
 * picks which world axis is used as the reference to cross with. */
void RotMatVec(f32 *n, MAT4 *m, int type) {
    f32 t[4];
    f32 c[4];
    f32 b[4];
    f32 a[4];

    flvecNormalize(n);
    switch ((u8)type) {
    case 0:
        t[0] = 0;
        t[1] = 0;
        t[2] = 1.0f;
        flvecOuterProduct(b, t, n);
        if (flvecInnerProduct(b, b) < 0.001f) {
            t[2] = 0;
            t[0] = 1.0f;
            flvecOuterProduct(b, t, n);
        }
        flvecNormalize(b);
        flvecCopy(c, n);
        flvecOuterProduct(a, c, b);
        break;
    case 1:
        t[0] = 0;
        t[1] = 0;
        t[2] = 1.0f;
        flvecOuterProduct(c, n, t);
        if (flvecInnerProduct(c, c) < 0.001f) {
            t[2] = 0;
            t[1] = 1.0f;
            flvecOuterProduct(c, t, n);
        }
        flvecNormalize(c);
        flvecCopy(b, n);
        flvecOuterProduct(a, c, b);
        break;
    case 2:
        t[0] = 0;
        t[1] = 1.0f;
        t[2] = 0;
        flvecOuterProduct(c, t, n);
        if (flvecInnerProduct(c, c) < 0.001f) {
            t[2] = 1.0f;
            t[1] = 0;
            flvecOuterProduct(c, n, t);
        }
        flvecNormalize(c);
        flvecCopy(a, n);
        flvecOuterProduct(b, a, c);
        break;
    }
    flmatInit(m);
    flvecCopy((f32 *)m, c);
    flvecCopy((f32 *)m + 4, b);
    flvecCopy((f32 *)m + 8, a);
}

void SetVector(f32 *v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void PointToPoint(f32 *out, f32 *a, f32 *b) {
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
}

void AddVector(f32 *out, f32 *a, f32 *b) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
}

void SubVector(f32 *out, f32 *a, f32 *b) {
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
}

void ScaleVector(f32 *out, f32 *a, f32 s) {
    out[0] = a[0] * s;
    out[1] = a[1] * s;
    out[2] = a[2] * s;
}

/* Unit normal of the triangle (p0,p1,p2) counter clockwise into n; returns 0 when degenerate. */
int UnitNormalVectorCCW(f32 *p0, f32 *p1, f32 *p2, f32 *n) {
    f32 e1[4];
    f32 e0[4];

    PointToPoint(e1, p0, p1);
    PointToPoint(e0, p2, p1);
    flvecOuterProduct(n, e0, e1);
    flvecNormalize(n);
    if (0.0f == flvecCalcLength(n)) {
        return 0;
    }
    return 1;
}

/* 2D cross product (XY plane) of (p1-p0) and (p2-p0); compares the two products as integers
 * so values one ulp apart count as equal. */
f32 NormalClipF3(f32 *p0, f32 *p1, f32 *p2) {
    f32 b;
    f32 a;
    s32 ia;
    s32 ib;
    s32 *pb = (s32 *)&b;

    a = (p1[0] - p0[0]) * (p2[1] - p0[1]);
    *(f32 *)pb = (p2[0] - p0[0]) * (p1[1] - p0[1]);
    ib = *pb;
    ia = *(s32 *)&a;
    if (ia == ib + 1) {
        a = *(f32 *)pb;
    }
    if (ia == ib - 1) {
        a = *(f32 *)pb;
    }
    return a - *(f32 *)pb;
}

/* Is p inside triangle abc (2D, x/y pairs)? 0 = outside or degenerate, 1 = inside a clockwise
 * triangle, 2 = inside a counter-clockwise one; edges count as inside. */
int NormalClipCheckF3(f32 *a, f32 *b, f32 *c, f32 *p) {
    f32 d = NormalClipF3(a, b, c);

    if (d == 0.0f) {
        return 0;
    }
    if (d < 0.0f) {
        if (!(NormalClipF3(a, b, p) <= 0.0f)) {
            return 0;
        }
        if (!(NormalClipF3(b, c, p) <= 0.0f)) {
            return 0;
        }
        return (u8)(((NormalClipF3(c, a, p) > 0.0f) ? 1 : 0) ^ 1);
    }
    if (NormalClipF3(a, b, p) < 0.0f) {
        return 0;
    }
    if (NormalClipF3(b, c, p) < 0.0f) {
        return 0;
    }
    return NormalClipF3(c, a, p) < 0.0f ? 0 : 2;
}

/* tri = 3 points, 3 floats apart (x, z, -); p = (x, z). 1 when p is inside. */
int PointHitCheckF3(f32 *tri, f32 *p) {
    s8 v;
    int cw = (int)NormalClipF3(tri, tri + 3, tri + 6) <= 0;
    int r;

    v = 0;
    r = NormalClipCheckF3(tri, tri + 3, tri + 6, p) & 0xFF;

    if (r != 0) {
        if (cw) {
            if (r == 1) {
                v = 1;
            } else {
                v--;
            }
        } else if (r == 2) {
            v++;
        } else {
            v--;
        }
    }
    return v > 0;
}
