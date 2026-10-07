/* lb_c511 - agent C round 5 0x005B3DA0-0x005B3E1C: Lb_ck_menu (lb_sys.x68 field, game_w byte 0xE). */
#include "lobby_b.h"
extern int lbmw;
void Lb_ck_menu(void) {
    s32 temp_a0;

    temp_a0 = lb_sys.x68;
    if (temp_a0 != 0xF) {
        lb_sys.x68 = 0xF;
        *((s8 *)&game_w + 0xE) = 1;
        Lbc_init_network_work();
        Lbc_set_prim(0, 0, 0);
        lb_menu_init();
        F(s8, lbmw, 5) = 1;
        F(s16, lbmw, 6) = 0;
        se_req(7, 0x11, 0);
    }
}
