/* Player code (SLPM_654.95 0x00151430-0x00151828): rate (velocity) helpers and action timers */
#include "pl.h"
#include "game.h"
#include "plf.h"
void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];

void rate_add(PLW *pl) {
    pl->pos[0] += pl->vel[0];
    pl->pos[1] += pl->vel[1];
    pl->pos[2] += pl->vel[2];
}

void rate_add_g(PLW *pl) {
    pl->vel[0] += pl->acc[0];
    pl->vel[1] += pl->acc[1];
    pl->vel[2] += pl->acc[2];
    pl->pos[0] += pl->vel[0];
    pl->pos[1] += pl->vel[1];
    pl->pos[2] += pl->vel[2];
}

void em_rate_add_g(PLW *pl) {
    pl->vel[0] += pl->acc[0];
    pl->vel[1] += pl->acc[1];
    pl->vel[2] += pl->acc[2];
    pl->pos[0] += pl->vel[0];
    pl->pos[1] += pl->vel[1];
    pl->pos[2] += pl->vel[2];
}

int rate_add_g2(PLW *pl) {
    f32 g;
    pl->vel[0] += pl->acc[0];
    pl->vel[1] += pl->acc[1];
    pl->vel[2] += pl->acc[2];
    pl->pos[0] += pl->vel[0];
    pl->pos[1] += pl->vel[1];
    pl->pos[2] += pl->vel[2];
    g = GetGroundHit(pl->pos);
    if (pl->pos[1] <= g) {
        pl->pos[1] = g;
        return 1;
    }
    return 0;
}

void rate_clear(PLW *pl) {
    pl->acc[2] = 0;
    pl->acc[1] = 0;
    pl->acc[0] = 0;
    pl->vel[2] = 0;
    pl->vel[1] = 0;
    pl->vel[0] = 0;
}

void em_rate_clear(PLW *pl) {
    pl->acc[2] = 0;
    pl->acc[1] = 0;
    pl->acc[0] = 0;
    pl->vel[2] = 0;
    pl->vel[1] = 0;
    pl->vel[0] = 0;
}

void rate_clear_g(PLW *pl) {
    pl->acc[2] = 0;
    pl->acc[1] = 0;
    pl->acc[0] = 0;
}

void em_rate_clear_g(PLW *pl) {
    pl->acc[2] = 0;
    pl->acc[1] = 0;
    pl->acc[0] = 0;
}

void shell_rate_add(PLW *pl) {
    pl->work030 += pl->work048;
    pl->work034 += pl->work04C;
    pl->work038 += pl->work050;
}

void shell_rate_add_g(PLW *pl) {
    pl->work048 += pl->work054;
    pl->work04C += pl->work058;
    pl->work050 += pl->work05C;
    pl->work030 += pl->work048;
    pl->work034 += pl->work04C;
    pl->work038 += pl->work050;
}

void action_timer_calc(PLW *pl, int flag) {
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

void em_action_timer_calc(PLW *pl, int flag) {
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
