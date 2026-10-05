/* em27_area - em27_local_area_move_init: per-stage stay / run-away timers
 * for monster 27 (table names from the symbol list; meaning a guess). */
#include "em.h"

extern s16 em27_stay_timer_tbl[];
extern s16 em27_runaway_timer_tbl[];

void em27_local_area_move_init(EMW *em) {
    em->stay_tm = em27_stay_timer_tbl[em->stg];
    em->runaway_tm = em27_runaway_timer_tbl[em->stg];
}
