/* shit2 - SLPM_654.95 0x00116540-0x001169FC (f_sphr part 3): grid helpers of
 * the stage hit data. BlockPlaceCgeck returns the quadrant (bit 0: x, bit 1:
 * z) of its grid cell that a position lies in; *FieldInCheck tell whether a
 * position is inside the loaded wall / ground / stage area (8 units
 * margin); GetWallTblAdrs returns the wall cell list for a position.
 * Cell sizes and counts are unsigned (u32 to float conversions). */
#include "types.h"
#include "hit3.h"

typedef struct STAGE_SZ {
    u8 _pad00[0x10];
    f32 sx;             /* 0x10 stage size x */
    f32 sz;             /* 0x14 stage size z */
} STAGE_SZ;
STAGE_SZ *Stage_data_get(int);

int BlockPlaceCgeck(f32 *p) {
    f32 x;
    int iz;
    int r;
    u32 t;
    int ix;
    f32 z;
    u32 cx;
    u32 cz;

    z = p[2];
    r = 0;
    cz = diorama_w.wcsz;
    iz = (int)(z / cz);
    x = p[0];
    cx = diorama_w.wcsx;
    ix = (int)(x / cx);
    t = (cx >> 1) + ix * cx;
    if (!(x < t)) r = 1;
    t = (cz >> 1) + iz * cz;
    if (!(z < t)) r += 2;
    return r;
}

int GroundFieldInCheck(f32 *p) {
    f32 x = p[0];
    f32 z;

    if (x < 8.0f || (z = p[2]) < 8.0f) return 0;
    if (x < (f32)((u32)diorama_w.gcsx * (u32)diorama_w.gnx) - 8.0f && z < (f32)((u32)diorama_w.gcsz * (u32)diorama_w.gnz) - 8.0f) return 1;
    return 0;
}

int WallFieldInCheck(f32 *p) {
    f32 x = p[0];
    f32 z;

    if (x < 8.0f || (z = p[2]) < 8.0f) return 0;
    if (x < (f32)((u32)diorama_w.wcsx * (u32)diorama_w.wnx) - 8.0f && z < (f32)((u32)diorama_w.wcsz * (u32)diorama_w.wnz) - 8.0f) return 1;
    return 0;
}

int AreaFieldInCheck(int stg, f32 *p) {
    STAGE_SZ *sd = Stage_data_get(stg & 0xFFFF);
    f32 x = p[0];
    f32 z;

    if (x < 8.0f || (z = p[2]) < 8.0f) return 0;
    if (x < sd->sx - 8.0f && z < sd->sz - 8.0f) return 1;
    return 0;
}

s32 GetWallTblAdrs(f32 *p) {
    f32 csz = (u32)diorama_w.wcsz;
    f32 csx = (u32)diorama_w.wcsx;
    int iz = (int)(p[2] / csz);
    int ix = (int)(p[0] / csx);

    return (s32)diorama_w.wtbl[iz + ix * diorama_w.wnz];
}
