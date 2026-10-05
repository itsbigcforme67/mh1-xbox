/* pv03 - pl vec math 0x00193150-0x00193344: plvecCalcLength, plvecNormalize, plvecInnerProduct, plvecOuterProduct, plMakePlaneFromPoints. Whole file in plvec_nm.c. */
#include "types.h"

float sqrtf(float);
float plvecCalcLength(float *v);
void plvecNormalize(float *v);
float plvecInnerProduct(float *a, float *b);
void plvecOuterProduct(float *out, float *a, float *b);







extern int base_addr_0038A308;
float plGetFcurveEndTime();



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
