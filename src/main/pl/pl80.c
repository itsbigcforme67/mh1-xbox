/* Player code (SLPM_654.95 0x0014DEA0-0x0014E0B4): Fue_item_set */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void Pl_vital_calc_item(PLW *, int);
void Pl_max_vital_calc(PLW *, int);
void func_639DF0(PLW *, int);
void set01_set(int, int, int);

void Fue_item_set(PLW *pl) {
    PLW *t = &player_work[game_w.master];
    pl->work917 = 1;
    if (Pl_master_ck(pl) != 0 || pl->stg == t->stg) {
        switch (pl->work88A) {
        case 0x8A:
            Pl_vital_calc(t, 0x14);
            Eft06_set(4.0f, t, 0, 0, 0xA);
            if (Pl_master_ck(pl) == 0) {
                Eft06_set(4.0f, pl, 0, 0, 0xA);
                return;
            }
            break;
        case 0x8B:
            t->x7BA = 0;
            Eft06_set(4.0f, t, 0, 1, 0xA);
            if (Pl_master_ck(pl) == 0) {
                Eft06_set(4.0f, pl, 0, 1, 0xA);
                return;
            }
            break;
        case 0x8C:
            t->work6A5 = 0xA;
            t->work6A6 = 0x1518;
            Eft06_set(4.0f, t, 0, 2, 0xA);
            if (Pl_master_ck(pl) == 0) {
                Eft06_set(4.0f, pl, 0, 2, 0xA);
                return;
            }
            break;
        case 0x8D:
            t->work6A9 = 0x14;
            t->work6AA = 0x1518;
            Eft06_set(4.0f, t, 0, 3, 0xA);
            if (Pl_master_ck(pl) == 0) {
                Eft06_set(4.0f, pl, 0, 3, 0xA);
            }
            break;
        }
    }
}
