/* Lobby browser: simple cell allocator over a fixed arena (BsMemCell), hand-written from m2c drafts + asm. */
#include "lobby_f.h"
typedef struct MCELL { s32 size; u8 *ptr; struct MCELL *next; } MCELL;   /* 0xC bytes, 512 cells, last one = end sentinel */
extern MCELL BsMemCell[0x200];
extern u8 D_0070FFF0[];
extern u8 BsCommonTemp1[];
extern u8 BsCommonTemp2[0x1A000];
void *memmove(void *, const void *, unsigned int);
void BsMemAllocInitialize(void) {
    memset(BsMemCell, 0, 0x1800);
    BsMemCell[0x1FF].next = 0;
    BsMemCell[0].size = 0x10;
    BsMemCell[0].ptr = D_0070FFF0;
    BsMemCell[0].next = &BsMemCell[0x1FF];
    BsMemCell[0x1FF].size = 1;
    BsMemCell[0x1FF].ptr = BsCommonTemp1;
    memset(BsCommonTemp2, 0, 0x1A000);
}
static MCELL *_inet_mem_get_free_cell_005E83C0(void) {
    int i;
    MCELL *c;
    c = BsMemCell;
    for (i = 0; i < 0x200; i++, c++) {
        if (c->size == 0) {
            return &BsMemCell[i];
        }
    }
    return 0;
}
MCELL *_inet_mem_get_this_cell(u8 *p) {
    MCELL *c;
    c = BsMemCell;
    for (;;) {
        if (c->ptr == p) {
            return c;
        }
        c = c->next;
        if (c == &BsMemCell[0x1FF]) {
            return 0;
        }
    }
}
u8 *BsMemAlloc(u32 size) {
    MCELL *cur;
    MCELL *n;
    MCELL *c;
    if (size == 0) {
        return 0;
    }
    size = size + (0x10 - (size & 0xF));
    cur = BsMemCell;
    for (;;) {
        n = cur->next;
        if (!(size > (u32)(n->ptr - (cur->ptr + cur->size)))) {
            c = _inet_mem_get_free_cell_005E83C0();
            if (c == 0) {
                return 0;
            }
            c->size = size;
            c->ptr = cur->ptr + cur->size;
            c->next = cur->next;
            cur->next = c;
            return c->ptr;
        }
        if (n == &BsMemCell[0x1FF]) {
            return 0;
        }
        cur = n;
    }
}
void BsMemFree(u8 *p) {
    MCELL *prev;
    MCELL *cur;
    prev = BsMemCell;
    cur = BsMemCell;
lp:
    if (cur->next != &BsMemCell[0x1FF]) {
        cur = prev->next;
        if (cur->ptr == p) {
            cur->size = 0;
            cur->ptr = 0;
            prev->next = cur->next;
            cur->next = 0;
            return;
        }
        prev = cur;
        goto lp;
    }
}
u8 *BsMemRealloc(u8 *p, u32 size) {
    u8 *nv;
    MCELL *cell;
    u32 s0;
    u32 s2;
    s2 = size + (0x10 - (size & 0xF));
    if (p == 0) {
        return BsMemAlloc(size);
    }
    if (s2 == 0) {
        BsMemFree(p);
        return 0;
    }
    cell = _inet_mem_get_this_cell(p);
    if (cell == 0) {
        return 0;
    }
    s0 = cell->size;
    nv = BsMemAlloc(size);
    if (nv == 0) {
        return 0;
    }
    BsMemFree(p);
    if (s0 < s2) {
        memmove(nv, p, s0);
    } else {
        memmove(nv, p, s2);
    }
    return nv;
}
