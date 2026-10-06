/* lb_by90 - agent B promoted near-match 0x00537C10-0x00537F20: Lb_shop_move_x, Lb_shop_move_xR (first drafted by tools/lbauto.py). */
#include "lobby_s.h"
extern f32 tagMoveY[];

s32 Lb_shop_move_x(void) {
    s16 *p;
    u32 i;

    i = 0;
    p = &lbShop.pos[0][0];
    do {
        if (lbShop.x16 != 0) {
            if (i == lbShop.x1A) {
                p[0] = (s16)(-25.0f + (f32)p[0]);
                p[1] = (s16)((f32)p[1] + tagMoveY[lbShop.x1A]);
            } else {
                p[0] += 0x22;
            }
        } else if (i == lbShop.mode) {
            p[0] = (s16)(-25.0f + (f32)p[0]);
            p[1] = (s16)((f32)p[1] + tagMoveY[lbShop.mode]);
        } else {
            p[0] += 0x22;
        }
        i++;
        p += 2;
    } while (i < 5U);
    if (--lbShop.wait == 0) {
        lbShop.x70 = 0;
        lbShop.wait = 6;
        return 1;
    }
    return 0;
}

s32 Lb_shop_move_xR(void) {
    s16 *p;
    u32 i;

    i = 0;
    p = &lbShop.pos[0][0];
    do {
        if (lbShop.x16 != 0) {
            if (i == lbShop.x1A) {
                p[0] = (s16)((f32)p[0] - -25.0f);
                p[1] = (s16)((f32)p[1] - (f32)(s32)tagMoveY[lbShop.x1A]);
            } else {
                p[0] -= 0x22;
            }
        } else if (i == lbShop.mode) {
            p[0] = (s16)((f32)p[0] - -25.0f);
            p[1] = (s16)((f32)p[1] - (f32)(s32)tagMoveY[lbShop.mode]);
        } else {
            p[0] -= 0x22;
        }
        i++;
        p += 2;
    } while (i < 5U);
    if (--lbShop.wait == 0) {
        lbShop.x70 = 0;
        lbShop.wait = 6;
        return 1;
    }
    return 0;
}
