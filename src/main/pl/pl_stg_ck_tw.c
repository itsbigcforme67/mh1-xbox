/* Pl_stg_ck_tw - are two players on the same stage?
 * Matches SLPM_654.95 0x00152030 (20 bytes) with mwcps2 3.0b52 -O4,p. */
#include "pl.h"

int Pl_stg_ck_tw(PLW *a, PLW *b) {
    return a->stg == b->stg;
}
