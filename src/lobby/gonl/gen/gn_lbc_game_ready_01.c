/* G online (PC): lbc_game_ready_01, written from Monster Hunter G's code at lobby 0x583DE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
void lbc_game_ready_01(void) {
    u8 *temp_a0;

    temp_a0 = (*(void **)&cw);
    (*(u8 *)((u8 *)(temp_a0) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9B)) + 1);
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0;
}

