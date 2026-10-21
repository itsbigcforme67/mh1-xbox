/* G online (PC): lobby_func_005E2F80, written from Monster Hunter G's code at lobby 0x5E2F80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * memset();
extern u8 CnetSys_w;

void lobby_func_005E2F80(void *arg0) {
    u32 var_a1;
    u32 var_v1;

    var_a1 = 0;
    (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
    (*(s32 *)((u8 *)(arg0) + 8)) = 1;
    (*(s32 *)((u8 *)(arg0) + 0xC)) = 1;
    (*(s8 *)((u8 *)(arg0) + 0x23)) = 0;
    var_v1 = (*(u32 *)((u8 *)(arg0) + 0x18));
    (*(u8 *)((u8 *)(arg0) + 0x24)) = 0U;
    do {
        if (var_v1 & 1) {
            (*(u8 *)((u8 *)(arg0) + 0x24)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x24)) + 1);
        }
        var_a1 += 1;
        var_v1 = var_v1 >> 1;
    } while (var_a1 < 0x20U);
    (*(s16 *)((u8 *)&CnetSys_w + 0x1032)) = 0;
    (*(s16 *)((u8 *)&CnetSys_w + 0x43B4)) = 0;
    memset(&(*(s32 *)((u8 *)&CnetSys_w + 0x43C8)), 0, 0x2428);
}

