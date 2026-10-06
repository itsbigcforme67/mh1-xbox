/* lb_gap02 - near-match fixes 0x005E9660-0x005E96C8: BsUrlEncode. Whole file in lb_ap.c. */
#include "lobby_f.h"
extern u8 BsCacheCurrentBaseUrlstr[];
extern s8 lit_928_00666260[];
u32 strlen();
int strncmp();
int BsUrlSchemeGet();

int BsUrlEncode(s8 *dst, u8 *src) {
    int i;
    i = 0;
    if (*src != 0) {
        do {
            *dst++ = 0x25;
            i += 3;
            *dst++ = lit_928_00666260[(*src & 0xF0) >> 4];
            *dst++ = lit_928_00666260[*src++ & 0xF];
        } while (*src != 0);
    }
    *dst = 0;
    return i;
}
