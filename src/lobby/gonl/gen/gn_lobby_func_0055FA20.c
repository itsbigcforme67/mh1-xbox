/* G online (PC): lobby_func_0055FA20, written from Monster Hunter G's code at lobby 0x55FA20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 get_questLevelNum();                     /* extern */

s32 lobby_func_0055FA20(void) {
    return get_questLevelNum(0x1E) != 0;
}

