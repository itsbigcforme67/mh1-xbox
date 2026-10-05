/* fs02 - fl system memory 0x0016F770-0x0016F8FC: flAllocMemory, flGetFrame, flReleaseFrame, flAllocMemoryS, flPS2GetSystemMemoryHandle, flPS2ReleaseSystemMemory, flPS2GetSystemBuffAdrs, flCompact, flPS2SystemTmpBuffInit. Whole file in flsys_nm.c. */
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
