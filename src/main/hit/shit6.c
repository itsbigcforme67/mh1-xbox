/* shit6 - SLPM_654.95 0x00119010-0x00119210 (f_sphr): add_vec_sub2. */
#include "types.h"

/* 0x00119010: push box v (size 6 floats: min x,y,z / max x,y,z, then extents)
 * outwards by v with limit r. Guess from the code. */
void add_vec_sub2(f32 *v, f32 *b, f32 r) {
    f32 x;

    x = v[0];
    if (!(x < 0.0f)) {
        if (b[0] + b[6] < x) {
            if (x <= r) {
                b[0] = x;
            } else {
                b[0] = r;
                b[6] = v[0] - r;
            }
        }
    } else if (!(b[3] + b[9] <= x)) {
        if (-x <= r) {
            b[3] = x;
        } else {
            b[3] = -r;
            b[9] = v[0] + r;
        }
    }
    x = v[1];
    if (!(x < 0.0f)) {
        if (b[1] + b[7] < x) {
            if (x <= r) {
                b[1] = x;
            } else {
                b[1] = r;
                b[7] = v[1] - r;
            }
        }
    } else if (!(b[4] + b[10] <= x)) {
        if (-x <= r) {
            b[4] = x;
        } else {
            b[4] = -r;
            b[10] = v[1] + r;
        }
    }
    x = v[2];
    if (!(x < 0.0f)) {
        if (b[2] + b[8] < x) {
            if (x <= r) {
                b[2] = x;
                return;
            }
            b[2] = r;
            b[8] = v[2] - r;
        }
    } else if (!(b[5] + b[11] <= x)) {
        if (-x <= r) {
            b[5] = x;
            return;
        }
        b[5] = -r;
        b[11] = v[2] + r;
    }
}
