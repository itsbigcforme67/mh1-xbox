/* fl motion set info (SLPM_654.95 0x00174290-0x001742F8): flGetMotionSetTime, flGetMotionSetLoopInfo. */
#include "types.h"

u8 *flPS2GetSystemBuffAdrs(int);

f32 flGetMotionSetTime(int h) {
    return *(f32 *)(flPS2GetSystemBuffAdrs(h) + 8);
}

int flGetMotionSetLoopInfo(int h, f32 *out) {
    u8 *p = flPS2GetSystemBuffAdrs(h);

    if (*(u16 *)p & 0x8000) {
        *out = *(f32 *)(p + 0xC);
        return 1;
    }
    *out = 0;
    return 0;
}
