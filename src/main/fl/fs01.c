/* fs01 - fl system memory 0x0016F5F0-0x0016F680: flMemset. Whole file in flsys_nm.c. */
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
