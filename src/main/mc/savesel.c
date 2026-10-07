/* save slot selection screen parts (0x002811C0-0x002814D8): disp_savesel_waku (frames and cursor), disp_savesel_moji (slot texts). disp_savesel 0x280EF0 stays asm, see wip/disp_savesel_nm.c */
#include "types.h"

extern u8 option_w[];
extern char lit_159_00384EA8[];
extern char lit_160_00384EB0[];
extern char lit_161_00384EB8[];
extern char lit_162_00384ED0[];
extern int sex_char_tbl[];

void flfntSetSize(int, int);
typedef struct {
    s16 x;
    s16 y;
    u8 colw;
    u8 h;
    u8 cols;
    u8 rows;
    s16 pal;
    u16 mode;
    u32 col;
} SFRM;
void DispFrameMessageA(SFRM *, char *, int);
void Sel_csr_disp(s16, s16, int, int, u32);

void font_print_ex(s16, s16, s16, char *, ...);

void disp_savesel_waku(u8 *e, int a, u8 c) {
    SFRM fr;
    s16 i;

    flfntSetSize(20, 20);
    fr.x = 50;
    fr.y = 88;
    fr.colw = 20;
    fr.h = 20;
    fr.cols = 12;
    fr.rows = 3;
    fr.pal = 5;
    fr.col = 0xC0402010;
    if (a == 2) {
        fr.mode = 2;
    } else {
        fr.mode = 4;
    }
    for (i = 0; i < 3; i++) {
        if (i == e[1]) {
            if (a == 0 || a == 2 || c >= 2) {
                Sel_csr_disp(fr.x + 115, fr.y - 3, 280, 70, 0xFF20C0C0);
            }
        }
        DispFrameMessageA(&fr, 0, 128);
        fr.y += 88;
    }
}

void disp_savesel_moji(u8 *e) {
    s16 i;
    s16 y;
    s16 col;
    u8 *s;

    s = option_w + 0x10;
    y = 90;
    for (i = 0; i < 3; i++) {
        col = (i == e[1]) ? 5 : 0;
        if (s[0] == 0) {
            flfntSetSize(24, 24);
            font_print_ex(122, y + 18, col, lit_159_00384EA8, i + 1);
        } else {
            u32 t;
            flfntSetSize(24, 24);
            font_print_ex(60, y, col, lit_160_00384EB0, i + 1, s + 8);
            flfntSetSize(18, 18);
            font_print_ex(60, y + 24, col, lit_161_00384EB8, sex_char_tbl[s[1] + 2]);
            t = *(u32 *)(s + 884);
            font_print_ex(60, y + 42, col, lit_162_00384ED0, t / 3600, t % 3600 / 60);
        }
        y += 88;
        s += 1152;
    }
}
