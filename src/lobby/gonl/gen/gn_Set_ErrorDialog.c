/* G online (PC): Set_ErrorDialog, written from Monster Hunter G's code at lobby 0x584360 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_eat_to_bell();
s32 SetDialogData();
extern u8 cw;
void Set_ErrorDialog(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_v1;

    temp_v1 = ((s8) (arg0));
    switch (temp_v1) {
    case 1:
        SetDialogData(0xA, arg1, arg2, arg3);
        return;
    case 3:
        SetDialogData(8, arg1, arg2, arg3);
        return;
    case 4:
    case 6:
        SetDialogData(0xB, arg1, arg2, arg3);
        return;
    case 5:
    case 7:
        Lb_eat_to_bell((*(s32 *)&cw) + 0xF75D, arg1, arg2, arg3);
        /* fallthrough */
    default:
        return;
    }
}

