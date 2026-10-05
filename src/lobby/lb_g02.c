/* lb_g02 - lobby player basics 0x005CDF70-0x005CE290: Lb_Pl_act_set, ck_pl_send, Lb_pl_to_normal, lb_pl_to_normal_clr. Whole file in lb_g.c. */
#include "lobby.h"










void Lb_Pl_act_set(pl, a, b, flags)
PLW *pl;
u8 a;
u8 b;
int flags;
{
    pl->work016 = pl->flag14;
    pl->work017 = pl->flag15;
    pl->flag14 = a;
    pl->flag15 = b;
    pl->x05 = 0;
    pl->x06 = 0;
    pl->x07 = 0;
    pl->cnt39A = ran_suu(1);
    pl->work6FF = 0;
    if (!((u16)flags & 1)) {
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
    lb_action_timer_calc(pl, 0);
    pl->act_flag = pl->act_flag & 0x80100;
    pl->work394 = 0;
    pl->chr_spd0 = 2.0f;
    pl->chr_spd1 = 2.0f;
    if (pl->id != game_w.master) {
        if ((s16)Lb_act_ck(pl, 0, 0) != 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
            goto send;
        }
    } else {
send:
        if (ck_pl_send(pl) == 1) {
            Lb_send_pl_status(pl);
        }
    }
}

int ck_pl_send(PLW *pl) {
    player_work[0].id = 0;
    player_work[1].id = 1;
    player_work[2].id = 2;
    player_work[3].id = 3;
    player_work[4].id = 4;
    player_work[5].id = 5;
    player_work[6].id = 6;
    player_work[7].id = 7;
    if (pl->id == game_w.master && PLU8(pl, 0x1E) == 0) {
        switch (game_w.stage) {
        case 0x4D:
        case 0x4C:
            return 1;
        }
    }
    return 0;
}

void Lb_pl_to_normal(PLW *pl, int st, int blend, int tm) {
    lb_pl_to_normal_clr();
    pl->work4E0 = 0;
    lb_pl_to_normal_clr2(pl);
    PLU8(pl, 0x388) = 0;
    switch (PLU8(pl, 0x388)) {
    default:
        lb_to_normal(pl, blend, tm);
        break;
    case 0xF:
        break;
    }
    lb_action_timer_calc(pl, 0);
}

void lb_pl_to_normal_clr(PLW *pl) {
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
