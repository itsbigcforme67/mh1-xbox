/* lb_a01 - lobby commer/trade/chair 0x005C5200-0x005C5378: lb_trade_start, lb_trade_check. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















#if !defined(G_PART) || G_PART == 1
#ifdef VERSION_MH2
#include "mh2_lb.h"
#endif
#ifdef VERSION_MH2
void lb_trade_start(s8 id, u8 *data) {
    PLW *pl = PLW_AT(id);
    LBTRADE t;
    if (pl->be_flag != 0) {
        memcpy(&t, data, 0xE);
        PL_W909(pl) = Lb_get_plID(t.plid);
        PL_W904(pl) = t.item;
        PL_W906(pl) = t.num;
        Lb_Pl_act_set2(pl, 0, 10, 0x10);
    }
}
#else
void lb_trade_start(s8 id, u8 *data) {
    PLW *pl = &player_work[id];
    LBTRADE t;
    if (pl->be_flag != 0) {
        memcpy(&t, data, 0xE);
        pl->work909 = Lb_get_plID(t.plid);
        pl->work904 = t.item;
        pl->work906 = t.num;
        Lb_Pl_act_set2(pl, 0, MH2V(0x2D, 0xA), 0x10);
    }
}
#endif
#endif

#if !defined(G_PART)
#ifdef VERSION_MH2
typedef struct { u8 p[0x862]; u8 w; } PL862;
#ifdef VERSION_MH2
void lb_trade_check(int a0, u8 *data) {
    PLM *pl = (PLM *)PLW_AT(game_w.master);
    LBTRADE t;
    memcpy(&t, data, 0xE);
    if ((s16)act_ck((PLW *)pl, 0, 10) != 0 && pl->trade_pl == (Lb_get_plID(t.plid) & 0xFF) && pl->trade_item == t.item
        && (s16)Pl_item_num_ck((PLW *)pl, t.item) >= t.num && pl->trade_num == t.num && pl->trade_st == 0) {
        Ud_item_stack(t.item, -t.num);
        Lb_send_item_result(a0, 0);
        pl->trade_st = 0xFF;
        return;
    }
    Lb_send_item_result(a0, 1);
}
#else
void lb_trade_check(int a0, u8 *data) {
    PLW *pl = PLW_AT(game_w.master);
    LBTRADE t;
    memcpy(&t, data, 0xE);
    if ((s16)act_ck(pl, 0, 10) != 0 && PL_W909(pl) == (Lb_get_plID(t.plid) & 0xFF) && PL_W904(pl) == t.item
        && (s16)Pl_item_num_ck(pl, t.item) >= t.num && PL_W906(pl) == t.num && ((PL862 *)pl)->w == 0) {
        Ud_item_stack(t.item, -t.num);
        Lb_send_item_result(a0, 0);
        ((PL862 *)pl)->w = 0xFF;
        return;
    }
    Lb_send_item_result(a0, 1);
}
#endif
#else
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
#endif
#endif
