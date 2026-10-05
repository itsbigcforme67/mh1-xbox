/* hit2.h - shared declarations for f_hit_28CE00 (hit2*.c): sphere, capsule,
 * plane and line intersection tests used by the shell/body hit checks. */
#ifndef HIT2_H
#define HIT2_H
#include "types.h"
#include "hit.h"

f32 flvecCalcDistance(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
f32 flAbs(f32);
f32 flSqrt(f32);
void flvecCopy(f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);

/* Local (static) in the original; global here because the file is split. */
u8 hit_point_sphr(f32 *p, f32 *c, f32 r);
void hit_cap_cap2_sub(f32 *a, f32 *v, f32 *out, f32 len, f32 r);
void hit_cap_cap3_sub(f32 *v, f32 *out, f32 r, f32 d);
void p_point_line2(f32 *p0, f32 *p1, f32 *dir, f32 *pt, f32 *out, f32 len);

u8 hit_sphr_sphr(f32 *a, f32 *b, f32 ra, f32 rb);

#endif
