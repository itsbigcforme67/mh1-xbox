/* G online (PC): gonl_cb_FileDownload, written from Monster Hunter G's code at lobby 0x5BFD00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

void gonl_cb_FileDownload(s64 arg0) {
    u8 g_frame[0x10] __attribute__((aligned(16)));   /* G's stack frame */
#define sp8 (*(s64 *)(g_frame + 0x8))

    sp8 = arg0;
    if ((s8) sp8 == 0) {
        (*(s8 *)((u8 *)&CnetSys_w + 0xF9B)) = 1;
        return;
    }
    (*(s8 *)((u8 *)&CnetSys_w + 0xF9B)) = 2;
}

