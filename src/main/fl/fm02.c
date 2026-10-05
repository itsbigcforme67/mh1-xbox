/* fm02 - fl misc 0x0018D880-0x0018D9C8: flInitPhaseStarted, flInitPhaseFinished, flSetFPS, mflInit, mflRegisterS, mflRegister, mflTemporaryUse, mflRetrieve, mflRelease, mflCompact, plmemInit, plmemRegister. Whole file in flmisc_nm.c. */
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














void flInitPhaseStarted(void) {
    flPs2State.phase = 0;
}

void flInitPhaseFinished(void) {
    flPs2State.phase = 2;
}

void flSetFPS(int fps) {
    flPs2State.fps = fps;
}

void mflInit(int base, int size, int align) {
    plmemInit(&sysmemmgr, sysmemblock, 0x3000, base, size, align, 1);
}

void mflRegisterS(int a) {
    plmemRegisterS(&sysmemmgr, a);
}

void mflRegister(int a) {
    plmemRegister(&sysmemmgr, a);
}

void mflTemporaryUse(int a) {
    plmemTemporaryUse(&sysmemmgr, a);
}

void mflRetrieve(int a) {
    plmemRetrieve(&sysmemmgr, a);
}

void mflRelease(int a) {
    plmemRelease(&sysmemmgr, a);
}

void mflCompact(void) {
    plmemCompact(&sysmemmgr);
}

void plmemInit(PLMEM *m, void *blocks, int count, int base, int size, int align, int dir) {
    m->count = count;
    m->blocks = blocks;
    m->size = size;
    m->dir = dir;
    m->align = align;
    if (dir != 0) {
        m->base = (base + align - 1) & ~(align - 1);
    } else {
        m->base = (base + size) & ~(align - 1);
    }
    m->cur = m->base;
    m->x1C = 0;
    m->x20 = 0;
    m->x24 = 0xFFFF;
    plMemset(blocks, 0, count << 4);
}

void plmemRegister(PLMEM *m, int a) {
    plmemRegisterAlign(m, a, m->align);
}
