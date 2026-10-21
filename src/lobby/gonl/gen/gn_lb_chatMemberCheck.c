/* G online (PC): lb_chatMemberCheck, written from Monster Hunter G's code at lobby 0x57FA20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_LobbyEntry();
s32 Lbs_request_enter_lobby();
s32 MH_lobbyClear();
extern u8 cw;
s32 lb_chatMemberCheck(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v0_2;
    u8 temp_v1_2;
    u8 *temp_v1;
    u8 *temp_v1_3;

    temp_v1 = (*(void **)&cw);
    temp_a0 = temp_v1 + 0xB9E;
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9E));
    switch (temp_v1_2) {                            /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_v1) + 0xB9E)) = (u8) (temp_v1_2 + 1);
        MH_lobbyClear(temp_a0, arg1, arg2, arg3);
        /* fallthrough */
    case 1:                                         /* switch 1 */
        temp_v0 = Lbs_request_enter_lobby(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_3) + 0xB9E)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9E)) + 1);
        default:                                    /* switch 1 */
            return 2;
        case 1:                                     /* switch 2 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 1;
        }
        break;
    case 2:                                         /* switch 1 */
        temp_v0_2 = CallBack_Result_Plaza_LobbyEntry(temp_a0, arg1, arg2, arg3);
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 0;
        case 1:                                     /* switch 3 */
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 1;
        }
        break;
    }
}

