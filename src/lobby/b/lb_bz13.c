/* lb_bz13 - lobby UI/client 0x005B3640-0x005B37AC: lm_net_status_i, lm_net_status_mv, lm_net_status_trans, lm_introduction_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;
extern char D_3E5505[];
extern u8 lm_menu2_tbl[8];
extern char statData[];

s32 lm_net_status_i(void) {
    if (Lbs_InRoomCheck() == 1) {
        return 1;
    }
    Lbc_init_network_work();
    return 0;
}

s32 lm_net_status_mv(s32 arg0) {
    s32 temp_v1;
    u8 temp_a0;
    u8 temp_a1;

    temp_v1 = arg0 & 0xFFFF;
    if (temp_v1 & 0x20) {
        temp_a0 = game_w.master;
        temp_a1 = F(u8, pNet, 8);
        *((u8 *)&D_3E5505 + (temp_a0 * 0xA00)) = temp_a1;
        Lbc_SendMiniData(temp_a0, temp_a1);
        cnWrap_SoundRequest(6);
        goto block_7;
    }
    if (temp_v1 & 0x40) {
        return 0x40;
    }
    F(u8, pNet, 8) = Lb_cursorUD(F(u8, pNet, 8), 5);
block_7:
    return arg0;
}

void lm_net_status_trans(void) {
    DispFrameList(&statData, (*(s32 *)(lm_menu2_tbl + 4)), F(u8, pNet, 8));
}

s32 lm_introduction_i(void) {
    Lbc_init_network_work();
    F(s8, pNet, 7) = 8;
    SetSceneSubTitle(2, 1, *(s32 *)0x389E7C + 0x70);
    SetMessageHaltFlag();
    return 0;
}
