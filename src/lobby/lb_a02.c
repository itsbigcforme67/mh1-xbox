/* lb_a02 - lobby commer/trade/chair 0x005C55D0-0x005C5678: Lb_chat_receipt, Lb_get_plID. Whole file in lb_a.c. */
#include "lobby.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















void Lb_chat_receipt(u8 *msg) {
    Chat_log_add(Lb_get_plID() & 0xFF, msg);
}

int Lb_get_plID(u8 *mac) {
    int i = 0;
    LBCOMMER *c = lbCommer;
    do {
        if (memcmp(c, mac, 6) == 0) return i;
        i = (i + 1) & 0xFF;
        c++;
    } while (i < 8);
    return 0xFF;
}
