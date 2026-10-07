/* fl library: GS render state packets (SLPM_654.95 0x001789B0-0x00178C88): flPS2SendRenderState_TEST builds the TEST register value (alpha test enable,
 * compare function from the render state bits 0x380000, reference value from flAlphaRefValue converted to PS2 range, depth test from bits 0x7000),
 * remembers it in flPs2State (+0x420 / +0x428 for context 1 / 2) and queues a packet writing TEST_1, TEST_2 or both (mode 0, 1, 2). */
#include "types.h"

typedef long u64;
typedef unsigned __int128 u128;

extern u8 flPs2State[];
#define PS2S(T, o) (*(T *)(flPs2State + (o)))
extern u8 flPs2VIF1Control[];
extern int flAlphaRefValue;
u64 *flPS2GetSystemTmpBuff();
int flPS2DmaAddQueue2();

int flPS2SendRenderState_TEST(int rs, int mode) {
    int atst;
    int aref;
    int ztst;
    u64 st;
    u128 *p;
    u128 *top;
    int n;

    aref = flAlphaRefValue;
    if (aref == 0xFF) {
        aref = 0x80;
    } else if (aref != 0) {
        aref >>= 1;
        if (aref == 0) {
            aref = 1;
        }
    }
    switch (rs & 0x380000) {
    case 0x0:
        atst = 0;
        break;
    case 0x380000:
        atst = 1;
        break;
    case 0x80000:
        atst = 2;
        break;
    case 0x180000:
        atst = 3;
        break;
    case 0x100000:
        atst = 4;
        break;
    case 0x300000:
        atst = 5;
        break;
    case 0x200000:
        atst = 6;
        break;
    case 0x280000:
        atst = 7;
        break;
    }
    switch (rs & 0x7000) {
    case 0x1000:
        ztst = 3;
        break;
    case 0x3000:
        ztst = 2;
        break;
    case 0x7000:
        ztst = 1;
        break;
    case 0x0:
        ztst = 0;
        break;
    case 0x2000:
    case 0x5000:
    case 0x4000:
    case 0x6000:
        ztst = 1;
        break;
    }
    st = ((u64)ztst << 17) | (((u64)aref * 16) | (((u64)atst * 2) | 1) | 0x10000);
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
        PS2S(u64, 0x420) = st;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x47;
        break;
    case 1:
        PS2S(u64, 0x428) = st;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x48;
        break;
    case 2:
        PS2S(u64, 0x420) = st;
        PS2S(u64, 0x428) = st;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x47;
        p++;
        *(u64 *)p = st;
        ((u64 *)p)[1] = 0x48;
        break;
    }
    flPS2DmaAddQueue2(0, (unsigned long)top & 0xFFFFFFFUL, top, flPs2VIF1Control);
    return 1;
}
