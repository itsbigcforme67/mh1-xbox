/* flm01 - angle conversion (SLPM_654.95 0x00173320-0x00173448). Whole file in flmath_nm.c. */
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
/* radians -> 16 bit angle (0x10000 = 360 degrees) */
int flConvertRtoS(f32 r) {
    f32 d = 180.0f * r / 3.1415927f;

    if (d < 0.0f) {
        d += 360.0f;
    }
    return (u32)(182.04445f * d);
}
/* 16 bit angle -> radians in (-pi, pi] */
f32 flConvertStoR(u32 s) {
    f32 r = 3.1415927f * (0.0054931641f * (f32)s) / 180.0f;

    if (!(r <= 3.1415927f)) {
        r += -6.2831855f;
    }
    return r;
}
