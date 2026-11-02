/* lb_a04 - lobby commer/trade 0x005C5380-0x005C5434: lb_trade_result. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];

#ifdef VERSION_MH2
#include "mh2_lb.h"
#endif
#ifdef VERSION_MH2
void lb_trade_result(u8 *data) {
    PLW *pl = PLW_MH2(game_w.master);
    LBTRADE2 t;
    void Ud_item_stack(u16, int);
    if ((s16)act_ck(pl, 0, 11) != 0) {
        memcpy(&t, data, 0xE);
        if (t.result == 0) {
            F(u8, pl, 0x862) = 1;
            Ud_item_stack(t.item, t.num);
            set01_set(1, 0xD, (s16)t.item);
            return;
        }
        F(u8, pl, 0x862) = 2;
    }
}
#else
void lb_trade_result(u8 *data) {
    PLW *pl = &player_work[game_w.master];
    LBTRADE2 t;
    void Ud_item_stack(u16, int);
    if ((s16)act_ck(pl, 0, 0x2E) != 0) {
        memcpy(&t, data, 0xE);
        if (t.result == 0) {
            pl->work90A = 1;
            Ud_item_stack(t.item, t.num);
            set01_set(1, 0xD, (s16)t.item);
            return;
        }
        pl->work90A = 2;
    }
}
#endif
