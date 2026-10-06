/* edit10 - select.bin 0x00538540-0x005386C0: cmn_mongon_set (expands one record of the bad-word table check_mongon into a string). Source in edit_nm.c. */
#include "select.h"

/* Expand one entry of check_mongon (16-byte records, 14 chars + length at +0xF; a record whose
   next record has -1 at +0xF continues) into out. Returns the length, or -1 if it is longer than max. */
int cmn_mongon_set(s8 *e, s8 *out, int max) {
    int k;
    int rem;
    int i;
    int j;
    int off;
    s8 len = e[0xF];
    if (max < len) {
        return -1;
    }
    k = 0;
    rem = len;
    if (e[0x1F] == -1) {
        off = 0;
        do {
            for (j = 0; j < 14; j++) {
                out[off + j] = e[j];
            }
            e += 0x10;
            off += 14;
            k++;
            rem -= 14;
        } while (e[0x1F] == -1);
    }
    for (i = 0; i < rem; i++) {
        out[k * 14 + i] = e[i];
    }
    out[i + k * 14] = 0;
    return len;
}
