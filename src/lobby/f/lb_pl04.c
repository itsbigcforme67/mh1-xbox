/* lb_pl04 - lobby player helpers (copies of main pl code) 0x005CE570-0x005CE5B8: Lb_stick_dir_set. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











int Lb_stick_dir_set(PLW *pl, int no) {
    return (pl->sw.ang[no] + (Get_view_dir() & 0xFFFF)) & 0xFFFF;
}
