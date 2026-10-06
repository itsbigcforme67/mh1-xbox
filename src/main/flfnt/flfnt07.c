/* flfnt07 - SLPM_654.95 0x00217620-0x00217674: flfntSjis2Index (Shift-JIS code to glyph index: 94 columns per JIS row,
 * -1 beyond the 7808 level-1/2 kanji and kana glyphs). Whole file in flfnt_nm.c. */
#include "types.h"

int flfntSjis2Jis(u32 c);

int flfntSjis2Index(u32 c) {
    int j = flfntSjis2Jis(c);
    int hi = ((j >> 8) - 0x21) * 0x5E;
    int idx = hi + ((j & 0xFF) - 0x21);
    if (idx >= 0x1E80) {
        idx = -1;
    }
    return idx;
}
