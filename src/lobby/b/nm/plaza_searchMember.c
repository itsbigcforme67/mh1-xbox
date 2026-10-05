#include "lobby_a.h"
extern char seekStr[];
extern char seekStr[];
extern char seekStr[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char seekStr[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char SearchCondition[];
extern char my_user_id[];
void plaza_searchMember(int arg0) {
    s16 temp_v0_4;
    s16 temp_v0_5;
    s16 temp_v1_2;
    s32 temp_a0_2;
    s32 temp_a0_5;
    s32 temp_a0_6;
    s32 temp_hi;
    s32 temp_hi_2;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_v0_3;
    s32 temp_v0_7;
    s32 temp_v0_8;
    int temp_a0_4;
    u8 temp_a0;
    u8 temp_a0_3;
    u8 temp_a1;
    u8 temp_a1_2;
    u8 temp_a2;
    u8 temp_a3;
    u8 temp_v0;
    u8 temp_v0_10;
    u8 temp_v0_2;
    u8 temp_v0_6;
    u8 temp_v0_9;
    u8 temp_v1;
    u8 var_v0;
    u8 var_v0_2;
    u8 var_v0_3;

    temp_a1 = F(u8, arg0, 3);
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, arg0, 3) = (u8) (temp_a1 + 1);
        F(u8, arg0, 6) = 0xFFU;
        F(u8, arg0, 0xA) = 0U;
        return;
    case 1:                                         /* switch 1 */
        F(s16, arg0, 0x28) = Get_sw_on2(0, temp_a1);
        temp_s0_2 = temp_s0 & 0xFFFF;
        if ((temp_s0_2 & 0x20) || (kb_input_ck_enter() == 1)) {
            temp_v1 = F(u8, arg0, 4);
            if ((temp_v1 != 0) && (temp_v1 != 1)) {
                F(u8, arg0, 3) = 3U;
                F(u8, arg0, 6) = (u8) F(u8, arg0, 0xA);
                F(u8, arg0, 0xA) = 6U;
            } else {
                seekStr[0] = 0;
                F(u8, arg0, 3) = (u8) (F(u8, arg0, 3) + 1);
            }
            cnWrap_SoundRequest(0);
        } else if (temp_s0_2 & 0x40) {
            tl_exit_sub_menu(0);
        } else if (temp_s0_2 & 0x400) {
            temp_v0 = F(u8, arg0, 4) + 1;
            F(u8, arg0, 4) = temp_v0;
            if ((temp_v0 & 0xFF) >= 4) {
                F(u8, arg0, 4) = 0U;
            }
            F(u8, arg0, 0xA) = 0U;
            cnWrap_SoundRequest(1);
        } else if (temp_s0_2 & 0x800) {
            temp_v0_2 = F(u8, arg0, 4);
            if (temp_v0_2 == 0) {
                var_v0 = 3;
            } else {
                var_v0 = temp_v0_2 - 1;
            }
            F(u8, arg0, 4) = var_v0;
            F(u8, arg0, 0xA) = 0U;
            cnWrap_SoundRequest(1);
        }
        temp_a0 = F(u8, arg0, 4);
        if ((temp_a0 != 0) && (temp_a0 != 1)) {
            F(u8, arg0, 0xA) = Lb_cursorUD(F(u8, arg0, 0xA), 5);
            return;
        }
    default:                                        /* switch 1 */
        return;
    case 2:                                         /* switch 1 */
        if (plaza_req_input(arg0, &seekStr) == 1) {
            if (strlen(&seekStr) != 0) {
                F(u8, arg0, 0xA) = 6U;
                F(u8, arg0, 3) = (u8) (F(u8, arg0, 3) + 1);
                return;
            }
            F(u8, arg0, 3) = 1U;
            return;
        }
        break;
    case 3:                                         /* switch 1 */
        temp_a0_2 = temp_s0 & 0xFFFF;
        F(s16, arg0, 0x28) = Get_sw_on2(0, temp_a1);
        if (temp_a0_2 & 0x20) {
            F(u8, arg0, 3) = 5U;
            SetDialogData(0x18, 5);
            cnWrap_SoundRequest(0);
            temp_a0_3 = F(u8, arg0, 4);
            switch (temp_a0_3) {                    /* switch 2; irregular */
            case 1:                                 /* switch 2 */
                strcpy((int)&SearchCondition + 4, &seekStr);
                F(u8, &SearchCondition, 1) = strlen(&seekStr);
                F(s8, &SearchCondition, 0) = 1;
                F(s8, arg0, 0xE) = 1;
                if (F(u8, &SearchCondition, 1) < 6) {
                    SetDialogData(0x1B, 3);
                    F(u8, arg0, 3) = 4U;
                    return;
                }
                break;
            case 0:                                 /* switch 2 */
                F(s8, arg0, 0xE) = 1;
                if (strlen(&seekStr, 3) == 0) {
                    SetDialogData(0x1C, 3);
                    F(u8, arg0, 3) = 4U;
                    return;
                }
                strcpy((int)&SearchCondition + 4, &seekStr);
                F(u8, &SearchCondition, 1) = strlen(&seekStr);
                F(s8, &SearchCondition, 0) = 2;
                return;
            case 2:                                 /* switch 2 */
                F(s8, arg0, 0xE) = 1;
                F(u8, &SearchCondition, 4) = (u8) F(u8, arg0, 6);
                F(u8, &SearchCondition, 1) = 1U;
                F(s8, &SearchCondition, 0) = 3;
                return;
            case 3:                                 /* switch 2 */
                F(s8, arg0, 0xE) = 1;
                F(u8, &SearchCondition, 4) = (u8) ((F(u8, arg0, 6) * 4) + 1);
                F(u8, &SearchCondition, 1) = 1U;
                F(s8, &SearchCondition, 0) = 6;
                F(s8, &SearchCondition, 5) = (s8) ((F(u8, arg0, 6) * 4) + 4);
                return;
            }
        } else if (temp_a0_2 & 0x40) {
            F(u8, arg0, 3) = 1U;
            F(u8, arg0, 0xA) = (u8) F(u8, arg0, 6);
            cnWrap_SoundRequest(3);
            return;
        }
        break;
    case 4:                                         /* switch 1 */
        F(s8, arg0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, arg0, 3) = 0U;
            F(u8, arg0, 0xA) = (u8) F(u8, arg0, 6);
            cnWrap_SoundRequest(0, temp_a1);
            return;
        }
        break;
    case 5:                                         /* switch 1 */
        F(s8, arg0, 0xC) = 1;
        temp_v0_3 = Lbc_ConditionSearch(&SearchCondition, 1);
        switch (temp_v0_3) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            if (*(u8 *)SearchResult == 0) {
                F(u8, arg0, 3) = 7U;
                SetDialogData(0x19, 3);
                return;
            }
            F(u8, arg0, 3) = 6U;
            temp_a0_4 = (int)SearchResult;
            F(s16, arg0, 0x26) = (s16) (((*(u8 *)temp_a0_4) / 7) + ((u8) (*(u8 *)temp_a0_4) >> 0x1F));
            F(s16, arg0, 0x24) = 0;
            F(u8, arg0, 0xA) = 0U;
            if ((*(u8 *)SearchResult % 7) != 0) {
                F(s16, arg0, 0x26) = (s16) (F(s16, arg0, 0x26) + 1);
                return;
            }
            break;
        case 1:                                     /* switch 3 */
            F(u8, arg0, 3) = 7U;
            SetDialogData(0x1A, 3);
            return;
        }
        break;
    case 6:                                         /* switch 1 */
        temp_a0_5 = temp_s0 & 0xFFFF;
        F(s16, arg0, 0x28) = Get_sw_on2(0, temp_a1);
        if (temp_a0_5 & 0x40) {
            tl_exit_sub_menu(0);
            return;
        }
        if (temp_a0_5 & 0x200) {
            F(u8, arg0, 3) = 8U;
            cnWrap_SoundRequest(6);
            return;
        }
        if (temp_a0_5 & 0x20) {
            temp_a3 = F(u8, arg0, 0xA);
            if (memcmp(SearchResult + ((temp_a3 + (F(s16, arg0, 0x24) * 7)) * 0x5C) + 4, &my_user_id, 8, temp_a3) == 0) {
                cnWrap_SoundRequest(7);
                return;
            }
            F(u8, arg0, 3) = 9U;
            temp_a2 = F(u8, arg0, 0xA);
            strcpy((s32)cw + 0x2F80, (int)SearchResult + ((temp_a2 + (F(s16, arg0, 0x24) * 7)) * 0x5C) + 4, temp_a2);
            memset((s32)cw + 0x2B9C, 0, 0x62);
            cnWrap_SoundRequest(6);
            return;
        }
        if (temp_a0_5 & 0x800) {
            if (F(s16, arg0, 0x26) >= 2) {
                temp_v0_4 = F(s16, arg0, 0x24) - 1;
                F(s16, arg0, 0x24) = temp_v0_4;
                if (((s16)temp_v0_4) < 0) {
                    F(s16, arg0, 0x24) = (s16) (F(s16, arg0, 0x26) - 1);
                }
                F(u8, arg0, 0xA) = 0U;
                cnWrap_SoundRequest(1);
                return;
            }
        } else if (temp_a0_5 & 0x400) {
            temp_v1_2 = F(s16, arg0, 0x26);
            if (temp_v1_2 >= 2) {
                temp_v0_5 = F(s16, arg0, 0x24) + 1;
                F(s16, arg0, 0x24) = temp_v0_5;
                if (((s16)temp_v0_5) >= temp_v1_2) {
                    F(s16, arg0, 0x24) = 0;
                }
                F(u8, arg0, 0xA) = 0U;
                cnWrap_SoundRequest(1);
                return;
            }
        } else {
            if (temp_a0_5 & 0x2000) {
                temp_v0_6 = F(u8, arg0, 0xA);
                if (temp_v0_6 == 0) {
                    if (F(s16, arg0, 0x24) == (F(s16, arg0, 0x26) - 1)) {
                        temp_hi = *(u8 *)SearchResult % 7;
                        if (temp_hi == 0) {
                            var_v0_2 = 6;
                        } else {
                            var_v0_2 = temp_hi - 1;
                        }
                    } else {
                        var_v0_2 = 6;
                    }
                } else {
                    var_v0_2 = temp_v0_6 - 1;
                }
                F(u8, arg0, 0xA) = var_v0_2;
                cnWrap_SoundRequest(1);
                return;
            }
            if (temp_a0_5 & 0x1000) {
                F(u8, arg0, 0xA) = (u8) (F(u8, arg0, 0xA) + 1);
                if (F(u8, arg0, 0xA) >= 7) {
                    F(u8, arg0, 0xA) = 0U;
                }
                if (F(s16, arg0, 0x24) == (F(s16, arg0, 0x26) - 1)) {
                    temp_hi_2 = *(u8 *)SearchResult % 7;
                    if ((temp_hi_2 != 0) && (F(u8, arg0, 0xA) >= temp_hi_2)) {
                        F(u8, arg0, 0xA) = 0U;
                    }
                }
                cnWrap_SoundRequest(1);
                return;
            }
        }
        break;
    case 7:                                         /* switch 1 */
        F(s8, arg0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, arg0, 3) = 1U;
            F(u8, arg0, 0xA) = 0U;
            cnWrap_SoundRequest(0, temp_a1);
            return;
        }
        break;
    case 8:                                         /* switch 1 */
        temp_a1_2 = F(u8, arg0, 0xA);
        temp_v0_7 = Plaza_add_friend(SearchResult + ((temp_a1_2 + (F(s16, arg0, 0x24) * 7)) * 0x5C) + 4, temp_a1_2);
        if ((temp_v0_7 != 1) && (temp_v0_7 != 0)) {
            return;
        }
        F(u8, arg0, 3) = 6U;
        return;
    case 9:                                         /* switch 1 */
        temp_v0_8 = getUserInfo(arg0, temp_a1);
        switch (temp_v0_8) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            F(u8, arg0, 3) = (u8) (F(u8, arg0, 3) + 1);
            F(u8, arg0, 0x12) = 0U;
            return;
        case 1:                                     /* switch 4 */
            SetDialogData_HTML((s32)cw + 0x32D1);
            F(u8, arg0, 3) = 0xBU;
            return;
        }
        break;
    case 10:                                        /* switch 1 */
        temp_a0_6 = temp_s0 & 0xFFFF;
        F(s16, arg0, 0x28) = Get_sw_on2(0, temp_a1);
        if (temp_a0_6 & 0x800) {
            temp_v0_9 = F(u8, arg0, 0x12);
            if (temp_v0_9 == 0) {
                var_v0_3 = 2;
            } else {
                var_v0_3 = temp_v0_9 - 1;
            }
            F(u8, arg0, 0x12) = var_v0_3;
            cnWrap_SoundRequest(1);
            return;
        }
        if (temp_a0_6 & 0x400) {
            temp_v0_10 = F(u8, arg0, 0x12) + 1;
            F(u8, arg0, 0x12) = temp_v0_10;
            if ((temp_v0_10 & 0xFF) >= 3) {
                F(u8, arg0, 0x12) = 0U;
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (temp_a0_6 & 0x40) {
            F(u8, arg0, 3) = 6U;
            cnWrap_SoundRequest(3);
            return;
        }
        break;
    case 11:                                        /* switch 1 */
        F(s8, arg0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, arg0, 3) = 6U;
            cnWrap_SoundRequest(0, temp_a1);
        }
        break;
    }
}
