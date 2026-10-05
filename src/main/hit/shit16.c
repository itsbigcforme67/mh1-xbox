/* shit16 - SLPM_654.95 0x0011B1E0-0x0011B298 (f_sphr): GetGroundTblAdrs, the ground
 * cell list for a position. */
#include "types.h"
#include "hit3.h"

s32 *GetGroundTblAdrs(f32 *p) {
    f32 csz = (u32)diorama_w.gcsz;
    f32 csx = (u32)diorama_w.gcsx;
    int iz = (int)(p[2] / csz);
    int ix = (int)(p[0] / csx);

    return (s32 *)diorama_w.gtbl[iz + ix * diorama_w.gnz];
}
