/* G online (PC): lbc_admin_message_02, written from Monster Hunter G's code at lobby 0x585080 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 lb_prim;
void lbc_admin_message_02(void) {
    (*(s32 *)((u8 *)&lb_prim + 0x74)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBC4)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xED6)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xED7)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xEDE)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xEDA)) = 0;
}

