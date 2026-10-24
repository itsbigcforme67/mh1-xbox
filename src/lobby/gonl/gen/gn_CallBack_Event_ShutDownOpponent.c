/* G online (PC): CallBack_Event_ShutDownOpponent, written from Monster Hunter G's code at lobby 0x586900 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
void CallBack_Event_ShutDownOpponent(void) {
    u8 temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
    if ((temp_a0 != 5) && (temp_a0 == 4)) {
        (*(s8 *)((u8 *)(temp_a1) + 0xB75)) = 1;
    }
}

