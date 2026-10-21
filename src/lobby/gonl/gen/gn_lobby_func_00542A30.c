/* G online (PC): lobby_func_00542A30, written from Monster Hunter G's code at lobby 0x542A30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 func_0010D8E0();                         /* extern */

s32 lobby_func_00542A30(void) {
    return (func_0010D8E0(1) & 0xFF) == 0x58;
}

