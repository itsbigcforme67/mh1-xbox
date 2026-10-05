/* lb_pl02 - lobby player helpers (copies of main pl code) 0x005CE290-0x005CE2B8: lb_pl_to_normal_clr2. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void lb_pl_to_normal_clr2(PLW *pl) {
    pl->vel[0] = 0.0f;
    pl->vel[1] = 0.0f;
    pl->vel[2] = 0.0f;
    pl->acc[0] = 0.0f;
    pl->acc[1] = 0.0f;
    pl->acc[2] = 0.0f;
    pl->act_flag &= 0x104;
}
