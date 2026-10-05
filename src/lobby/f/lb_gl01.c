/* lb_gl01 - near-match fixes 0x005D0750-0x005D09DC: trade_get_ck_005D0750. Whole file in lb_l.c. */
#include "lobby_f.h"





int trade_get_ck_005D0750(PLW *pl) {
    s16 i;
    PLW *o;
    u16 item;
    if (Online_ck() == 0) {
        return 0;
    }
    if (pl->work936 != 0) {
        return 0;
    }
    o = player_work;
    i = 0;
    do {
        if (o->be_flag != 0) {
            if (i != pl->id && (s16)act_ck(o, 0, 0x2D) != 0 && o->work909 == pl->id && flvecCalcDistance(pl->pos, o->pos) <= 300.0f) {
                item = o->work904;
                if (Item_data[item][2] < 3) {
                    if ((s16)Ud_item_num_ck(item) == 0) {
                        if ((s16)Ud_item_search_space() != 0) {
                            pl->work904 = o->work904;
                            pl->work906 = o->work906;
                            if (Pl_master_ck(pl) == 1) {
                                Lb_send_item_request(CWPLAYER(i) + 0x132C, o);
                            }
                            Lb_Pl_act_set2(pl, 0, 0x2E, 0);
                        } else {
                            set01_set2(lit_516_00664D50);
                            pl->work936 = 0x5A;
                        }
                    } else if ((s16)Ud_item_num_ck2(o->work904) >= o->work906) {
                        pl->work904 = o->work904;
                        pl->work906 = o->work906;
                        if (Pl_master_ck(pl) == 1) {
                            Lb_send_item_request(CWPLAYER(i) + 0x132C, o);
                        }
                        Lb_Pl_act_set2(pl, 0, 0x2E, 0);
                    } else {
                        set01_set(1, 0xF, (s16)o->work904);
                        pl->work936 = 0x5A;
                    }
                    return 1;
                }
            }
        }
        i++;
        o++;
    } while (i < 8);
    return 0;
}
