/* G online (PC): lobby_func_005AB9C0, written from Monster Hunter G's code at lobby 0x5AB9C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005ABAD0();
s32 lobby_func_005ABB50();
s32 lobby_func_005ABBA0();
s32 lobby_func_005ABC40();
s32 lobby_func_005ABC70();
s32 lobby_func_005ABDE0();
extern u8 jtbl_00644C20;
void lobby_func_005AB9C0(void *arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_v1) {
    case 0:
        lobby_func_005ABAD0(arg0, (s32) &jtbl_00644C20, arg2, arg3);
        return;
    case 1:
        lobby_func_005ABB50(arg0, (s32) &jtbl_00644C20, arg2, arg3);
        return;
    case 2:
        lobby_func_005ABBA0(arg0, (s32) &jtbl_00644C20, arg2, arg3);
        return;
    case 3:
        lobby_func_005ABC40(arg0, (s32) &jtbl_00644C20, arg2, arg3);
        return;
    case 4:
        lobby_func_005ABC70(arg0, (s32) &jtbl_00644C20, arg2, arg3);
        return;
    case 5:
        lobby_func_005ABDE0(arg0, (s32) &jtbl_00644C20, arg2, arg3);
        /* fallthrough */
    default:
        return;
    }
}

