/* camr3 - SLPM_654.95 0x00224CB0-0x00224DC8 (f_cam_223B50): complex number
 * helpers (re, im as two floats) used by the quintic root finder DKA5. */
#include "types.h"

typedef struct DCMPLX {
    f32 re;
    f32 im;
} DCMPLX;

void dCnvComplex(DCMPLX *c, f32 re, f32 im) {
    c->re = re;
    c->im = im;
}

void dSubComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b) {
    r->re = a->re - b->re;
    r->im = a->im - b->im;
}

void dMulComplex(DCMPLX *r, DCMPLX *a, DCMPLX *b) {
    DCMPLX t;

    t.re = a->re * b->re - a->im * b->im;
    t.im = a->re * b->im + a->im * b->re;
    *r = t;
}
