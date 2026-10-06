/* tx07 - flPS2GetPaletteVramBlock (SLPM_654.95 0x0018A490-0x0018A4E8). Whole file in tex_nm.c. */
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
