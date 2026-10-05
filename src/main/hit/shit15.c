/* shit15 - SLPM_654.95 0x00116940-0x001169F8 (f_sphr): GetWallTblAdrs, the wall
 * cell list (-1 terminated polygon pointers) for a position. */
#include "types.h"
#include "hit3.h"

s32 GetWallTblAdrs(f32 *p) {
    f32 csz = (u32)diorama_w.wcsz;
    f32 csx = (u32)diorama_w.wcsx;
    int iz = (int)(p[2] / csz);
    int ix = (int)(p[0] / csx);

    return (s32)diorama_w.wtbl[iz + ix * diorama_w.wnz];
}
