/* Lobby: player flags, adjust, targets (SLPM_654.95 lobby overlay 0x5CE400-0x5CF700). Whole file; runs split into lb_hNN.c */
#include "lobby_f.h"

u8 Lb_stick_pow_get(PLW *pl) {
    u8 r = 0;
    u8 c;
    if (lb_sys.x6C == 1 && pl->id == game_w.master) {
        return 0;
    }
    PLU8(pl, 0x8C8) = 0;
    if (game_w.master == pl->id) {
        u16 p = pl->sw.pow[0];
        if (p >= 0x78) {
            r = 3;
        } else if (p >= 0x55) {
            r = 1;
        } else if (p >= 0x28) {
            r = 1;
        }
    } else {
        c = pl->flag15;
        if (c == 0 || c == 0x55) {
            r = 0;
        } else if (c == 2 || c == 0x5B) {
            r = 1;
        } else {
            r = 5;
        }
    }
    if (PLU8(pl, 0x388) == 0 && pl->flag12 == 0 && (pl->sw.now & 0x10) && (r & 0xFF) && ((s16)Lb_act_ck(pl, 0, 0x24) == 0 || pl->work760 == 0)) {
        return 5;
    }
    return r;
}

void Lb_Pl_basic_flagset(PLW *pl, int a) {
    int t = a & 0xFF;
    int f = a & 0xFFFF;
    switch (t) {
    default:
        PLU8(pl, 0x388) = 0;
        break;
    case 1:
        PLU8(pl, 0x388) = 1;
        break;
    case 2:
        PLU8(pl, 0x388) = 2;
    }
    if (f & 0x8000) {
        lb_pl_flag_clr(pl, 8);
    } else {
        Lb_pl_flag_set(pl, 8);
    }
    lb_pl_flag_clr(pl, 1);
    lb_pl_flag_clr(pl, 2);
}

void Lb_Em_adj_calc(PLW *pl, s16 n) {
    F(s16, pl, 0x468) = n;
    F(f32, pl, 0x45C) = (F(f32, pl, 0x934) - pl->pos[0]) / (f32)n;
    F(f32, pl, 0x460) = (F(f32, pl, 0x938) - pl->pos[1]) / (f32)n;
    F(f32, pl, 0x464) = (F(f32, pl, 0x93C) - pl->pos[2]) / (f32)n;
}

void Lb_Em_pos_adj(PLW *pl) {
    u16 t = F(u16, pl, 0x468);
    u8 *p = (u8 *)pl + 0x444;
    if (t != 0) {
        F(s16, p, 0x24) = t - 1;
        if (t > 0) {
            pl->pos[0] = pl->pos[0] + F(f32, p, 0x18);
            pl->pos[1] = pl->pos[1] + F(f32, p, 0x1C);
            pl->pos[2] = pl->pos[2] + F(f32, p, 0x20);
        }
    }
}

int lb_ck_unique_act(int a0, u8 *p) {
    switch (F(u16, p, 2)) {
    case 7:
        return Lb_ck_target(a0, p + 4, 0x5A);
    }
    return 1;
}

int Lb_Pl_stg_ck(PLW *pl) {
    u8 s = pl->stg;
    if (s != game_w.stage) {
        return 0;
    }
    if (pl->id != game_w.master) {
        if (s == 0x4C || s == 0x4D) {
            return 1;
        }
        return 0;
    }
    return 1;
}

void Lb_act_set(pl, a, b)
PLW *pl;
u8 a;
u8 b;
{
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
    if (pl->x10 != 0) {
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
    lb_action_timer_calc(pl, 0);
    pl->act_flag = pl->act_flag & 0x100100;
    pl->work394 = 0;
    if (pl->work01E == 0 && pl->id != game_w.master) {
        if ((s16)Lb_act_ck(pl, 0, 0) != 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
            goto send;
        }
    } else {
send:
        if (ck_pl_send(pl) != 0) {
            if (pl->flag15 == 0x4D && game_w.stage == 0x4D) {
                lb_sys.chair_mask = lb_sys.chair_mask & ~(1 << **(u16 **)((u8 *)pl + 0x878));
            }
            Lb_send_pl_status(pl);
        }
    }
}
