/* G online (PC): lobby_func_005BFEA0, written from Monster Hunter G's code at lobby 0x5BFEA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005BFF80();
s32 lobby_func_005BFFC0();
s32 lobby_func_005C0040();
void lobby_func_005BFEA0(void *arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    u8 temp_a1;

    temp_a1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_a1) {                              /* irregular */
    case 0:
        lobby_func_005BFF80(arg0, (s32) temp_a1, arg2, arg3);
        return;
    case 1:
        lobby_func_005BFFC0(arg0, (s32) temp_a1, arg2, arg3);
        return;
    case 2:
        lobby_func_005C0040(arg0, (s32) temp_a1, arg2, arg3);
        return;
    }
}

