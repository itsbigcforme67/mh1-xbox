/* G online (PC): SetHelpLineMsg, written from Monster Hunter G's code at lobby 0x59F720 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_0059F6F0();                   /* extern */

void SetHelpLineMsg(s32 arg0) {
    lobby_func_0059F6F0(arg0 * 4);
}

