/* lb_z40 - auto-drafted 0x005D8160-0x005D8274: Lb_select (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * pNet;

s32 Lb_select(void) {
    s32 temp_v1;

    temp_v1 = Get_sw2(0) & 0xFFFF;
    if (temp_v1 & 0x20) {
        if (F(u8, pNet, 0xF) == 0) {
            cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (temp_v1 & 0x40) {
        if (F(u8, pNet, 0xF) != 1) {
            SetDialogYesNo(1);
            cnWrap_SoundRequest(3);
            goto block_15;
        }
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (temp_v1 & 0x800) {
        if (F(u8, pNet, 0xF) != 0) {
            SetDialogYesNo(0);
            cnWrap_SoundRequest(1);
        }
    } else if ((temp_v1 & 0x400) && (F(u8, pNet, 0xF) != 1)) {
        SetDialogYesNo(1);
        cnWrap_SoundRequest(1);
    }
block_15:
    return 2;
}
