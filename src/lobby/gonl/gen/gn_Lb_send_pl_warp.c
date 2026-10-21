/* G online (PC): Lb_send_pl_warp, written from Monster Hunter G's code at lobby 0x59A7A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lb_send_data();
void Lb_send_pl_warp(void *arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s16 *)(g_frame + 0x18))
#define sp1A (*(s16 *)(g_frame + 0x1A))

    sp18 = (s16) (*(f32 *)((u8 *)(arg0) + 0xAC));
    sp1A = (s16) (*(f32 *)((u8 *)(arg0) + 0xB4));
    lb_send_data(4, 2, (s32) &sp18);
}

