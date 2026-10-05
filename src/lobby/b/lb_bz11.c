/* lb_bz11 - lobby UI/client 0x005B3470-0x005B34C4: lm_friend_list_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;

s32 lm_friend_list_i(void) {
    Lbc_init_network_work();
    F(s8, pNet, 7) = 3;
    SetSceneSubTitle(2, 1, *(s32 *)0x389E78 + 0xDC);
    SetMessageHaltFlag();
    plaza_checkFriend();
    return 0;
}
