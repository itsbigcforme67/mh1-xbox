/* G online (PC): lobby_func_0056D730, written from Monster Hunter G's code at lobby 0x56D730 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
extern u8 pNet;
void lobby_func_0056D730(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    SetDialogData(0x3E, arg1, arg2, arg3);
    SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

