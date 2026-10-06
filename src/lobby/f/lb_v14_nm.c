/* lb_v14 - target list insert 0x005CF100-0x005CF1C0: lb_insert_target_list (sorted insert by priority 0x302, then distance 0x4C4). */
#include "lobby_f.h"
void lb_insert_target_list(u8 **list, u8 *tgt) {
    u8 *c;
    u8 *cur;
    u8 *prev;
    s16 ang;
    if (list == 0) {
        *list = tgt;
        return;
    }
    c = *list;
    prev = 0;
    cur = c;
    if (c != 0) {
        ang = *(s16 *)(tgt + 0x302);
        do {
            if (ang < *(u16 *)(cur + 0x302)) {
                break;
            }
            if (*(u16 *)(cur + 0x302) == ang) {
                if (ang < 3) {
                    if (!(*(f32 *)(cur + 0x4C4) <= *(f32 *)(tgt + 0x4C4))) {
                        break;
                    }
                } else if (*(f32 *)(cur + 0x4C4) < *(f32 *)(tgt + 0x4C4)) {
                    break;
                }
            }
            prev = cur;
            cur = *(u8 **)(cur + 0x3B0);
        } while (cur != 0);
    }
    if (prev == 0) {
        *(u8 **)(tgt + 0x3B0) = c;
        *list = tgt;
        return;
    }
    *(u8 **)(tgt + 0x3B0) = *(u8 **)(prev + 0x3B0);
    *(u8 **)(prev + 0x3B0) = tgt;
}
