/* flmat - read the translation row of a 4x4 float matrix: flmatGetTrans (SLPM_654.95 0x00171EE0-0x00171EFC). */
#include "types.h"

void flmatGetTrans(f32 *v, f32 *m) {
    v[0] = m[12];
    v[1] = m[13];
    v[2] = m[14];
}
