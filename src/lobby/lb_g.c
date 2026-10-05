/* Lobby: player basics (SLPM_654.95 lobby overlay 0x5CDBD0-0x5CE330). Whole file; runs split into lb_gNN.c */
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

void lb_to_normal(PLW *pl, int blend, int tm) {
    pl->act_tm0 = tm;
    pl->act_tm1 = tm;
    pl->char0 = 1;
    pl->char1 = 0x65;
    pl->blend0 = blend / 2;
    pl->blend1 = blend / 2;
    pl->flag14 = 0;
    pl->flag15 = 0;
    if (ck_pl_send(pl) != 0) {
        Lb_send_pl_status(pl);
    }
}

void lb_action_timer_calc(PLW *pl, int flag) {
    s16 b = pl->blend0;
    s16 t = pl->act_tm0;
    int bb;
    pl->work39C = 0;
    if (b < 0) {
        b = -b;
    }
    bb = b;
    pl->work39C += t - bb;
    if ((s16)flag != 0) {
        if (bb != 0) {
            b--;
        }
        pl->work3D0 += (b - t) & 0xFF;
    }
}
