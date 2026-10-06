#include "lobby_a.h"
extern char RecvMailInfo[];
extern char RecvMailInfo[];
extern char RecvMailInfo[];
extern char seekStr[];
extern char seekStr[];
extern char seekStr[];
extern char seekStr[];
extern char my_user_id[];
extern char seekStr[];
s32 plaza_mailBox(void) {
    int var_a1;
    s16 temp_a1_4;
    s32 temp_s0;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_9;
    s32 var_a0;
    s32 var_a0_2;
    s32 var_a0_3;
    int var_a2;
    s8 temp_v0;
    s8 temp_v0_4;
    s8 temp_v0_8;
    u8 temp_a0_6;
    u8 temp_a1;
    u8 temp_a1_2;
    u8 temp_a1_3;
    u8 temp_a1_5;
    u8 temp_a1_6;
    u8 temp_a1_7;
    u8 temp_v1_9;
    int temp_a0;
    int temp_a0_2;
    int temp_a0_3;
    int temp_a0_4;
    int temp_a0_5;
    int temp_a1_8;
    int temp_a2;
    int temp_a2_2;
    int temp_a3;
    int temp_v0_2;
    int temp_v0_3;
    int temp_v0_7;
    int temp_v1;
    int temp_v1_10;
    int temp_v1_11;
    int temp_v1_12;
    int temp_v1_13;
    int temp_v1_14;
    int temp_v1_15;
    int temp_v1_16;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;
    int temp_v1_5;
    int temp_v1_6;
    int temp_v1_7;
    int temp_v1_8;
    int var_a1_2;
    int var_a1_3;
    int var_a1_4;
    int var_a2_2;
    int var_a2_3;
    int var_a2_4;

    temp_a0 = (int)pNet;
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a1 = F(u8, temp_a0, 3);
    temp_a2 = temp_a0 + 3;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, temp_a0, 3) = (u8) (temp_a1 + 1);
        F(u8, pNet, 0xA) = 0U;
        F(u8, pNet, 0xE) = 0U;
    default:                                        /* switch 1 */
block_102:
        return 2;
    case 1:                                         /* switch 1 */
        F(s16, temp_a0, 0x26) = 0;
        var_a2 = 0;
        var_a1 = (int)&RecvMailInfo;
loop_5:
        if (F(s8, var_a1, 1) != 0) {
            temp_a0_2 = (int)pNet;
            var_a1 += 0x9A;
            var_a2 =  ((var_a2 + 1) << 0x30) >> 0x30;
            F(s16, temp_a0_2, 0x26) = (s16) (F(s16, temp_a0_2, 0x26) + 1);
            if (var_a2 >= 8) {

            } else {
                goto loop_5;
            }
        }
        temp_a1_2 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0, (u8) var_a1, (u8 *) var_a2);
        if (temp_a1_2 & 0x20) {
            temp_a3 = (int)pNet;
            if (F(s16, temp_a3, 0x26) != 0) {
                var_a0 = 0x4D;
                var_a1_2 = (int)cw + 0x2F7F;
                var_a2_2 = (int)&RecvMailInfo + (F(u8, temp_a3, 0xA) * 0x9A);
                do {
                    var_a0 -= 1;
                    temp_v0 = F(s8, var_a2_2, 1);
                    F(s8, var_a1_2, 0) = (s8) F(s8, var_a2_2, 0);
                    var_a2_2 += 2;
                    F(s8, var_a1_2, 1) = temp_v0;
                    var_a1_2 += 2;
                } while (var_a0 > 0);
                F(u8, temp_a3, 0xE) = (u8) F(u8, temp_a3, 0xA);
                F(u8, pNet, 0xA) = 0U;
                temp_a2_2 = (int)pNet;
                temp_a1_3 = F(u8, temp_a2_2, 0xE);
                *((u8 *)&RecvMailInfo + (temp_a1_3 * 0x9A)) = 0;
                F(u8, temp_a2_2, 3) = (u8) (F(u8, temp_a2_2, 3) + 1);
                cnWrap_SoundRequest(0, temp_a1_3, temp_a2_2, temp_a3);
            } else {
                cnWrap_SoundRequest(7, temp_a1_2);
            }
            goto block_102;
        }
        if (temp_a1_2 & 0x40) {
            return 3;
        }
        if (temp_a1_2 & 0x80) {
            F(u8, pNet, 3) = 8U;
            F(u8, pNet, 0xA) = 0U;
            F(s8, (u8 *)cw, 0x2F80) = 0;
            F(s8, (u8 *)cw, 0x2F88) = 0;
            F(s8, (u8 *)cw, 0x2F99) = 0;
            cnWrap_SoundRequest(6, temp_a1_2);
        } else {
            temp_v0_2 = (int)pNet;
            temp_a1_4 = F(s16, temp_v0_2, 0x26);
            if (temp_a1_4 >= 2) {
                F(u8, pNet, 0xA) = Lb_cursorUD(F(u8, temp_v0_2, 0xA), temp_a1_4);
            }
        }
        goto block_102;
    case 2:                                         /* switch 1 */
        temp_a1_5 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0, temp_a1, temp_a2);
        if (temp_a1_5 & 0x40) {
            temp_v1 = (int)pNet;
            F(u8, temp_v1, 0xA) = (u8) F(u8, temp_v1, 0xE);
            temp_v1_2 = (int)pNet;
            F(u8, temp_v1_2, 3) = (u8) (F(u8, temp_v1_2, 3) - 1);
            cnWrap_SoundRequest(3, temp_a1_5);
        } else if (temp_a1_5 & 0x200) {
            temp_v1_3 = (int)pNet;
            F(u8, temp_v1_3, 3) = (u8) (F(u8, temp_v1_3, 3) + 1);
            F(s8, (u8 *)cw, 0x2F99) = 0;
            cnWrap_SoundRequest(6, temp_a1_5);
        }
        goto block_102;
    case 3:                                         /* switch 1 */
        temp_a1_6 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0, temp_a1, temp_a2);
        if (temp_a1_6 & 0x40) {
            F(u8, pNet, 3) = 1U;
            temp_v1_4 = (int)pNet;
            F(u8, temp_v1_4, 0xA) = (u8) F(u8, temp_v1_4, 0xE);
            cnWrap_SoundRequest(3, temp_a1_6);
        } else if (temp_a1_6 & 0x20) {
            temp_v1_5 = (int)pNet;
            if (F(u8, temp_v1_5, 0xA) == 0) {
                F(u8, temp_v1_5, 3) = (u8) (F(u8, temp_v1_5, 3) + 1);
            } else {
                F(u8, temp_v1_5, 6) = 3U;
                var_a0_2 = 0x4D;
                temp_v0_3 = (int)cw;
                var_a2_3 = temp_v0_3 + 0x2F7F;
                var_a1_3 = temp_v0_3 + 0x3019;
                do {
                    var_a0_2 -= 1;
                    temp_v0_4 = F(s8, var_a2_3, 1);
                    F(s8, var_a1_3, 0) = (s8) F(s8, var_a2_3, 0);
                    var_a2_3 += 2;
                    F(s8, var_a1_3, 1) = temp_v0_4;
                    var_a1_3 += 2;
                } while (var_a0_2 > 0);
                F(u8, pNet, 3) = 5U;
                SetDialogData(0x2A, 2, var_a2_3);
                SetDialogYesNo(1);
            }
            cnWrap_SoundRequest(0);
        } else if ((temp_a1_6 & 0x3000) && (F(s8, (u8 *)cw, 0x2F99) != 0)) {
            temp_v1_6 = (int)pNet;
            F(u8, temp_v1_6, 0xA) = (u8) (F(u8, temp_v1_6, 0xA) ^ 1);
            cnWrap_SoundRequest(1, temp_a1_6);
        }
        goto block_102;
    case 4:                                         /* switch 1 */
        if (mail_input(temp_a0, (u8 *)cw + 0x2F99, temp_a2) == 1) {
            KinshiYogo_chk((u8 *)cw + 0x2F99);
            if (F(s8, (u8 *)cw, 0x2F99) != 0) {
                F(u8, pNet, 0xA) = 1U;
            } else {
                F(u8, pNet, 0xA) = 0U;
            }
            temp_v1_7 = (int)pNet;
            F(u8, temp_v1_7, 3) = (u8) (F(u8, temp_v1_7, 3) - 1);
        }
        goto block_102;
    case 5:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        temp_v0_5 = Lb_select(temp_a0, temp_a1, temp_a2);
        switch (temp_v0_5) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            SetDialogData(0x2D, 5);
            temp_v1_8 = (int)pNet;
            F(u8, temp_v1_8, 3) = (u8) (F(u8, temp_v1_8, 3) + 1);
            break;
        case 3:                                     /* switch 2 */
            temp_a0_3 = (int)pNet;
            F(u8, temp_a0_3, 3) = (u8) F(u8, temp_a0_3, 6);
            temp_a0_4 = (int)pNet;
            temp_v1_9 = F(u8, temp_a0_4, 6);
            if (temp_v1_9 == 1) {
                F(u8, temp_a0_4, 0xA) = temp_v1_9;
            }
            break;
        }
        goto block_102;
    case 6:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        temp_v0_6 = Lbc_SendMail(temp_a0, temp_a1, temp_a2);
        switch (temp_v0_6) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            SetDialogData(0x26, 3);
            temp_v1_10 = (int)pNet;
            F(u8, temp_v1_10, 3) = (u8) (F(u8, temp_v1_10, 3) + 1);
            break;
        case 1:                                     /* switch 3 */
            SetDialogData_HTML((u8 *)cw + 0x32D1);
            temp_v1_11 = (int)pNet;
            F(u8, temp_v1_11, 3) = (u8) (F(u8, temp_v1_11, 3) + 1);
            break;
        }
        goto block_102;
    case 7:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, pNet, 3) = 1U;
            temp_v1_12 = (int)pNet;
            F(u8, temp_v1_12, 0xA) = (u8) F(u8, temp_v1_12, 0xE);
            F(s8, (u8 *)cw, 0x2F99) = 0;
            cnWrap_SoundRequest(0, temp_a1, temp_a2);
        }
        goto block_102;
    case 8:                                         /* switch 1 */
        temp_a1_7 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0, temp_a1, temp_a2);
        if (temp_a1_7 & 0x40) {
            F(u8, pNet, 3) = 1U;
            temp_v1_13 = (int)pNet;
            F(u8, temp_v1_13, 0xA) = (u8) F(u8, temp_v1_13, 0xE);
            cnWrap_SoundRequest(3, temp_a1_7);
        } else if (temp_a1_7 & 0x20) {
            temp_a0_5 = (int)pNet;
            F(u8, temp_a0_5, 6) = (u8) F(u8, temp_a0_5, 0xA);
            temp_a1_8 = (int)pNet;
            temp_a0_6 = F(u8, temp_a1_8, 0xA);
            switch (temp_a0_6) {                    /* switch 4; irregular */
            case 0:                                 /* switch 4 */
                F(u8, temp_a1_8, 3) = (u8) (F(u8, temp_a1_8, 3) + 1);
                F(s8, pNet, 4) = 1;
                seekStr[0] = 0;
                F(s8, (u8 *)cw, 0x2F80) = 0;
                cnWrap_SoundRequest(0, (u8) temp_a1_8);
                break;
            case 1:                                 /* switch 4 */
                F(u8, temp_a1_8, 3) = 0xBU;
                cnWrap_SoundRequest(0, (u8) temp_a1_8);
                break;
            case 2:                                 /* switch 4 */
                F(u8, temp_a1_8, 6) = 8U;
                var_a0_3 = 0x4D;
                temp_v0_7 = (int)cw;
                var_a2_4 = temp_v0_7 + 0x2F7F;
                var_a1_4 = temp_v0_7 + 0x3019;
                do {
                    var_a0_3 -= 1;
                    temp_v0_8 = F(s8, var_a2_4, 1);
                    F(s8, var_a1_4, 0) = (s8) F(s8, var_a2_4, 0);
                    var_a2_4 += 2;
                    F(s8, var_a1_4, 1) = temp_v0_8;
                    var_a1_4 += 2;
                } while (var_a0_3 > 0);
                F(u8, pNet, 3) = 5U;
                SetDialogData(0x2A, 2, var_a2_4);
                SetDialogYesNo(1);
                cnWrap_SoundRequest(0);
                break;
            }
        } else if (F(s8, (u8 *)cw, 0x2F99) != 0) {
            F(u8, pNet, 0xA) = Lb_cursorUD(F(u8, pNet, 0xA), 3);
        } else {
            F(u8, pNet, 0xA) = Lb_cursorUD(F(u8, pNet, 0xA), 2);
        }
        goto block_102;
    case 9:                                         /* switch 1 */
        if (plaza_req_input(temp_a0, &seekStr, temp_a2) == 1) {
            strcpy((u8 *)cw + 0x2F80, &seekStr);
            if (strlen(&seekStr) < 6) {
                SetDialogData(0x1B, 3);
                F(u8, pNet, 3) = 0xCU;
                F(s8, (u8 *)cw, 0x2F80) = 0;
                F(s8, (u8 *)cw, 0x2F88) = 0;
            } else if (memcmp(&my_user_id, (u8 *)cw + 0x2F80, 6) == 0) {
                SetDialogData(0x2E, 3);
                F(u8, pNet, 3) = 0xCU;
                F(s8, (u8 *)cw, 0x2F80) = 0;
                F(s8, (u8 *)cw, 0x2F88) = 0;
            } else {
                temp_v1_14 = (int)pNet;
                F(u8, temp_v1_14, 3) = (u8) (F(u8, temp_v1_14, 3) + 1);
            }
        }
        goto block_102;
    case 10:                                        /* switch 1 */
        temp_v0_9 = getHandleFromID(temp_a0, temp_a1, temp_a2);
        if ((temp_v0_9 != 1) && (temp_v0_9 != 0)) {

        } else {
            F(u8, pNet, 3) = 8U;
            temp_v1_15 = (int)pNet;
            F(u8, temp_v1_15, 0xA) = (u8) (F(u8, temp_v1_15, 0xA) + 1);
            seekStr[0] = 0;
        }
        goto block_102;
    case 11:                                        /* switch 1 */
        if (mail_input(temp_a0, (u8 *)cw + 0x2F99, temp_a2) == 1) {
            KinshiYogo_chk((u8 *)cw + 0x2F99);
            if (F(s8, (u8 *)cw, 0x2F99) != 0) {
                temp_v1_16 = (int)pNet;
                F(u8, temp_v1_16, 0xA) = (u8) (F(u8, temp_v1_16, 0xA) + 1);
            } else {
                F(u8, pNet, 0xA) = 1U;
            }
            F(u8, pNet, 3) = 8U;
        }
        goto block_102;
    case 12:                                        /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, pNet, 3) = 8U;
            F(u8, pNet, 0xA) = 0U;
            cnWrap_SoundRequest(0, temp_a1, temp_a2);
        }
        goto block_102;
    }
}
