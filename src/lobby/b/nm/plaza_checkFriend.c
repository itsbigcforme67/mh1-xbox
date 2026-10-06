#include "lobby_a.h"
extern char tl_member_buff[];
extern char Friend_data[];
extern char tl_member_buff[];
extern char Friend_data[];
extern char Friend_data[];
extern char tl_member_buff[];
extern char tl_member_buff[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char Friend_data[];
extern char tl_member_buff[];
extern char tl_member_buff[];
extern char tl_member_buff[];
s32 plaza_checkFriend(void) {
    int var_s0;
    int var_s0_2;
    s16 temp_v0;
    s16 temp_v0_2;
    s16 temp_v1_18;
    s16 temp_v1_8;
    s32 temp_a0_7;
    s32 temp_a1_3;
    s32 temp_s0;
    s32 temp_v0_14;
    s32 temp_v0_15;
    s32 temp_v0_16;
    s32 temp_v0_20;
    s32 temp_v0_8;
    s32 temp_v1_2;
    s32 temp_v1_4;
    s32 var_a0;
    s32 var_a1;
    s32 var_s1;
    s32 var_s1_2;
    s8 temp_v0_13;
    u8 temp_a1;
    u8 temp_a2_2;
    u8 temp_a2_3;
    u8 temp_a2_4;
    u8 temp_a3;
    u8 temp_a3_2;
    u8 temp_v0_10;
    u8 temp_v0_11;
    u8 temp_v0_18;
    u8 temp_v0_19;
    u8 temp_v0_4;
    u8 temp_v0_6;
    u8 var_s0_3;
    u8 var_v0;
    int temp_a0;
    int temp_a0_10;
    int temp_a0_11;
    int temp_a0_12;
    int temp_a0_13;
    int temp_a0_2;
    int temp_a0_3;
    int temp_a0_4;
    int temp_a0_5;
    int temp_a0_6;
    int temp_a0_8;
    int temp_a0_9;
    int temp_a1_2;
    int temp_a1_4;
    int temp_a2;
    int temp_s0_2;
    int temp_s0_3;
    int temp_v0_12;
    int temp_v0_17;
    int temp_v0_3;
    int temp_v0_5;
    int temp_v0_7;
    int temp_v0_9;
    int temp_v1;
    int temp_v1_10;
    int temp_v1_11;
    int temp_v1_12;
    int temp_v1_13;
    int temp_v1_14;
    int temp_v1_15;
    int temp_v1_16;
    int temp_v1_17;
    int temp_v1_19;
    int temp_v1_3;
    int temp_v1_5;
    int temp_v1_6;
    int temp_v1_7;
    int temp_v1_9;
    int var_a2;
    int var_s1_3;

    temp_a0 = (int)pNet;
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a1 = F(u8, temp_a0, 3);
    temp_a2 = temp_a0 + 3;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, temp_a0, 3) = (u8) (temp_a1 + 1);
        F(u8, pNet, 0xA) = 0U;
        F(s8, pNet, 4) = 0;
        F(s16, pNet, 0x24) = 0;
        F(u8, pNet, 0xD) = 1U;
        get_friend_page_num(pNet, temp_a1, temp_a2);
        memset(&tl_member_buff, 0, 0x17E0);
        /* fallthrough */
    case 1:                                         /* switch 1 */
        if (getFriendNow(pNet, 0, 7) != 0) {

        } else {
            temp_v1 = (int)pNet;
            F(u8, temp_v1, 3) = (u8) (F(u8, temp_v1, 3) + 1);
        }
    default:                                        /* switch 1 */
block_153:
        return 2;
    case 2:                                         /* switch 1 */
        F(s16, pNet, 0x28) = Get_sw_on2(0);
        temp_v1_2 = temp_s0 & 0xFFFF;
        if (net_Check_FriendSuu(&Friend_data, 0x32) == 0) {
            if (temp_v1_2 & 0x40) {
                temp_v1_3 = (int)pNet;
                if (F(u8, temp_v1_3, 0xD) == 1) {
                    return 3;
                }
                F(u8, temp_v1_3, 3) = 0xBU;
                F(s8, (u8 *)cw, 0x2C08) = 0;
                *(s8 *)0x3F36AB = 0;
                if (F(u8, (u8 *)cw, 0x35D5) != 0) {
                    str_stop(1, 1);
                    str_pause(0, 1);
                } else {
                    str_pause(0, 1);
                    str_pause(1, 1);
                    str_stop(1);
                }
                F(u8, pNet, 0x12) = 3U;
                cpn_PutCNData();
                SaveNetFile_init();
                goto block_153;
            }
            if (temp_v1_2 & 0x2A0) {
                cnWrap_SoundRequest(7);
            }
            goto block_153;
        }
        temp_v1_4 = temp_s0 & 0xFFFF;
        if (temp_v1_4 & 0x40) {
            if (F(u8, pNet, 0xD) == 1) {
                return 3;
            }
            cnWrap_SoundRequest(3);
            F(u8, pNet, 3) = 0xBU;
            F(s8, (u8 *)cw, 0x2C08) = 0;
            *(u8 *)0x3F36AB = 0;
            str_pause(0, 1);
            str_pause(1, 1);
            F(u8, pNet, 0x12) = 3U;
            cpn_PutCNData();
            SaveNetFile_init();
            goto block_153;
        }
        if (temp_v1_4 & 0x20) {
            temp_a1_2 = (int)pNet;
            if ((*(s8 *)((u8 *)&tl_member_buff + 0x280 + (F(u8, temp_a1_2, 0xA) * 0x2FC))) != 0) {
                F(u8, temp_a1_2, 3) = (u8) (F(u8, temp_a1_2, 3) + 1);
                temp_a0_2 = (int)pNet;
                F(u8, temp_a0_2, 6) = (u8) F(u8, temp_a0_2, 0xA);
                F(u8, pNet, 0x12) = 0U;
                temp_a0_3 = (int)pNet;
                temp_a2_2 = F(u8, temp_a0_3, 6);
                strcpy((u8 *)cw + 0x2F80, (int)&Friend_data + ((temp_a2_2 + (F(s16, temp_a0_3, 0x24) * 7)) * 0x30), temp_a2_2);
                memset((u8 *)cw + 0x2B9C, 0, 0x62);
                F(s8, (u8 *)cw, 0x2F99) = 0;
                cnWrap_SoundRequest(0);
            } else {
                cnWrap_SoundRequest(7);
            }
        } else if (temp_v1_4 & 0x80) {
            F(u8, pNet, 3) = 5U;
            temp_a0_4 = (int)pNet;
            F(u8, temp_a0_4, 6) = (u8) F(u8, temp_a0_4, 0xA);
            F(u8, pNet, 0xA) = 0U;
            temp_a0_5 = (int)pNet;
            temp_a2_3 = F(u8, temp_a0_5, 6);
            temp_s0_2 = (int)&Friend_data + ((temp_a2_3 + (F(s16, temp_a0_5, 0x24) * 7)) * 0x30);
            strcpy((u8 *)cw + 0x2F80);
            strcpy((u8 *)cw + 0x2F88, temp_s0_2 + 8);
            F(s8, (u8 *)cw, 0x2F99) = 0;
            cnWrap_SoundRequest(6);
        } else if (temp_v1_4 & 0x200) {
            F(u8, pNet, 3) = 0xAU;
            temp_v1_5 = (int)pNet;
            F(u8, temp_v1_5, 6) = (u8) F(u8, temp_v1_5, 0xA);
            SetDialogData(0x16, 2);
            SetDialogYesNo(1);
            cnWrap_SoundRequest(6);
        } else if (temp_v1_4 & 0x800) {
            temp_v1_6 = (int)pNet;
            if (F(s16, temp_v1_6, 0x26) >= 2) {
                temp_v0 = F(s16, temp_v1_6, 0x24) - 1;
                F(s16, temp_v1_6, 0x24) = temp_v0;
                if (((s16)temp_v0) < 0) {
                    temp_v1_7 = (int)pNet;
                    F(s16, temp_v1_7, 0x24) = (s16) (F(s16, temp_v1_7, 0x26) - 1);
                }
                var_s1 = 0;
                var_s0 = (int)&tl_member_buff;
                F(u8, pNet, 0xA) = 0U;
                F(u8, pNet, 3) = 1U;
                do {
                    F(s8, var_s0, 0x280) = 0;
                    memset(var_s0 + 0x29A, 0, 0x40);
                    var_s1 = (var_s1 + 1) & 0xFF;
                    var_s0 += 0x2FC;
                } while (var_s1 < 8);
                cnWrap_SoundRequest(1);
            }
        } else if (temp_v1_4 & 0x400) {
            temp_a0_6 = (int)pNet;
            temp_v1_8 = F(s16, temp_a0_6, 0x26);
            if (temp_v1_8 >= 2) {
                temp_v0_2 = F(s16, temp_a0_6, 0x24) + 1;
                F(s16, temp_a0_6, 0x24) = temp_v0_2;
                if (((s16)temp_v0_2) >= temp_v1_8) {
                    F(s16, pNet, 0x24) = 0;
                }
                var_s1_2 = 0;
                var_s0_2 = (int)&tl_member_buff;
                F(u8, pNet, 0xA) = 0U;
                F(u8, pNet, 3) = 1U;
                do {
                    F(s8, var_s0_2, 0x280) = 0;
                    memset(var_s0_2 + 0x29A, 0, 0x40);
                    var_s1_2 = (var_s1_2 + 1) & 0xFF;
                    var_s0_2 += 0x2FC;
                } while (var_s1_2 < 8);
                cnWrap_SoundRequest(1);
            }
        } else if (temp_v1_4 & 0x2000) {
            temp_v0_3 = (int)pNet;
            temp_v0_4 = F(u8, temp_v0_3, 0xA);
            if (temp_v0_4 != 0) {
                F(u8, temp_v0_3, 0xA) = (u8) (temp_v0_4 - 1);
            } else {
                F(u8, temp_v0_3, 0xA) = 6U;
                temp_v0_5 = (int)pNet;
                if ((*(s8 *)((u8 *)&Friend_data + ((F(u8, temp_v0_5, 0xA) + (F(s16, temp_v0_5, 0x24) * 7)) * 0x30))) == 0) {
                    F(u8, pNet, 0xA) = (u8) ((net_Check_FriendSuu(&Friend_data, 0x32) % 7) - 1);
                }
            }
            cnWrap_SoundRequest(1);
        } else if (temp_v1_4 & 0x1000) {
            temp_v1_9 = (int)pNet;
            temp_v0_6 = F(u8, temp_v1_9, 0xA) + 1;
            F(u8, temp_v1_9, 0xA) = temp_v0_6;
            if ((temp_v0_6 & 0xFF) >= 7) {
                F(u8, pNet, 0xA) = 0U;
            } else {
                temp_v0_7 = (int)pNet;
                temp_a0_7 = F(u8, temp_v0_7, 0xA) + (F(s16, temp_v0_7, 0x24) * 7);
                if ((temp_a0_7 >= 0x32) || ((*(s8 *)((u8 *)&Friend_data + (temp_a0_7 * 0x30))) == 0)) {
                    F(u8, temp_v0_7, 0xA) = 0U;
                }
            }
            cnWrap_SoundRequest(1);
        }
        goto block_153;
    case 3:                                         /* switch 1 */
        temp_v0_8 = getUserInfo();
        switch (temp_v0_8) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0_8 = (int)pNet;
            F(u8, temp_a0_8, 3) = (u8) (F(u8, temp_a0_8, 3) + 1);
            F(s8, pNet, 0x13) = 3;
            F(u8, pNet, 0x12) = 0U;
            break;
        case 1:                                     /* switch 2 */
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            F(u8, pNet, 3) = 9U;
            break;
        }
        goto block_153;
    case 4:                                         /* switch 1 */
        temp_a1_3 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0);
        if (temp_a1_3 & 0x40) {
            F(u8, pNet, 3) = 2U;
            temp_v1_10 = (int)pNet;
            F(u8, temp_v1_10, 0xA) = (u8) F(u8, temp_v1_10, 6);
            cnWrap_SoundRequest(3);
        } else if (temp_a1_3 & 0x80) {
            temp_a0_9 = (int)pNet;
            F(u8, temp_a0_9, 3) = (u8) (F(u8, temp_a0_9, 3) + 1);
            F(u8, pNet, 0xA) = 0U;
            temp_a0_10 = (int)pNet;
            temp_a2_4 = F(u8, temp_a0_10, 6);
            temp_s0_3 = (int)&Friend_data + ((temp_a2_4 + (F(s16, temp_a0_10, 0x24) * 7)) * 0x30);
            strcpy((u8 *)cw + 0x2F80);
            strcpy((u8 *)cw + 0x2F88, temp_s0_3 + 8);
            F(s8, (u8 *)cw, 0x2F99) = 0;
            cnWrap_SoundRequest(6);
        } else if (temp_a1_3 & 0x200) {
            F(u8, pNet, 3) = 0xAU;
            temp_v1_11 = (int)pNet;
            F(u8, temp_v1_11, 6) = (u8) F(u8, temp_v1_11, 0xA);
            SetDialogData(0x16, 2);
            SetDialogYesNo(1);
            cnWrap_SoundRequest(6);
        } else if (temp_a1_3 & 0x800) {
            temp_v0_9 = (int)pNet;
            temp_v0_10 = F(u8, temp_v0_9, 0x12);
            if (temp_v0_10 == 0) {
                var_v0 = 2;
            } else {
                var_v0 = temp_v0_10 - 1;
            }
            F(u8, temp_v0_9, 0x12) = var_v0;
            cnWrap_SoundRequest(1);
        } else if (temp_a1_3 & 0x400) {
            temp_v1_12 = (int)pNet;
            temp_v0_11 = F(u8, temp_v1_12, 0x12) + 1;
            F(u8, temp_v1_12, 0x12) = temp_v0_11;
            if ((temp_v0_11 & 0xFF) >= 3) {
                F(u8, pNet, 0x12) = 0U;
            }
            cnWrap_SoundRequest(1, (u8 *) temp_a1_3);
        }
        goto block_153;
    case 5:                                         /* switch 1 */
        var_a1 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0);
        if (var_a1 & 0x40) {
            F(u8, pNet, 3) = 2U;
            temp_v1_13 = (int)pNet;
            F(u8, temp_v1_13, 0xA) = (u8) F(u8, temp_v1_13, 6);
            cnWrap_SoundRequest(3);
        } else if (var_a1 & 0x20) {
            temp_v1_14 = (int)pNet;
            if (F(u8, temp_v1_14, 0xA) == 0) {
                F(u8, temp_v1_14, 3) = (u8) (F(u8, temp_v1_14, 3) + 1);
            } else {
                F(u8, temp_v1_14, 3) = 7U;
                F(s8, pNet, 0xC) = 1;
                SetDialogData(0x2A, 2);
                SetDialogYesNo(1);
                temp_v0_12 = (int)cw;
                var_a0 = 0x4D;
                var_a2 = temp_v0_12 + 0x2F7F;
                var_a1 = (temp_v0_12 + 0x3019);
                do {
                    var_a0 -= 1;
                    temp_v0_13 = F(s8, var_a2, 1);
                    F(s8, var_a1, 0) = (s8) F(s8, var_a2, 0);
                    var_a2 += 2;
                    F(s8, var_a1, 1) = temp_v0_13;
                    var_a1 += 2;
                } while (var_a0 > 0);
            }
            cnWrap_SoundRequest(0, (u8 *) var_a1);
        } else if ((var_a1 & 0x3000) && (F(s8, (u8 *)cw, 0x2F99) != 0)) {
            temp_v1_15 = (int)pNet;
            F(u8, temp_v1_15, 0xA) = (u8) (F(u8, temp_v1_15, 0xA) ^ 1);
            cnWrap_SoundRequest(1);
        }
        goto block_153;
    case 6:                                         /* switch 1 */
        if (mail_input(temp_a0, (u8 *)cw + 0x2F99) == 1) {
            KinshiYogo_chk((u8 *)cw + 0x2F99);
            F(u8, pNet, 3) = 5U;
            if (F(s8, (u8 *)cw, 0x2F99) != 0) {
                F(u8, pNet, 0xA) = 1U;
            } else {
                F(u8, pNet, 0xA) = 0U;
            }
        }
        goto block_153;
    case 7:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        temp_v0_14 = Lb_select(temp_a0);
        switch (temp_v0_14) {                       /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            F(u8, pNet, 3) = 8U;
            SetDialogData(0x2D, 5);
            break;
        case 3:                                     /* switch 3 */
            F(u8, pNet, 3) = 5U;
            break;
        }
        goto block_153;
    case 8:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        temp_v0_15 = Lbc_SendMail(temp_a0);
        switch (temp_v0_15) {                       /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            SetDialogData(0x26, 3);
            temp_v1_16 = (int)pNet;
            F(u8, temp_v1_16, 3) = (u8) (F(u8, temp_v1_16, 3) + 1);
            break;
        case 1:                                     /* switch 4 */
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            temp_v1_17 = (int)pNet;
            F(u8, temp_v1_17, 3) = (u8) (F(u8, temp_v1_17, 3) + 1);
            break;
        }
        goto block_153;
    case 9:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            temp_a1_4 = (int)pNet;
            F(u8, temp_a1_4, 0xA) = (u8) F(u8, temp_a1_4, 6);
            F(u8, pNet, 3) = 2U;
            cnWrap_SoundRequest(0);
        }
        goto block_153;
    case 10:                                        /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        temp_v0_16 = Lb_select(temp_a0);
        switch (temp_v0_16) {                       /* switch 5; irregular */
        case 0:                                     /* switch 5 */
            F(u8, pNet, 0xD) = 0U;
            temp_a0_11 = (int)pNet;
            temp_a3 = F(u8, temp_a0_11, 0xA);
            memcpy((u8 *)cw + 0x2F80, (int)&Friend_data + ((temp_a3 + (F(s16, temp_a0_11, 0x24) * 7)) * 0x30), 8, temp_a3);
            temp_a0_12 = (int)pNet;
            temp_a3_2 = F(u8, temp_a0_12, 0xA);
            memcpy((u8 *)cw + 0x2F88, (int)&Friend_data + ((temp_a3_2 + (F(s16, temp_a0_12, 0x24) * 7)) * 0x30) + 8, 0x11, temp_a3_2);
            temp_v0_17 = (int)pNet;
            net_Delete_FriendData(&Friend_data, (F(u8, temp_v0_17, 0xA) + (F(s16, temp_v0_17, 0x24) * 7)) & 0xFF, 0x32);
            var_s0_3 = F(u8, pNet, 0xA);
            if (var_s0_3 < 6) {
                var_s1_3 = (int)&tl_member_buff + (var_s0_3 * 0x2FC);
                do {
                    memcpy(var_s1_3, (int)&tl_member_buff + ((var_s0_3 + 1) * 0x2FC), 0x2FC);
                    var_s0_3 = (var_s0_3 + 1) & 0xFF;
                    var_s1_3 += 0x2FC;
                } while (var_s0_3 < 6);
            }
            memset((int)&tl_member_buff + ((var_s0_3 & 0xFF) * 0x2FC), 0, 0x2FC);
            get_friend_page_num(pNet);
            temp_a0_13 = (int)pNet;
            temp_v1_18 = F(s16, temp_a0_13, 0x26);
            if (F(s16, temp_a0_13, 0x24) >= temp_v1_18) {
                F(s16, temp_a0_13, 0x24) = (s16) (temp_v1_18 - 1);
            } else {
                temp_v0_18 = F(u8, temp_a0_13, 0xA);
                if (temp_v0_18 != 0) {
                    F(u8, temp_a0_13, 0xA) = (u8) (temp_v0_18 - 1);
                }
            }
            F(s8, (u8 *)cw, 0x2C08) = 0;
            *(u8 *)0x3F36AB = 0;
            F(u8, pNet, 3) = 0xDU;
            SetDialogData(0x17, 3);
            break;
        case 3:                                     /* switch 5 */
            F(u8, pNet, 3) = 2U;
            break;
        }
        goto block_153;
    case 11:                                        /* switch 1 */
        temp_v0_19 = F(u8, temp_a0, 0x12);
        if (temp_v0_19 == 0) {
            temp_v0_20 = SaveNetFile_ForLobby();
            if ((temp_v0_20 != -1) && (temp_v0_20 != 1)) {
                goto block_153;
            }
            F(s8, (u8 *)cw, 0x2C08) = 1;
            *(u8 *)0x3F36AB = 1;
            str_pause(0, 0);
            str_pause(1, 0);
            if (F(u8, (u8 *)cw, 0x35D5) != 0) {
                str_fadein_vol(0, 0x1E, (*(int *)((u8 *)&D_32D471 + (game_w.stage * 2))));
            }
            return 3;
        }
        F(u8, temp_a0, 0x12) = (u8) (temp_v0_19 - 1);
        goto block_153;
    case 12:                                        /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        goto block_153;
    case 13:                                        /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            temp_v1_19 = (int)pNet;
            F(u8, temp_v1_19, 3) = (u8) (F(u8, temp_v1_19, 3) + 1);
            cnWrap_SoundRequest(0);
        }
        goto block_153;
    case 14:                                        /* switch 1 */
        if (getFriendNow(temp_a0, 6, 1) != 0) {

        } else {
            F(u8, pNet, 3) = 2U;
        }
        goto block_153;
    }
}
