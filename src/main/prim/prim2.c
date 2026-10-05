/* Primitive slots and ordering-table insertion. SLPM_654.95 0x00169300-0x0016989C.
 * prim (0x200 slots of 0x20 bytes) and prim2 (0x100 slots) are pools handed out by
 * get_prim/get_prim2 (prim_free_top = first slot that may be free; a slot is free while its
 * draw callback at +0x14 is 0). add_prim queues a slot on an ordering table by view depth. */
#include "types.h"

/* Same layout as PRIM in prim.h; not included because prim.h declares get_prim_ptr(s16) and
 * add_prim as void, while the originals take int and add_prim returns the slot. */
typedef struct PRIM {
    u8 _pad00[0x04];
    f32 depth;                      /* 0x04 distance from the camera (add_prim) */
    f32 pos[3];                     /* 0x08 */
    void (*trans)(struct PRIM *);   /* 0x14 draw callback */
    void *owner;                    /* 0x18 */
    s32 no;                         /* 0x1C index within the owner */
} PRIM;

extern PRIM prim[0x200];
extern PRIM prim2[0x100];
extern s32 prim_free_top;
extern s32 prim_free_top2;

typedef f32 FLMAT_[4][4];

void *memset(void *, int, unsigned int);
void flmatrLoad(void *, int);
void flvecApplyMat(void *, void *, void *);
void plplAdd(PRIM *, u32 *);
void plplAdd2(PRIM *, u32 *);
PRIM *plplNext(PRIM **);

PRIM *get_prim_ptr(int n) {
    PRIM *r = 0;

    if (n >= 0) {
        r = &prim[n];
    }
    return r;
}

PRIM *get_prim_ptr2(int n) {
    PRIM *r = 0;

    if (n >= 0) {
        r = &prim2[n];
    }
    return r;
}

int get_prim(void) {
    int i = prim_free_top;
    PRIM *p;
    int n;

    if (i >= 0x200) {
        return -1;
    }
    p = get_prim_ptr(i + 1);
    for (n = prim_free_top + 1; n < 0x200; n++) {
        if (p->trans == 0) {
            break;
        }
        p++;
    }
    prim_free_top = n;
    return (s16)i;
}

int get_prim2(void) {
    int i = prim_free_top2;
    PRIM *p;
    int n;

    if (i >= 0x100) {
        return -1;
    }
    p = get_prim_ptr2(i + 1);
    for (n = prim_free_top2 + 1; n < 0x100; n++) {
        if (p->trans == 0) {
            break;
        }
        p++;
    }
    prim_free_top2 = n;
    return (s16)i;
}

void release_prim(int n) {
    memset(get_prim_ptr(n), 0, 0x20);
    if (n < prim_free_top) {
        prim_free_top = n;
    }
}

void release_prim2(int n) {
    PRIM *p = get_prim_ptr2(n);
    void *owner = p->owner;
    s32 no = p->no;

    memset(p, 0, 0x20);
    p->owner = owner;
    p->no = no;
    if (n < prim_free_top2) {
        prim_free_top2 = n;
    }
}

/* Queues p in the ordering table ot (prio entries) by its distance from the camera.
 * flag != 0 keeps it even when behind the camera. Returns the slot used or -1. */
int add_prim(u32 *ot, PRIM *p, u32 prio, int flag) {
    f32 v[4];
    f32 o[4];
    FLMAT_ m;
    u32 idx;

    v[0] = p->pos[0];
    v[1] = p->pos[1];
    v[2] = p->pos[2];
    v[3] = 1.0f;
    flmatrLoad(m, 0x21);
    flvecApplyMat(o, v, m);
    o[2] = o[2] * -1.0f;
    if (o[2] < 0.0f) {
        if (flag != 0) {
            o[2] = 0.0f;
        } else {
            if (o[2] < -1600.0f) {
                return -1;
            }
            o[2] = 0.0f;
        }
    }
    p->depth = o[2];
    idx = (o[2] / 65000.0f) * (f32)prio;
    if (!(idx < prio)) {
        return -1;
    }
    if (prio == 0x40) {
        plplAdd2(p, ot + (prio - 1 - idx));
    } else {
        plplAdd(p, ot + (prio - 1 - idx));
    }
    return idx;
}

int add_prim2(u32 *ot, PRIM *p, int idx, int n) {
    if (!(idx < n && idx >= 0)) {
        return -1;
    }
    plplAdd(p, ot + (n - 1 - idx));
    return idx;
}

void draw_prim(PRIM *p) {
    for (;;) {
        p = plplNext(&p);
        if (p == 0) {
            break;
        }
        p->trans(p);
    }
}

void SetDiffuseColor(f32 *out, u32 col) {
    out[0] = (f32)((col >> 16) & 0xFF) * (1.0f / 255.0f);
    out[1] = (f32)((col >> 8) & 0xFF) * (1.0f / 255.0f);
    out[2] = (f32)(col & 0xFF) * (1.0f / 255.0f);
}
