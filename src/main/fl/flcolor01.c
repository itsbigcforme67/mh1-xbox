/* fl library: flPS2ConvColor (SLPM_654.95 0x00179CB0-0x00179DB0) splits a packed colour into its four bytes (r = bits 16-23, g, b, a = bits 24-31) and
 * converts them to PS2 range: PC alpha 0xFF becomes 0x80 and other non-zero values are halved (at least 1). With mode 0 all four bytes are converted
 * (colour channels as well), otherwise only the alpha byte; the bytes are packed again (a << 24 | r << 16 | g << 8 | b). */
#include "types.h"

u32 flPS2ConvColor(u32 col, int mode) {
    u8 c[4];
    int i;
    u8 *p;
    u8 *g;
    u8 *b;
    u8 *a;

    c[0] = col >> 16;
    g = &c[1];
    *g = col >> 8;
    b = &c[2];
    *b = col;
    a = &c[3];
    *a = col >> 24;
    if (mode == 0) {
        i = 0;
        p = c;
        for (; i < 4; i++) {
            if (*p == 0xFF) {
                *p = 0x80;
            } else if (*p != 0) {
                *p >>= 1;
                if (*p == 0) {
                    *p = 1;
                }
            }
            p++;
        }
    } else {
        if (*a == 0xFF) {
            *a = 0x80;
        } else if (*a != 0) {
            *a >>= 1;
            if (*a == 0) {
                *a = 1;
            }
        }
    }
    return (*a << 24) | (c[0] << 16) | (*g << 8) | *b;
}
