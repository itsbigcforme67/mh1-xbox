/* Player code (SLPM_654.95 0x00154270-0x00154324): Pl_item_get_se (pick-up sound by item kind). Uses plitem.h, see pl73.c. */
#include "pl.h"
#include "plitem.h"

int Pl_master_ck(PLW *);
void adx_se_set(PLW *, int);

void Pl_item_get_se(PLW *pl, int id) {
    if (Pl_master_ck(pl) != 0) {
        switch (Item_data[(u16)id].se) {
        case 0:
            adx_se_set(pl, 5);
            break;
        case 2:
            adx_se_set(pl, 6);
            break;
        case 3:
            adx_se_set(pl, 7);
            break;
        }
    }
}
