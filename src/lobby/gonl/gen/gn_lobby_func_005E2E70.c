/* G online (PC): lobby_func_005E2E70, written from Monster Hunter G's code at lobby 0x5E2E70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 jtbl_00646DB0;
s32 lobby_func_005E2F80();             /* extern */
s32 lobby_func_005E3000();             /* extern */
s32 lobby_func_005E3050();             /* extern */
s32 lobby_func_005E30F0();             /* extern */
s32 lobby_func_005E3120();             /* extern */
s32 lobby_func_005E3250();             /* extern */

void lobby_func_005E2E70(void *arg0) {
    u8 temp_v1;

    temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x22));
    switch (temp_v1) {
    case 0:
        lobby_func_005E2F80(&jtbl_00646DB0);
        return;
    case 1:
        lobby_func_005E3000(&jtbl_00646DB0);
        return;
    case 2:
        lobby_func_005E3050(&jtbl_00646DB0);
        return;
    case 3:
        lobby_func_005E30F0(&jtbl_00646DB0);
        return;
    case 4:
        lobby_func_005E3120(&jtbl_00646DB0);
        return;
    case 5:
        lobby_func_005E3250(&jtbl_00646DB0);
        /* fallthrough */
    default:
        return;
    }
}

