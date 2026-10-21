/* G online (PC): lobby_func_0056A810, written from Monster Hunter G's code at lobby 0x56A810 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
s32 SetDialogYesNo();
extern u8 g_main_2EFAD4;
extern u8 g_main_2EFAD4;
void lobby_func_0056A810(void *arg0) {
    (*(s16 *)((u8 *)(arg0) + 2)) = 0;
    (*(s16 *)((u8 *)(arg0) + 0)) = 0x10;
    (*(u16 *)((u8 *)(arg0) + 4)) = (u16) (*(u16 *)&g_main_2EFAD4);
    SetDialogData(0x42);
    SetDialogYesNo(1);
}

