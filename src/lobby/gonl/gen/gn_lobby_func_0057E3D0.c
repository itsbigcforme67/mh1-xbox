/* G online (PC): lobby_func_0057E3D0, written from Monster Hunter G's code at lobby 0x57E3D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Init_InterruptFlag();                /* extern */

void lobby_func_0057E3D0(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 2;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 3;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9F)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA0)) = 0;
    Init_InterruptFlag(2);
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB98)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xEE0)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB71)) = 2;
}

