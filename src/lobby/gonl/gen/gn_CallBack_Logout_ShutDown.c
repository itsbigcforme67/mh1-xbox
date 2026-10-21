/* G online (PC): CallBack_Logout_ShutDown, written from Monster Hunter G's code at lobby 0x585530 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 COM_R_No_Logout;
extern u8 cw;
void CallBack_Logout_ShutDown(void) {
    u8 temp_a0_2;
    u8 *temp_a0;

    temp_a0 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_a0) + 0xBA9)) == 0x36) {
        (*(u8 *)((u8 *)(temp_a0) + 0xBA9)) = 0U;
        temp_a0_2 = (*(u8 *)&COM_R_No_Logout);
        if (temp_a0_2 == 2) {
            (*(u8 *)&COM_R_No_Logout) = (u8) (temp_a0_2 + 1);
        }
    }
}

