#include "lobby_a.h"
extern int lbmw;
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
extern char jtbl_460_0065E720[];
void DispLobbyMenu(void) {
    u8 temp_a0;
    u8 temp_v1;
    int temp_a1;

    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    temp_a1 = lbmw;
    temp_a0 = F(u8, temp_a1, 0);
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        Disp_lb_menu(0);
        break;
    case 1:                                         /* switch 1 */
        temp_v1 = F(u8, temp_a1, 4);
        switch (temp_v1) {                          /* switch 2 */
        case 0:                                     /* switch 2 */
            Pit_disp_quest(&jtbl_460_0065E720);
            break;
        case 1:                                     /* switch 2 */
            Pit_disp_item_list(&jtbl_460_0065E720);
            break;
        case 2:                                     /* switch 2 */
            Pit_disp_item_mix(&jtbl_460_0065E720);
            break;
        case 3:                                     /* switch 2 */
            Pit_disp_data(&jtbl_460_0065E720);
            break;
        case 4:                                     /* switch 2 */
            Pit_disp_menu_status(&jtbl_460_0065E720);
            break;
        case 5:                                     /* switch 2 */
            Pit_disp_menu_equipment(&jtbl_460_0065E720);
            break;
        case 6:                                     /* switch 2 */
            Pit_disp_chat_cnfg(&jtbl_460_0065E720);
            break;
        case 7:                                     /* switch 2 */
            Pit_disp_chat_log(&jtbl_460_0065E720);
            break;
        case 8:                                     /* switch 2 */
            lm_place_trans(&jtbl_460_0065E720);
            break;
        case 10:                                    /* switch 2 */
            disp_lm_room_member(&jtbl_460_0065E720);
            break;
        case 9:                                     /* switch 2 */
            lm_member_trans(&jtbl_460_0065E720);
            break;
        case 11:                                    /* switch 2 */
            lm_friend_list_trans(&jtbl_460_0065E720);
            break;
        case 12:                                    /* switch 2 */
            lm_mail_box_trans(&jtbl_460_0065E720);
            break;
        case 14:                                    /* switch 2 */
            lm_net_status_trans(&jtbl_460_0065E720);
            break;
        case 13:                                    /* switch 2 */
            lm_introduction_trans(&jtbl_460_0065E720);
            break;
        }
        break;
    }
    if (*(u8 *)0x39DAD0 != 0) {
        Disp_menu_help();
    }
}
