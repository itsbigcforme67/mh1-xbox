/* lb_bz130 - lobby UI/client 0x005BDC90-0x005BDCE4: lobby_client_game_ready (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char lobby_client_game_ready_jmp_2838[];

void lobby_client_game_ready(void) {
    if (Check_InterruptFlag() == 0) {
        ((int (**)())&lobby_client_game_ready_jmp_2838)[F(u8, (u8 *)cw, 0x2C33)]();
        Disp_NowLoading2(4);
    }
}
