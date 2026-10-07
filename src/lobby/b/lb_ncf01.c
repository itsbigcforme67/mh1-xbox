/* lb_ncf01 - agent C 0x005B2AA0-0x005B2B2C: net_Check_FriendData (i = 0 before the guard). */
#include "lobby_a.h"

s32 net_Check_FriendData(char *arg0, s32 arg1, char *arg2) {
    s32 i;
    char *p;
    s32 n;

    n = arg1 & 0xFF;
    i = 0;
    if (0 < n) {
        p = arg0;
        do {
            if (*p != 0 && memcmp(p, arg2, 8) == 0) return i;
            i++;
            p += 0x30;
        } while (i < n);
    }
    return -1;
}