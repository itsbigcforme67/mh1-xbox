/* lb_pl05 - lobby player helpers (copies of main pl code) 0x005CE660-0x005CE71C: Lb_Pl_adj_calc. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void Lb_Pl_adj_calc(PLW *pl, int n) {
    if (!(flvecCalcDistance(pl->pos, &pl->work800) < 800.0f)) {
        pl->work818 = 0;
        pl->pos[0] = pl->work800;
        pl->pos[1] = pl->work804;
        pl->pos[2] = pl->work808;
    } else {
        pl->work818 = n;
        pl->work80C = (pl->work800 - pl->pos[0]) / (f32)n;
        pl->work810 = (pl->work804 - pl->pos[1]) / (f32)n;
        pl->work814 = (pl->work808 - pl->pos[2]) / (f32)n;
    }
}
