/* em14_area - em14_local_area_move_init: per-stage stay / run-away timers
 * for monster 14 (table names from the symbol list; meaning a guess). */
#include "em.h"

extern s16 em14_stay_timer_tbl[];
extern s16 em14_runaway_timer_tbl[];

void em14_local_area_move_init(EMW *em) {
    em->stay_tm = em14_stay_timer_tbl[em->stg];
    em->runaway_tm = em14_runaway_timer_tbl[em->stg];
}
