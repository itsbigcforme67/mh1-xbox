/* Texture/palette handles. SLPM_654.95 0x00188420-0x0018A4E8 (f_flps2_187D10, parts).
 * flTexture and flPalette are tables of 0x100 handles of 0x38 bytes; handle numbers are 1 based
 * (palette handles are returned shifted left 16). */
#include "types.h"

typedef struct TEXH {
    int x0, x4;
    u8 _pad08[0x10 - 8];
    s16 x10, x12, x14, x16, x18;
    s16 w;                      /* 0x1A */
    s16 h;                      /* 0x1C */
    u8 _pad1E[0x20 - 0x1E];
    int x20;                    /* 0x20 palette format */
    int sysmem;                 /* 0x24 system memory handle of the pixel data */
    u8 _pad28[0x34 - 0x28];
    s8 used;                    /* 0x34 */
    s8 resident;                /* 0x35 */
    u8 _pad36[2];
} TEXH;

extern TEXH flTexture[0x100];
extern TEXH flPalette[0x100];
extern int flCTNum;
extern int flPTNum;

void flPS2DmaTerminate(void);
void flPS2DeleteVramList(TEXH *);
void flPS2ReleaseSystemMemory(int);
void flMemset(void *, int, int);
int flPS2LockTexture(int, TEXH *, int, int, int);
int flPS2UnlockTexture(TEXH *);
int flPS2DeleteAllVramList(void);
int flPS2ReloadTexture(int, int);

int flPS2GetTextureHandle(void) {
    int i;
    TEXH *t = flTexture;

    for (i = 0; i < 0x100; i++) {
        if (t->used == 0) {
            break;
        }
        t++;
    }
    return i + 1;
}

int flPS2GetPaletteHandle(void) {
    int i;
    TEXH *t = flPalette;

    for (i = 0; i < 0x100; i++) {
        if (t->used == 0) {
            break;
        }
        t++;
    }
    return (i + 1) << 16;
}

int flReleaseTextureHandle(u32 h) {
    TEXH *t = &flTexture[h - 1];

    if (h == 0) {
        return 0;
    }
    if (h > 0x100) {
        return 0;
    }
    if (t->used == 0) {
        return 0;
    }
    flPS2DmaTerminate();
    flPS2DeleteVramList(t);
    if (t->sysmem != 0) {
        flPS2ReleaseSystemMemory(t->sysmem);
    }
    flMemset(t, 0, 0x38);
    flCTNum--;
    return 1;
}

int flReleaseTextureHandle_NOWAITDMA(u32 h) {
    TEXH *t = &flTexture[h - 1];

    if (h == 0) {
        return 0;
    }
    if (h > 0x100) {
        return 0;
    }
    if (t->used == 0) {
        return 0;
    }
    flPS2DeleteVramList(t);
    if (t->sysmem != 0) {
        flPS2ReleaseSystemMemory(t->sysmem);
    }
    flMemset(t, 0, 0x38);
    flCTNum--;
    return 1;
}

int flReleasePaletteHandle(u32 h) {
    TEXH *t = &flPalette[h - 1];

    if (h == 0) {
        return 0;
    }
    if (h > 0x100) {
        return 0;
    }
    if (t->used == 0) {
        return 0;
    }
    flPS2DmaTerminate();
    flPS2DeleteVramList(t);
    if (t->sysmem != 0) {
        flPS2ReleaseSystemMemory(t->sysmem);
    }
    flMemset(t, 0, 0x38);
    flPTNum--;
    return 1;
}

int flLockTexture(int a, u32 h, int b, int c) {
    TEXH *t = &flTexture[h - 1];

    if (h > 0x100) {
        return 0;
    }
    if (t->used != 0) {
        return flPS2LockTexture(a, t, b, c, 0);
    }
    return 0;
}

int flLockPalette(int a, u32 h, int *out, int c) {
    TEXH *t = &flPalette[h - 1];

    if (h > 0x100) {
        return 0;
    }
    if (t->used == 0) {
        return 0;
    }
    if (flPS2LockTexture(a, t, (int)out, c, 1) == 0) {
        return 0;
    }
    if (t->w == 0x10 && t->h == 0x10) {
        out[1] = 0x100;
        out[2] = 1;
    } else {
        out[1] = 0x10;
        out[2] = 1;
    }
    return 1;
}

int flUnlockTexture(u32 h) {
    TEXH *t = &flTexture[h - 1];

    if (h > 0x100) {
        return 0;
    }
    if (t->used != 0) {
        return flPS2UnlockTexture(t);
    }
    return 0;
}

int flUnlockPalette(u32 h) {
    TEXH *t = &flPalette[h - 1];

    if (h > 0x100) {
        return 0;
    }
    if (t->used != 0) {
        return flPS2UnlockTexture(t);
    }
    return 0;
}

int flCompactVRAM(void) {
    return flPS2DeleteAllVramList();
}

int flReloadTexture(int a, int b) {
    return flPS2ReloadTexture(a, b);
}

int flPS2GetTextureBuffWidth(s16 w) {
    int r = 1;
    int i;

    for (i = 1; i <= 10; i++) {
        if (!((r << i) < w)) {
            return (s16)i;
        }
    }
    return r;
}

int flPS2GetPaletteVramBlock(TEXH *p) {
    int r;
    if (p->h == 1) {
        return 2;
    }
    switch (p->x20) {
    case 0:
    case 1:
        return 4;
    case 2:
        r = 4;
    }
    return r;
}
int flPS2GetTextureSize(int fmt, int w, int h, int levels) {
    int size = 0;
    int i;

    for (i = 0; i < levels; i++) {
        switch (fmt) {
        case 0:
        case 1:
            size += w * h * 4;
            break;
        case 2:
        case 0xA:
            size += w * h * 2;
            break;
        case 0x13:
            size += w * h;
            break;
        case 0x14:
            size += (w * h) >> 1;
            break;
        }
        w >>= 1;
        h >>= 1;
    }
    return size;
}
