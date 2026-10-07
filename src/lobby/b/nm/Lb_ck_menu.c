#include "lobby_a.h"
extern int lbmw;
void Lb_ck_menu(void) {
    s32 temp_a0;

    temp_a0 = F(s32, &lb_sys, 0x68);
    if (temp_a0 != 0xF) {
        F(s32, &lb_sys, 0x68) = 0xF;
        *(s8 *)0x3F33FE = 1;
        Lbc_init_network_work();
        Lbc_set_prim(0, 0, 0);
        lb_menu_init();
        F(s8, lbmw, 5) = 1;
        F(s16, lbmw, 6) = 0;
        se_req(7, 0x11, 0);
    }
}
