/* G online (PC): lobby_func_0058F660, written from Monster Hunter G's code at lobby 0x58F660 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnWrap_SoundRequest();
s32 ListSelect();             /* extern */

u8 lobby_func_0058F660(s32 arg0, u8 arg1) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp2F (*(u8 *)(g_frame + 0x2F))
    s32 temp_v1;
    u8 var_v0;

    sp2F = arg1;
    if ((s32) sp2F < 6) {
        ListSelect(&sp2F, arg0, 6);
    } else {
        sp2F -= 6;
        ListSelect(&sp2F, arg0, 6);
        sp2F += 6;
    }
    temp_v1 = arg0 & 0xFFFF;
    if (temp_v1 & 0xC00) {
        if (temp_v1 & 0x800) {
            if ((s32) sp2F < 6) {
                sp2F += 0xC;
            }
            var_v0 = sp2F - 6;
            goto block_9;
        }
        sp2F += 6;
        var_v0 = sp2F - 0xC;
        if ((s32) sp2F >= 0xC) {
block_9:
            sp2F = var_v0;
        }
        cnWrap_SoundRequest(1);
    }
    return sp2F;
}

