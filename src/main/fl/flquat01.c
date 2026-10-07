/* fl library: flQuatSetRot (SLPM_654.95 0x001737C0-0x00173884) builds a rotation quaternion from an axis (normalised here) and an angle: xyz = axis * sin(a/2), w = cos(a/2).
 * flSqrt/flSin/flCos are the fl math wrappers. flQuatCnv (quaternion to matrix) is in flquat01_nm.c. Names are guesses. */
#include "types.h"

f32 flSqrt(f32);
f32 flSin(f32);
f32 flCos(f32);

void flQuatSetRot(f32 *q, f32 a) {
    f32 len;
    f32 sn;

    a *= 0.5f;
    len = flSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
    q[3] = 0;
    q[0] /= len;
    q[1] /= len;
    q[2] /= len;
    sn = flSin(a);
    q[3] = flCos(a);
    q[0] *= sn;
    q[1] *= sn;
    q[2] *= sn;
}
