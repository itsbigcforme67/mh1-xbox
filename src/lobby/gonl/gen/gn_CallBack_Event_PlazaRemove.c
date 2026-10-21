/* G online (PC): CallBack_Event_PlazaRemove, written from Monster Hunter G's code at lobby 0x586B90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 cnLBS_Get_ServerMessage();    /* extern */

void CallBack_Event_PlazaRemove(void) {
    u8 temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
    if ((temp_a0 != 5) && (temp_a0 == 2) && ((*(s8 *)((u8 *)(temp_a1) + 0xB71)) == 0)) {
        (*(s8 *)((u8 *)(temp_a1) + 0xB71)) = 1;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a1);
    }
}

