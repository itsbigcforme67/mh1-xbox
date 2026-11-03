/* G online (PC): lobby_func_0058BD30, written from Monster Hunter G's code at lobby 0x58BD30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 PitMenu;
s32 Online_ck();                                    /* extern */

s32 lobby_func_0058BD30(void) {
    if (Online_ck() == 1) {
        (*(s8 *)((u8 *)&PitMenu + PMO(0x10))) = 0;
        return 0;
    }
    return 1;
}

