/* G online (PC): lobby_func_0058BAA0, G's check of a hunter's mini data (look and equipment shown to others) before it
 * is sent: out-of-range values are reset; returns 1 when something was changed (G lobby 0x58BAA0; HAND-FIXED: written
 * by hand from G's code, m2c could not follow its two jump tables). */
#include "../gonl.h"

void *memcpy(void *, const void *, unsigned int);
extern u8 parts_max_tbl[];      /* G sub_main 0x509E40: 12-byte rows, u16 at +2 the highest value; from +0x20 u16s */

int lobby_func_0058BAA0(u8 *mini)
{
    u8 b[0x1C];
    int fixed = 0, k;
    memcpy(b, mini, 0x1C);
    if (b[1] >= 31) {
        b[1] = 30;
        fixed = 1;
    }
    if (b[0] == 5) {
        b[0] = 1;
        fixed = 1;
    } else if (b[0] >= 6) {     /* (case 6 stores 4 first, then 0 as the default does) */
        b[0] = 0;
        fixed = 1;
    }
    if (b[25] != 0 && b[25] != 1) {
        fixed = 1;
        b[25] = 0;
    }
    if ((b[16] & 0xF) >= 4) {
        b[16] = 0;
        fixed = 1;
    }
    if (b[26] >= 10) {
        b[26] = 0;
        fixed = 1;
    }
    if (b[24] >= 10) {
        b[24] = 0;
        fixed = 1;
    }
    if (b[11] != 6 && b[11] != 7) {
        b[11] = 6;
        *(u16 *)(b + 12) = 156;
        fixed = 1;
        *(u16 *)(b + 14) = 0;
        b[0] = 4;
    } else if (b[11] == 6) {
        if (*(u16 *)(b + 12) >= 381) {
            *(u16 *)(b + 12) = 156;
            fixed = 1;
            *(u16 *)(b + 14) = 0;
            b[0] = 4;
        }
    } else if (*(u16 *)(b + 12) >= 78) {
        fixed = 1;
        *(u16 *)(b + 12) = 1;
        *(u16 *)(b + 14) = 0;
        b[0] = 1;
    }
    for (k = 0; k < 6; k++) {
        if (k == 1) {
            if (b[4] == 0 || *(u16 *)(parts_max_tbl + b[25] * 12 + 2) < b[4]) {
                b[4] = 1;
                fixed = 1;
            }
        } else if (*(u16 *)(parts_max_tbl + 0x20 + 2 * k) < b[3 + k]) {
            b[3 + k] = 0;
            fixed = 1;
        }
    }
    memcpy(mini, b, 0x1C);
    return fixed;
}
