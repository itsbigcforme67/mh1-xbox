/* Player code (SLPM_654.95 0x00153560-0x00153604): Pl_max_vital_calc */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];

void Pl_max_vital_calc(PLW *pl, int dv) {
    if (Pl_master_ck(pl) != 0) {
        pl->work792 = pl->work792 + dv;
        if (pl->work792 < 2) {
            pl->work792 = 1;
        }
        if (pl->work792 >= 0x96) {
            pl->work792 = 0x96;
        }
        if (pl->vital >= pl->work792) {
            pl->vital = pl->work792;
        }
        if (pl->vital_red >= pl->work792) {
            pl->vital_red = pl->work792;
        }
    }
}
