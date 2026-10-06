/* lb_bz43 - lobby UI/client 0x005BE300-0x005BE350: lbc_matching_failed_00 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern char put_back[];

void lbc_matching_failed_00(void) {
    void *temp_v1;

    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C33) = (u8) (F(u8, temp_v1, 0x2C33) + 1);
    cnWrap_BgmVolume(0);
    Lbc_init_network_work();
    Lbc_set_prim(&put_back, 0, 0);
    fade_set(0xA);
}
