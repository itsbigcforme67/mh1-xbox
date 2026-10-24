/* G online (PC): lobby_func_00580EC0, written from Monster Hunter G's code at lobby 0x580EC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
void lobby_func_00580EC0(s64 arg0) {
    u8 g_frame[0x10] __attribute__((aligned(16)));   /* G's stack frame */
#define sp8 (*(s64 *)(g_frame + 0x8))
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp8 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0x1A)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp8 == 0) {
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0x63U;
    }
}

