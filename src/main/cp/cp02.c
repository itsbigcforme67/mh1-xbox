/* cp02 - vector and angle math 0x00120C80-0x00120F84: NvecFloatAdjust, RotateX, RotateY, RotateZ, cpInterVector, AarcTan2, nlCalcPoint, CalcDistanceXZ. Whole file in cpmath_nm.c. */
#include "types.h"

typedef f32 MAT4[4][4];

void flmatInit(void *);
void flmatSetXYZ33(void *, f32, f32, f32);
void flvecApplyMat33(void *, void *, void *);
void flvecApplyMat(void *, void *, void *);
void flvecNormalize(f32 *);
f32 flArcTan2(f32, f32);
void RotateX(MAT4 *, f32);
void RotateY(MAT4 *, f32);
void RotateZ(MAT4 *, f32);









void flvecOuterProduct(f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void flvecCopy(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
f32 flAbs(f32);
void flmatRotX33(void *, f32);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
void flmatMul33(void *, void *, void *);





















void NvecFloatAdjust(f32 *out, f32 *in) {
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    if (flAbs(in[0]) < 0.001f) {
        *(s32 *)&out[0] = 0;
    }
    if (flAbs(in[1]) < 0.001f) {
        *(s32 *)&out[1] = 0;
    }
    if (flAbs(in[2]) < 0.001f) {
        *(s32 *)&out[2] = 0;
    }
}

void RotateX(MAT4 *m, f32 r) {
    MAT4 t;

    flmatInit(&t);
    flmatRotX33(&t, r);
    flmatMul33(m, &t, m);
}

void RotateY(MAT4 *m, f32 r) {
    MAT4 t;

    flmatInit(&t);
    flmatRotY33(&t, r);
    flmatMul33(m, &t, m);
}

void RotateZ(MAT4 *m, f32 r) {
    MAT4 t;

    flmatInit(&t);
    flmatRotZ33(&t, r);
    flmatMul33(m, &t, m);
}

/* out = a * t + b * (1 - t) */
void cpInterVector(f32 *out, f32 *a, f32 *b, f32 t) {
    f32 u = 1.0f - t;

    out[0] = a[0] * t + b[0] * u;
    out[1] = a[1] * t + b[1] * u;
    out[2] = a[2] * t + b[2] * u;
}

s32 AarcTan2(f32 y, f32 x) {
    return 10430.378f * flArcTan2(y, x);
}

void nlCalcPoint(f32 *out, f32 *in, f32 *m) {
    flvecApplyMat33(out, in, m);
    out[0] += m[12];
    out[1] += m[13];
    out[2] += m[14];
}

f32 CalcDistanceXZ(f32 *a, f32 *b) {
    f32 v[6];

    SetVector(v, a[0], 0, a[2]);
    SetVector(v + 3, b[0], 0, b[2]);
    return flvecCalcDistance(v, v + 3);
}
