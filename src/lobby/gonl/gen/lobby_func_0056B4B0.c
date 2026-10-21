/* G online (PC): lobby_func_0056B4B0, written from Monster Hunter G's code at lobby 0x56B4B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 pNet;
s32 SetDialogData_HTML();                       /* extern */

void lobby_func_0056B4B0(void) {
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = 0;
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = (s16) (*(u8 *)((u8 *)((*(void **)&cw)) + 0x101A));
    SetDialogData_HTML();
}

