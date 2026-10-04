/* Player: return to the normal state, part 2: SLPM_654.95
 * 0x0014F330-0x0014F850. Pad input (sw_set_sub) is here too. */
#include "pl.h"
#include "game.h"


void pl_st_set(PLW *, s8);
void to_normal_fly(PLW *, s16, int);
void to_normal(PLW *, int, int);
void action_timer_calc(PLW *, int);
int Online_ck(void);
int Cockpit_menu_chk(void);
void pad_timer_calc(PLW *);

/* Raw pad state per controller, filled by the pad driver: [port][0/1]. */
extern s16 Plsw_buff[2][2];
extern u16 Plan_buff[2][2];
extern u16 Plan_ang[2][2];
extern u16 Plan_pow[2][2];

static void sw_set_sub(int no);

static void pl_to_normal_clr_etc(PLW *pl) {
    pl->work4E0 = 0;
}

void pl_to_normal_clr(PLW *pl) {
    pl->work04 = 0;
    pl->work2F4 = 0;
    pl->work2FC = 0;
    pl->work398 = 0;
    pl->work39C = 0;
    pl->work3D0 = 0;
    pl->work3D1 = 0;
    pl->work4D6 = 0;
    pl->act_flag &= 0x100;
    pl->work394 = 0;
    pl->chr_no0 = pl->id;
    pl->chr_no1 = pl->id;
    pl->work43C = 0;
    pl->work2F8 = 0;
    pl->work40C = 0;
    pl->work440 = 0;
    pl->work08 = 0;
    pl->work38A = 0;
    pl->work4E3 = 0;
    pl->work4DD = 0;
    pl->work3F4 = 0;
    pl->work615 = 0;
    pl->work87C = 0;
    pl->work56B = 0;
    pl->chr_spd0 = 2.0f;
    pl->chr_spd1 = 2.0f;
}

void pl_to_normal_clr2(PLW *pl) {
    pl->work3B4[0] = 0;
    pl->work3B4[1] = 0;
    pl->work3B4[2] = 0;
    pl->work3B4[3] = 0;
    pl->work3B4[4] = 0;
    pl->work3B4[5] = 0;
    pl->act_flag &= 0x104;
}

void pl_to_normal(PLW *pl, int st, int blend, int tm) {
    pl_to_normal_clr(pl);
    pl_to_normal_clr_etc(pl);
    pl->work720[0] = 0;
    pl->work724[0] = 0;
    pl->work72C[0] = 0;
    pl->work720[1] = 0;
    pl->work724[1] = 0;
    pl->work72C[1] = 0;
    pl->work720[2] = 0;
    pl->work724[2] = 0;
    pl->work72C[2] = 0;
    pl->work720[3] = 0;
    pl->work724[3] = 0;
    pl->work72C[3] = 0;
    pl->work8F0 = 1;
    if (st != 2) {
        pl_to_normal_clr2(pl);
        pl->st = 0;
    } else {
        pl_st_set(pl, 2);
    }
    switch (pl->st) {
    default:
        to_normal(pl, blend, tm);
        break;
    case 2:
        to_normal_fly(pl, blend, tm);
        break;
    case 15:
        break;
    }
    action_timer_calc(pl, 0);
}

void pl_to_normal_b(PLW *pl, int st, int blend, int tm) {
    pl_to_normal_clr(pl);
    pl_to_normal_clr_etc(pl);
    if (st != 2) {
        pl_to_normal_clr2(pl);
        pl->st = 0;
    } else {
        pl_st_set(pl, 2);
    }
    switch (pl->st) {
    default:
        to_normal(pl, blend, tm);
        break;
    case 2:
        to_normal_fly(pl, blend, tm);
        break;
    case 15:
        break;
    }
    action_timer_calc(pl, 0);
}

void pl_sw_set(void) {
    if (Online_ck() == 0) {
        sw_set_sub(0);
        return;
    }
    sw_set_sub(game_w.master);
}

static void sw_set_sub(int no) {
    PLW *pl = &player_work[no];
    PLSW *sw = &pl->sw;
    u16 port;

    if (Online_ck() == 0) {
        port = no;
    } else {
        port = 0;
    }
    sw->an_now = Plan_buff[port & 1][0];
    sw->an_old = Plan_buff[port & 1][1];
    sw->an_trg_old = sw->an_trg;
    sw->an_trg = sw->an_now & ~sw->an_old;
    sw->ang[0] = Plan_ang[port & 1][0];
    sw->ang[1] = Plan_ang[port & 1][1];
    sw->pow[0] = Plan_pow[port & 1][0];
    sw->pow[1] = Plan_pow[port & 1][1];
    sw->now = Plsw_buff[port & 1][0];
    sw->old = Plsw_buff[port & 1][1];
    sw->trg_old = sw->trg;
    sw->trg = (u16)sw->now & ~(u16)sw->old;
    sw->chg = ((u16)sw->now ^ (u16)sw->old) | ((u16)sw->now ^ Plsw_buff[port & 1][0]);
    sw->pad08[0] = sw->now;
    sw->pad08[1] = sw->old;
    sw->pad08[2] = sw->trg;
    sw->pad08[3] = sw->trg_old;
    if (Cockpit_menu_chk() == 0 && pl->work8C2 == 0) {
        if (pl->work88C != 0) {
            sw->now &= 0x10;
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
