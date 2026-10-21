/* G online (PC): SetDialogYesNo, written from Monster Hunter G's code at lobby 0x59F3F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 dialogData;
extern u8 pNet;

void SetDialogYesNo(s8 arg0) {
    (*(s8 *)((u8 *)&dialogData + 0x13)) = arg0;
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x30)) = arg0;
}

