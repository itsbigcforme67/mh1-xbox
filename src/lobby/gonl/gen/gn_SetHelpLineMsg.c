/* G online (PC): SetHelpLineMsg, written from Monster Hunter G's code at lobby 0x59F720 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_0059F6F0();
extern s32 g_lb_636C10;
extern s32 helpLineStr;

void SetHelpLineMsg(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;

    temp_a0 = arg0 * 4;
    helpLineStr = (*(s32 *)((u8 *)&g_lb_636C10 + temp_a0));
    lobby_func_0059F6F0(temp_a0, arg1, arg2, arg3);
}

