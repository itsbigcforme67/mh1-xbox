/* fs2_02 - font setup 0x00161C20-0x00161C9C: font_print_strings. Whole file in fontst2_nm.c. */
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
