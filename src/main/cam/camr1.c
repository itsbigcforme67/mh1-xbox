/* camr1 - SLPM_654.95 0x00223F90-0x00223FE0 (f_cam_223B50): dot products of
 * 3-vectors (the XZ one ignores y). */
#include "types.h"

f32 vInnerProductXZ(f32 *a, f32 *b) {
    return a[2] * b[2] + a[0] * b[0];
}

f32 vInnerProduct(f32 *a, f32 *b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
