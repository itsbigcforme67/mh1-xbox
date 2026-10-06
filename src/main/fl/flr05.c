/* flr05 - UV check and screen conversion (SLPM_654.95 0x00179B80-0x00179CB0). Whole file in flrend_nm.c. */
/* flrend_nm - SLPM_654.95 0x00173654-0x001738B4 (g_flFloor.s) and 0x00177540-0x00179DC4 (g_flBeginRender.s) small parts of the
   fl library: math wrappers, render begin/end flags and screen coordinate conversion (flPs2State fields). Working file. */
#include "types.h"
extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
f32 floorf(f32);
f32 powf(f32, f32);
void flQuatSetRot(f32 *, f32);
void flPS2CheckUV(s16 *a, u16 *u, u16 *v) {
    if (a[0xD] < *u) {
        *u = a[0xD];
    }
    if (a[0xE] < *v) {
        *v = a[0xE];
    }
}
int flPS2ConvScreenX(int x) {
    x = (s16)(x - PS2S(s16, 0x3AC));
    return x;
}
int flPS2ConvScreenY(int y) {
    y = (s16)(y - PS2S(s16, 0x3B0));
    if (PS2S(s32, 4) == 0) {
        y = (s16)(y >> 1);
    }
    return y;
}
f32 flPS2ConvScreenFX(f32 x) {
    x = x - (f32)PS2S(s32, 0x3AC);
    return x;
}
f32 flPS2ConvScreenFY(f32 y) {
    y = y - (f32)PS2S(s32, 0x3B0);
    if (PS2S(s32, 4) == 0) {
        y = y * 0.5f;
    }
    return y;
}
f32 flPS2ConvScreenFZ(f32 z) {
    z = z - 1.0f;
    z = z * -0.5f;
    z = z * PS2S(f32, 0x44);
    return z;
}
