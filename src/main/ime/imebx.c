/* imebx: is_jis (SLPM_654.95 0x00247200-0x00247270): true when both bytes of a JIS code are in 0x21..0x7E. IME engine (name entry), see imead.c for the context. */
#include "types.h"

int is_jis(c)
u16 c;
{
    int hi;
    int lo;
    int a;
    int b;
    int r;

    hi = c >> 8;
    lo = c & 0xFF;
    a = 0;
    r = 0;
    if ((hi & 0xFF) >= 0x21 && (hi & 0xFF) < 0x7F) {
        a = 1;
    }
    if (a != 0) {
        b = 0;
        if ((lo & 0xFF) >= 0x21 && (lo & 0xFF) < 0x7F) {
            b = 1;
        }
        if (b != 0) {
            r = 1;
        }
    }
    return r;
}
