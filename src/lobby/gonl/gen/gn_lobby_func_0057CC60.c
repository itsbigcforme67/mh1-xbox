/* G online (PC): lobby_func_0057CC60, written from Monster Hunter G's code at lobby 0x57CC60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
extern u8 cw;
extern u8 pNet;
s32 Init_InterruptFlag();                       /* extern */

void lobby_func_0057CC60(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 8;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    Init_InterruptFlag();
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA5E)) == 1) {
        SetDialogData(3);
    } else {
        SetDialogData(0x19);
    }
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
}

