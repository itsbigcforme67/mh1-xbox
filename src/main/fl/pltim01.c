/* SLPM_654.95 0x00193B00-0x00193F5C: TIM2 / BMP helpers of the pl library (plTIM2GetPixelAddressFromImage and friends). */
#include "types.h"

void plReport(char *);
extern char lit_110_0035C150[];

static u32 InputTim2AlignRegulation(u8 *p, u32 size);
static u8 *GetTim2PictureHead(u8 *p, int n);
static u8 *GetTim2PictureData(u8 *p, int a, int mip);
static u8 *GetTim2ClutData(u8 *p, int mip);
static int CheckTIM2FileHeader(u8 *p);

u8 *plTIM2GetPixelAddressFromImage(u8 *p, int mip) {
    return GetTim2PictureData(p, 0, mip);
}

u8 *plTIM2GetPaletteAddressFromImage(u8 *p) {
    return GetTim2ClutData(p, 0);
}

int plTIM2GetMipmapTextureNum(u8 *p) {
    if (CheckTIM2FileHeader(p) == 0) {
        return 0;
    }
    return (GetTim2PictureHead(p, 0)[0x11] - 1) & 0xFF;
}

u8 *plBMPGetPixelAddressFromImage(u8 *p) {
    if (*(u16 *)p != 0x4D42) {
        plReport(lit_110_0035C150);
        return 0;
    }
    return p + (*(u16 *)(p + 0xA) | (*(u16 *)(p + 0xC) << 16));
}

static u32 InputTim2AlignRegulation(u8 *p, u32 size) {
    u32 a = p[5] == 0 ? 0x10 : 0x80;

    if (size % a != 0) {
        size += a - (size - a * (size / a)) % a;
    }
    return size;
}

static u8 *GetTim2PictureHead(u8 *q, int n) {
    u8 *p = q;
    u8 *r;
    int i;

    q += InputTim2AlignRegulation(q, 0x10);
    r = q;
    if (n > 0) {
        for (i = 0; i < *(u16 *)(p + 6); ) {
            q += InputTim2AlignRegulation(p, 0x30);
            q += *(u32 *)r;
            i++;
            r = q;
            if (!(i < n)) break;
        }
    }
    return r;
}

static int CheckTIM2FileHeader(u8 *p) {
    if (p[0] != 0x54 || p[1] != 0x49 || p[2] != 0x4D || p[3] != 0x32) {
        int r;
        if (p[0] == 0x43 && p[1] == 0x4C && p[2] == 0x54) {
            r = 0;
        } else {
            r = 0;
        }
        return r;
    }
    switch (p[4]) {
    case 4:
        if (p[5] >= 2) {
    default:
            return 0;
        }
    case 3:
        {
            int r = 1;
            if (*(u16 *)(p + 6) != 1) r = 0;
            return r;
        }
    }
}

static u8 *GetTim2ClutData(u8 *p, int mip) {
    u8 *q = GetTim2PictureHead(p, 0);
    u8 *h = q;
    int a = 0x30;
    int m = h[0x11];

    if (m > 1) {
        a += (m < 5) ? 0x20 : 0x30;
    }
    q += InputTim2AlignRegulation(p, a);
    if (*(u32 *)(h + 4) != 0) {
        return q + *(u32 *)(h + 8);
    }
    return 0;
}
