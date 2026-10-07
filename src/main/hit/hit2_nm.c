/* hit2_nm - SLPM_654.95 0x0028CE00-0x00290560, whole file (not built).
 * Whole file f_hit_28CE00 0x0028CE00-0x00290560 (hit2*.c, near-matches in
 * hit2_nm.c): point/sphere/capsule/plane/line intersection tests. HPK is a
 * capsule packed by hit_cap_pk (ends, radius, axis, centre, bounding radius).
 * Near-matches (logic complete):
 *   hit_sphr_sphr2  18/64 off (scheduling)
 *   hit_cap_cap2_m  41/1253 off (t2/h float regs and i/j int regs swapped)
 *   hit_cap_cap3_m  90/945 off (same register swaps as hit_cap_cap2_m)
 *   hit_cap_sphr2_m, hit_line_sphr2: match here, where hit_point_sphr is a
 *   static of the same file, but not when built split (MWCC then assumes
 *   the full ABI clobber set for the call).
 * *_m variants return a contact point or push-out vector in out. */
#include "types.h"
#include "hit.h"

f32 flvecCalcDistance(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
f32 flAbs(f32);
f32 flSqrt(f32);
void flvecCopy(f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);

/* Local (static) in the original. */
static u8 hit_point_sphr(f32 *p, f32 *c, f32 r);
static void hit_cap_cap2_sub(f32 *a, f32 *v, f32 *out, f32 len, f32 r);
static void hit_cap_cap3_sub(f32 *v, f32 *out, f32 r, f32 d);
static void p_point_line2(f32 *p0, f32 *p1, f32 *dir, f32 *pt, f32 *out, f32 len);

u8 hit_sphr_sphr(f32 *a, f32 *b, f32 ra, f32 rb);


static u8 hit_point_sphr(f32 *p, f32 *c, f32 r) {
    f32 dx = c[0] - p[0];
    f32 dy = c[1] - p[1];
    f32 dz = c[2] - p[2];

    return dx * dx + dy * dy + dz * dz <= r * r;
}

u8 hit_sphr_sphr(f32 *a, f32 *b, f32 ra, f32 rb) {
    f32 dx = a[0] - b[0];
    f32 dy = a[1] - b[1];
    f32 dz = a[2] - b[2];
    f32 r = ra + rb;

    return dx * dx + dy * dy + dz * dz <= r * r;
}

u8 hit_sphr_sphr2(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb) {
    f32 d = flvecCalcDistance(a, b);
    f32 t;
    f32 dx;
    f32 dy;
    f32 dz;

    if (d <= ra + rb) {
        if (!(d <= 0.001f)) {
            t = rb / d;
            dx = a[0] - b[0];
            dy = a[1] - b[1];
            dz = a[2] - b[2];
            out[0] = dx * t + b[0];
            out[1] = dy * t + b[1];
            out[2] = dz * t + b[2];
        } else {
            flvecCopy(out, b);
        }
        return 1;
    }
    return 0;
}

u8 hit_sphr_sphr3(f32 *a, f32 *b, f32 *out, f32 ra, f32 rb) {
    f32 d = flvecCalcDistance(a, b);
    f32 r = ra + rb;
    f32 t;
    f32 dx;
    f32 dy;
    f32 dz;

    if (d <= r) {
        if (!(d <= 0.001f)) {
            t = (r - d) / d;
            dx = a[0] - b[0];
            dy = a[1] - b[1];
            dz = a[2] - b[2];
            out[0] = t * dx;
            out[1] = t * dy;
            out[2] = t * dz;
        } else {
            out[0] = 0.0f;
            out[1] = 0.0f;
            out[2] = 0.0f;
        }
        return 1;
    }
    return 0;
}

u8 hit_cap_cap2_m(HPK *k1, HPK *k2, f32 *out) {
    f32 t;
    f32 d1[3];
    f32 d2[3];
    f32 v[3];
    f32 w[3];
    f32 q2[3];
    f32 q1[3];
    f32 n[3];
    f32 len1;
    f32 rr;
    f32 dot;
    f32 t2;
    f32 len2;
    f32 h;
    f32 u;
    f32 a;
    f32 s1;
    f32 s2;
    f32 den;
    f32 e;
    u8 i;
    u8 j;

    if (hit_point_sphr(k1->c, k2->c, k1->cr + k2->cr) == 0) {
        return 0;
    }
    rr = k1->r + k2->r;
    len1 = 2.0f * (k1->cr - k1->r);
    if (len1 < 0.001f) {
        len1 = 0.001f;
    }
    len2 = 2.0f * (k2->cr - k2->r);
    if (len2 < 0.001f) {
        len2 = 0.001f;
    }
    d1[0] = k1->dir[0] / len1;
    d1[1] = k1->dir[1] / len1;
    d1[2] = k1->dir[2] / len1;
    d2[0] = k2->dir[0] / len2;
    d2[1] = k2->dir[1] / len2;
    d2[2] = k2->dir[2] / len2;
    dot = flvecInnerProduct(d1, d2);
    if (1.0f - flAbs(dot) < 0.001f) {
        /* parallel */
        v[0] = k1->p0[0] - k2->p0[0];
        v[1] = k1->p0[1] - k2->p0[1];
        v[2] = k1->p0[2] - k2->p0[2];
        t = flvecInnerProduct(v, d2);
        t2 = t * t;
        h = flvecInnerProduct(v, v) - t2;
        if (h < 0.0f) {
            return 0;
        }
        h = flSqrt(h);
        if (!(h <= rr)) {
            return 0;
        }
        if (h < 0.001f) {
            w[0] = d2[0] + 5.0f;
            w[1] = d2[1];
            w[2] = d2[2];
            flvecOuterProduct(n, d2, w);
            if (flAbs(n[0]) < 0.001f && flAbs(n[1]) < 0.001f && flAbs(n[2]) < 0.001f) {
                w[1] += 5.0f;
                flvecOuterProduct(n, d2, w);
            }
            flvecNormalize(n);
            n[0] *= k2->r;
            n[1] *= k2->r;
            n[2] *= k2->r;
        } else {
            n[0] = t * d2[0];
            n[1] = t * d2[1];
            n[2] = t * d2[2];
            q2[0] = k2->p0[0] + n[0];
            q2[1] = k2->p0[1] + n[1];
            q2[2] = k2->p0[2] + n[2];
            n[0] = k1->p0[0] - q2[0];
            n[1] = k1->p0[1] - q2[1];
            n[2] = k1->p0[2] - q2[2];
            e = k2->r / h;
            n[0] *= e;
            n[1] *= e;
            n[2] *= e;
        }
        if (t < 0.0f) {
            w[0] = k1->p1[0] - k2->p0[0];
            w[1] = k1->p1[1] - k2->p0[1];
            w[2] = k1->p1[2] - k2->p0[2];
            u = flvecInnerProduct(w, d2);
            if (!(u < 0.0f)) {
                if (u < len2) {
                    a = 0.5f * u;
                } else {
                    a = 0.5f * len2;
                }
                v[0] = a * d2[0];
                v[1] = a * d2[1];
                v[2] = a * d2[2];
                q2[0] = k2->p0[0] + v[0];
                q2[1] = k2->p0[1] + v[1];
                q2[2] = k2->p0[2] + v[2];
                out[0] = q2[0] + n[0];
                out[1] = q2[1] + n[1];
                out[2] = q2[2] + n[2];
                return 1;
            }
            if (t < u) {
                a = flvecCalcLength(w);
                if (a <= rr) {
                    if (a < 0.001f) {
                        a = 0.001f;
                    }
                    e = k2->r / a;
                    w[0] *= e;
                    w[1] *= e;
                    w[2] *= e;
                    out[0] = k2->p0[0] + w[0];
                    out[1] = k2->p0[1] + w[1];
                    out[2] = k2->p0[2] + w[2];
                    return 1;
                }
                return 0;
            }
            a = flvecCalcLength(v);
            if (a <= rr) {
                if (a < 0.001f) {
                    a = 0.001f;
                }
                e = k2->r / a;
                v[0] *= e;
                v[1] *= e;
                v[2] *= e;
                out[0] = k2->p0[0] + v[0];
                out[1] = k2->p0[1] + v[1];
                out[2] = k2->p0[2] + v[2];
                return 1;
            }
            return 0;
        }
        if (t <= len2) {
            w[0] = k1->p1[0] - k2->p0[0];
            w[1] = k1->p1[1] - k2->p0[1];
            w[2] = k1->p1[2] - k2->p0[2];
            u = flvecInnerProduct(w, d2);
            if (u < 0.0f) {
                a = 0.5f * t;
            } else if (!(u <= len2)) {
                a = t + 0.5f * (len2 - t);
            } else {
                a = t + 0.5f * (u - t);
            }
            v[0] = a * d2[0];
            v[1] = a * d2[1];
            v[2] = a * d2[2];
            q2[0] = k2->p0[0] + v[0];
            q2[1] = k2->p0[1] + v[1];
            q2[2] = k2->p0[2] + v[2];
            out[0] = q2[0] + n[0];
            out[1] = q2[1] + n[1];
            out[2] = q2[2] + n[2];
            return 1;
        }
        w[0] = k1->p1[0] - k2->p0[0];
        w[1] = k1->p1[1] - k2->p0[1];
        w[2] = k1->p1[2] - k2->p0[2];
        u = flvecInnerProduct(w, d2);
        if (u < 0.0f) {
            out[0] = k2->c[0] + n[0];
            out[1] = k2->c[1] + n[1];
            out[2] = k2->c[2] + n[2];
            return 1;
        }
        if (u <= len2) {
            a = u + 0.5f * (len2 - u);
            v[0] = a * d2[0];
            v[1] = a * d2[1];
            v[2] = a * d2[2];
            q2[0] = k2->p0[0] + v[0];
            q2[1] = k2->p0[1] + v[1];
            q2[2] = k2->p0[2] + v[2];
            out[0] = q2[0] + n[0];
            out[1] = q2[1] + n[1];
            out[2] = q2[2] + n[2];
            return 1;
        }
        if (t < u) {
            a = flSqrt(h * h + t2);
            if (a <= rr) {
                if (a < 0.001f) {
                    a = 0.001f;
                }
                e = k2->r / a;
                v[0] *= e;
                v[1] *= e;
                v[2] *= e;
                out[0] = k2->p0[0] + v[0];
                out[1] = k2->p0[1] + v[1];
                out[2] = k2->p0[2] + v[2];
                return 1;
            }
            return 0;
        }
        a = flSqrt(h * h + u * u);
        if (a <= rr) {
            if (a < 0.001f) {
                a = 0.001f;
            }
            e = k2->r / a;
            w[0] *= e;
            w[1] *= e;
            w[2] *= e;
            out[0] = k2->p0[0] + w[0];
            out[1] = k2->p0[1] + w[1];
            out[2] = k2->p0[2] + w[2];
            return 1;
        }
        return 0;
    }
    /* skew: closest points of the two infinite lines */
    v[0] = k2->p0[0] - k1->p0[0];
    v[1] = k2->p0[1] - k1->p0[1];
    v[2] = k2->p0[2] - k1->p0[2];
    t = flvecInnerProduct(v, d1);
    u = flvecInnerProduct(v, d2);
    den = 1.0f - dot * dot;
    s1 = (t - u * dot) / den;
    s2 = (t * dot - u) / den;
    if (s1 < 0.0f) {
        i = 0;
    } else if (s1 <= len1) {
        i = 1;
    } else {
        i = 2;
    }
    if (s2 < 0.0f) {
        j = 0;
    } else if (s2 <= len2) {
        j = 1;
    } else {
        j = 2;
    }
    switch (i) {
    case 0:
        p_point_line2(k2->p0, k2->p1, d2, k1->p0, q2, len2);
        v[0] = k1->p0[0] - q2[0];
        v[1] = k1->p0[1] - q2[1];
        v[2] = k1->p0[2] - q2[2];
        h = flvecCalcLength(v);
        switch (j) {
        case 0:
            p_point_line2(k1->p0, k1->p1, d1, k2->p0, q1, len1);
            w[0] = q1[0] - k2->p0[0];
            w[1] = q1[1] - k2->p0[1];
            w[2] = q1[2] - k2->p0[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap2_sub(q2, v, out, h, k2->r);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap2_sub(k2->p0, w, out, a, k2->r);
                return 1;
            }
            return 0;
        case 1:
            if (h <= rr) {
                hit_cap_cap2_sub(q2, v, out, h, k2->r);
                return 1;
            }
            return 0;
        case 2:
            p_point_line2(k1->p0, k1->p1, d1, k2->p1, q1, len1);
            w[0] = q1[0] - k2->p1[0];
            w[1] = q1[1] - k2->p1[1];
            w[2] = q1[2] - k2->p1[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap2_sub(q2, v, out, h, k2->r);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap2_sub(k2->p1, w, out, a, k2->r);
                return 1;
            }
            return 0;
        }
        break;
    case 1:
        switch (j) {
        case 0:
            p_point_line2(k1->p0, k1->p1, d1, k2->p0, q2, len1);
            v[0] = q2[0] - k2->p0[0];
            v[1] = q2[1] - k2->p0[1];
            v[2] = q2[2] - k2->p0[2];
            a = flvecCalcLength(v);
            if (a <= rr) {
                hit_cap_cap2_sub(k2->p0, v, out, a, k2->r);
                return 1;
            }
            return 0;
        case 1:
            v[0] = s1 * d1[0];
            v[1] = s1 * d1[1];
            v[2] = s1 * d1[2];
            q2[0] = k1->p0[0] + v[0];
            q2[1] = k1->p0[1] + v[1];
            q2[2] = k1->p0[2] + v[2];
            v[0] = s2 * d2[0];
            v[1] = s2 * d2[1];
            v[2] = s2 * d2[2];
            q1[0] = k2->p0[0] + v[0];
            q1[1] = k2->p0[1] + v[1];
            q1[2] = k2->p0[2] + v[2];
            v[0] = q2[0] - q1[0];
            v[1] = q2[1] - q1[1];
            v[2] = q2[2] - q1[2];
            a = flvecCalcLength(v);
            if (a <= rr) {
                if (a < 0.001f) {
                    flvecOuterProduct(v, d1, d2);
                    v[0] *= k2->r;
                    v[1] *= k2->r;
                    v[2] *= k2->r;
                    out[0] = q1[0] + v[0];
                    out[1] = q1[1] + v[1];
                    out[2] = q1[2] + v[2];
                } else {
                    hit_cap_cap2_sub(q1, v, out, a, k2->r);
                }
                return 1;
            }
            return 0;
        case 2:
            p_point_line2(k1->p0, k1->p1, d1, k2->p1, q2, len1);
            v[0] = q2[0] - k2->p1[0];
            v[1] = q2[1] - k2->p1[1];
            v[2] = q2[2] - k2->p1[2];
            a = flvecCalcLength(v);
            if (a <= rr) {
                hit_cap_cap2_sub(k2->p1, v, out, a, k2->r);
                return 1;
            }
            return 0;
        }
        break;
    case 2:
        p_point_line2(k2->p0, k2->p1, d2, k1->p1, q2, len2);
        v[0] = k1->p1[0] - q2[0];
        v[1] = k1->p1[1] - q2[1];
        v[2] = k1->p1[2] - q2[2];
        h = flvecCalcLength(v);
        switch (j) {
        case 0:
            p_point_line2(k1->p0, k1->p1, d1, k2->p0, q1, len1);
            w[0] = q1[0] - k2->p0[0];
            w[1] = q1[1] - k2->p0[1];
            w[2] = q1[2] - k2->p0[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap2_sub(q2, v, out, h, k2->r);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap2_sub(k2->p0, w, out, a, k2->r);
                return 1;
            }
            return 0;
        case 1:
            if (h <= rr) {
                hit_cap_cap2_sub(q2, v, out, h, k2->r);
                return 1;
            }
            return 0;
        case 2:
            p_point_line2(k1->p0, k1->p1, d1, k2->p1, q1, len1);
            w[0] = q1[0] - k2->p1[0];
            w[1] = q1[1] - k2->p1[1];
            w[2] = q1[2] - k2->p1[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap2_sub(q2, v, out, h, k2->r);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap2_sub(k2->p1, w, out, a, k2->r);
                return 1;
            }
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

static void hit_cap_cap2_sub(f32 *a, f32 *v, f32 *out, f32 len, f32 r) {
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

u8 hit_cap_sphr2_m(HPK *k, f32 *c, f32 *out, f32 r) {
    f32 dir[3];
    f32 v[3];
    f32 n[3];
    f32 m[3];
    f32 rr;
    f32 d;
    f32 px;
    f32 py;
    f32 pz;

    if (hit_point_sphr(c, k->c, k->cr + r) == 0) {
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
        if (!(flvecCalcLength(v) <= rr)) {
            return 0;
        }
        flvecNormalize(v);
        out[0] = r * v[0];
        out[1] = r * v[1];
        out[2] = r * v[2];
        out[0] += c[0];
        out[1] += c[1];
        out[2] += c[2];
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
        if (!(flvecCalcLength(v) <= rr)) {
            return 0;
        }
        flvecNormalize(v);
        out[0] = r * v[0];
        out[1] = r * v[1];
        out[2] = r * v[2];
        out[0] += c[0];
        out[1] += c[1];
        out[2] += c[2];
        return 1;
    }
    out[0] = -r * m[0];
    out[1] = -r * m[1];
    out[2] = -r * m[2];
    out[0] += c[0];
    out[1] += c[1];
    out[2] += c[2];
    return 1;
}

u8 hit_cap_cap3_m(HPK *k1, HPK *k2, f32 *out) {
    f32 t;
    f32 d1[3];
    f32 d2[3];
    f32 v[3];
    f32 w[3];
    f32 q2[3];
    f32 q1[3];
    f32 n[3];
    f32 len1;
    f32 rr;
    f32 dot;
    f32 t2;
    f32 len2;
    f32 h;
    f32 u;
    f32 a;
    f32 s1;
    f32 s2;
    f32 den;
    f32 e;
    u8 i;
    u8 j;

    if (hit_point_sphr(k1->c, k2->c, k1->cr + k2->cr) == 0) {
        return 0;
    }
    rr = k1->r + k2->r;
    len1 = 2.0f * (k1->cr - k1->r);
    if (len1 < 0.001f) {
        len1 = 0.001f;
    }
    len2 = 2.0f * (k2->cr - k2->r);
    if (len2 < 0.001f) {
        len2 = 0.001f;
    }
    d1[0] = k1->dir[0] / len1;
    d1[1] = k1->dir[1] / len1;
    d1[2] = k1->dir[2] / len1;
    d2[0] = k2->dir[0] / len2;
    d2[1] = k2->dir[1] / len2;
    d2[2] = k2->dir[2] / len2;
    dot = flvecInnerProduct(d1, d2);
    if (1.0f - flAbs(dot) < 0.001f) {
        /* parallel */
        v[0] = k2->p0[0] - k1->p0[0];
        v[1] = k2->p0[1] - k1->p0[1];
        v[2] = k2->p0[2] - k1->p0[2];
        t = flvecInnerProduct(v, d1);
        t2 = t * t;
        h = flvecInnerProduct(v, v) - t2;
        if (h < 0.0f) {
            return 0;
        }
        h = flSqrt(h);
        if (!(h <= rr)) {
            return 0;
        }
        if (h < 0.001f) {
            w[0] = d1[0] + 5.0f;
            w[1] = d1[1];
            w[2] = d1[2];
            flvecOuterProduct(n, d1, w);
            if (flAbs(n[0]) < 0.001f && flAbs(n[1]) < 0.001f && flAbs(n[2]) < 0.001f) {
                w[1] += 5.0f;
                flvecOuterProduct(n, d1, w);
            }
            flvecNormalize(n);
            n[0] *= rr;
            n[1] *= rr;
            n[2] *= rr;
        } else {
            e = (rr - h) / h;
            n[0] = t * d1[0];
            n[1] = t * d1[1];
            n[2] = t * d1[2];
            q2[0] = k1->p0[0] + n[0];
            q2[1] = k1->p0[1] + n[1];
            q2[2] = k1->p0[2] + n[2];
            n[0] = k2->p0[0] - q2[0];
            n[1] = k2->p0[1] - q2[1];
            n[2] = k2->p0[2] - q2[2];
            n[0] *= e;
            n[1] *= e;
            n[2] *= e;
        }
        if (t < 0.0f) {
            w[0] = k2->p1[0] - k1->p0[0];
            w[1] = k2->p1[1] - k1->p0[1];
            w[2] = k2->p1[2] - k1->p0[2];
            u = flvecInnerProduct(w, d1);
            if (!(u < 0.0f)) {
                flvecCopy(out, n);
                return 1;
            }
            if (t < u) {
                a = flvecCalcLength(w);
                if (a <= rr) {
                    e = (rr - a) / a;
                    out[0] = e * w[0];
                    out[1] = e * w[1];
                    out[2] = e * w[2];
                    return 1;
                }
                return 0;
            }
            a = flvecCalcLength(v);
            if (a <= rr) {
                e = (rr - a) / a;
                out[0] = e * v[0];
                out[1] = e * v[1];
                out[2] = e * v[2];
                return 1;
            }
            return 0;
        }
        if (t <= len2) {
            flvecCopy(out, n);
            return 1;
        }
        w[0] = k1->p1[0] - k2->p0[0];
        w[1] = k1->p1[1] - k2->p0[1];
        w[2] = k1->p1[2] - k2->p0[2];
        u = flvecInnerProduct(w, d2);
        if (u <= len2) {
            flvecCopy(out, n);
            return 1;
        }
        if (t < u) {
            a = flSqrt(h * h + t2);
            if (a <= rr) {
                e = (rr - a) / a;
                out[0] = e * v[0];
                out[1] = e * v[1];
                out[2] = e * v[2];
                return 1;
            }
            return 0;
        }
        a = flSqrt(h * h + u * u);
        if (a <= rr) {
            e = (rr - a) / a;
            out[0] = e * w[0];
            out[1] = e * w[1];
            out[2] = e * w[2];
            return 1;
        }
        return 0;
    }
    /* skew: closest points of the two infinite lines */
    v[0] = k2->p0[0] - k1->p0[0];
    v[1] = k2->p0[1] - k1->p0[1];
    v[2] = k2->p0[2] - k1->p0[2];
    t = flvecInnerProduct(v, d1);
    u = flvecInnerProduct(v, d2);
    den = 1.0f - dot * dot;
    s1 = (t - u * dot) / den;
    s2 = (t * dot - u) / den;
    if (s1 < 0.0f) {
        i = 0;
    } else if (s1 <= len1) {
        i = 1;
    } else {
        i = 2;
    }
    if (s2 < 0.0f) {
        j = 0;
    } else if (s2 <= len2) {
        j = 1;
    } else {
        j = 2;
    }
    switch (i) {
    case 0:
        p_point_line2(k2->p0, k2->p1, d2, k1->p0, q2, len2);
        v[0] = q2[0] - k1->p0[0];
        v[1] = q2[1] - k1->p0[1];
        v[2] = q2[2] - k1->p0[2];
        h = flvecCalcLength(v);
        switch (j) {
        case 0:
            p_point_line2(k1->p0, k1->p1, d1, k2->p0, q1, len1);
            w[0] = k2->p0[0] - q1[0];
            w[1] = k2->p0[1] - q1[1];
            w[2] = k2->p0[2] - q1[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap3_sub(v, out, rr, h);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap3_sub(w, out, rr, a);
                return 1;
            }
            return 0;
        case 1:
            if (h <= rr) {
                hit_cap_cap3_sub(v, out, rr, h);
                return 1;
            }
            return 0;
        case 2:
            p_point_line2(k1->p0, k1->p1, d1, k2->p1, q1, len1);
            w[0] = k2->p1[0] - q1[0];
            w[1] = k2->p1[1] - q1[1];
            w[2] = k2->p1[2] - q1[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap3_sub(v, out, rr, h);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap3_sub(w, out, rr, a);
                return 1;
            }
            return 0;
        }
        break;
    case 1:
        switch (j) {
        case 0:
            p_point_line2(k1->p0, k1->p1, d1, k2->p0, q2, len1);
            v[0] = k2->p0[0] - q2[0];
            v[1] = k2->p0[1] - q2[1];
            v[2] = k2->p0[2] - q2[2];
            a = flvecCalcLength(v);
            if (a <= rr) {
                hit_cap_cap3_sub(v, out, rr, a);
                return 1;
            }
            return 0;
        case 1:
            v[0] = s1 * d1[0];
            v[1] = s1 * d1[1];
            v[2] = s1 * d1[2];
            q2[0] = k1->p0[0] + v[0];
            q2[1] = k1->p0[1] + v[1];
            q2[2] = k1->p0[2] + v[2];
            v[0] = s2 * d2[0];
            v[1] = s2 * d2[1];
            v[2] = s2 * d2[2];
            q1[0] = k2->p0[0] + v[0];
            q1[1] = k2->p0[1] + v[1];
            q1[2] = k2->p0[2] + v[2];
            v[0] = q1[0] - q2[0];
            v[1] = q1[1] - q2[1];
            v[2] = q1[2] - q2[2];
            a = flvecCalcLength(v);
            if (a <= rr) {
                if (a < 0.001f) {
                    flvecOuterProduct(v, d1, d2);
                    out[0] = rr * v[0];
                    out[1] = rr * v[1];
                    out[2] = rr * v[2];
                } else {
                    hit_cap_cap3_sub(v, out, rr, a);
                }
                return 1;
            }
            return 0;
        case 2:
            p_point_line2(k1->p0, k1->p1, d1, k2->p1, q2, len1);
            v[0] = k2->p1[0] - q2[0];
            v[1] = k2->p1[1] - q2[1];
            v[2] = k2->p1[2] - q2[2];
            a = flvecCalcLength(v);
            if (a <= rr) {
                hit_cap_cap3_sub(v, out, rr, a);
                return 1;
            }
            return 0;
        }
        break;
    case 2:
        p_point_line2(k2->p0, k2->p1, d2, k1->p1, q2, len2);
        v[0] = q2[0] - k1->p1[0];
        v[1] = q2[1] - k1->p1[1];
        v[2] = q2[2] - k1->p1[2];
        h = flvecCalcLength(v);
        switch (j) {
        case 0:
            p_point_line2(k1->p0, k1->p1, d1, k2->p0, q1, len1);
            w[0] = k2->p0[0] - q1[0];
            w[1] = k2->p0[1] - q1[1];
            w[2] = k2->p0[2] - q1[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap3_sub(v, out, rr, h);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap3_sub(w, out, rr, a);
                return 1;
            }
            return 0;
        case 1:
            if (h <= rr) {
                hit_cap_cap3_sub(v, out, rr, h);
                return 1;
            }
            return 0;
        case 2:
            p_point_line2(k1->p0, k1->p1, d1, k2->p1, q1, len1);
            w[0] = k2->p1[0] - q1[0];
            w[1] = k2->p1[1] - q1[1];
            w[2] = k2->p1[2] - q1[2];
            a = flvecCalcLength(w);
            if (h < a) {
                if (h <= rr) {
                    hit_cap_cap3_sub(v, out, rr, h);
                    return 1;
                }
                return 0;
            }
            if (a <= rr) {
                hit_cap_cap3_sub(w, out, rr, a);
                return 1;
            }
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

static void hit_cap_cap3_sub(f32 *v, f32 *out, f32 r, f32 d) {
    f32 t;

    if (d != 0.0f) {
        t = r - d;
        if (d < 0.001f) {
            d = 0.001f;
        }
        t = t / d;
        out[0] = t * v[0];
        out[1] = t * v[1];
        out[2] = t * v[2];
    }
}

u8 hit_sphr_pln(f32 *c, f32 *p, f32 *n, f32 *out, f32 r) {
    f32 v[3];
    f32 nn[3];
    f32 d;

    v[0] = p[0] - c[0];
    v[1] = p[1] - c[1];
    v[2] = p[2] - c[2];
    flvecCopy(nn, n);
    flvecNormalize(nn);
    d = flvecInnerProduct(nn, v);
    if (d <= 0.0f && !(-d <= r)) {
        return 0;
    }
    d = d + r;
    out[0] = d * nn[0];
    out[1] = d * nn[1];
    out[2] = d * nn[2];
    return 1;
}

u8 hit_line_sphr2(HLINE *l, f32 *c, f32 r) {
    f32 d[3];
    f32 v[3];
    int a;
    int b;
    f32 t;

    if (hit_point_sphr(l->mid, c, r + l->half) == 0) {
        return 0;
    }
    a = hit_point_sphr(l->p0, c, r) != 0;
    b = hit_point_sphr(l->p1, c, r) != 0;
    if (a && b) {
        return 2;
    }
    if (a || b) {
        return 1;
    }
    flvecCopy(d, l->dir);
    flvecNormalize(d);
    v[0] = c[0] - l->p0[0];
    v[1] = c[1] - l->p0[1];
    v[2] = c[2] - l->p0[2];
    t = flvecInnerProduct(v, d);
    if (t < 0.0f) {
        return 0;
    }
    if (!(t <= 2.0f * l->half)) {
        return 0;
    }
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] - t * t <= r * r;
}

static void p_point_line2(f32 *p0, f32 *p1, f32 *dir, f32 *pt, f32 *out, f32 len) {
    f32 v[3];
    f32 t;

    v[0] = pt[0] - p0[0];
    v[1] = pt[1] - p0[1];
    v[2] = pt[2] - p0[2];
    t = flvecInnerProduct(v, dir);
    if (t < 0.0f) {
        flvecCopy(out, p0);
    } else if (!(t <= len)) {
        flvecCopy(out, p1);
    } else {
        v[0] = t * dir[0];
        v[1] = t * dir[1];
        v[2] = t * dir[2];
        out[0] = p0[0] + v[0];
        out[1] = p0[1] + v[1];
        out[2] = p0[2] + v[2];
    }
}
