/* Font setup and drawing. SLPM_654.95 0x001617E0-0x00161D40 (f_font, second part). */
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

/* Print each string of the NULL terminated list at (x, y), moving down by dy between lines. */
void font_print_strings(int x, int y, char **strs, int dy) {
    if (*strs != 0) {
        do {
            flfntLocate(x, y);
            font_print_sp((int)*strs);
            y = (s16)(y + dy);
            strs++;
        } while (*strs != 0);
    }
}

/* Half width ASCII to full width SJIS using zen_tbl (2 bytes per code); SJIS pairs are copied. */
void han2zen(u8 *src, u8 *dst) {
    int c;

    c = *src;
    while (c != 0) {
        if ((c >= 0x80 && c <= 0x9F) || (c >= 0xE0 && c < 0x100)) {
            dst[0] = c;
            dst[1] = src[1];
            src += 2;
            dst += 2;
        } else {
            u8 *p = zen_tbl + c * 2;

            src++;
            dst[0] = p[0];
            dst[1] = p[1];
            dst += 2;
        }
        c = *src;
    }
    *dst = 0;
}

void font_work_set(u8 *dst, s16 kind) {
    char buf[0x80];

    switch (kind) {
    case 0:
        sprintf(buf, lit_505_0035BB10, *(int *)(card_w + 0x18) + 1);
        han2zen((u8 *)buf, dst);
        break;
    case 1:
        sprintf(buf, lit_505_0035BB10, *(int *)(card_w + 0x48));
        han2zen((u8 *)buf, dst);
        break;
    }
}
