#include "lobby_s.h"
extern char lb_pit[];
extern char lb_pit[];
void armor_shop_trans(void) {
    int temp_a1;

    temp_a1 = F(s32, &lb_pit, 4) + (F(s8, &lb_pit, 8) * 8);
    if ((lbShop.step != 2) && (lbShop.step != 0)) {
        F(s8, &lb_pit, 0xB) = NPC_Message(F(s32, temp_a1, 4), F(s32, &lb_pit, 0), F(u16, temp_a1, 0), F(s8, &lb_pit, 9));
    }
}
