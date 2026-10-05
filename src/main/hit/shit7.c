/* shit7 - SLPM_654.95 0x0011C9F0-0x0011CA74 (f_sphr): check_angle. */
#include "types.h"

f32 flArcCos(f32);
f32 flvecInnerProduct(f32 *, f32 *);

/* 0x0011C9F0: angle (0x10000 = 360 degrees) between normal n and up. */
void check_angle(f32 *n, s32 *out) {
    f32 up[3];

    up[0] = 0.0f;
    up[1] = 1.0f;
    up[2] = 0.0f;
    *out = (s32)(0.5f + 65536.0f * flArcCos(flvecInnerProduct(n, up)) / 6.2831855f) & 0xFFFF;
}
