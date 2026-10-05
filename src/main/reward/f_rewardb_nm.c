/* Near-match, not built: reward_key_repeat (0x293370-0x293418), key repeat for
 * the reward screen's pad directions (first press: store keys and wait 8
 * frames; held: every 3 frames). Logic is complete; the original keeps the
 * work pointer in a2 where this build uses a0 (29 of 42 instructions differ,
 * all register names). */
#include "reward.h"

u16 reward_key_repeat(a, b)
u16 a;
u16 b;
{
    REWARD_W *w = &reward_w;
    u16 k;
    u16 m;
    s16 z;

    k = a & 0x3C00;
    if (k != 0) {
        w->x8 = k;
        w->xA = 8;
        return (s16)k;
    }
    m = b & 0x3C00;
    if (m == 0) {
        w->x8 = 0;
        z = 0;
        return z;
    }
    if (--w->xA > 0) {
        return 0;
    }
    w->xA = 3;
    w->x8 &= (s16)m;
    return w->x8;
}
