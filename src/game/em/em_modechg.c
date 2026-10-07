/* em_modechg - game.bin 0x00566500-0x00566628: Em_Mode_Chg (change a monster's
 * mode; mode 0 clears the per-player hate/target slots and the players'
 * "being looked at" byte) and em01_local_area_move_init (per-stage stay /
 * run-away timers, same shape as em08_area.c). */
#include "em.h"
#include "pl.h"

extern s16 em01_stay_timer_tbl[];
extern s16 em01_runaway_timer_tbl[];

int Em_Mode_Chg(EMW *em, int mode, s16 mode2) {
    s8 i;

    if (em->x888 == (u8)mode) {
        return 0;
    }
    if (mode2 > 0) {
        em->x886 = mode2;
    }
    em->x888 = mode;
    em->x83B &= 0xC0;
    if (em->x888 == 0) {
        em->x88C = 0;
        i = 0;
        em->x88F = 0;
        for (; i < 4; i++) {
            em->x8F4[i] = 0;
            em->x918[i] = 0;
            em->x890[i] = 0;
            if ((em->x7EE & (1 << i)) && player_work[i].be_flag != 0 && player_work[i].work7EE != 0) {
                player_work[i].work7EE = 0;
            }
        }
        em->x7EE = 0;
    }
    return 1;
}

void em01_local_area_move_init(EMW *em) {
    em->stay_tm = em01_stay_timer_tbl[em->stg];
    em->runaway_tm = em01_runaway_timer_tbl[em->stg];
}
