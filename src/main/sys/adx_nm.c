/* NONMATCHING (not built; asm is used). Adx_init is ~38 instructions off: loop register allocation in the
 * ADXT_Create loop (s2/s3 swapped) and the partition-load retry loop. */
/* ADX (CRI audio) start-up. SLPM_654.95 0x00100230-0x0010037C. */
#include "types.h"

typedef struct SIZ_TBL {
    s32 maxch;          /* passed to ADXT_Create */
    s32 size;           /* work size */
} SIZ_TBL;

typedef struct AFS_TBL {
    s32 name;           /* partition file */
    s32 size;
} AFS_TBL;

extern SIZ_TBL siz_tbl[3];
extern AFS_TBL afs_tbl[3];
extern s32 adxw[2];
extern s32 adxt[2];
extern char lit_167_003579B0[];     /* "" */

void flAdxInitialize(const char *);
void ADXT_Init(void);
void ADXT_EntryErrFunc(void (*)(void *, char *), void *);
s32 flAllocMemory(s32);
s32 ADXT_Create(s32, s32, s32);
int ADXF_LoadPartitionNw(int, s32, int, s32);
int ADXF_GetPtStat(int);
void str_init(void);
void movie_reset(void);
void flSfdCrisofdecInit(int);
void load_work_init(void);

static void adx_err_func(void *obj, char *msg) {
}

void Adx_init(void) {
    int i;
    int st;

    flAdxInitialize(lit_167_003579B0);
    ADXT_Init();
    ADXT_EntryErrFunc(adx_err_func, 0);
    for (i = 0; i < 2; i++) {
        adxw[i] = flAllocMemory(siz_tbl[i].size);
        do {
            adxt[i] = ADXT_Create(siz_tbl[i].maxch, adxw[i], siz_tbl[i].size);
        } while (adxt[i] == 0);
    }
    for (i = 0; i < 3; i++) {
retry:
        while (ADXF_LoadPartitionNw(i, afs_tbl[i].name, 0, afs_tbl[i].size) < 0) {
        }
        while ((st = ADXF_GetPtStat(i)) != 3) {
            if (st == 4) {
                goto retry;
            }
        }
    }
    str_init();
    movie_reset();
    flSfdCrisofdecInit(0);
    load_work_init();
}
