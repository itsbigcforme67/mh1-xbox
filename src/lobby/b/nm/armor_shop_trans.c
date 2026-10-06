#include "lobby_s.h"
void armor_shop_trans(void) {
    u8 *e;

    e = lb_pit.pos + lb_pit.x08 * 8;
    if ((lbShop.step != 2) && (lbShop.step != 0)) {
        *(s8 *)((u8 *)&lb_pit + 0xB) = NPC_Message(*(s32 *)(e + 4), lb_pit.x0, *(u16 *)e, lb_pit.x09);
    }
}
