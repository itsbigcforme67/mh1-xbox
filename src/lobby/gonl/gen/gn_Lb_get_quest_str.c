/* G online (PC): Lb_get_quest_str, written from Monster Hunter G's code at lobby 0x559D50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559C30();
s32 lobby_func_00559CE0();
extern u8 quest_title;

void *Lb_get_quest_str(s32 arg0, s32 arg1) {
    u8 *temp_a1;

    lobby_func_00559C30(arg0);
    lobby_func_00559CE0(arg0);
    temp_a1 = (u8 *)&(*(u8 *)((u8 *)&quest_title + 0x30)) + (((s64) (M2C_ERROR(/* Read from unset register $v0 */) << 0x38) >> 0x38) << 9);
    return *((*(s32 *)((u8 *)(M2C_ERROR(/* Read from unset register $v0 */)) + 0x18)) + temp_a1 + (arg1 * 4)) + temp_a1;
}

