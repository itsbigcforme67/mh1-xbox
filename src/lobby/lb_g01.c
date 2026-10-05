/* lb_g01 - lobby player basics 0x005CDBD0-0x005CDF38: lb_sw_set_sub, Lb_pl_chr_sub, Lb_pl_timer_calc. Whole file in lb_g.c. */
#include "lobby.h"










void lb_sw_set_sub(int no) {
    PLW *pl = &player_work[no];
    PLSW *sw = &pl->sw;
    sw->an_now = Plan_buff[0][0];
    sw->an_old = Plan_buff[0][1];
    sw->an_trg_old = sw->an_trg;
    sw->an_trg = sw->an_now & ~sw->an_old;
    sw->ang[0] = Plan_ang[0][0];
    sw->ang[1] = Plan_ang[0][1];
    if (Cockpit_menu_chk_lobby() == 0) {
        sw->pow[0] = Plan_pow[0][0];
        sw->pow[1] = Plan_pow[0][1];
    } else {
        sw->pow[0] = 0;
        sw->pow[1] = 0;
    }
    if (Cockpit_menu_chk() == 0 && pl->work8C2 == 0) {
        sw->now = Plsw_buff[0][0];
        sw->old = Plsw_buff[0][1];
        sw->trg_old = sw->trg;
        sw->trg = (u16)sw->now & ~(u16)sw->old;
        sw->chg = ((u16)sw->now ^ (u16)sw->old) | ((u16)sw->now ^ Plsw_buff[0][0]);
        sw->pad08[0] = sw->now;
        sw->pad08[1] = sw->old;
        sw->pad08[2] = sw->trg;
        sw->pad08[3] = sw->trg_old;
        if (pl->work88C != 0) {
            sw->now = 0;
            sw->old = 0;
            sw->trg = 0;
            sw->trg_old = 0;
        }
        pad_timer_calc(pl);
        return;
    }
    if (pl->work8C2 == 0) {
        sw->an_now &= 0x3C00;
        sw->an_old &= 0x3C00;
        sw->an_trg_old &= 0x3C00;
        sw->an_trg &= 0x3C00;
    } else {
        sw->an_now = 0;
        sw->an_old = 0;
        sw->an_trg_old = 0;
        sw->an_trg = 0;
    }
    sw->ang[1] = 0;
    sw->pow[1] = 0;
    sw->now = 0;
    sw->old = 0;
    sw->trg = 0;
    sw->trg_old = 0;
    sw->pad08[0] = 0;
    sw->pad08[1] = 0;
    sw->pad08[2] = 0;
    sw->pad08[3] = 0;
    sw->chg = 0;
}

void Lb_pl_chr_sub(PLW *pl) {
    cpRotMatrix(pl->ang, (u8 *)pl + 0x20);
    if (pl->x2FC[0] == 0) {
        pl->x2FC[0]++;
        frame_init(pl, pl->act_tm0, pl->blend0, 0);
    }
    if (pl->x2FC[1] == 0) {
        pl->x2FC[1]++;
        frame_init(pl, *(u16 *)&pl->act_tm1, pl->blend1, 1);
    }
    frame_move(pl);
}

void Lb_pl_timer_calc(PLW *pl) {
    u8 t;
    if (pl->x40A == 0) {
        if (pl_flag_ck(pl, 0x400001) != 0) {
            pl->work398 = pl->work398 + 2;
        } else {
            pl->work398 = 0;
        }
        pl->work39C = pl->work39C + 2;
        if (pl->work39C >= 0x186A0) {
            pl->work39C = 0x2710;
        }
        t = pl->work440;
        if (t != 0) {
            pl->work440 = (u8)(t - 1);
        }
        if (pl->work7D6 != 0) {
            pl->work7D6 = pl->work7D6 - 1;
        }
        if (pl->work760 != 0) {
            pl->work760 = pl->work760 - 1;
        }
        if (pl->work936 != 0) {
            pl->work936 = pl->work936 - 1;
        }
        if (pl->work7ED != 0) {
            pl->work7ED = pl->work7ED - 1;
        }
    }
}
