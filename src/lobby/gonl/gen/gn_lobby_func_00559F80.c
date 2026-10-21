/* G online (PC): lobby_func_00559F80, written from Monster Hunter G's code at lobby 0x559F80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559B40();
void * lobby_func_00559C70();
extern u8 cw;
void *lobby_func_00559F80(s32 arg0) {
    s32 temp_s0;
    u8 *var_v0;

    temp_s0 = arg0 & 0xFFFF;
    var_v0 = NULL;
    if (temp_s0 >= 0xC350) {
        if ((*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) == 1) {
            var_v0 = lobby_func_00559C70(arg0);
            if ((var_v0 != NULL) && ((*(u16 *)((u8 *)(var_v0) + 0x1E)) != temp_s0)) {
                return NULL;
            }
        }
        /* Duplicate return node #6. Try simplifying control flow for better match */
        return var_v0;
    }
    var_v0 = lobby_func_00559B40(arg0);
    return var_v0;
}

