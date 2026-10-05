/* rwkey02 - reward screen key repeat (SLPM_654.95 0x00293370-0x00293418): reward_key_repeat. Whole file in rwkey_nm.c. */
/* rwkey_nm - SLPM_654.95 0x00290E50-0x00290E80 Quest_price_return, 0x00293370-0x00293418 reward_key_repeat. */
#include "types.h"
#include "reward.h"
extern s32 quest_price;
void Gold_add(int);
/* Key auto-repeat for the reward screen (same scheme as pit_key_repeat): d-pad bits 0x3C00 repeat every 3 frames
   after 8; returns the keys to act on. */
u16 reward_key_repeat(now, hold)
u16 now;
u16 hold;
{
    REWARD_W *w = &reward_w;
    u16 k = now & 0x3C00;
    u16 h;
    u16 r;

    if (k != 0) {
        w->x8 = k;
        w->xA = 8;
        r = (s16)k;
    } else {
        h = hold & 0x3C00;
        if (h == 0) {
            r = w->x8 = 0;
        } else {
            w->xA--;
            r = 0;
            if (w->xA <= 0) {
                w->xA = 3;
                r = w->x8 &= h;
            }
        }
    }
    return r;
}

