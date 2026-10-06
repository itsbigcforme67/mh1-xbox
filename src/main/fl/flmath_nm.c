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

void flvecCopy(f32 *d, f32 *s) {
    f32 x = s[0];
    f32 y = s[1];
    f32 z = s[2];

    d[0] = x;
    d[1] = y;
    d[2] = z;
}

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

void flSinCos(f32 *s, f32 *c, f32 a) {
    f32 t[2];

    flPS2SinCosFast(t, a);
    *s = t[0];
    *c = t[1];
}
