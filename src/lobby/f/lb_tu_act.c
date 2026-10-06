/* lb_tu_act - one translation unit 0x005CDF70-0x005CF100: Lb_Pl_act_set, ck_pl_send, Lb_pl_to_normal, lb_pl_to_normal_clr, lb_pl_to_normal_clr2, lb_to_normal, lb_action_timer_calc, Lb_act_ck, Lb_stick_pow_get, Lb_stick_dir_set, Lb_Pl_basic_flagset, Lb_Pl_adj_calc, Lb_Em_adj_calc, Lb_Pl_pos_adj, Lb_Em_pos_adj, lb_pl_flag_clr, Lb_pl_flag_set, Lb_St_unique_adr_set, lb_ck_unique_act, Lb_put_unique_act_hint, Lb_Pl_stg_ck, Lb_hit_stop_calc, Lb_act_set. Built by tools/lbtu.py from the per-run files; functions that are
   not C yet stay original bytes (asm stubs, build/raw/*.inc). */
#include "lobby_f.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);
extern PLW player_work[];
extern u8 sw_flag_1260;
extern u8 em_work[];
f32 flSqrt(f32);
int Lb_get_angle();
void lb_target_angle();
void lb_insert_target_list();
u8 *Stage_unique_data_get();
void Lb_put_unique_act_hint();
int lb_ck_unique_act();
int Lb_Pl_stg_ck();
int SoftKeyboard_alive_check();
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

void lb_pl_to_normal_clr2(PLW *pl) {
    pl->vel[0] = 0.0f;
    pl->vel[1] = 0.0f;
    pl->vel[2] = 0.0f;
    pl->acc[0] = 0.0f;
    pl->acc[1] = 0.0f;
    pl->acc[2] = 0.0f;
    pl->act_flag &= 0x104;
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

static void lb_action_timer_calc(PLW *pl, int flag) {
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

int Lb_act_ck(PLW *pl, int a, int b) {
    if (pl->flag14 == (u8)a && pl->flag15 == (u8)b) {
        return 1;
    }
    return 0;
}

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

int Lb_stick_dir_set(PLW *pl, int no) {
    return (pl->sw.ang[no] + (Get_view_dir() & 0xFFFF)) & 0xFFFF;
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

void Lb_Pl_adj_calc(PLW *pl, int n) {
    if (!(flvecCalcDistance(pl->pos, &pl->work800) < 800.0f)) {
        pl->work818 = 0;
        pl->pos[0] = pl->work800;
        pl->pos[1] = pl->work804;
        pl->pos[2] = pl->work808;
    } else {
        pl->work818 = n;
        pl->work80C = (pl->work800 - pl->pos[0]) / (f32)n;
        pl->work810 = (pl->work804 - pl->pos[1]) / (f32)n;
        pl->work814 = (pl->work808 - pl->pos[2]) / (f32)n;
    }
}

void Lb_Em_adj_calc(PLW *pl, s16 n) {
    F(s16, pl, 0x468) = n;
    F(f32, pl, 0x45C) = (F(f32, pl, 0x934) - pl->pos[0]) / (f32)n;
    F(f32, pl, 0x460) = (F(f32, pl, 0x938) - pl->pos[1]) / (f32)n;
    F(f32, pl, 0x464) = (F(f32, pl, 0x93C) - pl->pos[2]) / (f32)n;
}

void Lb_Pl_pos_adj(PLW *pl) {
    s16 t = pl->work818;
    if (t != 0) {
        pl->work818 = t - 1;
        if (t > 0) {
            pl->pos[0] += pl->work80C;
            pl->pos[1] += pl->work810;
            pl->pos[2] += pl->work814;
        }
    }
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

void lb_pl_flag_clr(PLW *pl, int f) {
    if (!(f & 0x80000000)) {
        pl->act_flag = pl->act_flag & ~f;
    } else {
        pl->work394 = pl->work394 & ~(f & 0x7FFFFFFF);
    }
}

void Lb_pl_flag_set(PLW *pl, int f) {
    if (!(f & 0x80000000)) {
        pl->act_flag = pl->act_flag | f;
    } else {
        pl->work394 = pl->work394 | (f & 0x7FFFFFFF);
    }
}

void Lb_St_unique_adr_set(PLW *pl) {
    u8 *p;
    f32 z;
    f32 y;
    f32 dx;
    f32 dz;
    pl->fish878 = 0;
    p = Stage_unique_data_get(pl->stg);
    if (p == 0) {
        Lb_put_unique_act_hint(pl, -1);
        pl->fish878 = 0;
        return;
    }
    while (*(f32 *)(p + 4) != -1.0f) {
        z = *(f32 *)(p + 8);
        y = pl->pos[1];
        if (!(y < z - 50.0f) && y < 50.0f + z) {
            dx = pl->pos[0] - *(f32 *)(p + 4);
            dz = pl->pos[2] - *(f32 *)(p + 0xC);
            if (flSqrt(dx * dx + dz * dz) <= *(f32 *)(p + 0x10)) {
                if (lb_ck_unique_act(pl, p) == 1) {
                    pl->fish878 = p;
                    if (pl->id == game_w.master) {
                        Lb_put_unique_act_hint(pl, *(u16 *)(p + 2));
                    }
                } else {
                    Lb_put_unique_act_hint(pl, -1);
                }
                return;
            }
        }
        p += 0x18;
    }
    Lb_put_unique_act_hint(pl, -1);
    pl->fish878 = 0;
}

int lb_ck_unique_act(int a0, u8 *p) {
    switch (F(u16, p, 2)) {
    case 7:
        return Lb_ck_target(a0, p + 4, 0x5A);
    }
    return 1;
}

/* original bytes: build/raw/Lb_put_unique_act_hint.inc (config/c_rawfuncs.txt) */
asm void Lb_put_unique_act_hint()
{
#include "Lb_put_unique_act_hint.inc"
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

void Lb_hit_stop_calc(PLW *pl) {
    u8 a = pl->work409;
    if (a != 0) {
        pl->x40A += a;
        pl->work409 = 0;
        return;
    }
    if (pl->x40A != 0) pl->x40A--;
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

