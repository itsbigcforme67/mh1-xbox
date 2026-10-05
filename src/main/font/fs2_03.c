/* fs2_03 - font setup 0x00161D40-0x00161DE0: font_work_set. Whole file in fontst2_nm.c. */
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
