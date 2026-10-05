/* lb_pl06 - lobby player helpers (copies of main pl code) 0x005CE780-0x005CE7D0: Lb_Pl_pos_adj. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void Lb_Pl_pos_adj(PLW *pl) {
    s16 t = pl->work818;
    if (t != 0) {
        pl->work818 = t - 1;
        if (t > 0) {
            pl->pos[0] += pl->work80C;
            pl->pos[1] += pl->work810;
            pl->pos[2] += pl->work814;
        }
    }
}
