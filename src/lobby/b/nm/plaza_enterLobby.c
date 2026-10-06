#include "lobby_a.h"
extern char ClassInfo[];
extern char D_3A1622[];
extern char tl_member_buff[];
extern char ClassInfo[];
s32 plaza_enterLobby(void) {
    s32 temp_a1_2;
    s32 temp_s0;
    s32 temp_v0_3;
    s32 temp_v0_4;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a1_3;
    u8 temp_v1_3;
    u8 temp_v1_4;
    u8 var_v0;
    u8 var_v0_2;
    int temp_a1;
    int temp_a2;
    int temp_a2_2;
    int temp_a2_3;
    int temp_v0;
    int temp_v0_2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_5;
    int temp_v1_6;
    int temp_v1_7;

    temp_a1 = (int)pNet;
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a0 = F(u8, temp_a1, 3);
    temp_a2 = temp_a1 + 3;
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, temp_a1, 3) = (u8) (temp_a0 + 1);
    default:                                        /* switch 1 */
block_48:
        return 2;
    case 1:                                         /* switch 1 */
        temp_a1_2 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0);
        if (temp_a1_2 & 0x20) {
            F(u8, pNet, 3) = 4U;
            temp_a2_2 = (int)pNet;
            F(s8, &ClassInfo, 4) = (s8) (F(u8, temp_a2_2, 0xA) + 1);
            F(u8, (u8 *)cw, 0x2C41) = (u8) F(u8, temp_a2_2, 0xA);
            F(s8, (u8 *)cw, 0x2C35) = 0;
            SetDialogData(0x15, 5);
            F(s8, pNet, 0xC) = 1;
            cnWrap_SoundRequest(0);
            goto block_48;
        }
        if (temp_a1_2 & 0x40) {
            return 3;
        }
        if (temp_a1_2 & 0x2000) {
            temp_v1 = (int)pNet;
            temp_a0_2 = F(u8, temp_v1, 0xA);
            if ((temp_a0_2 % 7) != 0) {
                var_v0 = temp_a0_2 - 1;
            } else {
                var_v0 = (((F(u8, temp_v1, 0xA) / 7) + (temp_a0_2 >> 0x1F)) * 7) + 6;
            }
            F(u8, temp_v1, 0xA) = var_v0;
            cnWrap_SoundRequest(1, temp_v1 + 0xA);
        } else if (temp_a1_2 & 0x1000) {
            temp_v1_2 = (int)pNet;
            F(u8, temp_v1_2, 0xA) = (u8) (F(u8, temp_v1_2, 0xA) + 1);
            temp_v0 = (int)pNet;
            temp_v1_3 = F(u8, temp_v0, 0xA);
            if (temp_v1_3 < 0xE) {
                if ((temp_v1_3 % 7) == 0) {
                    goto block_19;
                }
            } else {
block_19:
                F(u8, temp_v0, 0xA) = (u8) (F(u8, temp_v0, 0xA) - 7);
            }
            cnWrap_SoundRequest(1, (u8 *) temp_a1_2);
        } else if (temp_a1_2 & 0xC00) {
            temp_v0_2 = (int)pNet;
            temp_v1_4 = F(u8, temp_v0_2, 0xA);
            var_v0_2 = temp_v1_4 - 7;
            if (temp_v1_4 >= 7) {

            } else {
                var_v0_2 = temp_v1_4 + 7;
            }
            F(u8, temp_v0_2, 0xA) = var_v0_2;
            cnWrap_SoundRequest(1, (u8 *) temp_a1_2);
        } else if (temp_a1_2 & 0x100) {
            temp_a2_3 = (int)pNet;
            temp_a1_3 = F(u8, temp_a2_3, 0xA);
            F(s8, temp_a2_3, 6) = (s8) (*(int *)((u8 *)&D_3A1622 + (temp_a1_3 * 0x15C)));
            cnWrap_SoundRequest(6, (u8 *) temp_a1_3);
            memset(&tl_member_buff, 0, 0x17E0);
            temp_v1_5 = (int)pNet;
            F(u8, temp_v1_5, 3) = (u8) (F(u8, temp_v1_5, 3) + 1);
        }
        goto block_48;
    case 2:                                         /* switch 1 */
        if (Lbs_GetLobbyMemberList(F(u8, temp_a1, 0xA)) == 1) {
            temp_v1_6 = (int)pNet;
            F(u8, temp_v1_6, 3) = (u8) (F(u8, temp_v1_6, 3) + 1);
        }
        goto block_48;
    case 3:                                         /* switch 1 */
        F(s16, pNet, 0x28) = Get_sw_on2(0);
        if (temp_s0 & 0xFFFF & 0x40) {
            F(u8, pNet, 3) = 1U;
            cnWrap_SoundRequest(3);
        }
        goto block_48;
    case 4:                                         /* switch 1 */
        F(s8, temp_a1, 0xC) = 1;
        temp_v0_3 = Lbs_request_enter_lobby(temp_a0);
        switch (temp_v0_3) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1_7 = (int)pNet;
            F(u8, temp_v1_7, 3) = (u8) (F(u8, temp_v1_7, 3) + 1);
            break;
        case 1:                                     /* switch 2 */
            F(u8, pNet, 3) = 6U;
            F(s16, (u8 *)cw, 0x30B6) = 0;
            F(s8, &ClassInfo, 4) = 0;
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            break;
        }
        goto block_48;
    case 5:                                         /* switch 1 */
        F(s8, temp_a1, 0xC) = 1;
        temp_v0_4 = Lbc_DownloadQuest(temp_a0);
        switch (temp_v0_4) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            fade_set(0xA);
            To_EnterLobby();
            str_stop(0);
            str_stop(1);
            return 0;
        case 1:                                     /* switch 3 */
            F(u8, pNet, 3) = 6U;
            SetDialogData(0xE, 5);
            goto block_48;
        }
        break;
    case 6:                                         /* switch 1 */
        F(s8, temp_a1, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(s8, (u8 *)cw, 0x2C32) = 0;
            F(s8, (u8 *)cw, 0x2C33) = 0;
            F(s8, (u8 *)cw, 0x2C34) = 0;
            return 1;
        }
        goto block_48;
    case 7:                                         /* switch 1 */
        F(s8, temp_a1, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, pNet, 3) = 0U;
        }
        goto block_48;
    }
}
