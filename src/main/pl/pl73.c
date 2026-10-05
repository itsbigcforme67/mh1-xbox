/* Player code (SLPM_654.95 0x001539D0-0x00153A68): Shell_type_set (sets the ammo type/count from the selected pouch slot).
 * Uses plitem.h instead of plf.h so that Item_data is a struct array (the original folds member offsets into the symbol). */
#include "pl.h"
#include "plitem.h"

void Shell_type_set(PLW *pl, int flag) {
    if (pl->work88E != 0xFF) {
        pl->ammo_type = Item_data[pl->item[pl->work88E].id].ammo;
        if (Item_data[pl->item[pl->work88E].id].max == 0xFF) {
            pl->work8BC = 0xFF;
        } else {
            pl->work8BC = pl->item[pl->work88E].num;
        }
        pl->work01D = Shell_data[pl->ammo_type].ammo_max;
        pl->work01C = 0;
    }
}
