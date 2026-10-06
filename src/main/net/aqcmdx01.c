/* SLPM_654.95 0x0022F650-0x0022F78C: AQQuickSortSub .. AQQuickSortSub. See aqcmd_nm.c. */
#include "types.h"

typedef struct AQNODE {
    struct AQNODE *prev;    /* 0x00 */
    struct AQNODE *next;    /* 0x04 */
    u32 time;               /* 0x08 packet time (sort key) */
    u8 x0C[3];
    u8 flag;                /* 0x0F bit 0x20: node in use */
} AQNODE;

typedef struct AQLIST {
    AQNODE *head;           /* 0x00 */
    AQNODE *tail;           /* 0x04 */
    u8 count;               /* 0x08 */
    u8 max;                 /* 0x09 nodes in the pool */
    u8 esize;               /* 0x0A node size minus 8 */
    u8 dirty;               /* 0x0B list needs sorting */
    u8 *base;               /* 0x0C node pool */
} AQLIST;

typedef struct AQUNIT {
    u8 x00[8];
    AQLIST a;               /* 0x08 */
    AQLIST b;               /* 0x18 */
} AQUNIT;

typedef struct CNGMSG {
    s8 x00;
    u8 pad01[3];
    s32 rd;
    u8 *base;
    s32 wr;
    s32 cap;
} CNGMSG;

void *memcpy(void *, const void *, int);
int CngNetAQBuffEmptyCheck();
u16 CngNet_MSG_ReadU16();
u8 CngNet_MSG_ReadU8();
u32 CngNet_MSG_ReadU32();
int CngNet_MSG_Read();
void AQQuickSortSub(AQNODE **v, int lo, int hi);
void AQSwap(AQNODE **a, AQNODE **b);
void MakePointerList(AQNODE **v, AQLIST *l);
void UpdateAQcommandList(AQLIST *l, AQNODE **v);
void AQQuickSort(AQNODE **v, int n);
void CngNetAQcommandSort(AQLIST *l);










void AQQuickSortSub(AQNODE **v, int lo, int hi) {
    int i = lo;
    int j = hi;
    u32 pivot = v[(lo + hi) / 2]->time;

    for (;;) {
        while (v[i]->time < pivot) {
            i++;
        }
        if (pivot < v[j]->time) {
            do {
                j--;
            } while (v[j]->time > pivot);
        }
        if (i < j) {
            AQSwap(&v[i], &v[j]);
            i++;
            j--;
            continue;
        }
        break;
    }
    if (lo < i - 1) {
        AQQuickSortSub(v, lo, i - 1);
    }
    if (j + 1 < hi) {
        AQQuickSortSub(v, j + 1, hi);
    }
}
