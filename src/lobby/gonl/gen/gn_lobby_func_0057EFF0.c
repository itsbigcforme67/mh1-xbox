/* G online (PC): lobby_func_0057EFF0, written from Monster Hunter G's code at lobby 0x57EFF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 cnLBS_Get_ServerMessage(); /* extern */

void lobby_func_0057EFF0(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0xC)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp18 == 0) {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 2;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) = 1;
            return;
        }
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0x63;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1, temp_a1 + 0xBA9);
    }
}

