/* lb_z134 - auto-drafted 0x005D5750-0x005D58B4: lb_pl_chat (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char lit_1807_00664E40[];

void lb_pl_chat(u8 *arg0) {
    u8 temp_a1;

    temp_a1 = F(u8, arg0, 0x15);
    switch (temp_a1) {
    case 0:
        lb_pl_chat00(arg0, 0);
        return;
    case 1:
        lb_pl_chat01(arg0, 0);
        return;
    case 2:
        lb_pl_chat02(arg0, 0);
        return;
    case 3:
        lb_pl_chat03(arg0, 0);
        return;
    case 4:
        lb_pl_chat04(arg0, 0);
        return;
    case 5:
        lb_pl_chat05(arg0, 0);
        return;
    case 6:
        lb_pl_chat06(arg0, 0);
        return;
    case 7:
        lb_pl_chat07(arg0, 0);
        return;
    case 8:
        lb_pl_chat08(arg0, 0);
        return;
    case 9:
        lb_pl_chat09(arg0, 0);
        return;
    case 10:
        lb_pl_chat10(arg0, 0);
        return;
    case 11:
        lb_pl_chat11(arg0, 0);
        return;
    case 12:
        lb_pl_chat12(arg0, 0);
        return;
    case 13:
        lb_pl_chat09(arg0, 1);
        return;
    case 14:
        lb_pl_chat09(arg0, 2);
        return;
    case 15:
        lb_pl_chat09(arg0, 3);
        return;
    case 16:
        lb_pl_chat16(arg0, 0);
        return;
    default:
        system_error(&lit_1807_00664E40, F(u8, arg0, 0x14), temp_a1 & 0xFF, F(u8, arg0, 0x17));
        return;
    }
}
