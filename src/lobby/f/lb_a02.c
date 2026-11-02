/* lb_a02 - lobby commer/trade/chair 0x005C55D0-0x005C5678: Lb_chat_receipt, Lb_get_plID. Whole file in lb_a.c. */
#include "g_alias.h"
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















#if !defined(G_PART) || G_PART == 1
void Lb_chat_receipt(u8 *msg) {
    Chat_log_add(Lb_get_plID() & 0xFF, msg);
}
#endif

#if !defined(G_PART) || G_PART == 2
#ifdef VERSION_MH2
#ifdef VERSION_MH2
int Lb_get_plID(u8 *mac) {
    int i;
    LBCOMMER *c;
    if (*(s8 *)mac == 0) {
        return 0xFF;
    }
    i = 0;
    c = lbCommer;
    do {
        if (memcmp(c, mac, 6) == 0) return i;
        i = (i + 1) & 0xFF;
        c++;
    } while (i < 10);
    return 0xFF;
}
#else
int Lb_get_plID(u8 *mac) {
    int i = 0;
    LBCOMMER *c = lbCommer;
    if (*(s8 *)mac == 0) {
        return 0xFF;
    }
    do {
        if (memcmp(c, mac, 6) == 0) return i;
        i = (i + 1) & 0xFF;
        c++;
    } while (i < 10);
    return 0xFF;
}
#endif
#else
int Lb_get_plID(u8 *mac) {
    int i = 0;
    LBCOMMER *c = lbCommer;
    do {
        if (memcmp(c, mac, 6) == 0) return i;
        i = (i + 1) & 0xFF;
        c++;
    } while (i < GV(8, 10));
    return 0xFF;
}
#endif
#endif
