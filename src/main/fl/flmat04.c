/* flmat - add a vector to the translation row of a 4x4 float matrix: flmatAddTrans2 (SLPM_654.95 0x00171EA0-0x00171ED4). */
#include "types.h"

void flmatAddTrans2(f32 *m, f32 *v) {
    m[12] = m[12] + v[0];
    m[13] += v[1];
    m[14] += v[2];
}
