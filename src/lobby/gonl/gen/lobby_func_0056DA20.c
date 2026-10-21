/* G online (PC): lobby_func_0056DA20, written from Monster Hunter G's code at lobby 0x56DA20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 tl_member_buff;
s32 memset();        /* extern */
extern u8 g_lb_6B46D0;

void lobby_func_0056DA20(void) {
    (*(u8 *)((u8 *)&tl_member_buff + 0x1383)) = 0;
    memset(&g_lb_6B46D0, 0, 0x110);
}

