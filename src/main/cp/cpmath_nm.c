/* Vector / angle helpers ("cp" = Capcom, name prefix of the original). SLPM_654.95 0x00120238-0x00121380
 * (g_cpAng2Rad). 16 bit angles (65536 = full turn) to radians, rotation matrices from angle
 * triples, angle between points, small vector ops, triangle tests, and the monster part
 * swapping helpers at the end (parts_init etc.). */
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

/* Which side of triangle (a,b,c) the point p is on: 0 outside, 1 inside (CW), 2 inside (CCW). */
int NormalClipCheckF3(f32 *a, f32 *b, f32 *c, f32 *p) {
    f32 r = NormalClipF3(a, b, c);

    if (0.0f == r) {
        return 0;
    }
    if (r < 0.0f) {
        if (!(NormalClipF3(a, b, p) <= 0.0f)) {
            return 0;
        }
        if (!(NormalClipF3(b, c, p) <= 0.0f)) {
            return 0;
        }
        return !(NormalClipF3(c, a, p) <= 0.0f);
    }
    if (NormalClipF3(a, b, p) < 0.0f) {
        return 0;
    }
    if (NormalClipF3(b, c, p) < 0.0f) {
        return 0;
    }
    if (NormalClipF3(c, a, p) < 0.0f) {
        return 0;
    }
    return 2;
}

int PointHitCheckF3(f32 *tri, f32 *pt) {
    s8 st = 0;
    int neg = !(0 < (s32)NormalClipF3(tri, tri + 3, tri + 6));
    u8 c = NormalClipCheckF3(tri, tri + 3, tri + 6, pt);

    if (c != 0) {
        if (neg) {
            if (c == 1) {
                st = 1;
            } else {
                st = st - 1;
            }
        } else if (c == 2) {
            st = st + 1;
        } else {
            st = st - 1;
        }
    }
    return 0 < st;
}

void NvecFloatAdjust(f32 *out, f32 *in) {
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    if (flAbs(in[0]) < 0.001f) {
        *(s32 *)&out[0] = 0;
    }
    if (flAbs(in[1]) < 0.001f) {
        *(s32 *)&out[1] = 0;
    }
    if (flAbs(in[2]) < 0.001f) {
        *(s32 *)&out[2] = 0;
    }
}

void RotateX(MAT4 *m, f32 r) {
    MAT4 t;

    flmatInit(&t);
    flmatRotX33(&t, r);
    flmatMul33(m, &t, m);
}

void RotateY(MAT4 *m, f32 r) {
    MAT4 t;

    flmatInit(&t);
    flmatRotY33(&t, r);
    flmatMul33(m, &t, m);
}

void RotateZ(MAT4 *m, f32 r) {
    MAT4 t;

    flmatInit(&t);
    flmatRotZ33(&t, r);
    flmatMul33(m, &t, m);
}

/* out = a * t + b * (1 - t) */
void cpInterVector(f32 *out, f32 *a, f32 *b, f32 t) {
    f32 u = 1.0f - t;

    out[0] = a[0] * t + b[0] * u;
    out[1] = a[1] * t + b[1] * u;
    out[2] = a[2] * t + b[2] * u;
}

s32 AarcTan2(f32 y, f32 x) {
    return 10430.378f * flArcTan2(y, x);
}

void nlCalcPoint(f32 *out, f32 *in, f32 *m) {
    flvecApplyMat33(out, in, m);
    out[0] += m[12];
    out[1] += m[13];
    out[2] += m[14];
}

f32 CalcDistanceXZ(f32 *a, f32 *b) {
    f32 v[6];

    SetVector(v, a[0], 0, a[2]);
    SetVector(v + 3, b[0], 0, b[2]);
    return flvecCalcDistance(v, v + 3);
}

/* Hair/cloth part visibility: 0x4E6+n bytes of the work, one visible variant out of a group.
 * kind 0xE selects 4 parts starting at 2? (guess: part group tables inlined), 0x12 selects 2 parts. */
void parts_chg(u8 *w, u8 kind, u8 which) {
    u8 first;
    u8 num;
    u8 i;

    if (kind == 0xE) {
        num = 2;
        first = 4;
    } else if (kind == 0x12) {
        num = 2;
        first = 2;
    } else {
        return;
    }
    for (i = first; i < first + num; i++) {
        if (i == which + first) {
            w[0x4E6 + i] = 1;
        } else {
            w[0x4E6 + i] = 0;
        }
    }
}

/* Clears the four hair sway slots (0x20 bytes each) at 0x624..0x63F. */
void yure_init(u8 *w) {
    s16 i;

    for (i = 0; i < 4; i++) {
        *(s32 *)(w + 0x628) = 0;
        *(s32 *)(w + 0x62C) = 0;
        *(s32 *)(w + 0x630) = 0;
        *(s32 *)(w + 0x634) = 0;
        *(s32 *)(w + 0x638) = 0;
        *(s32 *)(w + 0x63C) = 0;
        w[0x624] = 0;
        w[0x625] = 0;
        w[0x626] = 0;
        w[0x627] = 0;
        w += 0x20;
    }
}
