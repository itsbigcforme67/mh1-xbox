/* Player code (SLPM_654.95 0x0014F9B0-0x0014FB34): pl_body_make */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void flmatGetTrans(f32 *, u8 *);
void RotMatVec(f32 *, f32 *, int);
f32 flSqrt(f32);
void Pl_se_req2(PLW *, int, int, f32 *, int, int);

void pl_body_make(PLW *pl, f32 *out, f32 radius) {
    f32 c[3];
    f32 b[3];
    f32 v[3];
    f32 r[3];
    f32 m[16];
    flmatGetTrans(c, (u8 *)pl->work124 + 0x40);
    flmatGetTrans(b, (u8 *)pl->work130 + 0x40);
    out[0] = (c[0] + b[0]) / 2.0f;
    out[1] = (c[1] + b[1]) / 2.0f;
    out[2] = (c[2] + b[2]) / 2.0f;
    flmatGetTrans(out + 3, (u8 *)pl->work160 + 0x40);
    v[0] = out[3] - out[0];
    v[1] = out[4] - out[1];
    v[2] = out[5] - out[2];
    RotMatVec(v, m, 1);
    v[0] = 0;
    v[1] = radius;
    v[2] = 0;
    flvecApplyMat33(r, v, m);
    out[0] = out[0] + r[0];
    out[1] = out[1] + r[1];
    out[2] = out[2] + r[2];
    out[3] = out[3] - r[0];
    out[4] = out[4] - r[1];
    out[5] = out[5] - r[2];
    out[6] = radius;
}
