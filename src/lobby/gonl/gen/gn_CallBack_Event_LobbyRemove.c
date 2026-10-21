/* G online (PC): CallBack_Event_LobbyRemove, written from Monster Hunter G's code at lobby 0x586BF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_ServerMessage();
extern u8 cw;
void CallBack_Event_LobbyRemove(void) {
    u8 temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
    if ((temp_a0 != 5) && (temp_a0 == 3) && ((*(s8 *)((u8 *)(temp_a1) + 0xB72)) == 0)) {
        (*(s8 *)((u8 *)(temp_a1) + 0xB72)) = 1;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
    }
}

