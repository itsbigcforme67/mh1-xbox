/* lb_pl01 - lobby player helpers (copies of main pl code) 0x005CDB80-0x005CDBB4: lb_pl_chr_set_com. Whole file in lb_pl_nm.c. */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);











void lb_pl_chr_set_com(PLW *pl, int c, int blend, int tm, int slot) {
    (&pl->char0)[slot] = c;
    (&pl->blend0)[slot] = blend / 2;
    (&pl->act_tm0)[slot] = tm;
    pl->x2FC[slot] = 0;
}
