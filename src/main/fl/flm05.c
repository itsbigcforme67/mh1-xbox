/* fl library (SLPM_654.95 0x00172810-0x00172970): flmatMakeLookAt builds a view matrix from eye, target and up vector (flvec* helpers, VU0 routines). */
#include "types.h"

typedef struct FV3 { f32 x, y, z; } FV3;

void flvecNormalize();
void flvecOuterProduct();
f32 flvecInnerProduct();

void flmatMakeLookAt(f32 *m, FV3 *eye, FV3 *at, FV3 *up) {
    FV3 side;
    FV3 top;
    FV3 fwd;
    FV3 upv;

    fwd.x = eye->x - at->x;
    fwd.y = eye->y - at->y;
    fwd.z = eye->z - at->z;
    upv = *up;
    flvecNormalize(&fwd);
    flvecNormalize(&upv);
    flvecOuterProduct(&side, &upv, &fwd);
    flvecNormalize(&side);
    flvecOuterProduct(&top, &fwd, &side);
    m[3] = 0;
    m[7] = 0;
    m[11] = 0;
    m[15] = 1.0f;
    m[0] = side.x;
    m[4] = side.y;
    m[8] = side.z;
    m[1] = top.x;
    m[5] = top.y;
    m[9] = top.z;
    m[2] = fwd.x;
    m[6] = fwd.y;
    m[10] = fwd.z;
    m[12] = -flvecInnerProduct(eye, &side);
    m[13] = -flvecInnerProduct(eye, &top);
    m[14] = -flvecInnerProduct(eye, &fwd);
}
