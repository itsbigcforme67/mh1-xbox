/* fl hierarchy (node tree) walkers (SLPM_654.95 0x00173E60-0x00174058): flCalcTrans, flSetSkinTrans, flSetSkinTransMatrixList (flSetMatrixList follows in flnode_nm.c).
 * A node is 0xE0+ bytes: matrix at +0x40, skin matrix at +0x80, matrix index (s16) at +0xC4, parent +0xC8, next sibling +0xCC, first child +0xD0. */
#include "types.h"

typedef struct FLNODE FLNODE;
struct FLNODE {
    u8 x0[0xC4];
    s16 mi;             /* 0xC4 matrix list index */
    u8 xC6[2];
    FLNODE *parent;     /* 0xC8 */
    FLNODE *sib;        /* 0xCC */
    FLNODE *child;      /* 0xD0 */
};

extern u8 flMATRIX[];
void flmatMul(void *, void *, void *);
void flPS2_Mem_move64(void *, void *, int);

void flCalcTrans(FLNODE *n, FLNODE *p) {
    FLNODE *top = n;

loop:
    flmatMul(n, (u8 *)n + 0x40, p);
    if (n->child != 0) {
        p = n;
        n = n->child;
        goto loop;
    }
    if (n->sib != 0) {
        n = n->sib;
        goto loop;
    }
    while (n != top) {
        if (n->sib != 0) {
            p = n->sib->parent;
            n = n->sib;
            goto loop;
        }
        n = n->parent;
    }
}

void flSetSkinTrans(FLNODE *n) {
    FLNODE *top = n;

loop:
    flmatMul(flMATRIX + (n->mi << 6), (u8 *)n + 0x80, n);
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
}

void flSetSkinTransMatrixList(u8 *list, FLNODE *n) {
    FLNODE *top = n;

loop:
    flmatMul(list + (n->mi << 6), (u8 *)n + 0x80, n);
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
}
