/* fl library: GS render state packets (SLPM_654.95 0x001793E0-0x00179570): flPS2SendRenderState_FOGCOL / _TEX1 build a small DMA packet (DMA tag + VIF
 * DIRECT, a GIFtag with A+D format, then one register value / register number pair) in the system temp buffer and queue it on VIF1. The packet is built with a
 * quadword pointer (u128 *, advanced per quadword): that is what makes the compiler keep the pointer arithmetic. Field names are guesses. */
#include "types.h"

typedef long u64;
typedef unsigned __int128 u128;

extern u8 flPs2VIF1Control[];
u64 *flPS2GetSystemTmpBuff();
int flPS2DmaAddQueue2();

int flPS2SendRenderState_FOGCOL(u32 col) {
    u128 *p;
    u128 *top;

    p = (u128 *)flPS2GetSystemTmpBuff(0x30, 0x10);
    top = p;
    *(u64 *)p = ((u64)0xF000 << 16) | 2;
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = 0x51000002;
    p++;
    *(u64 *)p = 0x8001 | ((u64)0x10000000 << 32);
    ((u64 *)p)[1] = 0xE;
    p++;
    *(u64 *)p = (u64)((col >> 16) & 0xFF) | ((u64)((col >> 8) & 0xFF) << 8) | ((u64)(col & 0xFF) << 16);
    ((u64 *)p)[1] = 0x3D;
    flPS2DmaAddQueue2(0, (unsigned long)top & 0xFFFFFFFUL, top, flPs2VIF1Control);
    return 1;
}

int flPS2SendRenderState_TEX1(int rs, int mode) {
    u128 *p;
    u128 *top;

    p = (u128 *)flPS2GetSystemTmpBuff(0x30, 0x10);
    top = p;
    *(u64 *)p = ((u64)0xF000 << 16) | 2;
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = 0x50000002;
    p++;
    *(u64 *)p = 0x8001 | ((u64)0x10000000 << 32);
    ((u64 *)p)[1] = 0xE;
    p++;
    if ((rs & 0x10000) == 0x10000) {
        *(u64 *)p = 0;
    } else {
        *(u64 *)p = 0x60;
    }
    if (mode == 0) {
        ((u64 *)p)[1] = 0x14;
    } else {
        ((u64 *)p)[1] = 0x15;
    }
    flPS2DmaAddQueue2(0, (unsigned long)top & 0xFFFFFFFUL, top, flPs2VIF1Control);
    return 1;
}
