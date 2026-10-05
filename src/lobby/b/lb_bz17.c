/* lb_bz17 - lobby UI/client 0x005B4330-0x005B43CC: lb_menu_item_mv, lb_menu_mix_mv (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

s32 lb_menu_item_mv(void) {
    s32 temp_s0;

    temp_s0 = Menu_item_mv() & 0xFFFF;
    ItemCopy_Pl2Ud((u8 *)&player_work + (game_w.master * 0xA00));
    return temp_s0;
}

s32 lb_menu_mix_mv(void) {
    s32 temp_s0;

    temp_s0 = Menu_mix_mv() & 0xFFFF;
    ItemCopy_Pl2Ud((u8 *)&player_work + (game_w.master * 0xA00));
    return temp_s0;
}
