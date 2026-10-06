/* lb_ga02 - receivers 0x005C50F0-0x005C5200: lb_commer_message. Whole file in lb_a.c. */
#include "lobby_f.h"
extern char lit_238_0065ECF0[];
extern char lit_476_0065ED10[];
extern u8 D_3E54FB[];
















void lb_commer_message(s8 id, u8 *src) {
    u8 buf[0x120];
    char *name;
    if (CW8(0x35D5) != 0) {
        name = lbCommer[id].name;
        memcpy(name, src, 0x10);
        memcpy(CWPLAYER(id) + 0x1334, src, 0x10);
        memcpy(&lb_player[id].x04, src, 0x10);
        memset(buf, 0, 0x120);
        buf[0x11F] = 6;
        buf[0x11E] = 6;
        buf[0x11D] = 6;
        if (lbCommer[id].name != 0) {
            sprintf((char *)buf + 0x1C, lit_238_0065ECF0, lbCommer[id].name);
            Chat_log_add(0, buf);
        }
    }
}
