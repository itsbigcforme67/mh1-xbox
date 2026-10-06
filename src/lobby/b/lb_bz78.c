/* lb_bz78 - lobby UI/client 0x005B35B0-0x005B35FC: lm_mail_box_mv (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

int lm_mail_box_mv() {
    switch (plaza_mailBox()) {
    case 3:
        ClearMessageHaltFlag();
        return 0x40;
    default:
        return 0;
    }
}
