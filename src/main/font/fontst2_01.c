/* SLPM_654.95 0x00161CA0-0x00161D38: han2zen .. han2zen. See fontst2_nm.c. */
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
