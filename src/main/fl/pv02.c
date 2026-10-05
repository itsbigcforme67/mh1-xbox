/* pv02 - pl vec math 0x00192FA0-0x00192FA8: plGetFcurveTime. Whole file in plvec_nm.c. */
#include "types.h"

float sqrtf(float);
float plvecCalcLength(float *v);
void plvecNormalize(float *v);
float plvecInnerProduct(float *a, float *b);
void plvecOuterProduct(float *out, float *a, float *b);







extern int base_addr_0038A308;
float plGetFcurveEndTime();



float plGetFcurveTime() {
    return plGetFcurveEndTime();
}
