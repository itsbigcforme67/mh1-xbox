/* lb_by82 - agent B promoted near-match 0x0053C520-0x0053C610: armor_set_myArmor (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char User_data[];
extern char my_user_id[];

void armor_set_myArmor(void) {
    u8 *pl;

    pl = (u8 *)&player_work[game_w.master];
    Set_equip_idx(&User_data);
    Lb_player_release(pl);
    flCompact();
    lb_sys.x8D = 3;
    Set_userdata(pl);
    Lb_set_player(game_w.master, &my_user_id, &my_user_handle);
    Lb_player_load(pl);
    lb_sys.x78 = 1;
    Lb_set_mini_data((u8 *)&lbCommer[*(u16 *)(pl + 0xC)] + 0x1C);
    Lb_set_mini_data((u8 *)cw + *(u16 *)(pl + 0xC) * 0x2FC + 0x1346);
}
