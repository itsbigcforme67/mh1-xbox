/* NEAR-MATCH (not linked): flPS2VIF1MakeLoadImage 248 of 299 instructions differ (logic complete and believed equivalent: stack slot and saved register order of the 14 locals differ; declhill2 gets 256 -> 248). */
/* fl library GS image load packets (SLPM_654.95 0x0016DCA0-0x0016E150): flPS2VIF1MakeLoadImage builds the chain of VIF/GIF packets that uploads one
 * image to VRAM: per chunk of at most 0x70000 bytes a 0x80 byte packet (DMA cnt tag, GIF A+D BITBLTBUF/TRXPOS/TRXREG/TRXDIR, IMAGE tag) followed by a
 * ref tag to the pixel data (the last chunk ends with a refe tag). Returns the address of the last ref tag (0 when no chunk was made).
 * Parameters (names are guesses): p packet buffer, irq flag of the last tag, adrs pixel data, size bytes, vram/bw/fmt destination buffer base, width and
 * pixel format, x/y/w/h the rectangle. */
#include "types.h"

typedef unsigned long u64;

u32 *flPS2DmaAddCntTag();
u32 *flPS2DmaAddRefTag();
u32 *flPS2DmaAddRefeTag();

u8 *flPS2VIF1MakeLoadImage(u8 *p, int irq, u32 adrs, u32 size, s16 vram, s16 bw, s16 fmt, s16 x, s16 y, s16 w, s16 h) {
    u8 *last;
    u32 chunks;
    u32 i;
    int n;
    s16 rows;
    u32 qw;
    long bitblt;
    long trxpos;

    last = 0;
    chunks = size / 0x70000;
    if (size % 0x70000 != 0) {
        chunks++;
    }
    if (chunks == 1) {
        rows = h;
    } else {
        switch (fmt) {
        case 0:
        case 1:
            rows = 0x70000 / (w * 4);
            break;
        case 2:
        case 10:
            rows = 0x70000 / (w * 2);
            break;
        case 19:
            rows = 0x70000 / w;
            break;
        case 20:
            rows = 0x70000 / (w >> 1);
            break;
        }
    }
    if (bw == 0) {
        bw = 1;
    }
    i = 0;
    if (chunks != 0) {
        trxpos = (long)x << 32;
        bitblt = ((long)fmt << 56) | (((long)vram << 32) | ((long)bw << 48));
        do {
            switch (fmt) {
            case 0:
            case 1:
                n = w * rows * 4;
                break;
            case 2:
            case 10:
                n = w * rows * 2;
                break;
            case 19:
                n = w * rows;
                break;
            case 20:
                n = (w * rows) >> 1;
                break;
            }
            flPS2DmaAddCntTag(p, 6, 0, 0);
            *(u32 *)(p + 8) = 0x13000000;
            *(u32 *)(p + 0xC) = 0x51000006;
            *(u64 *)(p + 0x10) = 4 | ((long)0x10000000 << 32);
            *(u64 *)(p + 0x18) = 0xE;
            *(u64 *)(p + 0x20) = bitblt;
            *(u64 *)(p + 0x28) = 0x50;
            *(u64 *)(p + 0x30) = trxpos | ((long)y << 48);
            *(u64 *)(p + 0x38) = 0x51;
            *(u64 *)(p + 0x40) = w | ((long)rows << 32);
            *(u64 *)(p + 0x48) = 0x52;
            *(u64 *)(p + 0x50) = 0;
            *(u64 *)(p + 0x58) = 0x53;
            if (i >= chunks - 1) {
                qw = size >> 4;
                last = p + 0x70;
                *(u64 *)(p + 0x60) = (u64)qw | (0x8000 | ((long)0x08000000 << 32));
                *(u64 *)(p + 0x68) = 0;
                flPS2DmaAddRefeTag(last, qw, adrs, irq);
                *(u32 *)(p + 0x78) = 0;
                *(u32 *)(p + 0x7C) = qw | 0x51000000;
            } else {
                *(u64 *)(p + 0x60) = 0xF000 | ((long)0x08000000 << 32);
                *(u64 *)(p + 0x68) = 0;
                flPS2DmaAddRefTag(p + 0x70, 0x7000, adrs, 0, 0);
                *(u32 *)(p + 0x78) = 0;
                adrs += n;
                *(u32 *)(p + 0x7C) = 0x51007000;
                y = y + rows;
                p += 0x80;
                if (h < y + rows) {
                    rows = h - y;
                }
                size -= n;
            }
            i++;
        } while (i < chunks);
    }
    return last;
}
