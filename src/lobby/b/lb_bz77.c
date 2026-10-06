/* lb_bz77 - lobby UI/client 0x005B34D0-0x005B351C: lm_friend_list_mv (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

int lm_friend_list_mv() {
    switch (plaza_checkFriend()) {
    case 3:
        ClearMessageHaltFlag();
        return 0x40;
    default:
        return 0;
    }
}
