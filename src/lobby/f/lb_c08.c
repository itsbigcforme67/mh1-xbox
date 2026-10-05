/* lb_c08 - lobby small helpers 0x005CDF40-0x005CDF6C: Lb_Pl_act_set2. Whole file in lb_c.c. */
#include "lobby_f.h"













void Lb_Pl_act_set2(PLW *pl) {
    Lb_Pl_act_set();
    PLU8(pl, 0x6FF) = 1;
}
