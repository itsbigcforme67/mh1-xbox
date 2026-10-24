/* G online (PC): To_GoToTop, written from Monster Hunter G's code at lobby 0x584B90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetDialogData();
extern u8 cw;
void To_GoToTop(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 7;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0;
    SetDialogData(0x19, arg1, arg2, arg3);
}

