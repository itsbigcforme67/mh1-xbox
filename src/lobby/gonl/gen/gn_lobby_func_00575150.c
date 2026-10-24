/* G online (PC): lobby_func_00575150, written from Monster Hunter G's code at lobby 0x575150 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_main_2EFBC4;
extern u8 g_main_2EFBC4;
extern u8 pNet;
s32 lobby_func_00575150(void) {
    u8 *temp_v0;
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
    if ((temp_v1 != 7) && (temp_v1 != 3)) {
        temp_v0 = (*(u8 **)&g_main_2EFBC4);
        if (temp_v0 == NULL) {
            return 1;
        }
        return *temp_v0 != 0;
    }
    return 1;
}

