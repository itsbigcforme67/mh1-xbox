/* lb_bz102 - lobby UI/client 0x005B38A0-0x005B39AC: lm_logout_mv (first drafted by tools/lbauto.py). */
#include "lobby_a.h"

s32 lm_logout_mv(s32 arg0) {
    s32 temp_a1;

    temp_a1 = arg0 & 0xFFFF;
    F(s8, pNet, 0xC) = 1;
    if (temp_a1 & 0x20) {
        if (F(u8, pNet, 0xF) == 0) {
            Lbs_LogOutRequest(1, temp_a1);
            cnWrap_SoundRequest(6);
            return 0;
        }
        return 0x40;
    }
    if (temp_a1 & 0x40) {
        if (F(u8, pNet, 0xF) == 1) {
            return 0x40;
        }
        SetDialogYesNo(1, temp_a1);
        cnWrap_SoundRequest(3);
        goto block_16;
    }
    if (temp_a1 & 0x800) {
        if (F(u8, pNet, 0xF) != 0) {
            cnWrap_SoundRequest(1, temp_a1);
            SetDialogYesNo(0);
        }
    } else if ((temp_a1 & 0x400) && (F(u8, pNet, 0xF) != 1)) {
        cnWrap_SoundRequest(1, temp_a1);
        SetDialogYesNo(1);
    }
block_16:
    return 0;
}
