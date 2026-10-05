/* em04 (part 3) - game.bin 0x0058F3E0-0x0058F498. em04_effect_move (runs
 * the sound/effect script ef_move_sub once the monster is set up) and its
 * sound_call helper. ef_move_sub itself (the per-animation script, 0x58E500)
 * is a near-match in em04_nm.c and stays as assembly. */
#include "em04.h"

void em04_effect_move_0058F3E0(EMW *em) {
    EM04W *w = (EM04W *)em->ex;

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_0058E500(em, w);
        break;
    }
}

void sound_call_0058F430(EMW *em, int frame, int se) {
    if (em_frame_check(em, 0, (f32)frame)) {
        Em_se_req2(em, se, 0, em->pos, 6, 0);
    }
}

/* A file static in the original; data tables point at it. */
void dummy_em_prog_0058F490(void) {
}
