#include "lobby_a.h"

void lb_menu_status_i(void) {
    s16 var_v0;
    int temp_s0;

    temp_s0 = (int)&player_work + (game_w.master * 0xA00);
    Skill_set_PL(temp_s0);
    var_v0 = *(s8 *)0x3F3605 + 0x64 + (s16)skill_hp_calc_005B44B0(temp_s0);
    if (var_v0 > 0x96) {
        var_v0 = 0x96;
    }
    F(s16, temp_s0, 0x792) = (s16) var_v0;
    F(s16, temp_s0, 0x302) = (s16) var_v0;
    F(s16, temp_s0, 0x882) = (s16) (*(s16 *)0x3F3606 + 0x12C);
    F(s8, temp_s0, 0x6A4) = (s8) *(s8 *)0x3F3603;
    F(s8, temp_s0, 0x6A5) = 0;
    Pl_atck_adj_calc(temp_s0);
    F(s8, temp_s0, 0x6A8) = (s8) *(s8 *)0x3F3604;
    F(s8, temp_s0, 0x6A9) = 0;
    Pl_def_adj_calc(temp_s0);
    Pl_reg_calc(temp_s0);
    Menu_status_i();
}
