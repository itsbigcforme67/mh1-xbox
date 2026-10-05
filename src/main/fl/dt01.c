/* dt01 - DMA tags 0x0016D9C0-0x0016D9F0: flPS2DmaAddCntTag. Whole file in dmatag_nm.c. */
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
