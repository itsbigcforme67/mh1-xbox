/* Player code (SLPM_654.95 0x001532A0-0x00153420): adj calc, pos adj, trap check, item count */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"
void *get_joint_mat(PLW *, int, int);
void flmatRotY33(void *, f32);
void flmatRotZ33(void *, f32);
int Ana_ok_ck();
extern s16 *stg_eft_mdl_no[0x58];

void Pl_adj_calc(PLW *pl, int n) {
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

void Pl_pos_adj(PLW *pl) {
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

s16 Pl_trap_use_ck(PLW *pl) {
    if (Ana_ok_ck() == 0) {
        return -1;
    }
    return *stg_eft_mdl_no[pl->stg];
}

void Pl_item_cnt_up(PLW *pl) {
    game_w.x80[pl->id]++;
}
