/* aqcmd_nm - AQ command lists (SLPM_654.95 0x0022F240-0x0022F7A4, main.bin): received AQ packets of one slot are
 * kept in a doubly linked list of nodes (sorted by packet time) taken from a fixed node pool; the lists live in
 * the aqwork unit at +0x08 (priority list) and +0x18. Near-match C, not built. */
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

AQNODE *GetAQBuffPtr(AQLIST *l) {
    int i = 0;
    int max = l->max;
    AQNODE *n = (AQNODE *)l->base;

    if (0 < max) {
        for (;;) {
            if ((n->flag & 0x20) == 0) {
                return n;
            }
            i++;
            n = (AQNODE *)((u8 *)n + l->esize + 8);
            if (i >= max) {
                break;
            }
        }
    }
    return 0;
}

int CngNetAQdataToObj(AQUNIT *w, CNGMSG *m, u8 mode) {
    AQLIST *l;
    AQNODE *n;
    u8 rec[0xFC];
    u16 pl;
    u8 kind;
    u8 len;
    u8 *pr;
    u8 flag;
    int k;

    if (mode & 0x80) {
        l = &w->b;
    } else {
        l = &w->a;
    }
    if (CngNetAQBuffEmptyCheck(w, mode) == 0) {
        return 0;
    }
    n = GetAQBuffPtr(l);
    if (n == 0) {
        return 0;
    }
    if (l->count == 0) {
        n->prev = 0;
        l->head = n;
    } else {
        n->prev = l->tail;
        l->tail->next = n;
    }
    l->tail = n;
    n->next = 0;
    pl = CngNet_MSG_ReadU16(m);
    kind = CngNet_MSG_ReadU8(m);
    pr = &len;
    *pr = CngNet_MSG_ReadU8(m);
    *(u32 *)&rec[4] = CngNet_MSG_ReadU32(m);
    *(u16 *)&rec[8] = CngNet_MSG_ReadU16(m);
    k = CngNet_MSG_ReadU8(m) & 0xFF | 0x7F;
    rec[10] = k;
    rec[11] = CngNet_MSG_ReadU8(m);
    k = len;
    if (len % 4 != 0) {
        k = len + (4 - len % 4);
    }
    CngNet_MSG_Read(m, rec + 12, k - 0xC);
    memcpy((u8 *)n + 8, rec + 4, l->esize);
    n->flag = n->flag | 0x20;
    l->count = l->count + 1;
    l->dirty = 1;
    return 1;
}

u8 *CngNetAQcommandExec(AQUNIT *w, u8 mode, u32 now) {
    AQLIST *l;
    AQNODE *n;

    if (mode & 0x80) {
        l = &w->b;
    } else {
        l = &w->a;
    }
    if (l->count == 0) {
        return 0;
    }
    if (l->dirty != 0) {
        CngNetAQcommandSort(l);
        l->dirty = 0;
    }
    n = l->head;
    if (n->time <= now) {
        l->head = n->next;
        n->prev = 0;
        n->next = 0;
        n->flag = n->flag & 0xDF;
        l->count = l->count - 1;
        return (u8 *)n + 8;
    }
    return 0;
}

void CngNetAQcommandSort(AQLIST *l) {
    AQNODE *v[0x100];

    MakePointerList(v, l);
    AQQuickSort(v, l->count);
    UpdateAQcommandList(l, v);
}

void MakePointerList(AQNODE **v, AQLIST *l) {
    AQNODE *n = l->head;

    if (n != 0) {
        do {
            *v = n;
            n = n->next;
            v++;
        } while (n != 0);
    }
}

void UpdateAQcommandList(AQLIST *l, AQNODE **v) {
    int i;
    AQNODE *n;

    l->head = v[0];
    l->tail = v[l->count - 1];
    i = 0;
    if (0 < l->count) {
        do {
            n = *v;
            if (i - 1 < 0) {
                n->prev = 0;
            } else {
                n->prev = v[-1];
            }
            if (i + 1 < l->count) {
                n->next = v[1];
            } else {
                n->next = 0;
            }
            i++;
            v++;
        } while (i < l->count);
    }
}

void AQQuickSort(AQNODE **v, int n) {
    if (n > 1) {
        AQQuickSortSub(v, 0, n - 1);
    }
}

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

void AQSwap(AQNODE **a, AQNODE **b) {
    AQNODE *t = *a;

    *a = *b;
    *b = t;
}
