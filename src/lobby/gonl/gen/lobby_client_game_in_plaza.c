/* G online (PC): lobby_client_game_in_plaza, written from Monster Hunter G's code at lobby 0x57E150 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 MH_lobbyClear();          /* extern */
s32 cnLBS_Get_ServerMessage(); /* extern */

void lobby_client_game_in_plaza(s64 arg0) {
    s64 sp18;
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;
    u8 *temp_a2;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && (temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xBA9)), temp_a2 = temp_a1 + 0xBA9, (temp_a0 == 7))) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp18 == 0) {
            MH_lobbyClear(temp_a0, temp_a1, temp_a2);
            temp_a0_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0xB9C)) + 1);
            return;
        }
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1, temp_a2);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 0x63U;
    }
}

