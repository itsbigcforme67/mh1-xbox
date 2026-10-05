/* tx05 - texture handles 0x00189CC0-0x00189CD8: flCompactVRAM, flReloadTexture. Whole file in tex_nm.c. */
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















int flCompactVRAM(void) {
    return flPS2DeleteAllVramList();
}

int flReloadTexture(int a, int b) {
    return flPS2ReloadTexture(a, b);
}
