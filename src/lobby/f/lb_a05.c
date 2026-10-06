/* lb_a05 - lobby commer: lb_set_pl_status 0x005C5680-0x005C5844. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];

void lb_set_pl_status(u8 id, u8 *src) {
    u8 buf[0x10];
    PLW *pl;
    LBSTAT *st;
    pl = &player_work[id];
    flMemcpy(buf, src, 0x10);
    st = (LBSTAT *)buf;
    pl->work800 = st->x;
    pl->work808 = st->z;
    pl->flag14 = st->act14;
    pl->flag15 = st->act15;
    pl->ang_y = st->ang;
    PLU8(pl, 0x8EC) = st->x0D;
    Lb_Pl_adj_calc(pl, 0xA);
    if (st->act15 == 0x4C && pl->flag14 == 0 && st->act15 != pl->flag15) {
        Lb_Pl_act_set2(pl, 0, 0x4C, 0);
        return;
    }
    switch (st->act15) {
    case 0x4D:
        lb_sys.chair_mask = lb_sys.chair_mask & ~(1 << st->chair);
        break;
    case 0x4C:
    case 0x54:
    case 0x59:
    case 0x5A:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x60:
    case 0x4E:
    case 0x2A:
    case 0x29:
    case 0x2B:
        if (st->chair > 0 && st->chair < 0xB) {
            lb_sys.chair_mask = lb_sys.chair_mask | (1 << st->chair);
        }
        break;
    }
    Lb_act_set(pl, st->act14, st->act15);
}
