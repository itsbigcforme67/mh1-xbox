/* G online (PC): lobby_func_005AB9C0, written from Monster Hunter G's code at lobby 0x5AB9C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 jtbl_00644C20;
s32 lobby_func_005ABAD0();             /* extern */
s32 lobby_func_005ABB50();             /* extern */
s32 lobby_func_005ABBA0();             /* extern */
s32 lobby_func_005ABC40();             /* extern */
s32 lobby_func_005ABC70();             /* extern */
s32 lobby_func_005ABDE0();             /* extern */

void lobby_func_005AB9C0(void *arg0) {
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_v1) {
    case 0:
        lobby_func_005ABAD0(&jtbl_00644C20);
        return;
    case 1:
        lobby_func_005ABB50(&jtbl_00644C20);
        return;
    case 2:
        lobby_func_005ABBA0(&jtbl_00644C20);
        return;
    case 3:
        lobby_func_005ABC40(&jtbl_00644C20);
        return;
    case 4:
        lobby_func_005ABC70(&jtbl_00644C20);
        return;
    case 5:
        lobby_func_005ABDE0(&jtbl_00644C20);
        /* fallthrough */
    default:
        return;
    }
}

