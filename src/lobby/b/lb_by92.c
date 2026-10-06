/* lb_by92 - agent B promoted near-match 0x005AFE60-0x005AFEDC: Lb_shop_talk (first drafted by tools/lbauto.py). */
#include "lobby_s.h"

void Lb_shop_talk(void) {
    u8 *e;

    e = lb_pit.pos + lb_pit.x08 * 8;
    switch (lbShop.step) {
    case 1:
    case 3:
        *(s8 *)((u8 *)&lb_pit + 0xB) = NPC_Message(*(s32 *)(e + 4), lb_pit.x0, *(u16 *)e, lb_pit.x09);
        break;
    }
}
