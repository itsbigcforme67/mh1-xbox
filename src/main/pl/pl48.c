/* Player code (SLPM_654.95 0x0014C3E0-0x0014C4F4): pl_move (loop over the 8 players), pl_move_sub_sub (state group dispatcher: flag14 selects normal/attack/damage/die/demo/egg/chat) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_move(void) {
    s32 i;
    PLW *p;

    pl_sw_set();
    i = 0;
    p = player_work;
    do {
        pl_move_sub(p);
        i++;
        p++;
    } while (i < 8);
    hit_timer_calc_shl();
}

void pl_move_sub_sub(PLW *pl) {
    pl->work8F0 = 0;
    pl->work4D4 = 1;
    switch (pl->flag14) {
    case 0:
        pl_normal(pl);
        break;
    case 1:
        pl_attack(pl);
        break;
    case 2:
        pl_damage(pl);
        break;
    case 3:
        pl_die(pl);
        break;
    case 4:
        pl_demo(pl);
        break;
    case 5:
        pl_egg(pl);
        break;
    case 6:
        pl_chat(pl);
    }
}
