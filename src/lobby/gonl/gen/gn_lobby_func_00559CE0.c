/* G online (PC): lobby_func_00559CE0, written from Monster Hunter G's code at lobby 0x559CE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 quest_title;

void lobby_func_00559CE0(s32 arg0) {
    u8 *var_a1;   /* (s32 *) */
    s64 var_v0;

    var_v0 = 0;
    var_a1 = &(*(s32 *)((u8 *)&quest_title + 0x30));
loop_1:
    if ((*(u16 *)((u8 *)(((*(s32 *)var_a1) + var_a1)) + 0x1E)) == (arg0 & 0xFFFF)) {
        return;
    }
    var_v0 = ((s8) ((var_v0 + 1)));
    var_a1 += 0x200;
    if (var_v0 >= 3) {
        return;
    }
    goto loop_1;
}

