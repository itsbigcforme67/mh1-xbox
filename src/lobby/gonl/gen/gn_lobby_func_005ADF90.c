/* G online (PC): lobby_func_005ADF90, written from Monster Hunter G's code at lobby 0x5ADF90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;
s32 memset();        /* extern */
extern u8 g_lb_72E068;

void lobby_func_005ADF90(void *arg0) {
    (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
    (*(u8 *)((u8 *)&CnetSys_w + 0x1032)) = 0;
    memset(&g_lb_72E068, 0, 3);
    (*(s32 *)((u8 *)(arg0) + 8)) = 1;
    (*(s32 *)((u8 *)(arg0) + 0xC)) = 1;
    (*(s8 *)((u8 *)(arg0) + 0x23)) = 0;
}

