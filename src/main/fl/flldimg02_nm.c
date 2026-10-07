/* NEAR-MATCH (not linked): flPS2VIF1MakeEndLoadImage 2 of 25 instructions differ (the original sets a1 = 3 before a3 = 0). */
/* fl library (SLPM_654.95 0x0016E150-0x0016E1B4): flPS2VIF1MakeEndLoadImage closes the image load chain: an end tag, a DIRECTHL VIF code and a
 * GIF packet that writes TRXDIR-style flush register 0x3F. Part of the file with flPS2VIF1MakeLoadImage (flldimg01_nm.c). */
#include "types.h"

typedef long u64;

void flPS2DmaAddEndTag();
void flPS2VIF1CodeAddDirectHL();

void flPS2VIF1MakeEndLoadImage(u8 *p, int irq) {
    flPS2DmaAddEndTag(p, 3, irq, 0);
    flPS2VIF1CodeAddDirectHL(p + 0x10, 2);
    *(u64 *)(p + 0x20) = 0x8001 | ((u64)0x10000000 << 32);
    *(u64 *)(p + 0x28) = 0xE;
    *(u64 *)(p + 0x30) = 0;
    *(u64 *)(p + 0x38) = 0x3F;
}
