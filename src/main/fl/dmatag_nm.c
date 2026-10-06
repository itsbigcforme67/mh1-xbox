/* DMA/VIF tag builders. SLPM_654.95 0x0016D9C0-0x0016DCA0 (g_flReleaseClayHandle second part).
 * Each returns the next free quadword. Tag word 0 = qwc | id << 28 | irq << 31, word 1 = address
 * (bit 31 set when the address is scratchpad 0x7xxxxxxx). */
#include "types.h"

typedef unsigned __int128 u128;

u32 *flPS2DmaAddCntTag(u32 *p, int qwc, int irq) {
    *(u128 *)p = 0;
    p[0] = qwc + 0x10000000;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

u32 *flPS2DmaAddNextTag(u32 *p, int qwc, unsigned long addr, int irq) {
    unsigned long a = addr & 0x0FFFFFFF;
    long spr = 0;
    u32 w;
    if ((addr & 0x70000000) == 0x70000000) {
        spr = (int)0x80000000;
    }
    w = qwc + 0x20000000;
    *(u128 *)p = 0;
    p[0] = w;
    p[1] = a | spr;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

u32 *flPS2DmaAddRefTag(u32 *p, int qwc, unsigned long addr, int irq) {
    unsigned long a = addr & 0x0FFFFFFF;
    unsigned long spr = 0;

    if ((addr & 0x70000000) == 0x70000000) {
        spr = (int)0x80000000;
    }
    *(u128 *)p = 0;
    p[0] = qwc + 0x30000000;
    p[1] = a | spr;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

u32 *flPS2DmaAddRefeTag(u32 *p, int qwc, unsigned long addr, int irq) {
    unsigned long a = addr & 0x0FFFFFFF;
    unsigned long spr = 0;

    if ((addr & 0x70000000) == 0x70000000) {
        spr = (int)0x80000000;
    }
    *(u128 *)p = 0;
    p[0] = qwc;
    p[1] = a | spr;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

u32 *flPS2DmaAddCallTag(u32 *p, int qwc, unsigned long addr, int irq) {
    unsigned long a = addr & 0x0FFFFFFF;
    unsigned long spr = 0;

    if ((addr & 0x70000000) == 0x70000000) {
        spr = (int)0x80000000;
    }
    *(u128 *)p = 0;
    p[0] = qwc + 0x50000000;
    p[1] = a | spr;
    if (irq == 1) {
        p[0] |= 0x80000000;
    }
    return p + 4;
}

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
