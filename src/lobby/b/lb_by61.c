/* lb_by61 - agent B promoted near-match 0x0053C1B0-0x0053C21C: armor_shop2_trans (first drafted by tools/lbauto.py). */
#include "lobby_s.h"

void armor_shop2_trans(void) {
    int temp_a1;

    temp_a1 = F(s32, &lb_pit, 4) + (F(s8, &lb_pit, 8) * 8);
    switch (lbShop.step) {
    case 0:
    case 2:
        break;
    default:
            F(s8, &lb_pit, 0xB) = NPC_Message(F(s32, temp_a1, 4), F(s32, &lb_pit, 0), F(u16, temp_a1, 0), F(s8, &lb_pit, 9));
        break;
    }
}
