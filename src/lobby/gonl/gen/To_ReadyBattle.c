/* G online (PC): To_ReadyBattle, written from Monster Hunter G's code at lobby 0x583AA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Init_InterruptFlag();                       /* extern */

void To_ReadyBattle(void) {
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB99)) = 4;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9A)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
    Init_InterruptFlag();
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB98)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) = 0;
}

