/* G online (PC): lobby_func_005E2E70, written from Monster Hunter G's code at lobby 0x5E2E70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E2F80();
s32 lobby_func_005E3000();
s32 lobby_func_005E3050();
s32 lobby_func_005E30F0();
s32 lobby_func_005E3120();
s32 lobby_func_005E3250();
void lobby_func_005E2E70(void *arg0) {
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_v1) {
    case 0:
        lobby_func_005E2F80(arg0);
        return;
    case 1:
        lobby_func_005E3000(arg0);
        return;
    case 2:
        lobby_func_005E3050(arg0);
        return;
    case 3:
        lobby_func_005E30F0(arg0);
        return;
    case 4:
        lobby_func_005E3120(arg0);
        return;
    case 5:
        lobby_func_005E3250(arg0);
        /* fallthrough */
    default:
        return;
    }
}

