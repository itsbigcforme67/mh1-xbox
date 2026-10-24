/* G online (PC): CallBack_Event_LobbyFull, written from Monster Hunter G's code at lobby 0x5868B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_ServerMessage();
extern u8 cw;
void CallBack_Event_LobbyFull(void) {
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) {
        (*(s8 *)((u8 *)(temp_a1) + 0xB76)) = 2;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
    }
}

