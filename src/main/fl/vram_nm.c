/* VRAM page list. SLPM_654.95 0x0018B3B0-0x0018BAE0 (g_flPS2VramInit): a doubly linked list of
 * used VRAM ranges (flVramList, entries from the flVramControl pool of 0x200), plus the 3 static
 * areas. Textures/palettes (TEXH) remember their start page at +0x14 and a "resident" flag at +0x35. */
#include "types.h"

typedef struct VRC {            /* 0x1C bytes */
    struct VRC *prev;           /* 0x00 */
    struct VRC *next;           /* 0x04 */
    struct TEXH *owner;         /* 0x08 */
    int x0C, x10;               /* 0x0C copied from owner +4, +0 */
    s16 page;                   /* 0x14 first VRAM page */
    s16 len;                    /* 0x16 pages */
    s16 align;                  /* 0x18 */
    u8 _pad1A[2];
} VRC;

typedef struct TEXH {
    int x0, x4;
    u8 _pad08[8];
    s16 len;                    /* 0x10 pages */
    s16 align;                  /* 0x12 */
    s16 page;                   /* 0x14 */
    u8 _pad16[0x35 - 0x16];
    s8 resident;                /* 0x35 */
} TEXH;

typedef struct VRAMENT { s16 used; s16 page; int pages; } VRAMENT;

typedef struct FLST { u8 _pad00[0x48]; int vram_top; } FLST;

extern FLST flPs2State;
extern VRAMENT flVramStatic[3];
extern int flVramStaticNum;
extern int flCTH;
extern VRC flVramControl[0x200];
extern int flVramNum;
extern VRC *flVramList;

void flMemset(void *, int, int);
VRC *flPS2PullVramWork();
VRC *flPS2SearchVramList(TEXH *);
int flPS2AddVramList(VRC *, TEXH *);
VRC *flPS2SearchVramSpace(u32, int);

void flPS2VramInit(void) {
    int i;

    flVramStaticNum = 0;
    flCTH = 1;
    for (i = 0; i < 3; i++) {
        flMemset(&flVramStatic[i], 0, 8);
    }
    flVramNum = 0;
    flVramList = 0;
    for (i = 0; i < 0x200; i++) {
        flMemset(&flVramControl[i], 0, 0x1C);
    }
}

VRC *flPS2PullVramWork() {
    int i;

    for (i = 0; i < 0x200; i++) {
        if (flVramControl[i].page == 0) {
            flVramNum++;
            return &flVramControl[i];
        }
    }
    return 0;
}

void flPS2PushVramWork(VRC *w) {
    VRC *prev = w->prev;
    VRC *next = w->next;

    if (prev != 0) {
        prev->next = next;
    } else if (next != 0) {
        flVramList = next;
    } else {
        flVramList = 0;
    }
    if (next != 0) {
        next->prev = prev;
    }
    if (w->owner != 0) {
        w->owner->resident = 0;
        *(int *)w->owner = 4;
    }
    flMemset(w, 0, 0x1C);
    flVramNum--;
}

void flPS2ChainVramWork(VRC *after, VRC *w) {
    VRC *old = 0;

    if (after != 0) {
        old = after->next;
        after->next = w;
    } else {
        if (flVramList == 0) {
            flVramList = w;
        } else {
            old = flVramList;
            flVramList = w;
        }
    }
    w->prev = after;
    w->next = old;
    if (old != 0) {
        old->prev = w;
    }
}

VRC *flPS2SearchVramList(TEXH *owner) {
    VRC *p = flVramList;

    while (p != 0) {
        if (p->owner == owner) {
            return p;
        }
        p = p->next;
    }
    return 0;
}

int flPS2AddVramList(VRC *after, TEXH *t) {
    VRC *w;
    s16 page;

    if (after == 0) {
        if ((w = flPS2PullVramWork(flVramControl, 0x200)) == 0) {
            return 0;
        }
        flPS2ChainVramWork(after, w);
        page = flPs2State.vram_top;
    } else {
        int a;

        if ((w = flPS2PullVramWork(flVramControl, 0x200)) == 0) {
            return 0;
        }
        flPS2ChainVramWork(after, w);
        a = t->align - 1;
        page = ~a & (a + (after->page + after->len));
    }
    t->page = page;
    t->resident = 1;
    w->owner = t;
    w->x0C = t->x4;
    w->x10 = t->x0;
    w->page = page;
    w->len = t->len;
    w->align = t->align;
    return 1;
}

int flPS2RewriteVramList(VRC *at, TEXH *t) {
    if (at == 0) {
        if ((at = flPS2PullVramWork(flVramControl, 0x200)) == 0) {
            return 0;
        }
        flPS2ChainVramWork(0, at);
        at->page = flPs2State.vram_top;
    } else {
        if (at->next != 0) {
            while (at->next->page - at->page < t->len) {
                flPS2PushVramWork(at->next);
                if (at->next == 0) {
                    break;
                }
            }
        }
    }
    if (at->owner != 0) {
        at->owner->resident = 0;
        *(int *)at->owner = 4;
    }
    t->page = at->page;
    t->resident = 1;
    at->owner = t;
    at->x0C = t->x4;
    at->x10 = t->x0;
    at->len = t->len;
    at->align = t->align;
    return 1;
}

int flPS2DeleteAllVramList(void) {
    while (flVramList != 0) {
        flPS2PushVramWork(flVramList);
    }
    return 1;
}

int flPS2DeleteVramList(TEXH *owner) {
    VRC *p = flPS2SearchVramList(owner);

    if (p != 0) {
        flPS2PushVramWork(p);
    }
    return 1;
}

VRC *flPS2SearchVramSpace(u32 len, int align) {
    VRC *p = flVramList;
    VRC *next;
    u32 aligned;
    int end;
    int mask;

    if (p == 0) {
        return 0;
    }
    if (p->page != flPs2State.vram_top) {
        if (!((u32)(p->page - flPs2State.vram_top) < len)) {
            return 0;
        }
    }
    goto pre;
loop:
    end = p->page;
    end += p->len;
    next = p->next;
    aligned = mask & (end + align);
    if (next == 0) {
        if (end >= 0x4000) {
            return (VRC *)-1;
        }
        if (aligned + len >= 0x4000) {
            p = (VRC *)-1;
        }
        return p;
    }
    if (aligned < next->page) {
        if (!((u32)(next->page - aligned) < len)) {
            return p;
        }
    }
    p = next;
    goto loop;
pre:
    align--;
    mask = ~align;
    goto loop;
}

extern TEXH flTexture[];
extern TEXH flPalette[];

/* For each packed (palette << 16 | texture) handle of the list, makes sure the resident
 * texture/palette has VRAM space; returns 0 if some placement failed. */
int flPS2GetVramFreeArea(u32 *list, int n) {
    u32 e;
    int i;
    TEXH *t;
    VRC *r;

    for (i = 0; i < n; i++) {
        e = *list++;
        if ((u16)e != 0) {
            t = &flTexture[(u16)e - 1];
            if (t->resident == 0) {
                r = flPS2SearchVramSpace(t->len, t->align);
                if (r == (VRC *)-1) {
                    return 0;
                }
                if (flPS2AddVramList(r, t) == 0) {
                    return 0;
                }
            }
        }
        if ((e & 0xFFFF0000) >> 16 != 0) {
            t = &flPalette[((e & 0xFFFF0000) >> 16) - 1];
            if (t->resident == 0) {
                r = flPS2SearchVramSpace(t->len, t->align);
                if (r == (VRC *)-1) {
                    return 0;
                }
                if (flPS2AddVramList(r, t) == 0) {
                    return 0;
                }
            }
        }
    }
    return 1;
}
