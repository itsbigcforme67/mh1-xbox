/* lb_pl09 - lobby player helpers (copies of main pl code) 0x005D7B10-0x005D7B80: Lb_World_calc. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void Lb_World_calc(PLW *pl) {
    f32 *sd = Stage_data_get(pl->stg);
    pl->work754 = pl->pos[0] + sd[0];
    pl->work758 = pl->pos[1] + (sd[6] + sd[7]) / 2.0f;
    pl->work75C = pl->pos[2] + sd[1];
}
