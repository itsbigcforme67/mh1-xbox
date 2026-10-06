/* pl pointer list (SLPM_654.95 0x00194890-0x001949F8): plplInit (chain n cells), plplAdd (push), plplNext (pop, bit 0 of a link marks the last cell). */
#include "types.h"

void plplInit(int n, u32 *p) {
    int i;

    for (i = 0; i < n - 1; i++) {
        p[i] = (u32)(p + (i + 1));
    }
    p[i] = 0;
}

void plplAdd(u32 *cell, u32 *head) {
    u32 old = *head;

    *head = (u32)cell | 1;
    *cell = old;
}

u32 plplNext(u32 *head) {
    u32 *cur;
    u32 nxt;

    for (;;) {
        cur = (u32 *)*head;
        if (cur == 0) {
            return 0;
        }
        nxt = *cur;
        if (nxt & 1) {
            nxt &= ~1;
            *head = nxt;
            return nxt;
        }
        *head = nxt;
    }
}
