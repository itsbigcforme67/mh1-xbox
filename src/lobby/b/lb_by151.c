/* lb_by151 - agent B 0x005B3ED0-0x005B4330: Lb_menu_move_Core (village / lobby start menu: step 0 = open (ListSelect offline, Menu_select_mv online; confirm calls the page init), step 1 = per-page move handler; the *_i / *_mv handlers take no arguments, a0 is whatever lbmw left in it). */
#include "lobby_f.h"
#include "lbsys_g.h"
#ifndef __MWERKS__
unsigned short Menu_chatcnfg_mv();   /* (declared before its first use) */
#endif
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
#ifdef __MWERKS__
int Menu_chatcnfg_mv();
#else
unsigned short Menu_chatcnfg_mv();   /* the real return type (tools/check_kr_returns.py) */
#endif
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
#ifdef VERSION_MHG
int menu_option_i();
int lobby_func_0058CF40();
int Lb_menu_win_size_mv();
void Lb_menu_skill_set();

/* G's start menu is not MH1's: 7 rows offline, 16 online, and the rows moved (G lobby tables 0x5FDB90 offline, 0x5FDBD0
   online, labels lb_menu_msg_entry + 0x40 and lb_menu_msg). The value is the page number of the switches below and of
   DispLobbyMenu: MH1's rows, 16 = G's one-row Options page. G's main 0x2EEF18 gives an offline row its help line. */
#define LB_PAGE_OPTION 16
static const u8 lb_page_off[7] = {1, 2, 3, 0, 4, 5, LB_PAGE_OPTION};
static const u8 lb_page_on[16] = {1, 2, 3, 0, 11, 9, 8, LB_PAGE_OPTION, 4, 5, 13, 14, 12, 7, 6, 15};
static const u8 lb_help_off[7] = {0, 1, 2, 3, 8, 9, 7};

int Lb_menu_page(int row) {
    return Online_ck() == 1 ? lb_page_on[row] : lb_page_off[row];
}

int Lb_menu_help(int row) {
    return Online_ck() == 1 ? row : lb_help_off[row];
}

/* G 0x58E5A4: back from a page */
static void lb_menu_back(void) {
    if (F(s8, cw, CW_OFS(0x2C08)) == 0) {
        F(s8, cw, CW_OFS(0x2C08)) = 1;
        ((u8 *)&lb_sys)[0x94] = 1;
    }
}
#define LB_MENU_PAGE(row) Lb_menu_page(row)
#else
#define LB_MENU_PAGE(row) (row)
#endif
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
#ifndef VERSION_MHG
    if (Cockpit_chat_chk() == 1) {
        return 0;
    }
#endif
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
#ifdef VERSION_MHG
            ListSelect(lbmw + 4, keys, 7);
#else
            ListSelect(lbmw + 4, keys, 8);
#endif
        }
        mp = lbmw;
        k = keys & 0xFFFF;
#ifdef VERSION_MHG
        *(s16 *)0x39DAD2 = Lb_menu_help(mp[4]);
#else
        *(s16 *)0x39DAD2 = mp[4];
#endif
        sp = mp + 4;
        if (k & 0x20) {
            keys = 0;
            switch (LB_MENU_PAGE(*sp)) {
            case 0:
                Menu_quest_i();
                break;
            case 1:
                Menu_item_i();
                break;
            case 2:
#ifdef VERSION_MHG
                Lb_menu_skill_set();
#else
                F(s8, cw, CW_OFS(0x2C08)) = 0;
#endif
                keys = Menu_mix_i();
                break;
            case 3:
#ifdef VERSION_MHG
                Lb_menu_skill_set();
#endif
                Menu_data_i();
                break;
            case 4:
                lb_menu_status_i();
                break;
            case 5:
#ifdef VERSION_MHG
                Lb_menu_skill_set();
#endif
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
#ifdef VERSION_MHG
            case 12:
                keys = lobby_func_0058CF40();   /* G's names are MH1's by position: its Friends List uses the lm_mail_box pair, its mail these */
                break;
#else
            case 12:
                keys = lm_mail_box_i();
                break;
#endif
            case 14:
                keys = lm_net_status_i();
                break;
            case 13:
                keys = lm_introduction_i();
                break;
            case 15:
                keys = lm_logout_i();
                break;
#ifdef VERSION_MHG
            case LB_PAGE_OPTION:
                keys = menu_option_i();
                break;
#endif
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
        switch (LB_MENU_PAGE(F(u8, lbmw, 4))) {
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
#ifdef VERSION_MHG
        case 11:
            r = lm_mail_box_mv(keys);
            break;
        case 12:
            r = lm_friend_list_mv(keys);
            break;
#else
        case 11:
            r = lm_friend_list_mv(keys);
            break;
        case 12:
            r = lm_mail_box_mv(keys);
            break;
#endif
        case 13:
            r = lm_introduction_mv(keys);
            break;
        case 14:
            r = lm_net_status_mv(keys);
            break;
        case 15:
            r = lm_logout_mv(keys);
            break;
#ifdef VERSION_MHG
        case LB_PAGE_OPTION:
            r = Lb_menu_win_size_mv(keys);
            break;
#endif
        default:
            r = 0x40;
            break;
        }
        k = r & 0xFFFF;
        if (k & 0x8000) {
            F(u8, lbmw, 5) = 0;
#ifdef VERSION_MHG
            lb_menu_back();
            se_req(7, 0x14, 0);
#else
            F(u8, cw, CW_OFS(0x2C08)) = 1;
#endif
            return 1;
        }
        if (k & 0x40) {
            lb_menu_init();
            se_req(7, 0x14, 0);
#ifdef VERSION_MHG
            lb_menu_back();
#else
            F(u8, cw, CW_OFS(0x2C08)) = 1;
#endif
        }
        break;
    }
    return 0;
}
