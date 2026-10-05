#include "lobby_a.h"
extern int lbmw;
s32 Lb_menu_move_Core(void) {
    s32 temp_a1;
    s32 temp_v1_2;
    s32 var_v0;
    u16 var_s0;
    u8 temp_v0_2;
    u8 temp_v0_3;
    u8 temp_v0_4;
    int temp_a0;
    int temp_a0_2;
    int temp_v0;
    int temp_v1;

    if (F(u8, lbmw, 5) == 0) {
        return 1;
    }
    if (Chk_lb_status(0xF) == 0) {
        return 1;
    }
    if (Cockpit_chat_chk() == 1) {
        return 0;
    }
    temp_v0 = lbmw;
    var_s0 = F(u16, temp_v0, 6);
    F(u16, temp_v0, 6) = 0U;
    temp_a0 = lbmw;
    temp_v0_2 = F(u8, temp_a0, 0);
    switch (temp_v0_2) {                            /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        if (Online_ck(temp_a0) == 1) {
            Menu_select_mv(lbmw + 4, var_s0, 0x10);
            if (var_s0 & 0xFFFF & 0x200) {
                Name_ID_change();
            }
        } else {
            ListSelect(lbmw + 4, var_s0, 8);
        }
        temp_a0_2 = lbmw;
        temp_a1 = var_s0 & 0xFFFF;
        *(s16 *)0x39DAD2 = (s16) F(u8, temp_a0_2, 4);
        if (temp_a1 & 0x20) {
            temp_v0_3 = F(u8, temp_a0_2, 4);
            var_s0 = 0;
            switch (temp_v0_3) {                    /* switch 2 */
            case 0:                                 /* switch 2 */
            case 16:                                /* switch 3 */
                Menu_quest_i(lbmw);
                break;
            case 1:                                 /* switch 2 */
            case 17:                                /* switch 3 */
                Menu_item_i(lbmw);
                break;
            case 2:                                 /* switch 2 */
            case 18:                                /* switch 3 */
                F(s8, (u8 *)cw, 0x2C08) = 0;
                var_s0 = Menu_mix_i(lbmw);
                break;
            case 3:                                 /* switch 2 */
            case 19:                                /* switch 3 */
                Menu_data_i(lbmw);
                break;
            case 4:                                 /* switch 2 */
            case 20:                                /* switch 3 */
                lb_menu_status_i(lbmw);
                break;
            case 5:                                 /* switch 2 */
            case 21:                                /* switch 3 */
                Menu_equipment_i(lbmw);
                break;
            case 6:                                 /* switch 2 */
            case 22:                                /* switch 3 */
                var_s0 = Menu_chatcnfg_i(lbmw);
                break;
            case 7:                                 /* switch 2 */
            case 23:                                /* switch 3 */
                var_s0 = Menu_chatlog_i(lbmw);
                break;
            case 8:                                 /* switch 2 */
            case 24:                                /* switch 3 */
                var_s0 = lm_place_i(lbmw);
                break;
            case 10:                                /* switch 2 */
            case 26:                                /* switch 3 */
                var_s0 = lm_room_member_i(lbmw);
                break;
            case 9:                                 /* switch 2 */
            case 25:                                /* switch 3 */
                var_s0 = lm_member_list_i(lbmw);
                break;
            case 11:                                /* switch 2 */
            case 27:                                /* switch 3 */
                var_s0 = lm_friend_list_i(lbmw);
                break;
            case 12:                                /* switch 2 */
            case 28:                                /* switch 3 */
                var_s0 = lm_mail_box_i(lbmw);
                break;
            case 14:                                /* switch 2 */
            case 30:                                /* switch 3 */
                var_s0 = lm_net_status_i(lbmw);
                break;
            case 13:                                /* switch 2 */
            case 29:                                /* switch 3 */
                var_s0 = lm_introduction_i(lbmw);
                break;
            case 15:                                /* switch 2 */
            case 31:                                /* switch 3 */
                var_s0 = lm_logout_i(lbmw);
                break;
            default:                                /* switch 2 */
                var_s0 = 1;
                break;
            }
            if (var_s0 == 0) {
                temp_v1 = lbmw;
                F(u8, temp_v1, 0) = (u8) (F(u8, temp_v1, 0) + 1);
                se_req(7, 0x13, 0);
            } else {
                se_req(7, 0x15, 0);
            }
            goto block_66;
        }
        if (temp_a1 & 0x8040) {
            F(u8, temp_a0_2, 5) = 0U;
            se_req(7, 0x14, 0);
            return 1;
        }
block_66:
    default:                                        /* switch 1 */
        return 0;
    case 1:                                         /* switch 1 */
        temp_v0_4 = F(u8, temp_a0, 4);
        if (temp_v0_4 < 0x10U) {
            switch (temp_v0_4) {                    /* switch 3 */
            case 0:                                 /* switch 3 */
                var_v0 = Menu_quest_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 1:                                 /* switch 3 */
                var_v0 = lb_menu_item_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 2:                                 /* switch 3 */
                var_v0 = lb_menu_mix_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 3:                                 /* switch 3 */
                var_v0 = Menu_data_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 4:                                 /* switch 3 */
                var_v0 = Menu_status_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 5:                                 /* switch 3 */
                var_v0 = Menu_equipment_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 6:                                 /* switch 3 */
                var_v0 = Menu_chatcnfg_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 7:                                 /* switch 3 */
                var_v0 = Menu_chatlog_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 8:                                 /* switch 3 */
                var_v0 = lm_place_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 9:                                 /* switch 3 */
                var_v0 = lm_member_list_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 10:                                /* switch 3 */
                var_v0 = lm_room_member_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 11:                                /* switch 3 */
                var_v0 = lm_friend_list_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 12:                                /* switch 3 */
                var_v0 = lm_mail_box_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 13:                                /* switch 3 */
                var_v0 = lm_introduction_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 14:                                /* switch 3 */
                var_v0 = lm_net_status_mv(var_s0) & 0xFFFF;
                goto block_62;
            case 15:                                /* switch 3 */
                var_v0 = lm_logout_mv(var_s0) & 0xFFFF;
                goto block_62;
            }
        } else {
            var_v0 = 0x40;
block_62:
            temp_v1_2 = var_v0 & 0xFFFF;
            if (temp_v1_2 & 0x8000) {
                F(u8, lbmw, 5) = 0U;
                F(s8, (u8 *)cw, 0x2C08) = 1;
                return 1;
            }
            if (temp_v1_2 & 0x40) {
                lb_menu_init();
                se_req(7, 0x14, 0);
                F(s8, (u8 *)cw, 0x2C08) = 1;
            }
            goto block_66;
        }
        break;
    }
}
