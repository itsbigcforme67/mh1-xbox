/* flmat - copy the 3x3 block of a 4x4 float matrix: flmatCopy33 (SLPM_654.95 0x00172CE0-0x00172D2C). */
#include "types.h"

void flmatCopy33(f32 *d, f32 *s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[4] = s[4];
    d[5] = s[5];
    d[6] = s[6];
    d[8] = s[8];
    d[9] = s[9];
    d[10] = s[10];
}
