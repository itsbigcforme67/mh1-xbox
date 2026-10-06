/* SLPM_654.95 0x00193100-0x0019314C: plmatCopy33 .. plmatCopy33. See plvec_nm.c. */
#include "types.h"

float sqrtf(float);
float plvecCalcLength(float *v);
void plvecNormalize(float *v);
float plvecInnerProduct(float *a, float *b);
void plvecOuterProduct(float *out, float *a, float *b);







extern int base_addr_0038A308;
float plGetFcurveEndTime();



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
