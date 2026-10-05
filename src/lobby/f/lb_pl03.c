/* lb_pl03 - lobby player helpers (copies of main pl code) 0x005CE3C0-0x005CE3F8: Lb_act_ck. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











s16 Lb_act_ck(PLW *pl, int a, int b) {
    if (pl->flag14 == (u8)a && pl->flag15 == (u8)b) {
        return 1;
    }
    return 0;
}
