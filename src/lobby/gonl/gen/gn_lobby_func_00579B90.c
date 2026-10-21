/* G online (PC): lobby_func_00579B90, written from Monster Hunter G's code at lobby 0x579B90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 release_texture();          /* extern */

void lobby_func_00579B90(void) {
    release_texture(0x13F, 4);
    release_texture(0x144, 0x10);
}

