#include "lobby_b.h"
extern char event_eat_trans_ot0[];
extern char event_eat_trans_ot1[];
extern char lit_216_0065B900[];
void Lb_eat(void) {
    PLW *pl;

    pl = &player_work[game_w.master];
    switch (lb_sys.x06) {
    case 2:
    case 8:
        break;
    case 0:
        lb_sys.x06 = 8;
        Lbc_init_network_work(pl);
        Lbc_set_prim(event_eat_trans_ot0, event_eat_trans_ot1, 0);
        lb_eat_set();
        break;
    case 1:
        lb_sys.x06 = lb_sys.x06 + 1;
        Lb_act_set(pl, 0, 0x56);
        break;
    case 3:
        switch ((s8)event_eat_rcpt(network_work)) {
        case 0:
            lb_sys.x06 = 8;
            F(s8, cw, 0x35D6) = 1;
            break;
        case 1:
            lb_sys.x06 = 7;
            break;
        case 2:
            break;
        }
        break;
    case 4:
        lb_sys.x06 = 8;
        Lb_act_set(pl, 0, 0x61);
        break;
    case 5:
        if (F(u8, pNet, 6) == 0) {
            set01_set2(lit_216_0065B900);
            cnWrap_SoundRequest(2);
            lb_sys.x06 = lb_sys.x06 + 1;
        } else {
            event_eat_set_msg();
            lb_sys.x06 = lb_sys.x06 + 1;
        }
        break;
    case 6:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lbc_init_network_work(pl);
        }
        break;
    case 7:
        if (lb_sys.x76 == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lb_Pl_act_set(pl, 0, 0x4D, 0);
            Lbc_init_network_work();
        }
        break;
    }
}
