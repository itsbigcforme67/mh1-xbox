/* Lobby player helpers (SLPM_654.95 lobby overlay): near-copies of the main player code (pl_chr_set_com, pl_to_normal_clr2,
   act_ck, stick_dir_set, Pl_adj_calc, Pl_pos_adj, pl_flag_set/clr, hit_stop_calc, World_calc). */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
f32 *Stage_data_get(int stg);
int Get_view_dir();
f32 flvecCalcDistance(f32 *, f32 *);

void lb_pl_chr_set_com(PLW *pl, int c, int blend, int tm, int slot) {
    (&pl->char0)[slot] = c;
    (&pl->blend0)[slot] = blend / 2;
    (&pl->act_tm0)[slot] = tm;
    pl->x2FC[slot] = 0;
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

s16 Lb_act_ck(PLW *pl, int a, int b) {
    if (pl->flag14 == (u8)a && pl->flag15 == (u8)b) {
        return 1;
    }
    return 0;
}

int Lb_stick_dir_set(PLW *pl, int no) {
    return (pl->sw.ang[no] + (Get_view_dir() & 0xFFFF)) & 0xFFFF;
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

void Lb_hit_stop_calc(PLW *pl) {
    u8 a = pl->work409;
    if (a != 0) {
        pl->x40A += a;
        pl->work409 = 0;
        return;
    }
    if (pl->x40A != 0) pl->x40A--;
}

void Lb_World_calc(PLW *pl) {
    f32 *sd = Stage_data_get(pl->stg);
    pl->work754 = pl->pos[0] + sd[0];
    pl->work758 = pl->pos[1] + (sd[6] + sd[7]) / 2.0f;
    pl->work75C = pl->pos[2] + sd[1];
}
