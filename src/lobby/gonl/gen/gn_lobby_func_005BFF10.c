/* G online (PC): lobby_func_005BFF10, written from Monster Hunter G's code at lobby 0x5BFF10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void lobby_func_005BFF10(void *arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s8 *)(g_frame + 0x18))

    (*(s8 *)((u8 *)(arg0) + 0x20)) = 0;
    (*(s8 *)((u8 *)(arg0) + 0x21)) = 0;
    sp18 = 0;
    (*(s32 (**)(s64, s8 *))((u8 *)(arg0) + 4))(*(s64 *)&sp18, &sp18);
}

