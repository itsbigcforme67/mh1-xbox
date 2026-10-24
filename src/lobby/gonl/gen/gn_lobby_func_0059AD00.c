/* G online (PC): lobby_func_0059AD00, written from Monster Hunter G's code at lobby 0x59AD00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lb_send_dataTU(); /* extern */

void lobby_func_0059AD00(s32 arg0, s8 arg1) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1F (*(s8 *)(g_frame + 0x1F))

    sp1F = arg1;
    lb_send_dataTU(1, 3, &sp1F, arg0);
}

