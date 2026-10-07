/* fl library: flCheckMeshFOV (SLPM_654.95 0x001738C0-0x00173A48) tests a bounding sphere (centre v, radius r) against the view volume: the centre is
 * transformed by the matrix m (column vectors m[0..3], m[4..7], ...) into o, then compared with the near/far limits pl[16], pl[17] and the four side planes
 * (x/z and y/z coefficient pairs at pl[0],pl[2] / pl[4],pl[6] / pl[9],pl[10] / pl[13],pl[14]). Returns 1 when the sphere may be visible. Names are guesses. */
#include "types.h"

int flCheckMeshFOV(f32 *v, f32 *o, f32 *m, f32 *pl, f32 r) {
    o[2] = m[14] + (m[2] * v[0] + m[6] * v[1] + m[10] * v[2]);
    if (o[2] - r > pl[16]) {
        return 0;
    }
    if (o[2] + r < pl[17]) {
        return 0;
    }
    o[0] = m[12] + (m[0] * v[0] + m[4] * v[1] + m[8] * v[2]);
    if (o[0] * pl[0] + o[2] * pl[2] > r) {
        return 0;
    }
    if (o[0] * pl[4] + o[2] * pl[6] > r) {
        return 0;
    }
    o[1] = m[13] + (m[1] * v[0] + m[5] * v[1] + m[9] * v[2]);
    if (o[1] * pl[9] + o[2] * pl[10] > r) {
        return 0;
    }
    if (o[1] * pl[13] + o[2] * pl[14] > r) {
        return 0;
    }
    return 1;
}
