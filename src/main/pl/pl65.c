/* Player code (SLPM_654.95 0x00152D50-0x00152D64): Get_Active_itemnum */
#include "pl.h"
#include "game.h"
#include "plf.h"

long Get_Active_itemnum(PLW *pl) {
    return pl->item[pl->work888].num;
}
