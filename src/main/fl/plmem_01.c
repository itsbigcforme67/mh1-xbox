/* SLPM_654.95 0x0018D9D0-0x0018DB24: plmemRegisterAlign .. plmemRegisterAlign. See plmem_nm.c. */
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









int plmemRegisterAlign(PLMEM *m, u32 size, int align) {
    int h;
    if (!(plmemGetFreeSpace(m) > size + align)) {
        return 0;
    }
    if (size == 0) {
        return 0;
    }
    h = plmemPullHandle(m);
    if (h == 0xFFFF) {
        return 0;
    }
    m->blocks[h].x0A = 0;
    m->blocks[h].size = size;
    m->blocks[h].align = m->align;
    if (m->dir != 0) {
        int a;

        m->blocks[h].addr = m->cur;
        a = m->blocks[h].addr;
        m->cur = ~(align - 1) & (a + size + align - 1);
    } else {
        m->blocks[h].addr = ~(align - 1) & (m->cur - size);
        m->cur = m->blocks[h].addr;
    }
    m->used = m->used + size;
    plmemAppendBlockList(m, h);
    return h + 1;
}
