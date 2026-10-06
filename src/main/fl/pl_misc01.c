/* pl helpers (SLPM_654.95 0x00193F60-0x00193FF0): plMemset. */
#include "types.h"

void plMemset(s8 *d, s8 v, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *d++ = v;
    }
}

