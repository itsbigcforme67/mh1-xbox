/* SLPM_654.95 0x0010A210-0x0010A44C: tri_in_check .. tri_in_check. See tri_nm.c. */
#include "types.h"
#include "fl.h"

f32 flArcCos(f32);
int flConvertRtoS(f32);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecNormalize(f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flmatCopy(void *, void *);
extern f32 sys_old_pos[][3];
extern FLMAT sys_old_mat[];




int tri_in_check(f32 *tri, f32 *p) {
    f32 c[3];
    f32 b[3];
    f32 a[3];
    f32 ab, bc, ca;
    int s;

    c[0] = tri[0] - p[0];
    c[1] = tri[1] - p[1];
    c[2] = tri[2] - p[2];
    b[0] = tri[3] - p[0];
    b[1] = tri[4] - p[1];
    b[2] = tri[5] - p[2];
    a[0] = tri[6] - p[0];
    a[1] = tri[7] - p[1];
    a[2] = tri[8] - p[2];
    flvecNormalize(c);
    flvecNormalize(b);
    flvecNormalize(a);
    ab = flvecInnerProduct(c, b);
    bc = flvecInnerProduct(b, a);
    ca = flvecInnerProduct(a, c);
    if (!(ab <= 1.0f)) ab = 1.0f;
    if (!(bc <= 1.0f)) bc = 1.0f;
    if (!(ca <= 1.0f)) ca = 1.0f;
    if (ab < -1.0f) ab = -1.0f;
    if (bc < -1.0f) bc = -1.0f;
    if (ca < -1.0f) ca = -1.0f;
    ab = flArcCos(ab);
    bc = flArcCos(bc);
    ca = flArcCos(ca);
    s = flConvertRtoS(ab) & 0xFFFF;
    s = (flConvertRtoS(bc) & 0xFFFF) + s;
    {
        int t = flConvertRtoS(ca) & 0xFFFF;
        if (t + s >= 0xF000) return 1;
    }
    return 0;
}
