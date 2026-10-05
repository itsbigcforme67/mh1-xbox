/* lb_bz44 - lobby UI/client 0x005BE450-0x005BE574: lbc_matching_failed_03, lbc_matching_failed_04, lbc_matching_failed_05 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;
extern u8 net_char_change;

void lbc_matching_failed_03(void) {
    void *temp_a0;

    F(s8, pNet, 0xC) = 1;
    if ((Fade_busy_ck() & 0xFF) != 1) {
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
    }
}

void lbc_matching_failed_04(void) {
    void *temp_a0;

    if (net_char_change != 0) {
        temp_a0 = (u8 *)cw;
        F(u8, temp_a0, 0x2C33) = (u8) (F(u8, temp_a0, 0x2C33) + 1);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        return;
    }
    all_reset();
    Lbs_load();
    F(s8, pNet, 0x11) = 1;
    To_GoToTop();
    cnWrap_BgmRequest(0);
}

void lbc_matching_failed_05(void) {
    if (cnLbc_LoadModelWait(1) != 0) {
        F(s8, (u8 *)cw, 0x2C08) = 1;
        cnWrap_ScreenReset();
        all_reset();
        Lbs_load();
        To_GoToTop();
        F(s8, pNet, 0x11) = 1;
        cnWrap_BgmRequest(0);
    }
}
