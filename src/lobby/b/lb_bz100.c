/* lb_bz100 - lobby UI/client 0x005B3600-0x005B363C: lm_mail_box_trans (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

void lm_mail_box_trans(void) {
    plaza_mailBoxTrans(0xD8, 0x38, 1);
    DispSceneSubTitle();
    DispButtonHelp(pNet);
    Put_receive_mark(1);
}
