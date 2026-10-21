/* G online (PC): lobby_func_00559FF0, written from Monster Hunter G's code at lobby 0x559FF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559FF0(void *arg0) {
    if (arg0 == NULL) {
        return 0;
    }
    return ((*(u8 *)((u8 *)(arg0) + 0)) | (((*(u8 *)((u8 *)(arg0) + 1)) << 8) & 0xFF00)) & 0xFFFF;
}

