/* Player code (SLPM_654.95 0x00153420-0x001534DC): Pl_vital_calc */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];

void Pl_vital_calc(PLW *pl, int dv) {
    if (Pl_master_ck(pl) != 0) {
        if (Game_clear_ck(0) == 1) {
            if ((s16)dv < 0) {
                return;
            }
        }
        if (pl->flag14 != 3) {
            pl->vital = pl->vital + dv;
            if (pl->vital <= 0) {
                pl->vital = 0;
            }
            if (pl->vital >= pl->work792) {
                pl->vital = pl->work792;
            }
            if (pl->vital >= pl->vital_red) {
                pl->vital_red = pl->vital;
            }
        }
    }
}
