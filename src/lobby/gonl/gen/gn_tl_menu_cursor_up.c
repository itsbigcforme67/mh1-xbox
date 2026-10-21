/* G online (PC): tl_menu_cursor_up, written from Monster Hunter G's code at lobby 0x5691F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 g_main_2EEE70;

void tl_menu_cursor_up(void) {
    s32 var_t0;
    u8 temp_a0_2;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_t1;
    u8 *var_a3;

    temp_a1 = (*(void **)&pNet);
    temp_t1 = (*(s32 *)((u8 *)&g_main_2EEE70 + ((*(u8 *)((u8 *)(temp_a1) + 0x29)) * 4)));
    (*(s16 *)((u8 *)(temp_a1) + 0x20)) = 0;
    do {
        temp_a0 = (*(void **)&pNet);
        temp_a0_2 = (*(u8 *)((u8 *)(temp_a0) + 0x2A));
        var_t0 = 0;
        if (temp_a0_2 == 0) {
            var_a3 = temp_t1;
loop_3:
            if ((*(u8 *)((u8 *)(var_a3) + 4)) != 2) {
                var_t0 += 1;
                var_a3 += 4;
                if (var_t0 >= 0x14) {

                } else {
                    goto loop_3;
                }
            }
            (*(u8 *)((u8 *)(temp_a0) + 0x2A)) = (u8) var_t0;
        } else {
            (*(u8 *)((u8 *)(temp_a0) + 0x2A)) = (u8) (temp_a0_2 - 1);
        }
    } while (*(temp_t1 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2A)) * 4)) != 1);
}

