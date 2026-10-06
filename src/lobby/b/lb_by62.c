/* lb_by62 - agent B promoted near-match 0x0053D760-0x0053D7CC: armor_shop_trans (first drafted by tools/lbauto.py). */
#include "lobby_s.h"

void armor_shop_trans(void) {
    u8 *e;

    e = lb_pit.pos + lb_pit.x08 * 8;
    switch (lbShop.step) {
    case 0:
    case 2:
        break;
    default:
            *(s8 *)((u8 *)&lb_pit + 0xB) = NPC_Message(*(s32 *)(e + 4), lb_pit.x0, *(u16 *)e, lb_pit.x09);
        break;
    }
}
