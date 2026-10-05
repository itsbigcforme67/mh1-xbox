/* lb_bz99 - lobby UI/client 0x005B3520-0x005B355C: lm_friend_list_trans (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void lm_friend_list_trans(void) {
    plaza_checkFriendTrans(0xD8, 0x38, 1);
    DispSceneSubTitle();
    DispButtonHelp(pNet);
    Put_receive_mark(1);
}
