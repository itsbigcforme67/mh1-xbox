/* lb_bz18 - lobby UI/client 0x005B4540-0x005B458C: lm_room_member_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * lbmw;

s32 lm_room_member_i(void) {
    Lbc_init_network_work();
    F(s8, lbmw, 9) = 0;
    F(s8, lbmw, 0xA) = 0;
    F(s8, lbmw, 0xB) = 0;
    *(s16 *)0x39DAD2 = 0x11;
    lm_room_member_mv(0);
    return 0;
}
