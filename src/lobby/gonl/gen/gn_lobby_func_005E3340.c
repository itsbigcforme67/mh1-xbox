/* G online (PC): lobby_func_005E3340, written from Monster Hunter G's code at lobby 0x5E3340 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

void lobby_func_005E3340(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s64 *)(g_frame + 0x10))
#define sp18 (*(s8 *)(g_frame + 0x18))
#define sp19 (*(s8 *)(g_frame + 0x19))

    sp10 = arg0;
    if ((s8) sp10 == 0) {
        (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 0x23)) = 1;
        sp18 = 2;
        sp19 = 0xB;
        (*(s32 (**)(s64, s8 *))((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 4))(*(s64 *)&sp18, &sp18);
        return;
    }
    (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&CnetSys_w + 0xDC0))) + 0x23)) = 2;
}

