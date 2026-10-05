/* lb_a03 - lobby commer/trade/chair 0x005C5B40-0x005C5D4C: lb_set_chair, lb_recv_myChair, lb_send_my_status, lb_chidori_off. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















void lb_set_chair(int a0, u8 *p) {
    u8 me;
    if (p[0] == 0) {
        me = game_w.master;
        if (me == (Lb_get_plID(p + 2) & 0xFF) && lb_sys.x68 == 0x18) {
            lb_sys.x68 = 0;
            lb_sys.x6C = 0;
        }
        return;
    }
    lb_sys.chair_mask = lb_sys.chair_mask | (1 << p[1]);
    me = game_w.master;
    if (me == (Lb_get_plID(p + 2) & 0xFF)) {
        if (game_w.stage == 0x4D && lb_sys.x68 == 0x18) {
            Lb_pl_to_chair();
            return;
        }
        Lb_send_chair_release(&player_work[me]);
    }
}

void lb_recv_myChair(u8 *p) {
    lb_sys.chair_mask = lb_sys.chair_mask | (1 << *p);
}

void lb_send_my_status(int a0) {
    Lb_send_stage();
    if (game_w.stage == 0x4D || game_w.stage == 0x4C) {
        Lb_send_pl_status(&player_work[game_w.master]);
        if (D_3E54FB[game_w.master * 0xA00] != 0) {
            Lb_send_trade_startTU(&player_work[game_w.master], a0);
        }
    }
}

void lb_chidori_off(s8 id) {
    PLW *pl = &player_work[id];
    PLU8(pl, 0x8EC) = 0;
    PLU8(pl, 0x90F) = 0;
}
