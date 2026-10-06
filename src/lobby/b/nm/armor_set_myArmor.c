#include "lobby_a.h"
extern char User_data[];
extern char my_user_id[];
void armor_set_myArmor(void) {
    u8 temp_a1;
    int temp_s0;

    temp_a1 = game_w.master;
    temp_s0 = (int)&player_work + (temp_a1 * 0xA00);
    Set_equip_idx(&User_data);
    Lb_player_release(temp_s0);
    flCompact();
    F(s8, &lb_sys, 0x8D) = 3;
    Set_userdata(temp_s0);
    Lb_set_player(game_w.master, &my_user_id, &my_user_handle);
    Lb_player_load(temp_s0);
    F(s8, &lb_sys, 0x78) = 1;
    Lb_set_mini_data((int)&lbCommer + (F(u16, temp_s0, 0xC) * 0x5C) + 0x1C);
    Lb_set_mini_data((s32)cw + (F(u16, temp_s0, 0xC) * 0x2FC) + 0x1346);
}
