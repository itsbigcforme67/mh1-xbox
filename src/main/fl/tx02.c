/* tx02 - texture handles 0x00188800-0x00188A80: flPS2GetPaletteHandle, flReleaseTextureHandle, flReleaseTextureHandle_NOWAITDMA, flReleasePaletteHandle. Whole file in tex_nm.c. */
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
