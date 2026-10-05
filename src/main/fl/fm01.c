/* fm01 - fl misc 0x00187C50-0x00187D10: flPS2GetStaticVramArea. Whole file in flmisc_nm.c. */
#include "types.h"

typedef struct FLPS2 {
    u8 _pad00[0x48];
    int vram_top;               /* 0x48 next free static VRAM page */
    u8 _pad4C[0x58 - 0x4C];
    int fps;                    /* 0x58 */
    u8 _pad5C[0x400 - 0x5C];
    int phase;                  /* 0x400 init phase (0 started, 2 finished) */
} FLPS2;
extern FLPS2 flPs2State;

typedef struct PLMEM {
    int count;                  /* 0x00 block count */
    int size;                   /* 0x04 */
    void *blocks;               /* 0x08 */
    int dir;                    /* 0x0C 1 = allocate upward */
    int cur;                    /* 0x10 */
    int base;                   /* 0x14 aligned start */
    int align;                  /* 0x18 */
    int x1C, x20, x24;
} PLMEM;

typedef struct VRAMENT { s16 used; s16 page; int pages; } VRAMENT;

extern PLMEM sysmemmgr;
extern u8 sysmemblock[];
extern int flVramStaticNum;
extern VRAMENT flVramStatic[3];

void plMemset(void *, int, int);
void plmemRegisterAlign(PLMEM *, int, int);
void plmemRegisterS(PLMEM *, int);
void plmemRegister(PLMEM *, int);
void plmemTemporaryUse(PLMEM *, int);
void plmemRetrieve(PLMEM *, int);
void plmemRelease(PLMEM *, int);
void plmemCompact(PLMEM *);
void plmemInit(PLMEM *, void *, int, int, int, int, int);
void flPS2DeleteAllVramList(void);














int flPS2GetStaticVramArea(int size) {
    int i;
    int pages;
    VRAMENT *e;

    if (flVramStaticNum >= 3) {
        return 0;
    }
    flPS2DeleteAllVramList();
    pages = (((u32)(size + 0xFF) >> 8) + 0x1F) & ~0x1F;
    e = flVramStatic;
    for (i = 0; i < 3; i++) {
        if (e->used == 0) {
            break;
        }
        e++;
    }
    e->used = 1;
    e->page = flPs2State.vram_top;
    e->pages = pages;
    flVramStaticNum++;
    flPs2State.vram_top += pages;
    return (u16)e->page;
}
