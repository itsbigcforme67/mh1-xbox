/* fs2_01 - font setup 0x00161770-0x00161968: font_set, font_set2, font_stack_reset, font_draw, font_draw_stack_no. Whole file in fontst2_nm.c. */
#include "types.h"

extern u32 nfcol_tbl[][3];
extern u32 nfrvcol_tbl[][4];
extern int font_reset_flag;
extern u8 *zen_tbl;
extern u8 card_w[];
extern char lit_505_0035BB10[];

typedef struct NP { u8 _pad00[0xC]; void *area; } NP;
extern NP *np;

void flfntStackReset(void);
void flfntDraw(int);
int flfntGetSystemMemorySize(void);
void flGetFrame(void *);
void *flAllocMemory(int);
void flfntCreate(void *);
void flfntInit(void);
void flfntSetHalftype(int);
void flfntSetPalData(int, u32, u32, u32, u32);
int load_file_mdl(void *, int);
int sprintf(char *, const char *, ...);
void flfntLocate(int, int);
void font_print_sp(int);









void font_set(void) {
    int i;
    u32 *p;
    int size = flfntGetSystemMemorySize();
    u8 fr[8];

    flGetFrame(fr);
    flfntCreate(flAllocMemory(size));
    load_file_mdl(np->area, 0x6D2);
    flfntInit();
    flfntSetHalftype(1);
    p = nfcol_tbl[0];
    for (i = 1; i < 0xE; i++) {
        flfntSetPalData(i, 0, p[0], p[1], p[2]);
        p += 3;
    }
    p = nfrvcol_tbl[0];
    for (; i < 0x10; i++) {
        flfntSetPalData(i, p[0], p[1], p[2], p[3]);
        p += 4;
    }
}

void font_set2(void) {
    int i;
    u32 *p;

    p = nfcol_tbl[0];
    for (i = 1; i < 0xE; i++) {
        flfntSetPalData(i, 0, p[0], p[1], p[2]);
        p += 3;
    }
    p = nfrvcol_tbl[0];
    for (; i < 0x10; i++) {
        flfntSetPalData(i, p[0], p[1], p[2], p[3]);
        p += 4;
    }
}

void font_stack_reset(void) {
    flfntStackReset();
    font_reset_flag = 0;
}

void font_draw(void) {
    s16 i;

    for (i = 0; i < 5; i++) {
        flfntDraw(i);
    }
    font_reset_flag = 1;
}

void font_draw_stack_no(int n) {
    flfntDraw(n);
}
