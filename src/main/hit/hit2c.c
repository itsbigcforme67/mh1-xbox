/* hit2c - SLPM_654.95 0x0028E440-0x0028EE40.
 * Whole file f_hit_28CE00 0x0028CE00-0x00290560 (hit2*.c, near-matches in
 * hit2_nm.c): point/sphere/capsule/plane/line intersection tests. HPK is a
 * capsule packed by hit_cap_pk (ends, radius, axis, centre, bounding radius).
 * *_m variants return a contact point or push-out vector in out. */
#include "hit2.h"

void hit_cap_cap2_sub(f32 *a, f32 *v, f32 *out, f32 len, f32 r) {
    if (len < 0.001f) {
        flvecCopy(out, a);
        return;
    }
    len = r / len;
    v[0] *= len;
    v[1] *= len;
    v[2] *= len;
    out[0] = a[0] + v[0];
    out[1] = a[1] + v[1];
    out[2] = a[2] + v[2];
}

u8 hit_sphr_cap_m(f32 *c, HPK *k, f32 *out, f32 r) {
    f32 dir[3];
    f32 v[3];
    f32 n[3];
    f32 m[3];
    f32 p[3];
    f32 q[3];
    f32 d;
    f32 rr;

    if (hit_sphr_sphr(c, k->c, r, k->cr) == 0) {
        return 0;
    }
    flvecCopy(dir, k->dir);
    v[0] = k->p0[0] - c[0];
    v[1] = k->p0[1] - c[1];
    v[2] = k->p0[2] - c[2];
    flvecOuterProduct(n, dir, v);
    flvecOuterProduct(m, dir, n);
    flvecNormalize(m);
    d = flvecInnerProduct(m, v);
    rr = r + k->r;
    if (!(flAbs(d) <= rr)) {
        return 0;
    }
    n[0] = d * m[0];
    n[1] = d * m[1];
    n[2] = d * m[2];
    p[0] = c[0] + n[0];
    p[1] = c[1] + n[1];
    p[2] = c[2] + n[2];
    v[0] = p[0] - k->p0[0];
    v[1] = p[1] - k->p0[1];
    v[2] = p[2] - k->p0[2];
    if (flvecInnerProduct(v, dir) < 0.0f) {
        if (!(flvecCalcDistance(k->p0, c) <= rr)) {
            return 0;
        }
        v[0] = c[0] - k->p0[0];
        v[1] = c[1] - k->p0[1];
        v[2] = c[2] - k->p0[2];
        flvecNormalize(v);
        v[0] *= k->r;
        v[1] *= k->r;
        v[2] *= k->r;
        out[0] = k->p0[0] + v[0];
        out[1] = k->p0[1] + v[1];
        out[2] = k->p0[2] + v[2];
        return 1;
    }
    v[0] = p[0] - k->p1[0];
    v[1] = p[1] - k->p1[1];
    v[2] = p[2] - k->p1[2];
    dir[0] = -dir[0];
    dir[1] = -dir[1];
    dir[2] = -dir[2];
    if (flvecInnerProduct(v, dir) < 0.0f) {
        if (!(flvecCalcDistance(k->p1, c) <= rr)) {
            return 0;
        }
        v[0] = c[0] - k->p1[0];
        v[1] = c[1] - k->p1[1];
        v[2] = c[2] - k->p1[2];
        flvecNormalize(v);
        v[0] *= k->r;
        v[1] *= k->r;
        v[2] *= k->r;
        out[0] = k->p1[0] + v[0];
        out[1] = k->p1[1] + v[1];
        out[2] = k->p1[2] + v[2];
        return 1;
    }
    if (flAbs(d) < 0.001f) {
        /* sphere centre on the axis: pick any perpendicular. Note the
         * original bumps p[1] (not q[1]) on the second try. */
        flvecCopy(q, p);
        q[0] += 5.0f;
        v[0] = q[0] - k->p1[0];
        v[1] = q[1] - k->p1[1];
        v[2] = q[2] - k->p1[2];
        flvecOuterProduct(n, v, dir);
        if (flvecInnerProduct(n, n) < 0.001f) {
            p[1] += 5.0f;
            v[0] = q[0] - k->p1[0];
            v[1] = q[1] - k->p1[1];
            v[2] = q[2] - k->p1[2];
            flvecOuterProduct(n, v, dir);
        }
        flvecOuterProduct(v, n, dir);
        flvecNormalize(v);
    } else {
        v[0] = c[0] - p[0];
        v[1] = c[1] - p[1];
        v[2] = c[2] - p[2];
        flvecNormalize(v);
    }
    v[0] *= k->r;
    v[1] *= k->r;
    v[2] *= k->r;
    out[0] = p[0] + v[0];
    out[1] = p[1] + v[1];
    out[2] = p[2] + v[2];
    return 1;
}

u8 hit_cap_sphr_m(HPK *k, f32 *c, f32 *out, f32 r) {
    f32 dir[3];
    f32 v[3];
    f32 n[3];
    f32 m[3];
    f32 rr;
    f32 d;
    f32 px;
    f32 py;
    f32 pz;
    f32 len;

    if (hit_sphr_sphr(c, k->c, r, k->cr) == 0) {
        return 0;
    }
    flvecCopy(dir, k->dir);
    v[0] = k->p0[0] - c[0];
    v[1] = k->p0[1] - c[1];
    v[2] = k->p0[2] - c[2];
    flvecOuterProduct(n, dir, v);
    flvecOuterProduct(m, dir, n);
    flvecNormalize(m);
    rr = k->r + r;
    d = flvecInnerProduct(m, v);
    if (!(flAbs(d) <= rr)) {
        return 0;
    }
    n[0] = d * m[0];
    n[1] = d * m[1];
    n[2] = d * m[2];
    px = c[0] + n[0];
    py = c[1] + n[1];
    pz = c[2] + n[2];
    v[0] = px - k->p0[0];
    v[1] = py - k->p0[1];
    v[2] = pz - k->p0[2];
    if (flvecInnerProduct(v, dir) < 0.0f) {
        v[0] = k->p0[0] - c[0];
        v[1] = k->p0[1] - c[1];
        v[2] = k->p0[2] - c[2];
        len = flvecCalcLength(v);
        if (!(len <= rr)) {
            return 0;
        }
        flvecNormalize(v);
        len = rr - len;
        out[0] = len * v[0];
        out[1] = len * v[1];
        out[2] = len * v[2];
        return 1;
    }
    v[0] = px - k->p1[0];
    v[1] = py - k->p1[1];
    v[2] = pz - k->p1[2];
    dir[0] = -dir[0];
    dir[1] = -dir[1];
    dir[2] = -dir[2];
    if (flvecInnerProduct(v, dir) < 0.0f) {
        v[0] = k->p1[0] - c[0];
        v[1] = k->p1[1] - c[1];
        v[2] = k->p1[2] - c[2];
        len = flvecCalcLength(v);
        if (!(len <= rr)) {
            return 0;
        }
        flvecNormalize(v);
        len = rr - len;
        out[0] = len * v[0];
        out[1] = len * v[1];
        out[2] = len * v[2];
        return 1;
    }
    if (d < 0.0f) {
        rr = -rr;
    }
    len = rr - d;
    out[0] = len * m[0];
    out[1] = len * m[1];
    out[2] = len * m[2];
    return 1;
}
