/* fl library: GS render state packets (SLPM_654.95 0x00178530-0x0017881C): flPS2SendRenderState_SCISSOR clips the rectangle (x, y, w, h) to the screen,
 * remembers it in flPs2State (+0x448.. context 1 as x0 y0 x1 y1 and +0x440 the register value, +0x460.. / +0x458 context 2) and queues a packet writing
 * SCISSOR_1, _2 or both (mode 0, 1, 2). */
#include "types.h"

typedef long u64;
typedef unsigned __int128 u128;

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern u8 flPs2VIF1Control[];
extern int flWidth;
extern int flHeight;
u64 *flPS2GetSystemTmpBuff();
int flPS2DmaAddQueue2();

int flPS2SendRenderState_SCISSOR(int x, int y, int w, int h, int mode) {
    int x1;
    int y1;
    u128 *p;
    u128 *top;
    int n;
    u64 reg;

    if (x < 0) {
        x = 0;
    }
    if (flWidth < x + w) {
        w = flWidth - x;
    }
    x1 = x + w - 1;
    if (y < 0) {
        y = 0;
    }
    if (flHeight < y + h) {
        h = flHeight - y;
    }
    y1 = y + h - 1;
    if (mode != 2) {
        p = (u128 *)flPS2GetSystemTmpBuff(0x30, 0x10);
        n = 2;
    } else {
        p = (u128 *)flPS2GetSystemTmpBuff(0x40, 0x10);
        n = 3;
    }
    top = p;
    *(u64 *)p = (u64)(u32)((n + 0x70000000) | 0x80000000);
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = n | 0x50000000;
    p++;
    *(u64 *)p = (u64)(u32)(n - 1) | (0x8000 | ((u64)0x10000000 << 32));
    ((u64 *)p)[1] = 0xE;
    p++;
    switch (mode) {
    case 0:
        PS2S(int, 0x448) = x;
        PS2S(int, 0x44C) = y;
        PS2S(int, 0x450) = x1;
        PS2S(int, 0x454) = y1;
        reg = ((u64)y1 << 48) | (((u64)y << 32) | ((u64)x | ((u64)x1 << 16)));
        PS2S(u64, 0x440) = reg;
        *(u64 *)p = reg;
        ((u64 *)p)[1] = 0x40;
        break;
    case 1:
        PS2S(int, 0x460) = x;
        PS2S(int, 0x464) = y;
        PS2S(int, 0x468) = x1;
        PS2S(int, 0x46C) = y1;
        reg = ((u64)y1 << 48) | (((u64)y << 32) | ((u64)x | ((u64)x1 << 16)));
        PS2S(u64, 0x458) = reg;
        *(u64 *)p = reg;
        ((u64 *)p)[1] = 0x41;
        break;
    case 2:
        PS2S(int, 0x448) = x;
        PS2S(int, 0x44C) = y;
        PS2S(int, 0x460) = x;
        PS2S(int, 0x450) = x1;
        PS2S(int, 0x468) = x1;
        PS2S(int, 0x454) = y1;
        PS2S(int, 0x464) = y;
        reg = ((u64)y1 << 48) | (((u64)y << 32) | ((u64)x | ((u64)x1 << 16)));
        PS2S(int, 0x46C) = y1;
        PS2S(u64, 0x440) = reg;
        PS2S(u64, 0x458) = reg;
        *(u64 *)p = reg;
        ((u64 *)p)[1] = 0x40;
        p++;
        *(u64 *)p = reg;
        ((u64 *)p)[1] = 0x41;
        break;
    }
    flPS2DmaAddQueue2(0, (unsigned long)top & 0xFFFFFFFUL, top, flPs2VIF1Control);
    return 1;
}
