/* em20_horm - em20_horm_init / em20_horm_main (turn to face a player, as
 * em01_horm.c), then em21_local_area_move_init (per-stage timers). The
 * real file boundaries in this stretch are unknown. */
#include "em.h"
#include "pl.h"

/* Part of the per-monster work at EMW+0x444 used here. */
typedef struct EM_HORMW {
    u8 _pad00[0x14];
    u16 ang;            /* 0x14 target angle */
    u8 mode;            /* 0x16 0 start, 1 turning */
} EM_HORMW;

u16 Em_Calc_angY(f32 *, f32 *);
void em_pl_pos_set(EMW *, u8, f32 *);
void World_calc2(u8, f32 *, f32 *);
void em_char_set(EMW *, int, int, int);
int em_frame_check2(EMW *, int, f32);
void pl_flag_set(EMW *, u32);
void pl_flag_clr(EMW *, u32);

void em20_horm_init(EMW *em) {
    EM_HORMW *w = (EM_HORMW *)em->ex;
    PLW *pl;
    f32 p[3];
    f32 q[3];

    w->mode = 0;
    if (em->x617 == -1) {
        em->horm_ang = em->x39A;
        return;
    }
    pl = &((PLW *)player_work)[em->x617];
    em->x3B0 = pl;
    em_pl_pos_set(em, em->x617, p);
    World_calc2(pl->stg, p, q);
    em->horm_ang = Em_Calc_angY(em->x754, q);
}

int em20_horm_main(EMW *em) {
    EM_HORMW *w = (EM_HORMW *)em->ex;
    u32 a;
    s32 b;
    u16 d;
    int a2;

    switch (w->mode) {
    case 0:
        w->mode++;
        em->x3F4 = 1;
        w->ang = em->horm_ang;
        d = em->horm_ang - em->ang[1];
        if ((u16)(em->horm_ang + 0x200) <= 0x400) {
            return 1;
        }
        a2 = d;
        if (a2 <= 0xE38 || a2 >= 0xF1C8) {
            pl_flag_set(em, 0x20000);
            em_char_set(em, 3, 0, 0);
        } else if (a2 >= 0x8000) {
            em_char_set(em, 6, 0, 0);
        } else {
            em_char_set(em, 5, 0, 0);
        }
        break;
    case 1:
        b = em->ang[1];
        a = (u16)(w->ang - (u16)b);
        if (em->x194 == 0) {
            if (a <= 0xE38 || a >= 0xF1C8) {
                pl_flag_set(em, 0x20000);
                em_char_set(em, 3, 0, 0);
            } else {
                pl_flag_clr(em, 0x20000);
                if (a >= 0x8000) {
                    em_char_set(em, 6, 0, 0);
                } else {
                    em_char_set(em, 5, 0, 0);
                }
            }
        } else if (((a + 0x200) & 0xFFFF) < 0x400) {
            pl_flag_clr(em, 0x20000);
            em->ang[1] = w->ang;
            em->x3F4 = 0;
            return 1;
        } else if (a < 0x8000) {
            em->ang[1] = (u16)(b + 0x200);
        } else {
            em->ang[1] = (u16)(b - 0x200);
        }
        break;
    }
    return 0;
}

extern s16 em21_stay_timer_tbl[];
extern s16 em21_runaway_timer_tbl[];

void em21_local_area_move_init(EMW *em) {
    em->stay_tm = em21_stay_timer_tbl[em->stg];
    em->runaway_tm = em21_runaway_timer_tbl[em->stg];
}
