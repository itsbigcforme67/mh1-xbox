/* G online (PC): lobby_client_admin_message, written from Monster Hunter G's code at lobby 0x584CE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 lbc_admin_message_jmp_3260;

s32 lobby_client_admin_message(void) {
    u8 temp_v1;
    u8 *temp_a0;
    u8 *temp_a0_2;

    temp_a0 = (*(void **)&cw);
    if ((*(u8 *)((u8 *)(temp_a0) + 0xBC4)) == 0) {
        return 0;
    }
    if ((*(s8 *)((u8 *)(temp_a0) + 0xB70)) == 0) {
        return 0;
    }
    temp_v1 = (*(u8 *)((u8 *)(temp_a0) + 0xB99));
    switch (temp_v1) {                              /* irregular */
    case 0:
        return 0;
    case 5:
        return 0;
    case 4:
        return 0;
    default:
        if ((*(s8 *)((u8 *)(temp_a0) + 0xB74)) != 0) {
            return 0;
        }
        if ((*(s8 *)((u8 *)(temp_a0) + 0xB98)) != 0) {
            (*(s8 *)((u8 *)(temp_a0) + 0xB98)) = 0;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xEE0)) = 0U;
        }
        temp_a0_2 = (*(void **)&cw);
        if (((*(u8 *)((u8 *)(temp_a0_2) + 0xEE0)) != 0) && ((*(u8 *)((u8 *)(temp_a0_2) + 0xBC4)) != 2)) {
            return 0;
        }
        if ((s32) (*(u8 *)((u8 *)(temp_a0_2) + 0xED6)) >= 3) {
            (*(u8 *)((u8 *)(temp_a0_2) + 0xED6)) = 2U;
        }
        *(&lbc_admin_message_jmp_3260 + ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xED6)) * 4))(temp_a0_2);
        return 1;
    }
}

