/* lb_bz101 - lobby UI/client 0x005B3800-0x005B383C: lm_introduction_trans (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void lm_introduction_trans(void) {
    plaza_setMyCommentTrans(0xD8, 0x38, 1);
    DispSceneSubTitle();
    DispButtonHelp(pNet);
    Put_receive_mark(1);
}
