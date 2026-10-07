/* flmat - scale a 3x3 rotation block: flmatScaleFactor33 (SLPM_654.95 0x00171E50-0x00171E84) builds a scale matrix and multiplies it in. */
#include "types.h"

void flmatMakeScale33(f32 *, f32, f32, f32);
void flmatMul33_2(f32 *, f32 *);

void flmatScaleFactor33(f32 *m, f32 sx, f32 sy, f32 sz) {
    f32 t[16];

    flmatMakeScale33(t, sx, sy, sz);
    flmatMul33_2(m, t);
}
