/* shit4_nm (not built): the rest of f_sphr (SLPM_654.95 0x00114D90-0x0011CA70,
 * stage hit queries) written as C but not matched yet; see shit1.c (loader),
 * shit2.c (grid helpers) and shit3_nm.c (ground height queries). Function
 * by function status in docs/agents/agent-D.md. Names are guesses from the
 * code. */
#include "types.h"
#include "hit3.h"
#include "game.h"

void UnitNormalVectorCCW(f32 *, f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void flvecNormalize(f32 *);
f32 flArcCos(f32);
s32 *GetGroundTblAdrs(f32 *);
int GroundFieldInCheck(f32 *);
int PointHitCheckF3(f32 *, f32 *);

/* 0x00117BA0: is point p inside the triangle seen from direction n (all three
 * edge normals on the same side)? */
int NormalClipFace(f32 *p, f32 *tri, f32 *n) {
    f32 e[3];

    UnitNormalVectorCCW(tri, tri + 3, p, e);
    if (flvecInnerProduct(e, n) < 0.0f) return 0;
    UnitNormalVectorCCW(tri + 3, tri + 6, p, e);
    if (flvecInnerProduct(e, n) < 0.0f) return 0;
    UnitNormalVectorCCW(tri + 6, tri, p, e);
    if (flvecInnerProduct(e, n) < 0.0f) return 0;
    return 1;
}

/* 0x00117970: where does the edge e0-e1 reach plane (normal n) near p?
 * Returns 0 when both ends are on the same side, 1 / 2 and the crossing
 * point in out otherwise. */
int FaceLinePos(f32 *e, f32 *n, f32 *p, f32 *out) {
    f32 v[4];
    f32 d[6];
    int r;
    f32 *dd = d + 3;
    f32 a, b, t;

    PointToPoint(d, e + 3, e);
    PointToPoint(dd, e, e + 3);
    r = 0;
    PointToPoint(v, e, p);
    a = flvecInnerProduct(v, n);
    if (!(a < 0.0f)) r = 1;
    PointToPoint(v, e + 3, p);
    b = flvecInnerProduct(v, n);
    if (!(b < 0.0f)) r |= 2;
    switch (r) {
    case 3:
    case 0:
        r = 0;
        break;
    case 1:
        t = flvecInnerProduct(dd, n);
        if (t == 0.0f || t == -0.0f) {
            ScaleVector(v, n, a);
        } else {
            ScaleVector(v, dd, a / t);
        }
        PointToPoint(out, e, v);
        break;
    case 2:
        t = flvecInnerProduct(d, n);
        if (t == 0.0f || t == -0.0f) {
            ScaleVector(v, n, b);
        } else {
            ScaleVector(v, d, b / t);
        }
        PointToPoint(out, e + 3, v);
        break;
    }
    return r;
}

/* 0x00119010: push box v (size 6 floats: min x,y,z / max x,y,z, then extents)
 * outwards by v with limit r. Guess from the code. */
void add_vec_sub2(f32 *v, f32 *b, f32 r) {
    f32 x;

    x = v[0];
    if (!(x < 0.0f)) {
        if (b[0] + b[6] < x) {
            if (x <= r) {
                b[0] = x;
            } else {
                b[0] = r;
                b[6] = v[0] - r;
            }
        }
    } else if (!(b[3] + b[9] <= x)) {
        if (-x <= r) {
            b[3] = x;
        } else {
            b[3] = -r;
            b[9] = v[0] + r;
        }
    }
    x = v[1];
    if (!(x < 0.0f)) {
        if (b[1] + b[7] < x) {
            if (x <= r) {
                b[1] = x;
            } else {
                b[1] = r;
                b[7] = v[1] - r;
            }
        }
    } else if (!(b[4] + b[10] <= x)) {
        if (-x <= r) {
            b[4] = x;
        } else {
            b[4] = -r;
            b[10] = v[1] + r;
        }
    }
    x = v[2];
    if (!(x < 0.0f)) {
        if (b[2] + b[8] < x) {
            if (x <= r) {
                b[2] = x;
                return;
            }
            b[2] = r;
            b[8] = v[2] - r;
        }
    } else if (!(b[5] + b[11] <= x)) {
        if (-x <= r) {
            b[5] = x;
            return;
        }
        b[5] = -r;
        b[11] = v[2] + r;
    }
}

/* 0x0011C920: slide vector for a polygon pl under entity e (pos at +0xAC):
 * plane distance of the point lowered by ang/10000*scale, written to out. */
int check_slide(HPOLY *pl, u8 *e, s32 *ang, f32 *out, f32 scale) {
    f32 nn, t;
    f32 py;

    if (*ang < 0x1500) return 0;
    if (*ang == 0x4000) *ang = 0;
    py = *(f32 *)(e + 0xB0) - (f32)*ang / 10000.0f * scale;
    nn = pl->n[0] * pl->n[0] + pl->n[1] * pl->n[1] + pl->n[2] * pl->n[2];
    t = pl->d + (pl->n[0] * *(f32 *)(e + 0xAC) + pl->n[1] * py + pl->n[2] * *(f32 *)(e + 0xB4));
    out[0] = -(pl->n[0] * t / nn);
    out[1] = -(pl->n[1] * t / nn);
    out[2] = -(pl->n[2] * t / nn);
    return 1;
}

/* 0x0011C9F0: angle (0x10000 = 360 degrees) between normal n and up. */
void check_angle(f32 *n, s32 *out) {
    f32 up[3];

    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;
    *out = (s32)(0.5f + 65536.0f * flArcCos(flvecInnerProduct(n, up)) / 6.2831855f) & 0xFFFF;
}
