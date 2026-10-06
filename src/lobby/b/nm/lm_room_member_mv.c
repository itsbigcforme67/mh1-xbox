#include "lobby_a.h"
extern int lbmw;
extern char my_user_id[];
s32 lm_room_member_mv(s32 arg0) {
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_v0;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 var_s2;
    s8 temp_v0_2;
    u8 temp_a0;
    u8 var_s0;
    u8 var_s1;
    int temp_a0_2;
    int temp_a3;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;
    int temp_v1_5;
    int temp_v1_6;

    var_s2 = arg0;
    if (Lbs_InRoomCheck() == 0) {
        F(u8, lbmw, 9) = 0U;
        F(s8, lbmw, 8) = 0;
        F(s8, lbmw, 0xF) = -1;
        F(s8, lbmw, 0xE) = -1;
        F(s8, lbmw, 0xD) = -1;
        *(s16 *)0x39DAD2 = 0x12;
        return var_s2;
    }
    var_s1 = 0;
    var_s0 = 0;
    F(s8, lbmw, 8) = 1;
    F(u8, lbmw, 0xC) = 0xFFU;
    if (0U < 4U) {
        do {
            temp_v0 = Lb_room_member(var_s1 & 0xFF, 1);
            if (temp_v0 == 0) {
                F(s8, (lbmw + var_s0), 0xD) = -1;
                goto block_11;
            }
            if (strcmp(temp_v0, &my_user_id) != 0) {
                temp_v1 = lbmw;
                if ((F(u8, temp_v1, 9) > 0) && (strcmp(temp_v0, temp_v1 + 0x11) == 0)) {
                    F(u8, lbmw, 0xA) = var_s0;
                    F(u8, lbmw, 0xC) = var_s1;
                }
                F(u8, (lbmw + var_s0), 0xD) = var_s1;
block_11:
                var_s0 += 1;
            }
            var_s1 += 1;
        } while (var_s1 < 4U);
    }
    temp_a3 = lbmw;
    temp_a0 = F(u8, temp_a3, 9);
    switch (temp_a0) {                              /* irregular */
    case 0:
        temp_s0 = var_s2 & 0xFFFF;
        if (!(temp_s0 & 0x40)) {
            *(s8 *)0x39DAD0 = 1;
            *(u8 *)0x39DAD2 = 0x11;
            if (ListSelect(temp_a3 + 0xA, var_s2, 3) != 0) {
                F(u8, lbmw, 0xB) = 0U;
            }
            if (temp_s0 & 0x20) {
                temp_v1_2 = lbmw;
                temp_v0_2 = F(s8, (F(u8, temp_v1_2, 0xA) + temp_v1_2), 0xD);
                if ((temp_v0_2 > 0) && (temp_v0_3 = Lb_room_member(temp_v0_2 & 0xFF, 1), (temp_v0_3 != 0))) {
                    memcpy(lbmw + 0x11, temp_v0_3, 8);
                    temp_v1_3 = lbmw;
                    F(u8, temp_v1_3, 0xC) = (u8) F(s8, (F(u8, temp_v1_3, 0xA) + temp_v1_3), 0xD);
                    Lb_PlStatusSet(Lb_get_plID(temp_v0_3));
                    temp_v1_4 = lbmw;
                    *(u8 *)0x39DAD0 = 0;
                    F(u8, temp_v1_4, 9) = (u8) (F(u8, temp_v1_4, 9) + 1);
                    se_req(7, 0x13, 0);
                } else {
                    se_req(7, 0x15, 0);
                }
            }
        }
    case 1:
        temp_s0_2 = var_s2 & 0xFFFF;
        if ((temp_s0_2 & 0x40) || (F(u8, temp_a3, 0xC) == 0xFF)) {
            *(u8 *)0x39DAD0 = 1;
            *(u8 *)0x39DAD2 = 0x11;
            F(u8, temp_a3, 9) = 0U;
            var_s2 = var_s2 & 0xFFBF & 0xFFFF;
            se_req(7, 0x14, 0, temp_a3);
        } else {
            if (temp_s0_2 & 0xC00) {
                F(u8, temp_a3, 0xB) = (u8) (F(u8, temp_a3, 0xB) ^ 1);
                se_req(7, 0x11, 0, temp_a3);
            }
            temp_v1_5 = lbmw;
            if ((F(u8, temp_v1_5, 0xB) == 0) && (temp_s0_2 & 0x200)) {
                temp_v0_4 = Lb_room_member(F(u8, temp_v1_5, 0xC), 0);
                if (temp_v0_4 != 0) {
                    Lb_frendlist_entry(lbmw + 0x11);
                    temp_v1_6 = lbmw;
                    F(u8, temp_v1_6, 9) = (u8) (F(u8, temp_v1_6, 9) + 1);
                } else {
                    se_req(7, 0x15, 0);
                }
            }
        }
        break;
    case 2:
        if (F(u8, temp_a3, 0xC) == 0xFF) {
            F(u8, temp_a3, 9) = (u8) (temp_a0 + 1);
        }
        /* fallthrough */
    case 3:
        temp_v0_5 = Plaza_add_friend(SearchResult + 4, temp_a3 + 9, 3, temp_a3);
        if ((temp_v0_5 != 1) && (temp_v0_5 != 0)) {

        } else {
            temp_a0_2 = lbmw;
            if (F(u8, temp_a0_2, 0xC) != 0xFF) {
                F(u8, temp_a0_2, 9) = 1U;
            } else {
                F(u8, temp_a0_2, 9) = 0U;
            }
        }
        return 0;
    }
    return var_s2;
}
