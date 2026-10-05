/* em08_area - em08_local_area_move_init: per-stage stay / run-away timers
 * for monster 08 (table names from the symbol list; meaning a guess). */
#include "em.h"

extern s16 em08_stay_timer_tbl[];
extern s16 em08_runaway_timer_tbl[];

void em08_local_area_move_init(EMW *em) {
    em->stay_tm = em08_stay_timer_tbl[em->stg];
    em->runaway_tm = em08_runaway_timer_tbl[em->stg];
}
