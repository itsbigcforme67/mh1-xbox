/* Memory card save slot selection screen, SLPM_654.95 main 0x280EF0-0x2814E0.
 * Three slot frames (DispFrameMessageA) at y = 0x58, 0xB0, 0x108 with the slot caption;
 * option_w+0x10 + 0x480*i is the per-slot "used" flag byte. Guesses: field names. */
#include "types.h"

typedef struct SAVEFR {
    s16 x;          /* 0 */
    s16 y;          /* 2 */
    u8 w;           /* 4 */
    u8 h;           /* 5 */
    u8 a;           /* 6 */
    u8 b;           /* 7 */
    s16 c;          /* 8 */
    s16 d;          /* 0xA */
    u32 col;        /* 0xC */
} SAVEFR;

extern u8 option_w[];
extern char lit_159_00384EA8[], lit_160_00384EB0[], lit_161_00384EB8[], lit_162_00384ED0[];
void flfntSetSize(int, int);
void font_print_ex();
extern char *sex_char_tbl[];
void Sel_csr_disp();
void DispFrameMessageA(void *, int, int);

void disp_savesel(sel, mode, flag)
u8 *sel;
int mode;
u8 flag;
{
    SAVEFR fr;
    s16 i;
    s16 y;
    s16 col;
    u8 *o;

    flfntSetSize(0x14, 0x14);
    fr.x = 0x32;
    fr.y = 0x58;
    o = option_w + 0x10;
    fr.a = 0xC;
    fr.w = 0x14;
    fr.h = 0x14;
    y = 0x5A;
    fr.b = 3;
    fr.col = 0xC0402010;
    fr.c = 5;
    fr.d = mode == 2 ? 2 : 4;
    i = 0;
    do {
        col = 5;
        if (i == sel[1]) {
            if (mode == 0 || mode == 2 || flag >= 2) {
                Sel_csr_disp(fr.x + 0x73, fr.y - 3, 0x118, 0x46, 0xFF20C0C0);
            }
        } else {
            col = 0;
        }
        DispFrameMessageA(&fr, 0, 0x80);
        if (*o == 0) {
            flfntSetSize(0x18, 0x18);
            font_print_ex(0x7A, (s16)(y + 0x12), col, lit_159_00384EA8, i + 1);
        } else {
            flfntSetSize(0x18, 0x18);
            font_print_ex(0x3C, y, col, lit_160_00384EB0, i + 1, o + 8);
            flfntSetSize(0x12, 0x12);
            font_print_ex(0x3C, (s16)(y + 0x18), col, lit_161_00384EB8, sex_char_tbl[o[1] + 2]);
            font_print_ex(0x3C, (s16)(y + 0x2A), col, lit_162_00384ED0, *(u32 *)(o + 0x374) / 3600, *(u32 *)(o + 0x374) % 3600 / 60);
        }
        i++;
        y += 0x58;
        o += 0x480;
        fr.y += 0x58;
    } while (i < 3);
}

void disp_savesel_waku(sel, mode, flag)
u8 *sel;
int mode;
u8 flag;
{
    SAVEFR fr;
    s16 i;

    flfntSetSize(0x14, 0x14);
    fr.x = 0x32;
    fr.y = 0x58;
    fr.a = 0xC;
    fr.w = 0x14;
    fr.h = 0x14;
    fr.b = 3;
    fr.col = 0xC0402010;
    fr.c = 5;
    fr.d = mode == 2 ? 2 : 4;
    i = 0;
    do {
        if (i == sel[1] && (mode == 0 || mode == 2 || flag >= 2)) {
            Sel_csr_disp(fr.x + 0x73, fr.y - 3, 0x118, 0x46, 0xFF20C0C0);
        }
        DispFrameMessageA(&fr, 0, 0x80);
        i++;
        fr.y += 0x58;
    } while (i < 3);
}

void disp_savesel_moji(sel)
u8 *sel;
{
    s16 i;
    s16 y;
    s16 col;
    u8 *o;

    i = 0;
    y = 0x5A;
    o = option_w + 0x10;
    do {
        col = i == sel[1] ? 5 : 0;
        if (*o == 0) {
            flfntSetSize(0x18, 0x18);
            font_print_ex(0x7A, (s16)(y + 0x12), col, lit_159_00384EA8, i + 1);
        } else {
            flfntSetSize(0x18, 0x18);
            font_print_ex(0x3C, y, col, lit_160_00384EB0, i + 1, o + 8);
            flfntSetSize(0x12, 0x12);
            font_print_ex(0x3C, (s16)(y + 0x18), col, lit_161_00384EB8, sex_char_tbl[o[1] + 2]);
            font_print_ex(0x3C, (s16)(y + 0x2A), col, lit_162_00384ED0, *(u32 *)(o + 0x374) / 3600, *(u32 *)(o + 0x374) % 3600 / 60);
        }
        o += 0x480;
        i++;
        y += 0x58;
    } while (i < 3);
}
