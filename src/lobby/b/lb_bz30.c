/* lb_bz30 - lobby UI/client 0x005B9320-0x005B9400: To_BootUpBrowser, lbc_browser_00, lbc_browser_01, lbc_browser_02 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern s8 BsLbsErrNum;
extern s8 net_char_change;
extern s8 BS_MODE_R_NO;
extern char FirstURL[];
extern char first_url[];

void To_BootUpBrowser(void) {
    F(s8, (u8 *)cw, 0x2C43) = 0;
    F(s8, (u8 *)cw, 0x2C44) = 1;
    F(s8, (u8 *)cw, 0x2C08) = 0;
}

void lbc_browser_00(void) {
    void *temp_a0;

    temp_a0 = (u8 *)cw;
    F(u8, temp_a0, 0x2C43) = (u8) (F(u8, temp_a0, 0x2C43) + 1);
    F(s8, (u8 *)cw, 0x2C08) = 0;
    BsLbsErrNum = 0;
}

void lbc_browser_01(void) {
    void *temp_v1;

    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C43) = (u8) (F(u8, temp_v1, 0x2C43) + 1);
    all_reset();
    cnWrap_InitWork();
    Lbs_load();
    net_char_change = 1;
}

void lbc_browser_02(void) {
    void *temp_v1;

    temp_v1 = (u8 *)cw;
    F(u8, temp_v1, 0x2C43) = (u8) (F(u8, temp_v1, 0x2C43) + 1);
    BS_MODE_R_NO = 0;
    FlushCache(0);
    MainBsInitialize(1);
    memset(&FirstURL, 0, 0x100);
    strcpy(&FirstURL, &first_url);
}
