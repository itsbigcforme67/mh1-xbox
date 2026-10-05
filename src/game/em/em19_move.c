/* em19_move - game.bin 0x005EB780-0x005EBA08: em19 ground movement
 * helpers (turn toward the target by at most 0x40 or 0x71C per call,
 * move by the rate vector rotated to the facing angle) and
 * em20_local_area_move_init (em20 shares em06's run-away timers when its
 * kind is 6). File boundaries in this stretch are a guess. */
#include "em.h"

typedef struct EM19MW {
    u8 _pad00[0x34];
    f32 dist;           /* 0x34 distance left to the target */
} EM19MW;

u16 Em_Calc_angY(f32 *, f32 *);
void mot_miration_ret(EMW *, f32 *);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);

extern s16 em20_stay_timer_tbl[];
extern s16 em20_runaway_timer_tbl[];
extern s16 em06_runaway_timer_tbl[];

void em19_move_sub(EMW *em) {
    EM19MW *w = (EM19MW *)em->ex;
    f32 v[3];
    int a;

    a = (u16)(Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
    if (a <= 0x8000) {
        if (a <= 0x3F) {
            em->ang[1] += a;
        } else {
            em->ang[1] += 0x40;
        }
    } else {
        if (a > 0xFFC0) {
            em->ang[1] += a;
        } else {
            em->ang[1] -= 0x40;
        }
    }
    mot_miration_ret(em, v);
    w->dist -= v[2];
}

void em19_dir_adj(EMW *em) {
    int a;

    a = (u16)(Em_Calc_angY(em->pos, em->tgt_pos) - em->ang[1]);
    if (a <= 0x8000) {
        if (a <= 0x71B) {
            em->ang[1] += a;
        } else {
            em->ang[1] += 0x71C;
        }
    } else {
        if (a > 0xF8E4) {
            em->ang[1] += a;
        } else {
            em->ang[1] -= 0x71C;
        }
    }
}

void em19_rate_add_calc(EMW *em) {
    f32 v[3];

    flvecCopy(v, &em->rate_x);
    flvecRotY(v, 2.0f * (3.1415927f * ((360.0f * em->ang[1] / 65536.0f) / 360.0f)));
    em->pos[0] += v[0];
    em->pos[1] += v[1];
    em->pos[2] += v[2];
}

void em20_local_area_move_init(EMW *em) {
    em->stay_tm = em20_stay_timer_tbl[em->stg];
    if (em->kind == 6) {
        em->runaway_tm = em06_runaway_timer_tbl[em->stg];
    } else {
        em->runaway_tm = em20_runaway_timer_tbl[em->stg];
    }
}
