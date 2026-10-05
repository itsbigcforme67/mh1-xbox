/* lb_a01 - lobby commer/trade/chair 0x005C5200-0x005C5378: lb_trade_start, lb_trade_check. Whole file in lb_a.c. */
#include "lobby.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















void lb_trade_start(s8 id, u8 *data) {
    PLW *pl = &player_work[id];
    LBTRADE t;
    if (pl->be_flag != 0) {
        memcpy(&t, data, 0xE);
        pl->work909 = Lb_get_plID(t.plid);
        pl->work904 = t.item;
        pl->work906 = t.num;
        Lb_Pl_act_set2(pl, 0, 0x2D, 0x10);
    }
}

void lb_trade_check(int a0, u8 *data) {
    PLW *pl = &player_work[game_w.master];
    LBTRADE t;
    memcpy(&t, data, 0xE);
    if ((s16)act_ck(pl, 0, 0x2D) != 0 && pl->work904 == t.item && (s16)Pl_item_num_ck(pl, t.item) >= t.num) {
        Ud_item_stack(t.item, -t.num);
        Lb_send_item_result(a0, 0);
        pl->work90A = 0xFF;
        return;
    }
    Lb_send_item_result(a0, 1);
}
