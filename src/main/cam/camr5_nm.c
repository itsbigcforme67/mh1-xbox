/* camr5_nm (not built): DKA5 153/158, Cardano 99/203, k_HitWallCamera 46/104,
 * k_HitEmCamera 446/538 instructions differ (register choices, branch layout);
 * logic complete. DKA5 needs the d*Complex helpers defined earlier in the
 * same file to match (the original assumes a2 survives dMulComplex). */
/* camr5 - SLPM_654.95 0x00224D40-0x002258xx (f_cam_223B50, rail camera):
 * DKA5 finds the five complex roots of a quintic (Durand-Kerner, 26 rounds,
 * start radius from a Cauchy-style bound), Cardano solves a cubic (1-3 real
 * roots), k_HitWallCamera / k_HitEmCamera push the camera out of walls and
 * out of monsters (spheres/capsules, push01 table of 4 floats {x,y,z,r},
 * r == -1 ends it). Everything guessed from the code; not verified. */
#include "types.h"
#include "hit2.h"
#include "em.h"
#include "game.h"

#define ABS_(x) flAbs(x)

typedef struct DCMPLX {
    f32 re;
    f32 im;
} DCMPLX;

void dCnvComplex(DCMPLX *c, f32 re, f32 im);
void dSubComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b);
void dMulComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b);
void dDivComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b);
f32 flPow(f32, f32);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
f32 flCos(f32);
f32 flSin(f32);
extern f32 dka_init_tbl[10];
extern f32 dka_j_tbl[6];

void DKA5(DCMPLX *z, f32 *a) {
    f32 c[5];
    DCMPLX p;
    DCMPLX d;
    DCMPLX zi;
    DCMPLX t;
    DCMPLX q;
    f32 r;
    f32 m;
    f32 inv;
    int i, j, it, k;

    inv = 1.0f / a[0];
    c[0] = inv * a[1];
    c[1] = inv * a[2];
    c[2] = inv * a[3];
    c[3] = inv * a[4];
    c[4] = inv * a[5];
    r = 0.0f;
    for (j = 2; j < 6; j++) {
        m = flPow(flAbs(c[j - 1]), dka_j_tbl[j]);
        if (!(m <= r)) r = m;
    }
    r = r * 5.0f;
    for (k = 0; k < 10; k++) {
        ((f32 *)z)[k] = r * dka_init_tbl[k];
    }
    it = 0x19;
    do {
        for (i = 0; i < 5; i++) {
            dCnvComplex(&d, 1.0f, 0.0f);
            dCnvComplex(&p, 1.0f, 0.0f);
            zi = z[i];
            for (j = 0; j < 5; j++) {
                dMulComplex(&p, &p, &zi);
                p.re = p.re + c[j];
                if (j != i) {
                    dSubComplex(&t, &zi, &z[j]);
                    dMulComplex(&d, &d, &t);
                }
            }
            dDivComplex(&q, &p, &d);
            dSubComplex(&z[i], &zi, &q);
        }
    } while (--it >= 0);
}

int Cardano(f32 *roots, f32 *c) {
    f32 inv;
    f32 a, b, cc;
    f32 sh, p, q, D, u, r, ang, cs, sn;

    inv = 1.0f / c[0];
    a = c[1] * inv;
    b = c[2] * inv;
    cc = c[3] * inv;
    sh = (1.0f / 3.0f) * a;
    p = (1.0f / 3.0f) * b - sh * sh;
    q = 0.5f * (cc + sh * (sh * sh + sh * sh - b));
    if ((double)flAbs(p) < 1.0e-6 && (double)flAbs(q) < 1.0e-6) {
        roots[0] = -sh;
        return 1;
    }
    D = p * p * p + q * q;
    if (D > 0.0f) {
        if (!(q < 0.0f)) {
            u = flPow(q + flSqrt(D), 1.0f / 3.0f);
        } else {
            u = -flPow(-q + flSqrt(D), 1.0f / 3.0f);
        }
        if (p < 0.0f) {
            roots[0] = p / u - u;
        } else {
            roots[0] = (-2.0f * q * (u * u)) / (p * p + (u * u) * (u * u + p));
        }
        roots[0] = roots[0] - sh;
        return 1;
    }
    if (q < 0.0f) {
        r = flSqrt(-p);
    } else {
        r = -flSqrt(-p);
    }
    if ((double)flAbs(D) < 1.0e-6) {
        roots[0] = 2.0f * r - sh;
        roots[1] = -r - sh;
        return 2;
    }
    ang = (1.0f / 3.0f) * flArcTan2(flSqrt(-D), -q);
    cs = r * flCos(ang);
    sn = r * flSin(ang);
    roots[0] = 2.0f * cs - sh;
    roots[1] = (-cs - 1.7320508f * sn) - sh;
    roots[2] = (-cs + 1.7320508f * sn) - sh;
    return 3;
}

extern f32 push01[][4];
void SetVector(f32 *, f32, f32, f32);
u8 GetWallHitBit2(f32, f32 *, f32 *, f32 *, int);

f32 k_HitWallCamera(f32 *cam, f32 *tar, f32 *dist) {
    f32 a[3];
    f32 b[3];
    f32 c[3];
    f32 (*pp)[4] = push01;
    f32 *ay = &a[1];
    f32 *az = &a[2];
    f32 *by = &b[1];
    f32 *bz = &b[2];
    u8 hit;

    if (pp[0][3] != -1.0f) {
        do {
            a[0] = tar[0] + (*pp)[0];
            *ay = tar[1] + (*pp)[1];
            *az = tar[2] + (*pp)[2];
            b[0] = cam[0] + (*pp)[0];
            *by = cam[1] + (*pp)[1];
            *bz = cam[2] + (*pp)[2];
            SetVector(c, cam[0], cam[1], cam[2]);
            if (game_w.gate_open != 0) {
                hit = GetWallHitBit2((*pp)[3], a, b, cam, 0xC001);
            } else {
                hit = GetWallHitBit2((*pp)[3], a, b, cam, 0x8001);
            }
            if (hit != 0) {
                *dist = flvecCalcDistance(c, cam);
            } else {
                *dist = 0.0f;
            }
            pp++;
        } while ((*pp)[3] != -1.0f);
    }
    return 0.0f;
}

extern HBODY *D_63FA10[];
extern HBODY *D_610370[];
f32 GetGroundHit(f32 *);
u8 hit_sphr_sphr3(f32 *, f32 *, f32 *, f32, f32);

/* Fold one push vector component: keep the larger when the signs differ,
 * subtract when they agree (the original expands this by hand). */
#define PUSH_ACC(a, b)                              \
    if (((a) < 0.0f) != ((b) < 0.0f)) {             \
        if (flAbs(a) < flAbs(b)) (a) = -(b);        \
    } else {                                        \
        (a) -= (b);                                 \
    }

void k_HitEmCamera(f32 *cam) {
    HBODY *body;
    HCAP cap;
    HSPH sph;
    HPK pk;
    f32 pos[3];
    f32 out[3];
    f32 (*pp)[4];
    EMW *em;
    f32 px = 0.0f, py = 0.0f, pz = 0.0f;
    int i;
    int t;

    em = em_work;
    for (i = 0; i < 0x14; i++, em++) {
        if (em->be_flag != 0 && em->x10 != 0 && em->x01 != 0 && game_w.stage == em->stg) {
            pp = push01;
            if (game_w.x1DC == 0) {
                body = D_63FA10[em->kind];
            } else {
                body = D_610370[em->kind];
            }
            while ((*pp)[3] != -1.0f) {
                pos[0] = cam[0] + (*pp)[0];
                pos[1] = cam[1] + (*pp)[1];
                pos[2] = cam[2] + (*pp)[2];
                if (body->type == 0x7D) {
                    do {
                        body_ptr_ck2((HCHR *)em, &body);
                    } while (body->type == 0x7D);
                }
                t = hit_data_expand(em, body, &cap, &sph);
                while (t != -1) {
                    switch (t) {
                    case 0:
                        if (hit_sphr_sphr3(sph.c, pos, out, sph.r, (*pp)[3]) != 0) {
                            PUSH_ACC(px, out[0]);
                            PUSH_ACC(py, out[1]);
                            PUSH_ACC(pz, out[2]);
                        }
                        break;
                    case 1:
                        hit_cap_pk(&cap, &pk);
                        if (hit_cap_sphr_m(&pk, pos, out, (*pp)[3]) != 0) {
                            PUSH_ACC(px, out[0]);
                            PUSH_ACC(py, out[1]);
                            PUSH_ACC(pz, out[2]);
                        }
                        break;
                    }
                    body++;
                    if (body->type == 0x7D) {
                        do {
                            body_ptr_ck2((HCHR *)em, &body);
                        } while (body->type == 0x7D);
                    }
                    t = hit_data_expand(em, body, &cap, &sph);
                }
                pp++;
            }
        }
    }
    if (px == 0.0f && py == 0.0f && pz == 0.0f) return;
    cam[0] = cam[0] + px;
    cam[1] = cam[1] + py;
    cam[2] = cam[2] + pz;
    {
        f32 g = 10.0f + GetGroundHit(cam);
        if (cam[1] < g) cam[1] = g;
    }
}
