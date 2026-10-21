/* G online (PC): lobby_func_0059F410, written from Monster Hunter G's code at lobby 0x59F410 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * memset();
extern s32 dialogData;

void lobby_func_0059F410(s32 unused_a0, s32 unused_a1, s32 arg2) {
    memset(&dialogData, 0, arg2);
}

