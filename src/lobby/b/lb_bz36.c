/* lb_bz36 - lobby UI/client 0x005BB840-0x005BB8C8: lbc_in_lobby_00_00 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void lbc_in_lobby_00_00(void) {
    void *temp_v1;

    cnLbc_CheckInFloorOrder(1);
    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C34) = (u8) (F(u8, temp_v1, 0x2C34) + 1);
    F(s8, (u8 *)cw, 0x2C35) = 0;
    F(s8, (u8 *)cw, 0x2C39) = 0;
    F(s8, (u8 *)cw, 0x32C5) = 0;
    F(s8, (u8 *)cw, 0x32C2) = 0;
    cnWrap_InitWork();
    Lbc_init_network_work();
    Lbc_set_prim(0, 0, 0);
    Lbc_release();
    Clear_lobby_ram();
    str_stop(0);
    str_stop(1);
}
