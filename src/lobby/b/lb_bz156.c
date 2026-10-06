/* lb_bz156 - lobby UI/client 0x005B2B30-0x005B2C20: net_Delete_FriendData (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct FRIENDENT { f32 f[12]; } FRIENDENT;      /* friend list entry (0x30 bytes), copied word-wise through the FPU */

int net_Delete_FriendData(FRIENDENT *tbl, int idx, int n) {
    int i;
    FRIENDENT *src;
    FRIENDENT *dst;

    n = n & 0xFF;
    idx = idx & 0xFF;
    if (idx >= n) {
        return 0;
    }
    i = (idx + 1) & 0xFF;
    if (i <= n) {
        src = tbl + i;
        dst = src;
        do {
            if (i < n) {
                dst[-1] = src[0];
                *(u8 *)src = 0;
                *((u8 *)src + 8) = 0;
            } else {
                *(u8 *)&dst[-1] = 0;
                *((u8 *)&dst[-1] + 8) = 0;
            }
            i = (i + 1) & 0xFF;
            src++;
            dst++;
        } while (n >= i);
    }
    return 1;
}
