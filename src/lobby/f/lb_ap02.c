/* lb_ap02 - browser url/kanji helpers 0x005E96D0-0x005E9754: sjis2euc_sub. Whole file in lb_ap.c. */
#include "lobby_f.h"
extern u8 BsCacheCurrentBaseUrlstr[];
extern u8 lit_928_00666260[];
u32 strlen();
int strncmp();
int BsUrlSchemeGet();

int sjis2euc_sub(u32 v) {
    u32 hi;
    u32 lo;
    int a;
    int b;
    int base;
    hi = (v >> 8) & 0xFF;
    lo = v & 0xFF;
    if (hi < 0xA0U) {
        base = 0x71;
    } else {
        base = 0xB1;
    }
    b = ((hi - base) * 2) + 1;
    if (lo >= 0x9EU) {
        a = lo - 0x7E;
        b += 1;
    } else if (lo > 0x7FU) {
        a = lo - 0x20;
    } else {
        a = lo - 0x1F;
    }
    a = a | 0x80;
    b = b | 0x80;
    if (a == 0xA0) {
        a = 0xFE;
        b -= 1;
    }
    return (b << 8) | a;
}
