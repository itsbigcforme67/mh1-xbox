/* G online (PC): lbc_game_ready_02, written from Monster Hunter G's code at lobby 0x583E00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 net_char_change;
s32 cnLbc_LoadModelWait();              /* extern */
s32 cnLbc_LoadNetModel();           /* extern */

void lbc_game_ready_02(void) {
    s32 temp_a1;
    s32 var_a0;
    u8 temp_a0_2;
    u8 *temp_a0;
    u8 *temp_a0_3;

    temp_a0 = (*(void **)&cw);
    temp_a1 = temp_a0 + 0xB9C;
    temp_a0_2 = (*(u8 *)((u8 *)(temp_a0) + 0xB9C));
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_a0) + 0xB9C)) = (u8) (temp_a0_2 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBB2)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBB3)) = 0;
        var_a0 = 0;
        if ((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB1)) > 0) {
            do {
                var_a0 += 1;
                (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB2)) = (u8) ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB2)) + 1);
            } while (var_a0 < (s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBB1)));
        }
        cnLbc_LoadNetModel(2, temp_a1);
        (*(s8 *)&net_char_change) = 1;
        return;
    case 1:
        if (cnLbc_LoadModelWait(2, temp_a1) != 0) {
            temp_a0_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_3) + 0xB9B)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xB9B)) + 1);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0U;
        }
        return;
    }
}

