/* em15_senkai - game.bin 0x005D04F0-0x005D05F8: em15_senkai_target, the
 * same turn-toward-target code as em02_senkai_target (turn ang[1] by at
 * most w->turn; returns 1 when facing). Sits right after em15.c. */
#include "em.h"

typedef struct EM15TW {
    u8 _pad00[0x14];
    u16 dang;           /* 0x14 angle left to turn */
    u8 _pad16[0x30 - 0x16];
    s32 turn;           /* 0x30 maximum turn per call */
} EM15TW;

u16 Em_Calc_angY(f32 *, f32 *);

u16 em15_senkai_target(EMW *em) {
    EM15TW *w = (EM15TW *)em->ex;
    int a;
    u16 ret = 0;

    if (em->x881 == 0) {
        return 1;
    }
    w->dang = Em_Calc_angY(em->pos, em->tgt_pos);
    w->dang = w->dang - em->ang[1];
    if (w->dang != 0) {
        a = w->dang & 0xFFFF;
        if (a <= 0x8000) {
            if (a <= w->turn) {
                ret = 1;
                em->ang[1] += a;
            } else {
                em->ang[1] += w->turn;
            }
        } else {
            if (a >= 0x10000 - w->turn) {
                ret = 1;
                em->ang[1] -= 0x10000 - a;
            } else {
                em->ang[1] -= w->turn;
            }
        }
    }
    em->ang[1] = (u16)em->ang[1];
    return ret;
}
