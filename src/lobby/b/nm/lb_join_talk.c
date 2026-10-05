#include "lobby_a.h"
extern char lb_pit[];
extern char lb_pit[];
void lb_join_talk(void) {
    int temp_a1;

    temp_a1 = F(s32, &lb_pit, 4) + (F(s8, &lb_pit, 8) * 8);
    if ((F(s8, &lb_sys, 6) != 0xD) && (F(s8, &lb_sys, 6) != 0x10) && (F(s8, &lb_sys, 6) != 2) && (F(s8, &lb_sys, 6) != 9) && (F(s8, &lb_sys, 6) != 7)) {
        return;
    }
    F(s8, &lb_pit, 0xB) = NPC_Message(F(s32, temp_a1, 4), F(s32, &lb_pit, 0), F(u16, temp_a1, 0), F(s8, &lb_pit, 9));
}
