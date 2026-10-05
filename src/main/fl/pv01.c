/* pv01 - pl vec math 0x00192E30-0x00192E38: plFCVSetBaseAddress. Whole file in plvec_nm.c. */
#include "types.h"

float sqrtf(float);
float plvecCalcLength(float *v);
void plvecNormalize(float *v);
float plvecInnerProduct(float *a, float *b);
void plvecOuterProduct(float *out, float *a, float *b);







extern int base_addr_0038A308;
float plGetFcurveEndTime();



void plFCVSetBaseAddress(int a) {
    base_addr_0038A308 = a;
}
