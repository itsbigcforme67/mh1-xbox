/* Primitive pools. SLPM_654.95 0x00169230-0x00169300. */
#include "types.h"

typedef struct PRIM_ENT {
    u8 data[0x20];
} PRIM_ENT;

extern PRIM_ENT prim[0x200];
extern PRIM_ENT prim2[0x100];
extern PRIM_ENT pit_prim[4];
extern s32 prim_free_top;
extern s32 prim_free_top2;

void *memset(void *, int, unsigned int);

static void prim_init_sub(PRIM_ENT *p, int n) {
    int i;

    for (i = 0; i < n; i++) {
        memset(p, 0, sizeof(PRIM_ENT));
        p++;
    }
}

void prim_init(void) {
    prim_init_sub(prim, 0x200);
    prim_free_top = 0;
}

void prim_init2(void) {
    prim_init_sub(prim2, 0x100);
    prim_free_top2 = 0;
}

void pit_prim_init(void) {
    prim_init_sub(pit_prim, 4);
}
