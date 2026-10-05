/* Player code (SLPM_654.95 0x0014EB00-0x0014EF58): act_set, Pl_act_set, Pl_act_set2, act_ck (action change) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void act_set(PLW *pl, u8 a, u8 b) {
    int i;
    if (pl->x10 == 0) {
        pl->work016 = pl->flag14;
        pl->work017 = pl->flag15;
    }
    pl->flag14 = a;
    pl->flag15 = b;
    pl->x05 = 0;
    pl->x06 = 0;
    pl->x07 = 0;
    if (pl->x10 == 0) {
        pl->cnt39A = ran_suu(1);
    }
    if (pl->x10 != 0 || (act_ck(pl, 1, 8) == 0 && act_ck(pl, 1, 2) == 0 && act_ck(pl, 1, 0xA) == 0)) {
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
    }
    if (pl->x10 == 0 && pl->flag14 == 2) {
        pl->work615 = 0;
    }
    action_timer_calc(pl, 0);
    pl->act_flag = pl->act_flag & 0x100100;
    pl->work394 = 0;
    if (pl->x10 == 0 && pl->work01E == 0) {
        if (Pl_master_ck(pl) == 1) {
            net_send_pl(pl, 1, 0);
        } else if (act_ck(pl, 0, 0) != 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
    }
}

void Pl_act_set(PLW *pl, int a, int b, int flags) {
    int f = (u16)flags;
    int old;
    u16 r;
    int nm;
    pl->work016 = pl->flag14;
    pl->work017 = pl->flag15;
    pl->flag14 = a;
    pl->flag15 = b;
    pl->x05 = 0;
    pl->x06 = 0;
    pl->x07 = 0;
    if (f & 0x20) {
        old = *(u8 *)&pl->cnt39A;
        r = ran_suu(1);
        pl->cnt39A = r & 0xFF00;
        pl->cnt39A = pl->cnt39A | old;
    } else {
        pl->cnt39A = ran_suu(1);
    }
    nm = f & 1;
    pl->work6FF = 0;
    if (nm == 0) {
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
        pl->work8C9 = 0;
    }
    if (pl->flag14 == 2) {
        pl->work615 = 0;
    }
    action_timer_calc(pl, 0);
    pl->act_flag = pl->act_flag & 0x80100;
    pl->work394 = 0;
    pl->chr_spd0 = 2.0f;
    pl->chr_spd1 = 2.0f;
    if (Pl_master_ck(pl) == 1) {
        if (!(f & 2)) {
            if (!(f & 0x10)) {
                net_send_pl(pl, 1, flags);
            } else {
                net_send_pl(pl, 6, flags);
            }
        }
    } else if (act_ck(pl, 0, 0) != 0) {
        if (nm == 0) {
            pl_to_normal(pl, 0, 4, 0);
        } else {
            pl_to_normal_b(pl, 0, 4, 0);
        }
    }
}

void Pl_act_set2(PLW *pl, int a, int b, int c) {
    if (Pl_master_ck(pl) == 1) {
        if (pl->x738 == 0) {
            goto set;
        }
    } else {
set:
        Pl_act_set(pl, a, b, c);
        pl->work6FF = 1;
    }
}

s16 act_ck(PLW *pl, int a, int b) {
    if (pl->flag14 == (u8)a && pl->flag15 == (u8)b) {
        return 1;
    }
    return 0;
}
