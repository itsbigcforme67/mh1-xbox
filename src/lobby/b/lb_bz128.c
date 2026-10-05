/* lb_bz128 - lobby UI/client 0x005BA1C0-0x005BA23C: lobby_client_game_in_plaza (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lbc_in_plaza_jmp_1345[];

void lobby_client_game_in_plaza(void) {
    if (Check_InterruptFlag() == 0) {
        if (((int *)&lbc_in_plaza_jmp_1345)[F(u8, (u8 *)cw, 0x2C33)] == 0) {
            To_TopMenu();
        }
        ((int (**)())&lbc_in_plaza_jmp_1345)[F(u8, (u8 *)cw, 0x2C33)]();
    }
}
