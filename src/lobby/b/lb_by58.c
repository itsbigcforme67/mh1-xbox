/* lb_by58 - agent B promoted near-match 0x0053D230-0x0053D280: Lb_get_armor_num (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 D_3C7004[];
/* number of stocked armor pieces (6-byte records at D_3C7004) with the given kind (u8 at +1) and id (u16 at +2) */

s32 Lb_get_armor_num(s32 a, s32 b) {
    int cnt;
    int i;
    u8 *p;

    cnt = 0;
    p = D_3C7004;
    for (i = 0; i < 0x40; i++) {
        if (p[1] == (a & 0xFFFF) && *(u16 *)(p + 2) == (b & 0xFFFF)) {
            cnt++;
        }
        p += 6;
    }
    return cnt;
}
