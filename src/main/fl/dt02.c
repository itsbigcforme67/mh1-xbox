/* dt02 - DMA tags 0x0016DB60-0x0016DC98: flPS2DmaAddRetTag, flPS2DmaAddEndTag, flPS2VIF1CodeAddMscnt, flPS2VIF1CodeAddUnpackr, flPS2VIF1CodeAddDirectHL, flPS2VIF1CalcLoadImageSize, flPS2VIF1CalcEndLoadImageSize. Whole file in dmatag_nm.c. */
#include "types.h"

typedef unsigned __int128 u128;













u32 *flPS2DmaAddRetTag(u32 *p, int qwc, int irq) {
    *(u128 *)p = 0;
    p[0] = qwc + 0x60000000;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

u32 *flPS2DmaAddEndTag(u32 *p, int qwc, int irq) {
    *(u128 *)p = 0;
    p[0] = qwc + 0x70000000;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

u32 *flPS2VIF1CodeAddMscnt(u32 *p) {
    p[0] = 0x17000000;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    return p + 4;
}

u32 *flPS2VIF1CodeAddUnpackr(u32 *p, int a, int b) {
    p[0] = 0;
    p[1] = 0;
    p[2] = 0x01000404;
    p[3] = (b | 0x8000) | (a << 16) | 0x6C000000;
    return p + 4;
}

u32 *flPS2VIF1CodeAddDirectHL(u32 *p, int n) {
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = n | 0x51000000;
    return p + 4;
}

int flPS2VIF1CalcLoadImageSize(u32 n) {
    u32 q = n / 0x70000;

    if (n % 0x70000 != 0) {
        q++;
    }
    return q << 7;
}

int flPS2VIF1CalcEndLoadImageSize(void) {
    return 0x40;
}
