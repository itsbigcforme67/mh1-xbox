/* lb_by151 - agent B 0x005B3ED0-0x005B4330: Lb_menu_move_Core (village / lobby start menu: step 0 = open (ListSelect offline, Menu_select_mv online; confirm calls the page init), step 1 = per-page move handler; the *_i / *_mv handlers take no arguments, a0 is whatever lbmw left in it). */
#include "lobby_f.h"
extern u8 *lbmw;
int Chk_lb_status();
int Cockpit_chat_chk();
int Online_ck();
void Menu_select_mv();
void ListSelect();
void Name_ID_change();
void Menu_quest_i();
void Menu_item_i();
int Menu_mix_i();
void Menu_data_i();
void lb_menu_status_i();
void Menu_equipment_i();
int Menu_chatcnfg_i();
int Menu_chatlog_i();
int lm_place_i();
int lm_room_member_i();
int lm_member_list_i();
int lm_friend_list_i();
int lm_mail_box_i();
int lm_net_status_i();
int lm_introduction_i();
int lm_logout_i();
int Menu_quest_mv();
int lb_menu_item_mv();
int lb_menu_mix_mv();
int Menu_data_mv();
int Menu_status_mv();
int Menu_equipment_mv();
int Menu_chatcnfg_mv();
int Menu_chatlog_mv();
int lm_place_mv();
int lm_member_list_mv();
int lm_room_member_mv();
int lm_friend_list_mv();
int lm_mail_box_mv();
int lm_introduction_mv();
int lm_net_status_mv();
int lm_logout_mv();
void lb_menu_init();
void se_req();
s32 Lb_menu_move_Core(void) {
    int keys;
    u16 r;
    u8 *mp;
    u8 *sp;
    int k;

    if (F(u8, lbmw, 5) == 0) {
        return 1;
    }
    if (Chk_lb_status(0xF) == 0) {
        return 1;
    }
    if (Cockpit_chat_chk() == 1) {
        return 0;
    }
    keys = F(u16, lbmw, 6);
    F(u16, lbmw, 6) = 0;
    switch (F(u8, lbmw, 0)) {
    case 0:
        if (Online_ck() == 1) {
            Menu_select_mv(lbmw + 4, keys, 0x10);
            if ((u16)keys & 0x200) {
                Name_ID_change();
            }
        } else {
            ListSelect(lbmw + 4, keys, 8);
        }
        mp = lbmw;
        k = keys & 0xFFFF;
        *(s16 *)0x39DAD2 = mp[4];
        sp = mp + 4;
        if (k & 0x20) {
            keys = 0;
            switch (*sp) {
            case 0:
                Menu_quest_i();
                break;
            case 1:
                Menu_item_i();
                break;
            case 2:
                F(s8, cw, 0x2C08) = 0;
                keys = Menu_mix_i();
                break;
            case 3:
                Menu_data_i();
                break;
            case 4:
                lb_menu_status_i();
                break;
            case 5:
                Menu_equipment_i();
                break;
            case 6:
                keys = Menu_chatcnfg_i();
                break;
            case 7:
                keys = Menu_chatlog_i();
                break;
            case 8:
                keys = lm_place_i();
                break;
            case 10:
                keys = lm_room_member_i();
                break;
            case 9:
                keys = lm_member_list_i();
                break;
            case 11:
                keys = lm_friend_list_i();
                break;
            case 12:
                keys = lm_mail_box_i();
                break;
            case 14:
                keys = lm_net_status_i();
                break;
            case 13:
                keys = lm_introduction_i();
                break;
            case 15:
                keys = lm_logout_i();
                break;
            default:
                keys = 1;
                break;
            }
            if (keys == 0) {
                F(u8, lbmw, 0) = F(u8, lbmw, 0) + 1;
                se_req(7, 0x13, 0);
            } else {
                se_req(7, 0x15, 0);
            }
        } else if (k & 0x8040) {
            F(u8, mp, 5) = 0;
            se_req(7, 0x14, 0);
            return 1;
        }
        break;
    case 1:
        switch (F(u8, lbmw, 4)) {
        case 0:
            r = Menu_quest_mv(keys);
            break;
        case 1:
            r = lb_menu_item_mv(keys);
            break;
        case 2:
            r = lb_menu_mix_mv(keys);
            break;
        case 3:
            r = Menu_data_mv(keys);
            break;
        case 4:
            r = Menu_status_mv(keys);
            break;
        case 5:
            r = Menu_equipment_mv(keys);
            break;
        case 6:
            r = Menu_chatcnfg_mv(keys);
            break;
        case 7:
            r = Menu_chatlog_mv(keys);
            break;
        case 8:
            r = lm_place_mv(keys);
            break;
        case 9:
            r = lm_member_list_mv(keys);
            break;
        case 10:
            r = lm_room_member_mv(keys);
            break;
        case 11:
            r = lm_friend_list_mv(keys);
            break;
        case 12:
            r = lm_mail_box_mv(keys);
            break;
        case 13:
            r = lm_introduction_mv(keys);
            break;
        case 14:
            r = lm_net_status_mv(keys);
            break;
        case 15:
            r = lm_logout_mv(keys);
            break;
        default:
            r = 0x40;
            break;
        }
        k = r & 0xFFFF;
        if (k & 0x8000) {
            F(u8, lbmw, 5) = 0;
            F(u8, cw, 0x2C08) = 1;
            return 1;
        }
        if (k & 0x40) {
            lb_menu_init();
            se_req(7, 0x14, 0);
            F(u8, cw, 0x2C08) = 1;
        }
        break;
    }
    return 0;
}
