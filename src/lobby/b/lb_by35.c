/* lb_by35 - agent B promoted near-match 0x005B2A10-0x005B2A50: net_Check_FriendSuu (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 net_Check_FriendSuu(char *p, s32 n) {
    s32 cnt;
    s32 i;

    n &= 0xFF;
    cnt = 0;
    for (i = 0; i < n; i++) {
        if (*p == 0) break;
        cnt++;
        p += 0x30;
    }
    return cnt;
}
