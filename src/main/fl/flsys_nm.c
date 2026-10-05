/* fl system memory helpers. SLPM_654.95 0x0016F5F0-0x0016FA00 (g_flPS2DmaAddQueue, last part):
 * flMemset/flMemcpy, the frame memory stack wrappers (flFMS), system memory handles and the
 * double-buffered system temp buffer in flPs2State (+0x404 buffer index, +0x408 base,
 * +0x40C end, +0x410 cursor, +0x414[2] handles). */
#include "types.h"

typedef struct FLSYS {
    u8 _pad00[0x400];
    int phase;                  /* 0x400 */
    int tmp_idx;                /* 0x404 */
    u8 *tmp_base;               /* 0x408 */
    u8 *tmp_end;                /* 0x40C */
    u8 *tmp_cur;                /* 0x410 */
    int tmp_handle[2];          /* 0x414 */
} FLSYS;
extern FLSYS flPs2State;
extern u8 flFMS[];

extern char lit_404_0035BE10[], lit_405_0035BE20[];

int fmsAllocMemory(void *, int, int);
int fmsGetFrame(void *, int, int);
void fmsReleaseFrame(void *, int);
int mflRegisterS(int);
int mflRegister(int);
void mflRelease(int);
u8 *mflRetrieve(int);
void flCompact(void);
void flPS2DmaTerminate(void);
void mflCompact(void);
void flPS2ClayRetouchMaterialTag(void);
void flPS2DmaWait(void);
void __assert(char *, int, char *);
int flPS2GetSystemMemoryHandle(int, int);
u8 *flPS2GetSystemBuffAdrs(int);
void flPS2SystemTmpBuffFlush(void);

void flMemset(u8 *d, int v, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *d++ = v;
    }
}

void flMemcpy(u8 *d, s8 *s, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *d++ = *s++;
    }
}

int flAllocMemory(int n) {
    return fmsAllocMemory(flFMS, n, 0);
}

int flGetFrame(int out) {
    return fmsGetFrame(flFMS, 0, out);
}

void flReleaseFrame(int n) {
    fmsReleaseFrame(flFMS, n);
}

int flAllocMemoryS(int n) {
    return fmsAllocMemory(flFMS, n, 1);
}

int flPS2GetSystemMemoryHandle(int size, int sys) {
    int h = mflRegisterS(size);

    if (h == 0) {
        flCompact();
        h = mflRegister(size);
        if (h == 0) {
            __assert(lit_404_0035BE10, 0x1D9, lit_405_0035BE20);
        }
    }
    return h;
}

void flPS2ReleaseSystemMemory(int h) {
    mflRelease(h);
}

u8 *flPS2GetSystemBuffAdrs(int h) {
    return mflRetrieve(h);
}

void flCompact(void) {
    flPS2DmaTerminate();
    mflCompact();
    flPS2ClayRetouchMaterialTag();
}

void flPS2SystemTmpBuffInit(void) {
    int i;
    int *s = (int *)&flPs2State;

    for (i = 0; i < 2; i++) {
        s[0x105] = flPS2GetSystemMemoryHandle(0x80000, 1);
        s++;
    }
    flPS2SystemTmpBuffFlush();
}

void flPS2SystemTmpBuffFlush(void) {
    switch (flPs2State.phase) {
    case 0:
    case 2:
    case 1:
        flPs2State.tmp_base = flPS2GetSystemBuffAdrs(flPs2State.tmp_handle[flPs2State.tmp_idx]);
        flPs2State.tmp_cur = flPs2State.tmp_base;
        flPs2State.tmp_end = flPs2State.tmp_base + 0x80000;
        break;
    }
}

u8 *flPS2GetSystemTmpBuff(int size, int align) {
    u8 *p = (u8 *)(((u32)flPs2State.tmp_cur + align - 1) & ~(align - 1));
    u8 *end = p + size;

    if (end > flPs2State.tmp_end) {
        flPS2DmaWait();
        p = flPs2State.tmp_base;
        end = p + size;
    }
    flPs2State.tmp_cur = end;
    return p;
}
