/* lb_pl07 - lobby player helpers (copies of main pl code) 0x005CE820-0x005CE8B0: lb_pl_flag_clr, Lb_pl_flag_set. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void lb_pl_flag_clr(PLW *pl, int f) {
    if (!(f & 0x80000000)) {
        pl->act_flag = pl->act_flag & ~f;
    } else {
        pl->work394 = pl->work394 & ~(f & 0x7FFFFFFF);
    }
}

void Lb_pl_flag_set(PLW *pl, int f) {
    if (!(f & 0x80000000)) {
        pl->act_flag = pl->act_flag | f;
    } else {
        pl->work394 = pl->work394 | (f & 0x7FFFFFFF);
    }
}
