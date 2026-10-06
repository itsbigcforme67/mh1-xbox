/* camarea06 - 0x00223410-0x002234F8: Area_XZ_Check(q, p): is point p inside the quad of a camera-area box
 * (corners (q0,q2) (q1,q3) (q4,q6) (q5,q7), four edge cross products)? 0 inside, 2 outside. */
#include "types.h"

int Area_XZ_Check(f32 *q, f32 *p) {
    f32 dx0 = p[0] - q[0];
    f32 dz0 = p[2] - q[2];

    if ((q[2] - q[6]) * dx0 - dz0 * (q[0] - q[4]) < 0.0f) {
        return 2;
    }
    if ((q[7] - q[2]) * dx0 - dz0 * (q[5] - q[0]) < 0.0f) {
        return 2;
    }
    if ((q[6] - q[3]) * (p[0] - q[1]) - (p[2] - q[3]) * (q[4] - q[1]) < 0.0f) {
        return 2;
    }
    if ((q[3] - q[7]) * (p[0] - q[1]) - (p[2] - q[3]) * (q[1] - q[5]) < 0.0f) {
        return 2;
    }
    return 0;
}
