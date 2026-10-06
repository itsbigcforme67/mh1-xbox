/* SLPM_654.95 0x0016F900-0x0016F98C: flPS2SystemTmpBuffFlush .. flPS2SystemTmpBuffFlush. See flsys_nm.c. */
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














void flPS2SystemTmpBuffFlush(void) {
    switch (flPs2State.phase) {
    case 0:
    case 2:
    case 1:
        flPs2State.tmp_base = flPS2GetSystemBuffAdrs(flPs2State.tmp_handle[flPs2State.tmp_idx]);
        flPs2State.tmp_cur = flPs2State.tmp_base;
        flPs2State.tmp_end = (u8 *)((u32)flPs2State.tmp_base + 0x80000);
        break;
    }
}
