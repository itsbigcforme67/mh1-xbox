/* lb_by119 - agent B 0x005B3DA0-0x005B3E1C: Lb_ck_menu (village start menu opened from the walking loop). */
#include "lobby_s.h"
extern u8 *lbmw;
#ifdef VERSION_MH2
void Lb_ck_menu(void) {
    *(s32 *)0x8BDC18 = 0;
    *(s32 *)0x8BDC10 = 0;
    *(s8 *)0x8BEB5E = 3;
    Lbc_init_network_work();
    Lbc_set_prim(0, 0, 0, 0);
    *(s8 *)0x8EA8FC = 0;
}
#else
void Lb_ck_menu(void) {
    if (lb_sys.x68 != 0xF) {
        lb_sys.x68 = 0xF;
        *(s8 *)0x3F33FE = 1;
        Lbc_init_network_work();
        Lbc_set_prim(0, 0, 0);
        lb_menu_init();
        F(s8, lbmw, 5) = 1;
        F(s16, lbmw, 6) = 0;
        se_req(7, 0x11, 0);
    }
}
#endif
