/* pm01 - plmem 0x0018DF10-0x0018DF50: plmemRetrieve. Whole file in plmem_nm.c. */
#include "types.h"

typedef struct PLBLK {          /* 0x10 bytes */
    u32 addr;                   /* 0x00 */
    u32 size;                   /* 0x04 */
    s16 align;                  /* 0x08 */
    s16 x0A;                    /* 0x0A */
    u16 prev;                   /* 0x0C */
    u16 next;                   /* 0x0E */
} PLBLK;

typedef struct PLMEM {
    int count;                  /* 0x00 block table entries */
    int size;                   /* 0x04 */
    PLBLK *blocks;              /* 0x08 */
    int dir;                    /* 0x0C */
    u32 cur;                    /* 0x10 next free address */
    u32 base;                   /* 0x14 */
    int align;                  /* 0x18 */
    int used;                   /* 0x1C bytes registered */
    int x20;                    /* 0x20 */
    u32 head;                   /* 0x24 first block of the list (0xFFFF none) */
} PLMEM;

void *plMemset(void *, int, int);
int plmemGetFreeSpace(PLMEM *);
int plmemPullHandle(PLMEM *);
void plmemAppendBlockList(PLMEM *, int);
void plmemDeleteBlockList(PLMEM *, int);
void plmemCompact(PLMEM *);









u32 plmemRetrieve(PLMEM *m, int h) {
    int i = h - 1;

    if (!(i < m->count) || h == 0) {
        return 0;
    }
    return m->blocks[i].addr;
}
