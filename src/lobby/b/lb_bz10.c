/* lb_bz10 - lobby UI/client 0x005B2E40-0x005B2E88: lm_member_list_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;

s32 lm_member_list_i(void) {
    u8 temp_v1_2;
    void *temp_v1;

    Lbc_init_network_work();
    temp_v1 = pNet;
    temp_v1_2 = F(u8, temp_v1, 8);
    if (temp_v1_2 == game_w.master) {
        F(u8, temp_v1, 8) = (u8) (temp_v1_2 + 1);
    }
    F(s8, pNet, 3) = 0;
    return 0;
}
