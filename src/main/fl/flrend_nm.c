/* flrend_nm - SLPM_654.95 0x00173654-0x001738B4 (g_flFloor.s) and 0x00177540-0x00179DC4 (g_flBeginRender.s) small parts of the
   fl library: math wrappers, render begin/end flags and screen coordinate conversion (flPs2State fields). Working file. */
#include "types.h"

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))

f32 floorf(f32);
f32 powf(f32, f32);
void flQuatSetRot(f32 *, f32);

f32 flFloor(f32 x) {
    return floorf(x);
}

f32 flPow(f32 x, f32 y) {
    return powf(x, y);
}

void flQuatSetRot2(f32 *axis, f32 *q, f32 ang) {
    q[3] = 0.0f;
    q[0] = axis[0];
    q[1] = axis[1];
    q[2] = axis[2];
    flQuatSetRot(q, ang);
}

int flBeginRender(void) {
    return PS2S(s32, 0x400) = 1;
}

int flEndRender(void) {
    PS2S(s32, 0x400) = 2;
    return 1;
}

void flPS2SetClearColor(u32 c) {
    PS2S(u32, 0x3A0) = c;
}

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

void flAdjustScreen(int a, int b) {
    PS2S(s32, 0x3BC) = a;
    PS2S(s32, 0x3C0) = b;
}
