/* camrz01 - 0x00224660-0x002246E8: ZoomRateCalc(dist, zoom): field-of-view scale for a camera-to-target
 * distance. zoom[2]/zoom[3] = near/far distance, zoom[4]/zoom[5] = scale at near/far, linear in between. */
#include "types.h"

f32 ZoomRateCalc(f32 x, f32 *z) {
    if (x <= z[2]) {
        return z[4];
    }
    if (!(x < z[3])) {
        return z[5];
    }
    if (z[3] == z[2]) {
        return 0.5f * (z[4] + z[5]);
    }
    return z[4] + (z[5] - z[4]) * (x - z[2]) / (z[3] - z[2]);
}
