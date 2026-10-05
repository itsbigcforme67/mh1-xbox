/* lb_pl08 - lobby player helpers (copies of main pl code) 0x005CEF30-0x005CEF70: Lb_hit_stop_calc. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void Lb_hit_stop_calc(PLW *pl) {
    u8 a = pl->work409;
    if (a != 0) {
        pl->x40A += a;
        pl->work409 = 0;
        return;
    }
    if (pl->x40A != 0) pl->x40A--;
}
