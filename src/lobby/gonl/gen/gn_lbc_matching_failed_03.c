/* G online (PC): lbc_matching_failed_03, written from Monster Hunter G's code at lobby 0x584210 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
extern u8 cw;
extern u8 pNet;
void lbc_matching_failed_03(void) {
    u8 *temp_a0;

    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
    if ((Fade_busy_ck() & 0xFF) != 1) {
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9B)) + 1);
    }
}

