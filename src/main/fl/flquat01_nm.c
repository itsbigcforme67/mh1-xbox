/* NEAR-MATCH (not linked): flQuatCnv (SLPM_654.95 0x001736B0), 63 of 67 instructions differ (float register numbering and operand order of the s * product terms only):
 * quaternion (x, y, z, w) to 4x4 rotation matrix, scale 2/|q|^2 (0 for a zero quaternion). */
#include "types.h"

void flQuatCnv(f32 *q, f32 *m) {
    f32 x, y, z, w;
    f32 xx, yy, zz, n, s;
    f32 xy, xz, yz, wx, wy, wz;

    y = q[1];
    x = q[0];
    z = q[2];
    w = q[3];
    s = 0.0f;
    yy = y * y;
    xx = x * x;
    zz = z * z;
    n = w * w + (zz + (xx + yy));
    if (!(n <= 0.0f)) {
        s = 2.0f / n;
    }
    xy = s * (x * y);
    xz = s * (x * z);
    wx = s * (w * x);
    yy = s * yy;
    xx = s * xx;
    yz = s * (y * z);
    wz = s * (w * z);
    zz = s * zz;
    wy = s * (w * y);
    m[0] = 1.0f - (yy + zz);
    m[1] = xy + wz;
    m[2] = xz - wy;
    m[4] = xy - wz;
    m[5] = 1.0f - (xx + zz);
    m[6] = yz + wx;
    m[8] = xz + wy;
    m[9] = yz - wx;
    m[10] = 1.0f - (xx + yy);
    *(int *)(m + 11) = 0;
    *(int *)(m + 7) = 0;
    *(int *)(m + 3) = 0;
    *(int *)(m + 14) = 0;
    *(int *)(m + 13) = 0;
    *(int *)(m + 12) = 0;
    *(int *)(m + 15) = 0x3F800000;
}

