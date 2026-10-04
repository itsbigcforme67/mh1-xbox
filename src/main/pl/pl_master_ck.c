/* Pl_master_ck - is this player the session master?
 * Matches SLPM_654.95 0x0014FC20 (24 bytes) with mwcps2 3.0b52 -O4,p. */
#include "pl.h"
#include "game.h"


int Pl_master_ck(PLW *pl) {
    return pl->id == game_w.master;
}
