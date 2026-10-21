/* G online (PC): CallBack_Result_Lobby_SetRoomRule, written from Monster Hunter G's code at lobby 0x5823A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
void CallBack_Result_Lobby_SetRoomRule(void) {
    u8 *temp_a1;
    u8 *temp_a1_2;

    temp_a1 = (*(void **)&cw);
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0x21)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        temp_a1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a1_2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a1_2) + 0xB9D)) + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) = 1;
    }
}

