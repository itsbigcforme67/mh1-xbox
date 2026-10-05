/* lb_bz12 - lobby UI/client 0x005B3560-0x005B35AC: lm_mail_box_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;

s32 lm_mail_box_i(void) {
    Lbc_init_network_work();
    F(s8, pNet, 7) = 4;
    SetSceneSubTitle(2, 1, *(s32 *)0x389E78 + 0x100);
    SetMessageHaltFlag();
    return 0;
}
