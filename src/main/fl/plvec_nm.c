/* pl vector helpers. SLPM_654.95 0x00193100-0x00193350 and 0x00192E30 (g_plBMPGetPixelAddressFromImage):
 * small vec3 math (sqrtf is the inline sqrt.s) and a plane from three points (normal xyz, offset w). */
#include "types.h"

float sqrtf(float);
float plvecCalcLength(float *v);
void plvecNormalize(float *v);
float plvecInnerProduct(float *a, float *b);
void plvecOuterProduct(float *out, float *a, float *b);

void plmatCopy33(f32 *dst, f32 *src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[8] = src[8];
    dst[9] = src[9];
    dst[10] = src[10];
}

float plvecCalcLength(float *v) {
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void plvecNormalize(float *v) {
    float len = plvecCalcLength(v);

    if (0.0f == len) {
        v[2] = 0.0f;
        v[1] = 0.0f;
        v[0] = 0.0f;
    } else {
        v[0] = v[0] / len;
        v[1] = v[1] / len;
        v[2] = v[2] / len;
    }
}

float plvecInnerProduct(float *a, float *b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void plvecOuterProduct(float *out, float *a, float *b) {
    out[0] = a[1] * b[2] - b[1] * a[2];
    out[1] = a[2] * b[0] - b[2] * a[0];
    out[2] = a[0] * b[1] - b[0] * a[1];
}

void plMakePlaneFromPoints(float *plane, float *p0, float *p1, float *p2) {
    float e1[4];
    float e2[4];
    float n[4];

    e1[0] = p1[0] - p0[0];
    e1[1] = p1[1] - p0[1];
    e1[2] = p1[2] - p0[2];
    e2[0] = p2[0] - p0[0];
    e2[1] = p2[1] - p0[1];
    e2[2] = p2[2] - p0[2];
    plvecOuterProduct(n, e1, e2);
    plvecNormalize(n);
    plane[0] = n[0];
    plane[1] = n[1];
    plane[2] = n[2];
    plane[3] = -plvecInnerProduct(n, e1);
}

extern int base_addr_0038A308;
float plGetFcurveEndTime();

void plFCVSetBaseAddress(int a) {
    base_addr_0038A308 = a;
}

float plGetFcurveTime() {
    return plGetFcurveEndTime();
}
