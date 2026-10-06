/* lb_a06 - lobby commer: lb_set_pl_pos 0x005C5850-0x005C5974. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];

void lb_set_pl_pos(u8 id, u8 *src, u8 mode) {
    u8 buf[0xC];
    PLW *pl;
    LBPOS *p;
    pl = &player_work[id];
    flMemcpy(buf, src, 0xC);
    p = (LBPOS *)buf;
    if (mode == 0) {
        pl->work800 = p->x;
        pl->work808 = p->z;
        pl->ang_y = p->ang;
        if (pl->stg != game_w.stage) {
            *(LBV3 *)pl->pos = *(LBV3 *)&pl->work800;
            pl->ang[1] = *(u16 *)&pl->ang_y;
        } else {
            Lb_Pl_adj_calc(pl, 0xA);
        }
        pl->stg = p->stg;
    } else if (mode == 2) {
        pl->work800 = p->x;
        pl->work808 = p->z;
        *(LBV3 *)pl->pos = *(LBV3 *)&pl->work800;
        *(LBV3 *)&pl->work5A0 = *(LBV3 *)pl->pos;
        pl->ang[1] = *(u16 *)&pl->ang_y = p->ang;
    }
}
