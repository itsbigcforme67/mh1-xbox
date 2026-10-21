/* G online (PC): lobby_func_0056D730, written from Monster Hunter G's code at lobby 0x56D730 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
s32 SetDialogData();                     /* extern */
s32 SetDialogYesNo();                    /* extern */

void lobby_func_0056D730(void) {
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    SetDialogData(0x3E);
    SetDialogYesNo(1);
}

