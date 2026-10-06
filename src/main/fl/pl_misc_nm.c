/* plMemmove near-match (SLPM_654.95 0x00193FF0-0x001941F0), 96 of 128 instructions differ; kept out of the build. */
#include "types.h"

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
