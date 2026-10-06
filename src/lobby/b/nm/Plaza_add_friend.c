#include "lobby_a.h"
extern char my_user_id[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
s32 Plaza_add_friend(s32 arg0) {
    s32 temp_a3;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 var_v0;
    u8 temp_a0;
    u8 temp_v0_3;
    int temp_a1;
    int temp_a2;
    int temp_s1;
    int temp_v1;
    int temp_v1_2;

    temp_a3 = Get_sw2(0) & 0xFFFF;
    F(s8, pNet, 0xC) = 1;
    temp_a1 = (int)pNet;
    temp_a0 = F(u8, temp_a1, 5);
    temp_a2 = temp_a1 + 5;
    switch (temp_a0) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, temp_a1, 5) = (u8) (temp_a0 + 1);
        SetDialogData(0x1D, 2, temp_a2, temp_a3);
        SetDialogYesNo(0);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        *(s8 *)0x3F36AB = 0;
    default:                                        /* switch 1 */
block_55:
        return 2;
    case 1:                                         /* switch 1 */
        temp_v0 = Lb_select(temp_a0);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1 = (int)pNet;
            F(u8, temp_v1, 5) = (u8) (F(u8, temp_v1, 5) + 1);
            if (memcmp(arg0, &my_user_id, 8) == 0) {
                F(u8, pNet, 5) = 6U;
                SetDialogData(0x1E, 3);
            } else if (net_Check_FriendData(&Friend_data, 0x32, arg0) == -1) {
                if (net_Check_FriendFree(&Friend_data, 0x32) == -1) {
                    F(u8, pNet, 5) = 6U;
                    SetDialogData(0x24, 3);
                } else {
                    SetDialogData(0x23, 2);
                    SetDialogYesNo(0);
                }
            } else {
                SetDialogData(0x22, 2);
                SetDialogYesNo(0);
            }
            break;
        case 3:                                     /* switch 2 */
            F(u8, pNet, 5) = 6U;
            SetDialogData(0x1F, 3);
            break;
        }
        goto block_55;
    case 2:                                         /* switch 1 */
        temp_v0_2 = Lb_select(temp_a0);
        switch (temp_v0_2) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            var_v0 = net_Check_FriendData(&Friend_data, 0x32, arg0);
            if (var_v0 == -1) {
                var_v0 = net_Check_FriendFree(&Friend_data, 0x32);
                if (var_v0 == -1) {
                    var_v0 = 0;
                }
                F(s8, pNet, 0xD) = 0;
            } else {
                F(s8, pNet, 0xD) = 1;
            }
            temp_s1 = (int)&Friend_data + (var_v0 * 0x30);
            memset(temp_s1, 0, 0x30);
            memcpy(temp_s1, arg0, 9);
            memcpy(temp_s1 + 8, arg0 + 8, 0x11);
            str_pause(0, 1);
            str_pause(1, 1);
            F(u8, pNet, 0x12) = 3U;
            cpn_PutCNData();
            SaveNetFile_init();
            temp_v1_2 = (int)pNet;
            F(u8, temp_v1_2, 5) = (u8) (F(u8, temp_v1_2, 5) + 1);
            F(s8, (u8 *)cw, 0x2C08) = 0;
            *(u8 *)0x3F36AB = 0;
            break;
        case 3:                                     /* switch 3 */
            F(u8, pNet, 5) = 0U;
            break;
        }
        goto block_55;
    case 3:                                         /* switch 1 */
        temp_v0_3 = F(u8, temp_a1, 0x12);
        if (temp_v0_3 == 0) {
            temp_v0_4 = SaveNetFile_ForLobby(temp_a0);
            switch (temp_v0_4) {                    /* switch 4; irregular */
            case -1:                                /* switch 4 */
                str_pause(0, 0);
                str_pause(1, 0);
                if (F(u8, (u8 *)cw, 0x35D5) != 0) {
                    str_fadein_vol(0, 0x1E, (*(int *)((u8 *)&D_32D471 + (game_w.stage * 2))));
                }
                F(s8, (u8 *)cw, 0x2C08) = 1;
                *(u8 *)0x3F36AB = 1;
                F(u8, pNet, 5) = 0U;
                return 0;
            case 1:                                 /* switch 4 */
                str_pause(0, 0);
                str_pause(1, 0);
                if (F(u8, (u8 *)cw, 0x35D5) != 0) {
                    str_fadein_vol(0, 0x1E, (*(int *)((u8 *)&D_32D471 + (game_w.stage * 2))));
                }
                F(u8, pNet, 5) = 0U;
                F(s8, (u8 *)cw, 0x2C08) = 1;
                *(u8 *)0x3F36AB = 1;
                return 0;
            }
        } else {
            F(u8, temp_a1, 0x12) = (u8) (temp_v0_3 - 1);
            goto block_55;
        }
        break;
    case 4:                                         /* switch 1 */
        F(u8, temp_a1, 5) = (u8) (temp_a0 + 1);
        goto block_55;
    case 5:                                         /* switch 1 */
        if (temp_a3 & 0xFFFF & 0x20) {
            F(u8, temp_a1, 5) = 0U;
            cnWrap_SoundRequest(0);
            F(s8, (u8 *)cw, 0x2C08) = 1;
            *(u8 *)0x3F36AB = 1;
            return 0;
        }
        goto block_55;
    case 6:                                         /* switch 1 */
        if (temp_a3 & 0xFFFF & 0x20) {
            F(u8, temp_a1, 5) = 0U;
            cnWrap_SoundRequest(0);
            F(s8, (u8 *)cw, 0x2C08) = 1;
            *(u8 *)0x3F36AB = 1;
            return 1;
        }
        goto block_55;
    }
}
