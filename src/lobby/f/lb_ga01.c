/* lb_ga01 - near-match fixes 0x005C5440-0x005C55C4: lb_check_mini_data. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















int lb_check_mini_data(int a0, int a1, u8 *mini) {
    int ret = 0;
    s8 id = a0;
    u8 *m = CWPLAYER(id) + 0x1346;
    if (memcmp(m + 0xE, mini + 0xE, 6) != 0) {
        if ((s8)cw[id + 0x2BFE] == 0) {
            Lb_set_mini_data_to_pl(a0, mini);
            Lb_set_player(a0 & 0xFF, a1, CWPLAYER(id) + 0x1334);
        } else {
            Lb_player_release(&player_work[id]);
            Lb_set_mini_data_to_pl(a0, mini);
            Lb_set_player(a0 & 0xFF, a1, CWPLAYER(id) + 0x1334);
            ret = 1;
        }
        cw[id + 0x2BFE] = 0;
    } else if (memcmp(m + 8, mini + 8, 6) != 0) {
        Lb_set_mini_data_to_pl(a0, mini);
    }
    memcpy(m, mini, 0x40);
    if (id == game_w.master) {
        memcpy(my_user_mini_data, mini, 0x18);
    }
    return ret;
}
