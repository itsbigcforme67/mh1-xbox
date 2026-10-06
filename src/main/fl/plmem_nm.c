/* plmem: Capcom's block manager. SLPM_654.95 0x0018D9D0-0x0018E430 (f_plmemregisteralign).
 * A PLMEM manages one address range; handles are 1 based indexes into its block table (0x10 byte
 * entries: address, size, align, prev/next in a list sorted by address). dir 1 allocates upward
 * from the start, dir 0 downward from the end. plmemRegisterS / plmemCompact (the big ones) are
 * not done. */
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

u32 plmemTemporaryUse(PLMEM *m, int size) {
    u32 need = ~(m->align - 1) & (size + m->align - 1);

    if (plmemGetFreeSpace(m) < need) {
        plmemCompact(m);
        need = ~(m->align - 1) & (need + m->align - 1);
        if (plmemGetFreeSpace(m) < need) {
            return 0;
        }
    }
    if (m->dir != 0) {
        {
            int b = m->base;

            return b + m->size - need;
        }
    }
    return m->base - m->size;
}

u32 plmemRetrieve(PLMEM *m, int h) {
    int i = h - 1;

    if (!(i < m->count) || h == 0) {
        return 0;
    }
    return m->blocks[i].addr;
}

int plmemRelease(PLMEM *m, int h) {
    PLBLK *b;
    u32 size;

    h--;
    if (!(h < m->count)) {
        return 0;
    }
    size = m->blocks[h].size;
    if (size == 0) {
        return 0;
    }
    m->used -= size;
    m->blocks[h].size = 0;
    m->blocks[h].addr = 0;
    plmemDeleteBlockList(m, h);
    return 1;
}

int plmemGetFreeSpace(PLMEM *m) {
    if (m->dir != 0) {
        {
            int b = m->base;

            return b + m->size - m->cur - m->x20;
        }
    }
    return m->cur - (m->base - m->size) - m->x20;
}

int plmemPullHandle(PLMEM *m) {
    int i;
    PLBLK *b;

    i = 0;
    if (i < m->count) {
        b = m->blocks;
        do {
            if (b->size == 0) {
                plMemset(&m->blocks[i], 0, 0x10);
                return i;
            }
            i++;
            b++;
        } while (i < m->count);
    }
    return 0xFFFF;
}

void plmemAppendBlockList(PLMEM *m, int h) {
    PLBLK *b = m->blocks;
    PLBLK *w = &b[h];
    PLBLK *c;
    u32 i;
    u32 prev;
    u32 hold;

    if (m->head == 0xFFFF) {
        m->head = h;
        w->prev = 0xFFFF;
        w->next = 0xFFFF;
        return;
    }
    prev = 0xFFFF;
    i = m->head;
    c = &b[i];
    if (m->dir != 0) {
        if (!(w->addr < c->addr)) {
            hold = prev;
            while (c->addr < w->addr) {
                prev = i;
                i = c->next;
                if (i == hold) {
                    break;
                }
                c = &b[i];
            }
        }
    } else {
        if (!(c->addr < w->addr)) {
            hold = prev;
            while (w->addr < c->addr) {
                prev = i;
                i = c->next;
                if (i == hold) {
                    break;
                }
                c = &b[i];
            }
        }
    }
    w->prev = prev;
    w->next = i;
    if (prev != 0xFFFF) {
        b[prev].next = h;
    }
    if (i != 0xFFFF) {
        b[i].prev = h;
    }
}

void plmemDeleteBlockList(PLMEM *m, int h) {
    PLBLK *w = &m->blocks[h];

    if (w->prev != 0xFFFF) {
        m->blocks[w->prev].next = w->next;
    } else {
        m->head = w->next;
        if (m->head == 0xFFFF) {
            m->cur = m->base;
        }
    }
    if (w->next != 0xFFFF) {
        m->blocks[w->next].prev = w->prev;
    }
}
