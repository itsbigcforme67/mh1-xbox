/* G online (PC): tl_menu_cursor_down, written from Monster Hunter G's code at lobby 0x569290 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 g_main_2EEE70;

void tl_menu_cursor_down(void) {
    s32 temp_a4;
    u8 temp_a0_3;
    u8 temp_a2;
    u8 *temp_a0;
    u8 *temp_a0_2;

    temp_a4 = *(&g_main_2EEE70 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)) * 4));
loop_1:
    temp_a0 = (*(void **)&pNet);
    temp_a2 = (*(u8 *)((u8 *)(temp_a0) + 0x2A));
    if (*(u8 *)(temp_a4 + (temp_a2 * 4)) == 2) {
        (*(u8 *)((u8 *)(temp_a0) + 0x2A)) = 0U;
    } else {
        (*(u8 *)((u8 *)(temp_a0) + 0x2A)) = (u8) (temp_a2 + 1);
    }
    temp_a0_2 = (*(void **)&pNet);
    temp_a0_3 = *(u8 *)(temp_a4 + ((*(u8 *)((u8 *)(temp_a0_2) + 0x2A)) * 4));
    if (temp_a0_3 != 1) {
        if (temp_a0_3 == 2) {
            (*(u8 *)((u8 *)(temp_a0_2) + 0x2A)) = 0U;
        }
        goto loop_1;
    }
}

