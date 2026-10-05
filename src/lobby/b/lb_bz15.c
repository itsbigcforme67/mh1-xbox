/* lb_bz15 - lobby UI/client 0x005B3D20-0x005B3D98: Lb_pl_status_t, Lb_Menu_Init (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;
extern u8 * lbmw;
extern char lb_menu_w[];

void Lb_pl_status_t(u8 *arg0) {
    void *temp_v1;

    font_set_stack_no(F(s32, arg0, 0x18));
    temp_v1 = pNet;
    Lb_PlayerStatus((u8 *)&lb_player + (F(u8, temp_v1, 8) * 0x38), F(u8, temp_v1, 3));
}

void Lb_Menu_Init(void) {
    lbmw = (u8 *)&lb_menu_w;
    F(s8, lbmw, 4) = 0;
    F(s8, lbmw, 5) = 0;
    F(s8, lbmw, 0xA) = 0;
}
