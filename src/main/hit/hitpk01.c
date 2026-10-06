/* hitpk01 - capsule / line packing for the hit tests (SLPM_654.95 0x0028CC40-0x0028CE00): hit_line2_pk, hit_cap_pk. Whole file in hitpk_nm.c. */
/* hitpk_nm - SLPM_654.95 0x0028CC40-0x0028CE00: pack a capsule / line segment for the hit tests (HPK / HLINE, see
   include/hit.h). The two functions sit right before hit2all.c's range. */
#include "types.h"
#include "hit.h"
void flvecCopy(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
/* Line from a to b: p0, p1, dir = b - a, middle, half length (distance p0 to middle). */
void hit_line2_pk(f32 *a, f32 *b, HLINE *l) {
    flvecCopy(l->p0, a);
    flvecCopy(l->p1, b);
    l->dir[0] = b[0] - a[0];
    l->dir[1] = b[1] - a[1];
    l->dir[2] = b[2] - a[2];
    l->mid[0] = 0.5f * (a[0] + b[0]);
    l->mid[1] = 0.5f * (a[1] + b[1]);
    l->mid[2] = 0.5f * (a[2] + b[2]);
    l->half = flvecCalcDistance(l->p0, l->mid);
}
/* Capsule (two end points + radius at +0x18) to HPK: dir, middle, bounding radius = half length + radius. */
void hit_cap_pk(HCAP *c, HPK *k) {
    flvecCopy(k->p0, c->p[0]);
    flvecCopy(k->p1, c->p[1]);
    k->r = c->r;
    k->dir[0] = c->p[1][0] - c->p[0][0];
    k->dir[1] = c->p[1][1] - c->p[0][1];
    k->dir[2] = c->p[1][2] - c->p[0][2];
    k->c[0] = 0.5f * (c->p[0][0] + c->p[1][0]);
    k->c[1] = 0.5f * (c->p[0][1] + c->p[1][1]);
    k->c[2] = 0.5f * (c->p[0][2] + c->p[1][2]);
    k->cr = flvecCalcDistance(c->p[0], k->c);
    k->cr += c->r;
}
