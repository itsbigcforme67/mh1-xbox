/* NEAR-MATCH (not linked): flmatCopy (SLPM_654.95 0x00172CB0-0x00172CD4) 6 of 9 instructions differ (only which registers hold the four loaded quadwords). */
#include "types.h"

typedef unsigned __int128 u128;

typedef struct M4 { u128 q[4]; } M4;

void flmatCopy(M4 *d, M4 *s) {
    *d = *s;
}
