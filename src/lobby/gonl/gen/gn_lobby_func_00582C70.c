/* G online (PC): lobby_func_00582C70, written from Monster Hunter G's code at lobby 0x582C70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SoftKeyboard_exit();
extern u8 cw;
s32 Init_InterruptFlag();                /* extern */

void lobby_func_00582C70(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 3;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 1;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    Init_InterruptFlag(3);
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB98)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xEE0)) = 0;
    SoftKeyboard_exit();
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB72)) = 2;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB73)) = 2;
}

