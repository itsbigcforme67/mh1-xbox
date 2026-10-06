/* flr02 - quaternion from axis and angle (SLPM_654.95 0x00173890-0x001738B4). Whole file in flrend_nm.c. */
/* flrend_nm - SLPM_654.95 0x00173654-0x001738B4 (g_flFloor.s) and 0x00177540-0x00179DC4 (g_flBeginRender.s) small parts of the
   fl library: math wrappers, render begin/end flags and screen coordinate conversion (flPs2State fields). Working file. */
#include "types.h"
extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
f32 floorf(f32);
f32 powf(f32, f32);
void flQuatSetRot(f32 *, f32);
void flQuatSetRot2(f32 *axis, f32 *q, f32 ang) {
    q[3] = 0.0f;
    q[0] = axis[0];
    q[1] = axis[1];
    q[2] = axis[2];
    flQuatSetRot(q, ang);
}
