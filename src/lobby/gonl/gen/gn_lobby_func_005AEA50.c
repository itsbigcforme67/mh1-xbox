/* G online (PC): lobby_func_005AEA50, written from Monster Hunter G's code at lobby 0x5AEA50 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

void lobby_func_005AEA50(s64 arg0) {
    u8 g_frame[0x10] __attribute__((aligned(16)));   /* G's stack frame */
#define sp8 (*(s64 *)(g_frame + 0x8))

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0x23)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0x23)) | 4);
        return;
    }
    (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0x23)) = (u8) ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xEB0))) + 0x23)) | 0x80);
}

