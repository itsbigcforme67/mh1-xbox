/* G online (PC): tk_logout, written from Monster Hunter G's code at lobby 0x585320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Logout_ShutDown();
s32 cnLBS_LogoutLobbyServer();
s32 cnLBS_ShutDownLobbyServer();
extern u8 COM_R_No_Disconnect;
extern u8 COM_R_No_Logout;
extern u8 cw;
extern u8 game_w;
s32 CpInetGetStatus();                            /* extern */
s32 disconnect();                                 /* extern */

s32 tk_logout(s32 arg0) {
    s32 temp_v0_2;
    s32 var_s0;
    u32 temp_v0;
    u32 temp_v0_3;
    u8 temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_2;

    temp_a1 = (*(u8 *)&COM_R_No_Logout);
    var_s0 = 0;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)&COM_R_No_Logout) = (u8) (temp_a1 + 1);
        break;
    case 1:                                         /* switch 1 */
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x708;
        temp_v0 = arg0 & 0xFF;
        if ((*(u8 *)((u8 *)&game_w + 0x3F)) != 0) {
            switch (temp_v0) {                      /* switch 2 */
            case 0:                                 /* switch 2 */
            case 2:                                 /* switch 2 */
            case 7:                                 /* switch 2 */
                (*(u8 *)&COM_R_No_Logout) = (u8) ((*(u8 *)&COM_R_No_Logout) + 1);
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x36;
                cnLBS_LogoutLobbyServer((s32) CallBack_Logout_ShutDown);
                break;
            case 1:                                 /* switch 2 */
            case 3:                                 /* switch 2 */
            case 4:                                 /* switch 2 */
            case 5:                                 /* switch 2 */
                (*(u8 *)&COM_R_No_Logout) = (u8) ((*(u8 *)&COM_R_No_Logout) + 1);
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x36;
                cnLBS_ShutDownLobbyServer((s32) CallBack_Logout_ShutDown);
                break;
            case 6:                                 /* switch 2 */
                (*(u8 *)&COM_R_No_Logout) = 3U;
                break;
            }
        } else {
            (*(u8 *)&COM_R_No_Logout) = 3U;
        }
        break;
    case 2:                                         /* switch 1 */
        temp_v1 = (*(void **)&cw);
        (*(s32 *)((u8 *)(temp_v1) + 0xBB4)) = (s32) ((*(s32 *)((u8 *)(temp_v1) + 0xBB4)) - 1);
        if (CpInetGetStatus(temp_a1) == 0) {
            if ((*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) < 0) {
                goto block_18;
            }
        } else {
block_18:
            (*(u8 *)&COM_R_No_Logout) = (u8) ((*(u8 *)&COM_R_No_Logout) + 1);
        }
        break;
    case 3:                                         /* switch 1 */
        (*(u8 *)&COM_R_No_Logout) = (u8) (temp_a1 + 1);
        if ((*(u8 *)((u8 *)&game_w + 0x3F)) != 0) {
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x3C;
        } else {
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 1;
        }
        break;
    case 4:                                         /* switch 1 */
        temp_v1_2 = (*(void **)&cw);
        temp_v0_2 = (*(s32 *)((u8 *)(temp_v1_2) + 0xBB4)) - 1;
        (*(s32 *)((u8 *)(temp_v1_2) + 0xBB4)) = temp_v0_2;
        if (temp_v0_2 < 0) {
            (*(s8 *)&COM_R_No_Disconnect) = 0;
            (*(u8 *)&COM_R_No_Logout) = (u8) ((*(u8 *)&COM_R_No_Logout) + 1);
        }
        break;
    case 5:                                         /* switch 1 */
        temp_v0_3 = arg0 & 0xFF;
        switch (temp_v0_3) {                        /* switch 3 */
        case 0:                                     /* switch 3 */
        case 2:                                     /* switch 3 */
        case 4:                                     /* switch 3 */
        case 5:                                     /* switch 3 */
        case 6:                                     /* switch 3 */
        case 7:                                     /* switch 3 */
            var_s0 = 1;
            break;
        case 1:                                     /* switch 3 */
        case 3:                                     /* switch 3 */
            if (disconnect(temp_a1) != 0) {
                var_s0 = 1;
            }
            break;
        }
        break;
    }
    return var_s0;
}

