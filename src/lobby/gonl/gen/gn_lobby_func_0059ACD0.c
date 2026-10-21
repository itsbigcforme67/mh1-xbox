/* G online (PC): lobby_func_0059ACD0, written from Monster Hunter G's code at lobby 0x59ACD0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lb_send_data();
void lobby_func_0059ACD0(s32 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1E (*(s16 *)(g_frame + 0x1E))

    sp1E = arg0 & 0xFF;
    lb_send_data(2, 3, &sp1E);
}

