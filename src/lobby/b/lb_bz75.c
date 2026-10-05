/* lb_bz75 - lobby UI/client 0x005B2A50-0x005B2A98: net_Check_FriendFree (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

int net_Check_FriendFree(p, n)
char *p;
u8 n;
{
    int i;
    for (i = 0; i < n; i++, p += 0x30) {
        if (*p == 0) return i;
    }
    return -1;
}
