/* flm02 - abs / arc / exp / sin / cos wrappers (SLPM_654.95 0x00173540-0x00173618). Whole file in flmath_nm.c. */
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
f32 flConvertRtoS_f(f32 r);
f32 flAbs(f32 x) {
    if (x < 0.0f) {
        x = -x;
    }
    return x;
}
f32 flArcCos(f32 x) {
    return acosf(x);
}
f32 flArcSin(f32 x) {
    return flArcTan(x / flSqrt(1.0f + -x * x));
}
f32 flArcTan(f32 x) {
    return atanf(x);
}
f32 flArcTan2(f32 y, f32 x) {
    return atan2f(y, x);
}
f32 flExp(f32 x) {
    return expf(x);
}
f32 flSin(f32 a) {
    return flPS2SinFast(a);
}
f32 flCos(f32 a) {
    return flPS2CosFast(a);
}
