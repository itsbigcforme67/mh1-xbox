/* pl helpers (SLPM_654.95 0x00193F60-0x001941F0): plMemset, plMemmove. */
#include "types.h"

void plMemset(s8 *d, s8 v, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *d++ = v;
    }
}

void plMemmove(s8 *dst, s8 *src, int n) {
    int i;

    if ((u32)src < (u32)dst && (u32)dst < (u32)src + n) {
        int last = n - 1;
        i = 0;
        dst += last;
        src += last;
        for (; i < n; i++) {
            *dst-- = *src--;
        }
    } else {
        for (i = 0; i < n; i++) {
            *dst++ = *src++;
        }
    }
}
