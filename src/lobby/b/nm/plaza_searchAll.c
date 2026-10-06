#include "lobby_a.h"
extern char SearchCondition[];
extern char SearchCondition[];
extern char my_user_id[];
void plaza_searchAll(void) {
    s16 temp_v0_2;
    s16 temp_v0_3;
    s16 temp_v1_3;
    s32 temp_a1_4;
    s32 temp_hi;
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 var_a1;
    int temp_a1_2;
    u8 temp_a1_3;
    u8 temp_a2;
    u8 temp_a2_2;
    u8 temp_a3_2;
    u8 temp_v0_10;
    u8 temp_v0_4;
    u8 temp_v0_9;
    u8 var_v0;
    u8 var_v0_2;
    int temp_a0;
    int temp_a0_2;
    int temp_a0_3;
    int temp_a0_4;
    int temp_a0_5;
    int temp_a0_6;
    int temp_a0_7;
    int temp_a0_8;
    int temp_a1;
    int temp_a3;
    int temp_v0_5;
    int temp_v0_8;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_4;
    int temp_v1_5;

    temp_a0 = (int)pNet;
    temp_s0 = Get_sw2(0) & 0xFFFF;
    temp_a2 = F(u8, temp_a0, 3);
    temp_a3 = temp_a0 + 3;
    switch (temp_a2) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        F(u8, temp_a0, 3) = 5U;
        SearchCondition[0] = 1;
        SetDialogData(0x18, 5);
        return;
    case 5:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        temp_v0 = Lbc_ConditionSearch(&SearchCondition, 0);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a1 = (int)pNet;
            F(u8, temp_a1, 3) = (u8) (F(u8, temp_a1, 3) + 1);
            temp_a1_2 = (int)SearchResult;
            F(s16, pNet, 0x26) = (s16) (((*(u8 *)temp_a1_2) / 7) + ((u8) (*(u8 *)temp_a1_2) >> 0x1F));
            F(s16, pNet, 0x24) = 0;
            F(u8, pNet, 0xA) = 0U;
            if ((*(u8 *)SearchResult % 7) != 0) {
                temp_a0_2 = (int)pNet;
                F(s16, temp_a0_2, 0x26) = (s16) (F(s16, temp_a0_2, 0x26) + 1);
                return;
            }
            return;
        case 1:                                     /* switch 2 */
            SetDialogData(0x2B, 0);
            F(u8, pNet, 3) = 7U;
            return;
        }
        break;
    case 6:                                         /* switch 1 */
        var_a1 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0, 5);
        if (var_a1 & 0x40) {
            tl_exit_sub_menu(0, var_a1);
            return;
        }
        if (var_a1 & 0x200) {
            F(u8, pNet, 3) = 8U;
            cnWrap_SoundRequest(6, var_a1);
            return;
        }
        if (var_a1 & 0x20) {
            temp_v1 = (int)pNet;
            temp_a3_2 = F(u8, temp_v1, 0xA);
            if (memcmp(SearchResult + ((temp_a3_2 + (F(s16, temp_v1, 0x24) * 7)) * 0x5C) + 4, &my_user_id, 8, temp_a3_2) == 0) {
                cnWrap_SoundRequest(7);
                return;
            }
            F(u8, pNet, 3) = 9U;
            temp_a0_3 = (int)pNet;
            temp_a2_2 = F(u8, temp_a0_3, 0xA);
            strcpy((s32)cw + 0x2F80, (int)SearchResult + ((temp_a2_2 + (F(s16, temp_a0_3, 0x24) * 7)) * 0x5C) + 4, temp_a2_2);
            memset((s32)cw + 0x2B9C, 0, 0x62);
            cnWrap_SoundRequest(6);
            return;
        }
        if (var_a1 & 0x800) {
            temp_a0_4 = (int)pNet;
            if (F(s16, temp_a0_4, 0x26) >= 2) {
                temp_v0_2 = F(s16, temp_a0_4, 0x24) - 1;
                F(s16, temp_a0_4, 0x24) = temp_v0_2;
                if (((s16)temp_v0_2) < 0) {
                    temp_v1_2 = (int)pNet;
                    F(s16, temp_v1_2, 0x24) = (s16) (F(s16, temp_v1_2, 0x26) - 1);
                }
                F(u8, pNet, 0xA) = 0U;
                cnWrap_SoundRequest(1, var_a1);
                return;
            }
        } else if (var_a1 & 0x400) {
            temp_a0_5 = (int)pNet;
            temp_v1_3 = F(s16, temp_a0_5, 0x26);
            if (temp_v1_3 >= 2) {
                temp_v0_3 = F(s16, temp_a0_5, 0x24) + 1;
                F(s16, temp_a0_5, 0x24) = temp_v0_3;
                if (((s16)temp_v0_3) >= temp_v1_3) {
                    F(s16, pNet, 0x24) = 0;
                }
                F(u8, pNet, 0xA) = 0U;
                cnWrap_SoundRequest(1, var_a1);
                return;
            }
        } else {
            if (var_a1 & 0x2000) {
                temp_a0_6 = (int)pNet;
                temp_v0_4 = F(u8, temp_a0_6, 0xA);
                if (temp_v0_4 == 0) {
                    if (F(s16, temp_a0_6, 0x24) == (F(s16, temp_a0_6, 0x26) - 1)) {
                        temp_hi = *(u8 *)SearchResult % 7;
                        if (temp_hi == 0) {
                            var_v0 = 6;
                        } else {
                            var_v0 = temp_hi - 1;
                        }
                    } else {
                        var_v0 = 6;
                    }
                } else {
                    var_v0 = temp_v0_4 - 1;
                }
                F(u8, temp_a0_6, 0xA) = var_v0;
                cnWrap_SoundRequest(1, (temp_a0_6 + 0xA));
                return;
            }
            if (var_a1 & 0x1000) {
                temp_v1_4 = (int)pNet;
                F(u8, temp_v1_4, 0xA) = (u8) (F(u8, temp_v1_4, 0xA) + 1);
                temp_v0_5 = (int)pNet;
                if (F(u8, temp_v0_5, 0xA) >= 7) {
                    F(u8, temp_v0_5, 0xA) = 0U;
                }
                temp_a0_7 = (int)pNet;
                if (F(s16, temp_a0_7, 0x24) == (F(s16, temp_a0_7, 0x26) - 1)) {
                    var_a1 = *(u8 *)SearchResult % 7;
                    if ((var_a1 != 0) && (F(u8, temp_a0_7, 0xA) >= var_a1)) {
                        F(u8, temp_a0_7, 0xA) = 0U;
                    }
                }
                cnWrap_SoundRequest(1);
                return;
            }
        }
        break;
    case 7:                                         /* switch 1 */
        F(s8, temp_a0, 0xC) = 1;
        if (temp_s0 & 0xFFFF & 0x20) {
            F(u8, pNet, 3) = 6U;
            cnWrap_SoundRequest(0, 1);
            return;
        }
        break;
    case 8:                                         /* switch 1 */
        temp_a1_3 = F(u8, temp_a0, 0xA);
        temp_v0_6 = Plaza_add_friend(SearchResult + ((temp_a1_3 + (F(s16, temp_a0, 0x24) * 7)) * 0x5C) + 4, temp_a1_3, temp_a2, temp_a3);
        if ((temp_v0_6 != 1) && (temp_v0_6 != 0)) {
            return;
        }
        F(u8, pNet, 3) = 6U;
        return;
    case 9:                                         /* switch 1 */
        temp_v0_7 = getUserInfo();
        switch (temp_v0_7) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            temp_a0_8 = (int)pNet;
            F(u8, temp_a0_8, 3) = (u8) (F(u8, temp_a0_8, 3) + 1);
            F(u8, pNet, 0x12) = 0U;
            return;
        case 1:                                     /* switch 3 */
            SetDialogData_HTML((s32)cw + 0x32D1);
            F(u8, pNet, 3) = 7U;
            return;
        }
        break;
    case 10:                                        /* switch 1 */
        temp_a1_4 = temp_s0 & 0xFFFF;
        F(s16, pNet, 0x28) = Get_sw_on2(0);
        if (temp_a1_4 & 0x800) {
            temp_v0_8 = (int)pNet;
            temp_v0_9 = F(u8, temp_v0_8, 0x12);
            if (temp_v0_9 == 0) {
                var_v0_2 = 2;
            } else {
                var_v0_2 = temp_v0_9 - 1;
            }
            F(u8, temp_v0_8, 0x12) = var_v0_2;
            cnWrap_SoundRequest(1);
            return;
        }
        if (temp_a1_4 & 0x400) {
            temp_v1_5 = (int)pNet;
            temp_v0_10 = F(u8, temp_v1_5, 0x12) + 1;
            F(u8, temp_v1_5, 0x12) = temp_v0_10;
            if ((temp_v0_10 & 0xFF) >= 3) {
                F(u8, pNet, 0x12) = 0U;
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (temp_a1_4 & 0x40) {
            F(u8, pNet, 3) = 6U;
            cnWrap_SoundRequest(3);
        }
        break;
    }
}
