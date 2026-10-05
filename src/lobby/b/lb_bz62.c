/* lb_bz62 - lobby UI/client 0x005C4480-0x005C4498: lb_npc_die, lb_npc_erase (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void lb_npc_die(u8 *arg0) {
    F(u8, arg0, 4) = (u8) (F(u8, arg0, 4) + 1);
}

void lb_npc_erase(void) {
    push_em_work();
}
