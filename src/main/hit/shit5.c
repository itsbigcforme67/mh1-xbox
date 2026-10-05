/* shit5 - SLPM_654.95 0x00117BA0-0x00117C98 (f_sphr): NormalClipFace. */
#include "types.h"

void UnitNormalVectorCCW(f32 *, f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);

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
