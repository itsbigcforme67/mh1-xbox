/* Player code (SLPM_654.95 0x001513A0-0x001513B0): pad_timer_calc */
#include "pl.h"
#include "game.h"
#include "plf.h"
void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];

void pad_timer_calc(PLW *pl) {
    pad_timer_calc_sub(pl, (u16)~pl->sw.now);
}
