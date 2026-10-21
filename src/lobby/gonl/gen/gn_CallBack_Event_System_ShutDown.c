/* G online (PC): CallBack_Event_System_ShutDown, written from Monster Hunter G's code at lobby 0x586860 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 cnLBS_Get_ServerMessage();    /* extern */

void CallBack_Event_System_ShutDown(void) {
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) {
        (*(s8 *)((u8 *)(temp_a1) + 0xB76)) = 1;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1);
    }
}

