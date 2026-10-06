/* lb_bz33 - lobby UI/client 0x005BA170-0x005BA1BC: lbc_top_menu_06 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;
extern char ClassInfo[];

void lbc_top_menu_06(void) {
    F(s8, (u8 *)cw, 0x2C33) = 1;
    F(s8, (u8 *)cw, 0x2C34) = 0;
    F(s8, &ClassInfo, 0) = 0;
    F(s8, &ClassInfo, 4) = 0;
    F(s8, &ClassInfo, 8) = 0;
    Lbc_init_network_work();
    F(s8, pNet, 6) = 0;
}
