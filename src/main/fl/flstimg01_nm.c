/* NEAR-MATCH work file: flPS2StoreImageB (SLPM_654.95 0x0016E1C0-0x0016E6E0, 1312 bytes): reads a rectangle of GS VRAM back to main memory in chunks
 * of at most 0x70000 bytes. Per chunk it builds a 0x90 byte GIF packet in the system temp buffer (A+D: BITBLTBUF, TRXPOS, TRXREG, FINISH, TRXDIR = 1),
 * masks the GS FINISH interrupt, sends the packet on VIF1, waits until the handler has counted the quadwords (flPs2StoreImageSize) down, then restores
 * IMR and switches the GS bus back (BUSDIR 0). Parameters (names are guesses): dst main memory buffer, size total bytes, sbp VRAM base, sbw buffer width,
 * psm pixel format, x/y/w/h the rectangle (h is the 9th argument, on the stack). */
#include "types.h"

typedef long u64;
typedef unsigned __int128 u128;

typedef struct DCH { u8 lo : 6; u8 b6 : 1; u8 b7 : 1; } DCH;
extern int flPs2StoreImageSize;
extern u8 *flPs2StoreImageAdrs;
extern u64 flPs2StoreImageOldIMR;
extern u128 vif1_fifo;
extern u8 flPs2State[];
extern int flPs2VIF1Control[];

void flPS2DmaWait(void);
u8 *flPS2GetSystemTmpBuff(int, int);
void flPS2DmaAddEndTag();
void EnableIntc(int);
void FlushCache(int);
int sceDmaSend(void *, void *);
int sceGsSyncPath(int, int);

void flPS2StoreImageB(u8 *dst, u32 size, s16 sbp, s16 sbw, s16 psm, s16 x, s16 y, s16 w, s16 h) {
    u32 i;
    int n;
    u8 *ch;
    s16 rows;
    u8 *p;
    u32 chunks;
    long bitblt;
    long trxpos;

    flPS2DmaWait();
    chunks = size / 0x70000;
    if (size % 0x70000 != 0) {
        chunks++;
    }
    if (chunks == 1) {
        rows = h;
    } else {
        switch (psm) {
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
    if (sbw == 0) {
        sbw = 1;
    }
    ch = *(u8 **)(flPs2State + 0x3C4 + flPs2VIF1Control[0] * 4);
    i = 0;
    if (0 < chunks) {
        trxpos = x;
        bitblt = ((long)psm << 24) | (sbp | ((long)sbw << 16));
        do {
            switch (psm) {
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
                n = (w * rows) / 2;
                break;
            }
            p = flPS2GetSystemTmpBuff(0x90, 0x10);
            flPS2DmaAddEndTag(p, 7, 0, 0);
            *(u32 *)(p + 8) = 0;
            *(u32 *)(p + 0xC) = 0;
            *(u32 *)(p + 0x10) = 0;
            *(u32 *)(p + 0x14) = 0x6008000;
            *(u32 *)(p + 0x18) = 0x13000000;
            *(u32 *)(p + 0x1C) = 0x50000006;
            *(u64 *)(p + 0x20) = 0x8005 | ((long)0x10000000 << 32);
            *(u64 *)(p + 0x28) = 0xE;
            *(u64 *)(p + 0x30) = bitblt;
            *(u64 *)(p + 0x38) = 0x50;
            *(u64 *)(p + 0x40) = trxpos | ((long)y << 16);
            *(u64 *)(p + 0x48) = 0x51;
            *(u64 *)(p + 0x50) = w | ((long)rows << 32);
            *(u64 *)(p + 0x58) = 0x52;
            *(u64 *)(p + 0x60) = 0;
            *(u64 *)(p + 0x68) = 0x61;
            *(u64 *)(p + 0x70) = 1;
            *(u64 *)(p + 0x78) = 0x53;
            flPs2StoreImageSize = (u32)n >> 4;
            flPs2StoreImageAdrs = dst;
            *(volatile u64 *)0x12001000 = 2;
            flPs2StoreImageOldIMR = *(volatile u64 *)0x12001010;
            *(volatile u64 *)0x12001010 = -0x201;
            EnableIntc(0);
            ((DCH *)ch)->b6 = 0;
            ((DCH *)ch)->b7 = 0;
            FlushCache(0);
            sceDmaSend(ch, p);
            sceGsSyncPath(0, 0);
            while (flPs2StoreImageSize != 0) {
            }
            sceGsSyncPath(0, 0);
            *(volatile u32 *)0x10003C00 = 0;
            *(volatile u64 *)0x12001040 = 0;
            *(volatile u64 *)0x12001010 = flPs2StoreImageOldIMR;
            *(volatile u128 *)0x10005000 = vif1_fifo;
            y += rows;
            if (h < y + rows) {
                rows = h - y;
            }
            dst += n;
            i++;
        } while (i < chunks);
    }
}
