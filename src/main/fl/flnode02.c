/* fl hierarchy (node tree) group search (SLPM_654.95 0x00174170-0x00174290): flSetMotionExSub, flFindGroupRoot. Node layout as in flnode01.c (+0xC6 group id). */
#include "types.h"

typedef struct FLNODE FLNODE;
struct FLNODE {
    u8 x0[0xC4];
    s16 mi;             /* 0xC4 */
    u16 grp;            /* 0xC6 group id */
    FLNODE *parent;     /* 0xC8 */
    FLNODE *sib;        /* 0xCC */
    FLNODE *child;      /* 0xD0 */
    int handle;         /* 0xD4 */
    int data;           /* 0xD8 */
};

extern int cur_handle;

int flSetMotionExSub(FLNODE *n, int *src, int id) {
    FLNODE *top;
    u16 g = id;

    top = n;
loop:
    if (n->grp == g) {
        n->handle = cur_handle;
        n->data = *src;
        src++;
        if (n->child != 0) {
            n = n->child;
            goto loop;
        }
    }
    if (n->sib != 0 && n != top) {
        n = n->sib;
        goto loop;
    }
    while (n != top) {
        while (n->sib != 0) {
            n = n->sib;
            if (n->grp == g) {
                goto loop;
            }
        }
        n = n->parent;
    }
    return 1;
}

FLNODE *flFindGroupRoot(FLNODE *n, int id) {
    FLNODE *top = n;
    u16 g = id;

loop:
    if (n->grp == g) {
        return n;
    }
    if (n->child != 0) {
        n = n->child;
        goto loop;
    }
    if (n->sib != 0) {
        n = n->sib;
        goto loop;
    }
    while (n != top) {
        if (n->sib != 0) {
            n = n->sib;
            goto loop;
        }
        n = n->parent;
    }
    return 0;
}
