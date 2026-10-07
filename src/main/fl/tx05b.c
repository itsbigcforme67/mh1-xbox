/* tx05b - SLPM_654.95 0x00189CE0-0x00189EDC: flPS2ReloadTexture. For each packed (palette << 16 | texture) handle of the list makes sure the
 * texture and the palette are resident in VRAM (searching free space, or moving other entries away), uploads the changed ones and ends the
 * transfer with one end-of-load packet. Returns how many were uploaded. flPS2VramTrans is file-static in fltex01.c (alias in config/main_aliases.txt). */
#include "types.h"

typedef struct TEXH {
    int x0, x4;
    u8 _pad08[8];
    s16 len;                    /* 0x10 pages */
    s16 align;                  /* 0x12 */
    u8 _pad14[0x35 - 0x14];
    s8 resident;                /* 0x35 */
    u8 _pad36[2];
} TEXH;

typedef struct VRC VRC;

extern TEXH flTexture[0x100];
extern TEXH flPalette[0x100];
extern u8 flPs2VIF1Control[];

int flPS2SearchVramSpace(int, int);
int flPS2AddVramList(int, TEXH *);
int flPS2SearchVramChange(TEXH *, int *, int);
int flPS2RewriteVramList(int, TEXH *);
void flPS2VramTrans(TEXH *);
int flPS2VIF1CalcEndLoadImageSize(int);
long flPS2GetSystemTmpBuff(int, int);
void flPS2VIF1MakeEndLoadImage(long, int);
int flPS2DmaAddQueue2(int, unsigned long, long, void *);

int flPS2ReloadTexture(int n, int *list) {
    TEXH *t;
    int k;
    int cnt;
    u32 tex;
    u32 pal;
    TEXH *last;
    int i;
    int *p;
    TEXH *tt;
    TEXH *pp;
    int r;
    long buf;

    last = 0;
    cnt = 0;
    i = 0;
    if (0 < n) {
        p = list;
        do {
            k = 0;
            tex = *p & 0xFFFF;
            pal = (u32)(*p & 0xFFFF0000) >> 16;
            tt = &flTexture[tex - 1];
            pp = &flPalette[pal - 1];
            do {
                if (k == 0) {
                    if (tex == 0 || tex >= 0x100) {
                        goto next;
                    }
                    t = tt;
                } else {
                    if (pal == 0 || pal >= 0x100) {
                        goto next;
                    }
                    t = pp;
                }
                if (t->resident == 0) {
                    r = flPS2SearchVramSpace(t->len, t->align);
                    if (r != -1) {
                        flPS2AddVramList(r, t);
                    } else {
                        do {
                            r = flPS2SearchVramChange(t, list, n);
                        } while (r == -1);
                        flPS2RewriteVramList(r, t);
                    }
                    flPS2VramTrans(t);
                    last = t;
                    cnt++;
                }
next:
                k++;
            } while (k < 2);
            p++;
            i++;
        } while (i < n);
    }
    if (last != 0) {
        buf = flPS2GetSystemTmpBuff(flPS2VIF1CalcEndLoadImageSize(0), 0x10);
        flPS2VIF1MakeEndLoadImage(buf, 1);
        flPS2DmaAddQueue2(0, (unsigned long)buf & 0xFFFFFFFUL, buf, flPs2VIF1Control);
    }
    return cnt;
}
