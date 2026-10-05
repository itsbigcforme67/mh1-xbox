/* lb_bz46 - lobby UI/client 0x005BF2E0-0x005BF370: lbc_admin_message_02, Init_InterruptFlag, Lbs_CheckMatchingFlag (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void lbc_admin_message_02(void) {
    F(s8, (u8 *)cw, 0x2C5C) = 0;
    F(s8, (u8 *)cw, 0x2F6E) = 0;
    F(s8, (u8 *)cw, 0x2F6F) = 0;
    F(s8, (u8 *)cw, 0x2F76) = 0;
    F(s8, (u8 *)cw, 0x2F72) = 0;
}

void Init_InterruptFlag(void) {
    memset((u8 *)cw + 0x2C09, 0, 3);
    F(s8, (u8 *)cw, 0x2C0C) = 0;
    F(s8, (u8 *)cw, 0x2C0D) = 0;
    F(s8, (u8 *)cw, 0x2C45) = 0;
    cnLBS_Init_LobbyBgProcess();
    cnLBS_Init_LobbyBgBurstProcess();
}

s32 Lbs_CheckMatchingFlag(void) {
    return F(s8, (u8 *)cw, 0x2C0C) != 0;
}
