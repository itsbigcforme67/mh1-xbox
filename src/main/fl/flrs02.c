/* fl library: GS render state packets (SLPM_654.95 0x00178820-0x001789B0): flPS2SendRenderState_ZBUF builds the ZBUF register value from the render state
 * word and flPs2State, remembers it in flPs2State (+0x430 / +0x438 for context 1 / 2) and queues a packet writing context 1, 2 or both (mode 0, 1, 2). */
#include "types.h"

typedef long u64;
typedef unsigned __int128 u128;

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern u8 flPs2VIF1Control[];
u64 *flPS2GetSystemTmpBuff();
int flPS2DmaAddQueue2();

int flPS2SendRenderState_ZBUF(int rs, int mode) {
    u64 st;
    u128 *p;
    u128 *top;
    int n;

    st = ((u64)((rs & 0x8000) != 0x8000) << 32) | ((u64)(int)(PS2S(u32, 0x40) >> 5) | ((u64)PS2S(int, 0x34) << 24));
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
        PS2S(u64, 0x430) = st;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x4E;
        break;
    case 1:
        PS2S(u64, 0x438) = st;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x4F;
        break;
    case 2:
        PS2S(u64, 0x430) = st;
        PS2S(u64, 0x438) = st;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x4E;
        p++;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x4F;
        break;
    }
    flPS2DmaAddQueue2(0, (unsigned long)top & 0xFFFFFFFUL, top, flPs2VIF1Control);
    return 1;
}
