/* flr03 - render begin / end (SLPM_654.95 0x00177540-0x00177564). Whole file in flrend_nm.c. */
/* flrend_nm - SLPM_654.95 0x00173654-0x001738B4 (g_flFloor.s) and 0x00177540-0x00179DC4 (g_flBeginRender.s) small parts of the
   fl library: math wrappers, render begin/end flags and screen coordinate conversion (flPs2State fields). Working file. */
#include "types.h"
extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
f32 floorf(f32);
f32 powf(f32, f32);
void flQuatSetRot(f32 *, f32);
int flBeginRender(void) {
    return PS2S(s32, 0x400) = 1;
}
int flEndRender(void) {
    PS2S(s32, 0x400) = 2;
    return 1;
}
