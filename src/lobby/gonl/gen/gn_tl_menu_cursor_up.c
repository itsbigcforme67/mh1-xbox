/* G online (PC): tl_menu_cursor_up, written from Monster Hunter G's code at lobby 0x5691F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern s32 plazaMenuTbl;

void tl_menu_cursor_up(void) {
    s32 var_a4;
    u8 temp_a0_2;
    u8 *temp_a0;
    u8 *temp_a1;
    u8 *temp_a5;
    u8 *var_a3;

    temp_a1 = (*(void **)&pNet);
    temp_a5 = (*(s32 *)((u8 *)&plazaMenuTbl + ((*(u8 *)((u8 *)(temp_a1) + 0x29)) * 4)));
    (*(s16 *)((u8 *)(temp_a1) + 0x20)) = 0;
    do {
        temp_a0 = (*(void **)&pNet);
        temp_a0_2 = (*(u8 *)((u8 *)(temp_a0) + 0x2A));
        var_a4 = 0;
        if (temp_a0_2 == 0) {
            var_a3 = temp_a5;
loop_3:
            if ((*(u8 *)((u8 *)(var_a3) + 4)) != 2) {
                var_a4 += 1;
                var_a3 += 4;
                if (var_a4 >= 0x14) {

                } else {
                    goto loop_3;
                }
            }
            (*(u8 *)((u8 *)(temp_a0) + 0x2A)) = (u8) var_a4;
        } else {
            (*(u8 *)((u8 *)(temp_a0) + 0x2A)) = (u8) (temp_a0_2 - 1);
        }
    } while (*(temp_a5 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2A)) * 4)) != 1);
}

