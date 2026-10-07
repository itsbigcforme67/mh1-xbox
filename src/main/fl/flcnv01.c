/* fl library: flPS2ConvertAlpha (SLPM_654.95 0x001701A0-0x00170228) halves the alpha byte of every pixel of a w x h RGBA image (PS2 alpha 0x80 = opaque):
 * 0xFF becomes 0x80, other non-zero values are shifted right once (at least 1). */
#include "types.h"

void flPS2ConvertAlpha(u8 *p, int w, int h) {
    int x;
    int y;
    u8 a;

    y = 0;
    if (0 < h) {
        do {
            x = 0;
            if (0 < w) {
                do {
                    a = p[3];
                    if (a == 0xFF) {
                        a = 0x80;
                    } else if (a != 0) {
                        a >>= 1;
                        if (a == 0) {
                            a = 1;
                        }
                    }
                    p[3] = a;
                    x++;
                    p += 4;
                } while (x < w);
            }
            y++;
        } while (y < h);
    }
}
