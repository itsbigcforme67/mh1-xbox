/* Player code (SLPM_654.95 0x00134C20-0x00134E68): clr_pl_work, player_mv, pl_init. */
#include "pl.h"
#include "game.h"
#include "plf.h"

void clr_pl_work(void) {
    int i;
    PLW *p = player_work;
    for (i = 0; i < 8; i++) {
        memset(p, 0, 0xA00);
        p++;
    }
    memset(pl_cnt_w, 0, 0x10);
}

void player_mv(void) {
    switch (pl_cnt_w[0]) {
    case 0:
        pl_init(0);
    case 1:
        pl_move();
    case 2:
    case 3:
        break;
    }
}

void pl_init(int mode) {
    int i;
    PLW *pl;
    s16 m = mode;
    pl_cnt_w[1] = 0;
    pl_cnt_w[0] = 1;
    pl = player_work;
    for (i = 0; i < 8; i++) {
        if (pl->be_flag != 0) {
            pl->work568 = get_prim();
            if (pl->work568 != -1) {
                pl->work564 = get_prim_ptr(pl->work568);
                PS32(pl->work564, 0x18) = pl->id;
                PPTR(pl->work564, 0x14) = trans_pl_sub;
            }
            if (m != 0) {
                if (m == 1 && pl->x738 != 0) goto clr;
            } else {
clr:
                pl_work_clr(pl, i, pl_prog_tbl[0]);
            }
            if (m == 0) {
                if (Pl_master_ck(pl) == 1) {
                    pl->x73A = pl->stg;
                    pl->work73C = pl->pos[0];
                    pl->work740 = pl->pos[1];
                    pl->work744 = pl->pos[2];
                    pl->work570 = pl->ang[1];
                    net_send_pl(pl, 3, 0);
                }
                eft01_set(pl, 1);
                Eft06_set(1.0f, pl, 7, 0, 1);
            }
        }
        pl++;
    }
    hit_chk_init();
}
