/* G online (PC): Lb_select, written from Monster Hunter G's code at lobby 0x59D630 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
s32 Get_sw2();                               /* extern */
s32 SetDialogYesNo();                    /* extern */
s32 cnWrap_SoundRequest();               /* extern */

s32 Lb_select(void) {
    s32 temp_v1;

    temp_v1 = Get_sw2(0) & 0xFFFF;
    if (temp_v1 & 0x20) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) == 0) {
            cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (temp_v1 & 0x40) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 1) {
            SetDialogYesNo(1);
            cnWrap_SoundRequest(3);
            goto block_15;
        }
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (temp_v1 & 0x800) {
        if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 0) {
            SetDialogYesNo(0);
            cnWrap_SoundRequest(1);
        }
    } else if ((temp_v1 & 0x400) && ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x30)) != 1)) {
        SetDialogYesNo(1);
        cnWrap_SoundRequest(1);
    }
block_15:
    return 2;
}

