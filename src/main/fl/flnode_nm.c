/* Near-match (not linked): flSetMatrixList (0x00174060), 11 of 36 instructions differ: the original keeps the matrix count in s2, the index
 * pointer in s1 and the matrix pointer in s0; this build has them in s0 / s2 / s1. */
#include "types.h"

extern u8 flMATRIX[];
void flPS2_Mem_move64(void *, void *, int);

void flSetMatrixList(s16 *idx, u8 *base) {
    int i = 0;
    u8 *m = flMATRIX;
    s16 *p = idx + 1;
    s16 cnt = *idx;

    for (; i < cnt; i++) {
        flPS2_Mem_move64(base + (*p << 6), m, 1);
        p++;
        m += 0x40;
    }
}
