/* G online (PC): lobby_func_00559EB0, written from Monster Hunter G's code at lobby 0x559EB0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * Lb_get_quest_str();
s32 lobby_func_00559C30();
extern u8 cw;
extern void *g_lb_636814;

void *lobby_func_00559EB0(s32 arg0) {
    s32 temp_a0;
    s32 temp_v0;

    temp_v0 = lobby_func_00559C30(arg0);
    temp_a0 = arg0 & 0xFFFF;
    if (temp_a0 >= 0xC350) {
        if (temp_v0 == 0) {
            return g_lb_636814;
        }
        if (temp_a0 == (*(u16 *)((u8 *)(temp_v0) + 0x1E))) {
            if ((*(s8 *)((u8 *)((*(void **)&cw)) + 0xB97)) == 0) {
                goto block_6;
            }
            return Lb_get_quest_str(arg0, 0);
        }
block_6:
        return g_lb_636814;
    }
    return *(*(void ***)((u8 *)(temp_v0) + 0x18));
}

