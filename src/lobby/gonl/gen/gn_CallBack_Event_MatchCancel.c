/* G online (PC): CallBack_Event_MatchCancel, written from Monster Hunter G's code at lobby 0x586930 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_ServerMessage();
extern u8 cw;
void CallBack_Event_MatchCancel(void) {
    u8 temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
    if ((temp_a0 != 5) && (temp_a0 == 4)) {
        cnLBS_Get_ServerMessage(temp_a1 + 0xF75D);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB75)) = 2;
    }
}

