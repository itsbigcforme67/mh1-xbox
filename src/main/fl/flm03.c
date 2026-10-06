/* flm03 - flvecCopy (SLPM_654.95 0x00173300-0x0017331C). Whole file in flmath_nm.c. */
/* flmath_nm - SLPM_654.95 0x00173290-0x00173654 (flmat / flvec / flArc*): scalar math wrappers of the fl library (the VU0
   matrix and vector routines are inline assembly and stay as asm). Working file. */
#include "types.h"
extern u8 flMATRIX[];
f32 acosf(f32);
f32 atanf(f32);
f32 atan2f(f32, f32);
f32 expf(f32);
f32 flSqrt(f32);
f32 flArcTan(f32);
f32 flPS2SinFast(f32);
f32 flPS2CosFast(f32);
void flPS2SinCosFast(f32 *, f32);
typedef struct FV3 { f32 x, y, z; } FV3;
void flvecCopy(f32 *d, f32 *s) {
    *(FV3 *)d = *(FV3 *)s;
}
f32 flConvertRtoS_f(f32 r);
